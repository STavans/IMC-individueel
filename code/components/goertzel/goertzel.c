/**
 * Example ESP-ADF application using a Goertzel filter
 * 
 * This application is an adaptation of the 
 * Voice activity detection (VAD) example application in the
 * ESP-ADF framework by Espressif
 * https://github.com/espressif/esp-adf/tree/master/examples/speech_recognition/vad
 * 
 * Goertzel algoritm initially implemented by P.S.M. Goossens,
 * adapted by Hans van der Linden
 * 
 * Avans Hogeschool, Opleiding Technische Informatica
 */

#include <math.h>

#include "freertos/FreeRTOS.h"

#include "esp_err.h"
#include "esp_log.h"
#include "board.h"
#include "audio_common.h"
#include "audio_pipeline.h"
#include "i2s_stream.h"
#include "raw_stream.h"
#include "filter_resample.h"

#include "goertzel_filter.h"
#include "goertzel.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"

static const char *TAG = "GOERTZEL-EXAMPLE";

#define GOERTZEL_SAMPLE_RATE_HZ 8000	// Sample rate in [Hz]
#define GOERTZEL_FRAME_LENGTH_MS 100	// Block length in [ms]

#define GOERTZEL_BUFFER_LENGTH (GOERTZEL_FRAME_LENGTH_MS * GOERTZEL_SAMPLE_RATE_HZ / 1000) // Buffer length in samples

#define GOERTZEL_DETECTION_THRESHOLD 50.0f  // Detect a tone when log magnitude is above this value

#define AUDIO_SAMPLE_RATE 48000         // Audio capture sample rate [Hz]

static int isRunning = 0;

static const int GOERTZEL_DETECT_FREQS[] = {
    589
};

#define GOERTZEL_NR_FREQS ((sizeof GOERTZEL_DETECT_FREQS) / (sizeof GOERTZEL_DETECT_FREQS[0]))

static audio_element_handle_t createI2sStream(int sampleRate, audio_stream_type_t type)
{
    i2s_stream_cfg_t i2sCfg = I2S_STREAM_CFG_DEFAULT();
    i2sCfg.type = type;
    i2sCfg.i2s_config.sample_rate = sampleRate;
    audio_element_handle_t i2sStream = i2s_stream_init(&i2sCfg);
    return i2sStream;
}

static audio_element_handle_t createResampleFilter(int sourceRate, int sourceChannels, int destRate, int destChannels)
{
    rsp_filter_cfg_t rspCfg = DEFAULT_RESAMPLE_FILTER_CONFIG();
    rspCfg.src_rate = sourceRate;
    rspCfg.src_ch = sourceChannels;
    rspCfg.dest_rate = destRate;
    rspCfg.dest_ch = destChannels;
    audio_element_handle_t filter = rsp_filter_init(&rspCfg);
    return filter;
}

static audio_element_handle_t createRawStream()
{
    raw_stream_cfg_t rawCfg = {
        .out_rb_size = 8 * 1024,
        .type = AUDIO_STREAM_READER,
    };
    audio_element_handle_t rawReader = raw_stream_init(&rawCfg);
    return rawReader;
}

static audio_pipeline_handle_t createPipeline()
{
    audio_pipeline_cfg_t pipelineCfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    audio_pipeline_handle_t pipeline = audio_pipeline_init(&pipelineCfg);
    return pipeline;
}

/**
 * Determine if a frequency was detected or not, based on the magnitude that the
 * Goertzel filter calculated
 * Use a logarithm for the magnitude
 */
static void detect_freq(int targetFreq, float magnitude)
{
    float logMagnitude = 10.0f * log10f(magnitude);

    if (logMagnitude > GOERTZEL_DETECTION_THRESHOLD) {
        ESP_LOGI(TAG, "Detection at frequency %d Hz (magnitude %.2f, log magnitude %.2f)", targetFreq, magnitude, logMagnitude);
        menu_updateMenu(menu_getLcdInfo(), (void*) "IETS");
    } else
    {
        menu_updateMenu(menu_getLcdInfo(), (void*) "NIKS");
    }
}

