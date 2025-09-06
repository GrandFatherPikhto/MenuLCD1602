#include <string.h>
#include <stdio.h>

#include "common.h"
#include "config.h"
#include "lcd1602.h"
#include "menu.h"

#define CONFIG_PWM_DIVIDER_MAX 10000  ///< Максимальное значение делителя ШИМ
#define CONFIG_PWM_DIVIDER_MIN 2      ///< Минимальное значение делителя ШИМ
#define CONFIG_CONFIG_PWM_FREQUENCY_MAX 1000000 ///< Максимальное значение частоты ШИМ
#define CONFIG_CONFIG_PWM_FREQUENCY_MIN 5000    ///< Минимальное значение частоты ШИМ

#define CONFIG_HI_CHANNEL 0
#define CONFIG_LO_CHANNEL 1

#define CONFIG_CHANNEL_ENABLE 0x01
#define CONFIG_CHANNEL_PWM_ENABLE 0x02

#define CONFIG_FILE_NAME "/home/yevst/Projects/CCPP/STM32/MenuLCD1602/config.bin"

void config_change_start(bool enable)
{
    printf("%s:%d Work: %s\n", __FILE__, __LINE__, enable ? "On" : "Off");
}

void config_change_ch_hi_duration(int32_t duration)
{
    printf("%s:%d CH1 Duration: %d\n", __FILE__, __LINE__, duration);
}

#if 0

typedef struct {
    int divider;
    int frequency;
} frequency_t;

typedef struct {
    uint32_t magic;        // Магическое число для идентификации
    uint16_t version;      // Версия формата
    uint16_t checksum;     // Контрольная сумма
    uint32_t data_size;    // Размер данных
} file_header_t;

static const frequency_t s_frequencies[] = {
    {5, 2000000}, // 2.0 MHz
    {8, 1250000}, // 1.25 MHz
    {10, 1000000}, // 1.0 MHz
    {16, 625000}, // 0.625 MHz
    {20, 500000}, // 0.5 MHz
    {25, 400000}, // 0.4 MHz
    {32, 312500}, // 0.3125 MHz
    {40, 250000}, // 0.25 MHz
    {50, 200000}, // 0.2 MHz
    {64, 156250}, // 0.15625 MHz
    {80, 125000}, // 0.125 MHz
    {100, 100000}, // 0.1 MHz
    {125, 80000}, // 80.0 kHz
    {128, 78125}, // 78.125 kHz
    {160, 62500}, // 62.5 kHz
    {200, 50000}, // 50.0 kHz
    {250, 40000}, // 40.0 kHz
    {320, 31250}, // 31.25 kHz
    {400, 25000}, // 25.0 kHz
    {500, 20000}, // 20.0 kHz
    {625, 16000}, // 16.0 kHz
    {640, 15625}, // 15.625 kHz
    {800, 12500}, // 12.5 kHz
    {1000, 10000}, // 10.0 kHz
    {1250, 8000}, // 8.0 kHz
    {1600, 6250}, // 6.25 kHz
    {2000, 5000}, // 5.0 kHz
    {2500, 4000}, // 4.0 kHz
    {3125, 3200}, // 3.2 kHz
    {3200, 3125}, // 3.125 kHz
    {4000, 2500}, // 2.5 kHz
    {5000, 2000}, // 2.0 kHz
    {6250, 1600}, // 1.6 kHz
    {8000, 1250}, // 1.25 kHz
    {10000, 1000}, // 1.0 kHz
};

static uint8_t s_factors[] = {1, 2, 5, 10, 25, 50};

/**
 * @brief Структура для хранения данных ШИМ
 * 
 */
typedef struct
{
    uint8_t pwm_divider_idx;   ///< Делитель ШИМ
    frequency_t freq;
    uint16_t start;
    uint16_t duration;
    int8_t duty;
    uint8_t start_factor_idx;
    uint8_t duration_factor_idx;
    uint8_t state;
} channel_data_t;

/**
 * @brief Структура для хранения данных элементов
 * 
 */
typedef struct 
{
    channel_data_t channel_data [2]; ///< Данные каналов
} config_context_t;


static config_context_t s_context = {0}; ///< Данные элементов

