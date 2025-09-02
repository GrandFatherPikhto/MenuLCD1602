# Проект заготовки "Menu" для LCD1602

Этот проект реализует систему меню для дисплея LCD1602, управляемую микроконтроллерами STM32 или ESP32. Меню поддерживает взаимодействие с пользователем через ротационный энкодер и кнопки.

## Содержание

- [Описание](#описание)
- [Функции](#функции)
- [Требования](#требования)
- [Установка](#установка)
- [Настройка](#настройка)
- [Использование](#использование)
- [Лицензия](#лицензия)

## Описание

Система меню обеспечивает иерархическую навигацию с возможностью установки функций обратного вызова для каждого элемента меню. Она поддерживает кольцевую навигацию и переходы на дочерние и родительские уровни.

## Функции

- **Иерархия меню**: Возможность создания многоуровневых меню.
- **Кольцевая структура**: Последний элемент связан с первым, что позволяет циклическую прокрутку.
- **Обратные вызовы**: Каждый элемент меню может запускать функцию обратного вызова.
- **Поддержка ротационного энкодера**: Навигация и выбор через энкодер.
- **Поддержка нажатия кнопок**: Переходы между меню и подменю через кнопки.

## Требования

- **Аппаратная часть**:
  - Дисплей LCD1602
  - микроконтроллер STM32 или ESP32
  - Ротационный энкодер с кнопкой

- **Программная часть**:
  - Компилятор C (например, GCC)
  - Среда разработки STM32CubeMX или платформа Arduino, если используется ESP32

## Установка

1. **Клонирование репозитория**:
```bash
   git clone https://github.com/your-username/menu-lcd1602.git
   cd menu-lcd1602
```

Отображение меню: Меню отображается на LCD1602, обновляясь при изменении текущей позиции.

Пример кода для инициализации меню:

```c
void Menu_Init(void)
{
    menu_node_t *start = menu_activate_node(NULL, "Start", NULL, NULL);
    menu_node_t *options = menu_activate_node(NULL, "Options", NULL, NULL);

    menu_node_t *channel1 = menu_activate_node(options, "Channel 1", NULL, NULL);
    menu_node_t *enable1 = menu_activate_node(channel1, "Enable", NULL, NULL);
    menu_node_t *pwm1 = menu_activate_node(channel1, "PWM", NULL, NULL);
    menu_node_t *pwm_enable = menu_activate_node(pwm1, "Enable", NULL, NULL);
    menu_node_t *pwm_duty_1 = menu_activate_node(pwm1, "Duty", NULL, NULL);
    menu_activate_node(pwm_duty_1, "x1", NULL, NULL);
    menu_activate_node(pwm_duty_1, "x10", NULL, NULL);
    menu_node_t *pwm_freq_1 = menu_activate_node(pwm1, "Frequency", NULL, NULL);
    menu_activate_node(pwm_freq_1, "x1", &config_set_hi_pwm_period01, &config_pwm_hi_channel_freq);
    menu_activate_node(pwm_freq_1, "x10", &config_set_hi_pwm_period10, &config_pwm_hi_channel_freq);
}
```

Функция ```action``` отвечает за обработку изменений значений энкодера (delta)

Функция ```print``` отвечает за возвращение строки, отображаемой в меню, когда активно состояния ```ACTION```