void toneDetectionTask(void *p)
{
    isRunning = 1;

    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2sStreamReader;
    audio_element_handle_t resampleFilter;
    audio_element_handle_t rawReader;

    goertzel_filter_cfg_t filtersCfg[GOERTZEL_NR_FREQS];
    goertzel_filter_data_t filtersData[GOERTZEL_NR_FREQS];

    ESP_LOGI(TAG, "Number of Goertzel detection filters is %d", GOERTZEL_NR_FREQS);

    ESP_LOGI(TAG, "Create raw sample buffer");
    int16_t *rawBuffer = (int16_t *) malloc((GOERTZEL_BUFFER_LENGTH * sizeof(int16_t)));
    if (rawBuffer == NULL) {
        ESP_LOGE(TAG, "Memory allocation for raw sample buffer failed");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Setup Goertzel detection filters");
    for (int f = 0; f < GOERTZEL_NR_FREQS; f++) {
        filtersCfg[f].sample_rate = GOERTZEL_SAMPLE_RATE_HZ;
        filtersCfg[f].target_freq = GOERTZEL_DETECT_FREQS[f];
        filtersCfg[f].buffer_length = GOERTZEL_BUFFER_LENGTH;
        esp_err_t error = goertzelFilter_setup(&filtersData[f], &filtersCfg[f]);
        ESP_ERROR_CHECK(error);
    }

    ESP_LOGI(TAG, "Create pipeline");
    pipeline = createPipeline();

    ESP_LOGI(TAG, "Create audio elements for pipeline");
    i2sStreamReader = createI2sStream(AUDIO_SAMPLE_RATE, AUDIO_STREAM_READER);
    resampleFilter = createResampleFilter(AUDIO_SAMPLE_RATE, 2, GOERTZEL_SAMPLE_RATE_HZ, 1);
    rawReader = createRawStream();

    ESP_LOGI(TAG, "Register audio elements to pipeline");
    audio_pipeline_register(pipeline, i2sStreamReader, "i2s");
    audio_pipeline_register(pipeline, resampleFilter, "rsp_filter");
    audio_pipeline_register(pipeline, rawReader, "raw");

    ESP_LOGI(TAG, "Link audio elements together to make pipeline ready");
    const char *linkTag[3] = {"i2s", "rsp_filter", "raw"};
    audio_pipeline_link(pipeline, &linkTag[0], 3);

    ESP_LOGI(TAG, "Start pipeline");
    audio_pipeline_run(pipeline);

    while (isRunning) {
        raw_stream_read(rawReader, (char *) rawBuffer, GOERTZEL_BUFFER_LENGTH * sizeof(int16_t));
        for (int f = 0; f < GOERTZEL_NR_FREQS; f++) {
            float magnitude;

            esp_err_t error = goertzelFilter_process(&filtersData[f], rawBuffer, GOERTZEL_BUFFER_LENGTH);
            ESP_ERROR_CHECK(error);

            if (goertzelFilter_newMagnitude(&filtersData[f], &magnitude)) {
                detect_freq(filtersCfg[f].target_freq, magnitude);
            }
        }
        vTaskDelay(50 / portTICK_RATE_MS);
    }

    // Clean up (if we somehow leave the while loop, that is...)
    ESP_LOGI(TAG, "Deallocate raw sample buffer memory");
    free(rawBuffer);

    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    audio_pipeline_unregister(pipeline, i2sStreamReader);
    audio_pipeline_unregister(pipeline, resampleFilter);
    audio_pipeline_unregister(pipeline, rawReader);

    audio_pipeline_deinit(pipeline);

    audio_element_deinit(i2sStreamReader);
    audio_element_deinit(resampleFilter);
    audio_element_deinit(rawReader);

    vTaskDelete(NULL);
}

void goertzel_start(void)
{
    xTaskCreate(&toneDetectionTask, "tone_detection_task", 1024 * 3, NULL, 10, NULL);
}

void goertzel_stop()
{
    isRunning = 0;
}