inline static int s_calc_pwm_frequency (uint8_t divider);
static void s_config_pwm_channel_str(uint8_t idx, char *value, size_t len);
static void s_set_pwm_divider (uint8_t idx, int8_t delta);
static int s_load_channel_data_with_header(const char* filename, config_context_t *data);
static int s_save_channel_data_with_header(const char* filename, const config_context_t *data);
static void s_set_channel_start(uint8_t idx, int8_t delta);
static void s_set_channel_duration(uint8_t idx, int8_t delta);
static void s_set_channel_pwm_duty(uint8_t idx, int8_t delta);
static void s_set_channel_enable(uint8_t idx, int8_t delta);
static void s_set_channel_pwm_enable(uint8_t idx, int8_t delta);

#define FILE_MAGIC 0x4348414E // 'CHAN'
#define FILE_VERSION 1

/**
 * @brief Инициализация данных элементов
 */
void config_init (void)
{
    memset(&s_context, 0, sizeof(config_context_t));

    channel_data_t *ch = &(s_context.channel_data[0]);
    ch->pwm_divider_idx = 2;
    if (access(CONFIG_FILE_NAME, F_OK) == 0) {
        printf("Файл %s существует.\n", CONFIG_FILE_NAME);
        s_load_channel_data_with_header(CONFIG_FILE_NAME, &s_context);
    } else {
        printf("Файл %s не существует.\n", CONFIG_FILE_NAME);
        s_save_channel_data_with_header(CONFIG_FILE_NAME, &s_context);
    }    
}

void config_save(void)
{
    s_save_channel_data_with_header(CONFIG_FILE_NAME, &s_context);
}

/**
 * @brief Вычисляет частоту ШИМ
 */
inline static int s_calc_pwm_frequency (uint8_t idx)
{
    if (s_context.channel_data[idx].pwm_divider_idx > sizeof(s_frequencies) / sizeof(frequency_t))
        return -1;
    uint8_t pos = s_context.channel_data[idx].pwm_divider_idx;
    int frequency = s_frequencies[pos].frequency;
    s_context.channel_data[idx].freq.divider = s_frequencies[pos].divider;
    s_context.channel_data[idx].freq.frequency = s_frequencies[pos].frequency;
    // printf("%s:%d:\tpos: %u, divider: %d, frequency: %d\n", __FILE__, __LINE__, pos, divider, frequency);
    return frequency;
}

static void s_config_pwm_channel_str(uint8_t idx, char *value, size_t len)
{
    int frequency = s_calc_pwm_frequency(idx);
    //printf("%s:%d:\tfrequency: %d\n", __FILE__, __LINE__, frequency);

    if (frequency <= 0) {
        snprintf(value, len, "Error!");
    } else if (frequency >= 1000000)
    {
        snprintf(value, len, "Lo: %.1f MHz", (float)frequency / 1000000.0);
    } else if (frequency >= 1000)
    {
        snprintf(value, len, "Lo: %.1f kHz", (float)frequency / 1000.0);
    } else 
    {
        snprintf(value, len, "Lo: %.1f Hz", (float)frequency / 1000.0);
    }
}

void config_pwm_hi_channel_str(char *value, size_t len)
{
    s_config_pwm_channel_str(CONFIG_HI_CHANNEL, value, len);
}

void config_pwm_lo_channel_str(char *value, size_t len)
{
    s_config_pwm_channel_str(CONFIG_LO_CHANNEL, value, len);
}

/** 
 * @return void
 */
