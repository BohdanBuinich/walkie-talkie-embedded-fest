#pragma once

#include "driver/i2s.h"
namespace I2S {

    class I2SBase {
    protected:
        void static i2s_adc_data_scale(uint8_t *d_buff, uint8_t *s_buff, uint32_t len) {
            uint32_t j = 0;
            uint32_t dac_value = 0;
            for (int i = 0; i < len; i += 2) {
                dac_value = ((((uint16_t)(s_buff[i + 1] & 0xf) << 8) | ((s_buff[i + 0]))));
                d_buff[j++] = 0;
                d_buff[j++] = dac_value * 256 / 1024;
            }
        }

        const int I2S_SAMPLE_RATE{16000};
        const i2s_bits_per_sample_t I2S_SAMPLE_BITS{I2S_BITS_PER_SAMPLE_32BIT};
    };

    class I2S_TX : public I2SBase {
    public:
        void write_data(char *buf_ptr, int buf_size) {
            size_t i2s_bytes_write;
            i2s_write(I2S_PORT_TX, buf_ptr, buf_size, &i2s_bytes_write, portMAX_DELAY);
        }

        void init() {
            i2s_driver_install(I2S_PORT_TX, &i2s_config_tx, 0, NULL);
            i2s_set_pin(I2S_PORT_TX, &pin_config_tx);
        }

        void uninst() {
            i2s_driver_uninstall(I2S_PORT_TX);
        }

    private:
        const uint8_t I2S_WS_TX{12};
        const uint8_t I2S_SCK_TX{13};
        const uint8_t I2S_DATA_OUT_TX{15};
        const i2s_port_t I2S_PORT_TX{I2S_NUM_0};

        const i2s_pin_config_t pin_config_tx = {
                .bck_io_num = I2S_SCK_TX,
                .ws_io_num = I2S_WS_TX,
                .data_out_num = I2S_DATA_OUT_TX,
                .data_in_num = I2S_PIN_NO_CHANGE
        };

        const i2s_config_t i2s_config_tx = {
            .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = I2S_SAMPLE_RATE,
            .bits_per_sample = I2S_SAMPLE_BITS,
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 32,
            .dma_buf_len = 64,
            .use_apll = false,
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0
        };
    };

    class I2S_RX : public I2SBase {
    public:
        void read_data() {
            size_t bytes_read;
            i2s_read(I2S_PORT_RX, (void *)i2s_read_buff, I2S_READ_LEN, &bytes_read, portMAX_DELAY);
            i2s_adc_data_scale(flash_write_buff, (uint8_t *) i2s_read_buff, I2S_READ_LEN);
        }

        void buff_init() {
            i2s_read_buff = (uint8_t *) calloc(I2S_READ_LEN, sizeof(uint8_t));
            flash_write_buff = (uint8_t *) calloc(I2S_READ_LEN, sizeof(uint8_t));
        }

        void init() {
            i2s_driver_install(I2S_PORT_RX, &i2s_config_rx, 0, NULL);
            i2s_set_pin(I2S_PORT_RX, &pin_config_rx);
        }

        void uninst() {
            i2s_driver_uninstall(I2S_PORT_RX);
        }

        const uint32_t I2S_READ_LEN{1024};
        uint8_t *flash_write_buff;
    private:
        const i2s_config_t i2s_config_rx = {
                .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
                .sample_rate = I2S_SAMPLE_RATE,
                .bits_per_sample = I2S_SAMPLE_BITS,
                .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
                .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
                .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
                .dma_buf_count = 32,
                .dma_buf_len = 64,
                .use_apll = false,
                .tx_desc_auto_clear = false,
                .fixed_mclk = 0
        };

        const uint8_t I2S_WS_RX{25};
        const uint8_t I2S_SCK_RX{26};
        const uint8_t I2S_SD_RX{27};
        const i2s_port_t I2S_PORT_RX{I2S_NUM_1};

        const i2s_pin_config_t pin_config_rx = {
                .bck_io_num = I2S_SCK_RX,
                .ws_io_num = I2S_WS_RX,
                .data_out_num = I2S_PIN_NO_CHANGE,
                .data_in_num = I2S_SD_RX
        };

        uint8_t *i2s_read_buff;
    };
}