static void s_set_pwm_divider (uint8_t idx, int8_t delta)
{
    int size = sizeof(s_frequencies) / sizeof(frequency_t);
    int pos = s_context.channel_data[idx].pwm_divider_idx;

    pos += delta;
    while (pos < 0)
        pos += size;
    pos %= size;


    if (pos >=0 && pos < size)
    {
        s_context.channel_data[idx].pwm_divider_idx = pos;
    } else
    {
        s_context.channel_data[idx].pwm_divider_idx = 2;
    }

    // printf("%s:%d\tpos = %d, delta=%d\n", __FILE__, __LINE__, s_context.channel_data[idx].pos, delta);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 01
 */
void config_set_hi_pwm_freq (int8_t delta)
{
    s_set_pwm_divider(CONFIG_HI_CHANNEL, delta);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 01
 */
void config_set_lo_pwm_freq (int8_t delta)
{
    s_set_pwm_divider(CONFIG_LO_CHANNEL, delta);
}

void config_set_hi_start (int8_t delta)
{
    s_set_channel_start(CONFIG_HI_CHANNEL, delta);
}

void config_set_lo_start (int8_t delta)
{
    s_set_channel_start(CONFIG_LO_CHANNEL, delta);
}

void config_set_hi_duration (int8_t delta)
{
    s_set_channel_duration(CONFIG_HI_CHANNEL, delta);
}

void config_set_lo_duration (int8_t delta)
{
    s_set_channel_duration(CONFIG_LO_CHANNEL, delta);
}

void config_set_hi_enable (int8_t delta)
{
    s_set_channel_enable(CONFIG_HI_CHANNEL, delta);
}

void config_set_lo_enable (int8_t delta)
{
    s_set_channel_enable(CONFIG_LO_CHANNEL, delta);
}

void config_set_hi_pwm_enable (int8_t delta)
{
    s_set_channel_pwm_enable(CONFIG_HI_CHANNEL, delta);
}

void config_set_lo_pwm_enable (int8_t delta)
{
    s_set_channel_pwm_enable(CONFIG_LO_CHANNEL, delta);
}

void config_set_hi_pwm_duty (int8_t delta)
{
    s_set_channel_pwm_duty(CONFIG_HI_CHANNEL, delta);
}

void config_set_lo_pwm_duty (int8_t delta)
{
    s_set_channel_pwm_duty(CONFIG_LO_CHANNEL, delta);
}


static int s_save_channel_data_with_header(const char* filename, const config_context_t *data) {
    FILE* file = fopen(filename, "wb");
    if (!file) return -1;
    
    // Создаем заголовок
    file_header_t header = {
        .magic = FILE_MAGIC,
        .version = FILE_VERSION,
        .checksum = 0, // Можно рассчитать контрольную сумму
        .data_size = sizeof(config_context_t)
    };
    
    // Записываем заголовок
    if (fwrite(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        return -2;
    }
    
    // Записываем данные
    if (fwrite(data, sizeof(config_context_t), 1, file) != 1) {
        fclose(file);
        return -3;
    }
    
    fclose(file);
    return 0;
}

static int s_load_channel_data_with_header(const char* filename, config_context_t *data) {
    FILE* file = fopen(filename, "rb");
    if (!file) return -1;
    
    // Читаем заголовок
    file_header_t header;
    if (fread(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        return -2;
    }
    
    // Проверяем магическое число и версию
    if (header.magic != FILE_MAGIC || header.version != FILE_VERSION) {
        fclose(file);
        return -3;
    }
    
    // Проверяем размер данных
    if (header.data_size != sizeof(config_context_t)) {
        fclose(file);
        return -4;
    }
    
    // Читаем данные
    if (fread(data, sizeof(config_context_t), 2, file) != 2) {
        fclose(file);
        return -5;
    }
    
    fclose(file);
    return 0;
}

static void s_set_channel_start(uint8_t idx, int8_t delta)
{
    channel_data_t *channel = &s_context.channel_data[idx];
    int new_start = channel->start + delta * s_factors[channel->start_factor_idx];
    int max_start = PULSE_MAX_TICKS - channel->duration;
    
    channel->start = (new_start < 0) ? 0 : (new_start > max_start) ? max_start : new_start;
}

static void s_set_channel_duration(uint8_t idx, int8_t delta)
{
    channel_data_t *channel = &s_context.channel_data[idx];
    int new_duration = channel->duration + delta * s_factors[channel->duration_factor_idx];
    int max_duration = PULSE_MAX_TICKS - channel->duration;
    
    channel->duration = (new_duration < 0) ? 0 : (new_duration > max_duration) ? max_duration : new_duration;
}

static void s_set_channel_pwm_duty(uint8_t idx, int8_t delta)
{
    channel_data_t *channel = &(s_context.channel_data[idx]);
    int new_duty = channel->duty + delta;
    channel->duty = (new_duty < 0) ? 0 : (new_duty >= 100) ? 100 : new_duty;
}

static void s_set_channel_enable(uint8_t idx, int8_t delta)
{
    channel_data_t *channel = &s_context.channel_data[idx];
    bool enable = (IS_FLAG_SET(channel->state, CONFIG_CHANNEL_ENABLE) + delta) % 2;
    
    enable ? SET_FLAG(channel->state, CONFIG_CHANNEL_ENABLE)
           : RESET_FLAG(channel->state, CONFIG_CHANNEL_ENABLE);
}

static void s_set_channel_pwm_enable(uint8_t idx, int8_t delta)
{
    channel_data_t *channel = &s_context.channel_data[idx];
    bool enable = (IS_FLAG_SET(channel->state, CONFIG_CHANNEL_PWM_ENABLE) + delta) % 2;
    
    enable ? SET_FLAG(channel->state, CONFIG_CHANNEL_PWM_ENABLE)
           : RESET_FLAG(channel->state, CONFIG_CHANNEL_PWM_ENABLE);
}
#endif