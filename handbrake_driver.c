#include "handbrake_driver.h"

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define HANDBRAKE_BUTTON_UP_GPIO            (1)
#define HANDBRAKE_BUTTON_DOWN_GPIO          (5)
#define HANDBRAKE_BUTTON_MODE_TOGGLE_GPIO   (9)
#define GPIO_NUM                            (28)
#define LEVER_DEBOUNCE_TIME_MS              (20)
#define MODE_TOGGLE_BUTTON_DEBOUNCE_TIME_MS (200)

typedef enum {
  MODE_SEQUENTIAL,
  MODE_HANDBRAKE,
  MODE_NUM
} HANDBRAKE_MODE;

static HANDBRAKE_MODE g_mode = MODE_SEQUENTIAL;

static const HANDBRAKE_ACTION action_map[MODE_NUM][GPIO_NUM] = {
    [MODE_SEQUENTIAL][HANDBRAKE_BUTTON_UP_GPIO]          = HANDBRAKE_ACTION_UP,
    [MODE_SEQUENTIAL][HANDBRAKE_BUTTON_DOWN_GPIO]        = HANDBRAKE_ACTION_DOWN,
    [MODE_SEQUENTIAL][HANDBRAKE_BUTTON_MODE_TOGGLE_GPIO] = HANDBRAKE_ACTION_NONE,
    [MODE_HANDBRAKE][HANDBRAKE_BUTTON_UP_GPIO]           = HANDBRAKE_ACTION_BRAKE,
    [MODE_HANDBRAKE][HANDBRAKE_BUTTON_DOWN_GPIO]         = HANDBRAKE_ACTION_NONE,
    [MODE_HANDBRAKE][HANDBRAKE_BUTTON_MODE_TOGGLE_GPIO]  = HANDBRAKE_ACTION_NONE
};

void init_buttons() {
    board_led_on();
    gpio_init(HANDBRAKE_BUTTON_UP_GPIO);
    gpio_pull_up(HANDBRAKE_BUTTON_UP_GPIO);
    gpio_set_dir(HANDBRAKE_BUTTON_UP_GPIO, GPIO_IN);

    gpio_init(HANDBRAKE_BUTTON_DOWN_GPIO);
    gpio_pull_up(HANDBRAKE_BUTTON_DOWN_GPIO);
    gpio_set_dir(HANDBRAKE_BUTTON_DOWN_GPIO, GPIO_IN);

    gpio_init(HANDBRAKE_BUTTON_MODE_TOGGLE_GPIO);
    gpio_pull_up(HANDBRAKE_BUTTON_MODE_TOGGLE_GPIO);
    gpio_set_dir(HANDBRAKE_BUTTON_MODE_TOGGLE_GPIO, GPIO_IN);
}

HANDBRAKE_ACTION gpio_to_button(uint8_t gpio) {
    return action_map[g_mode][gpio];
}

HANDBRAKE_ACTION get_handbrake_action()
{
    HANDBRAKE_ACTION button_pressed = HANDBRAKE_ACTION_NONE;

    if(gpio_get(HANDBRAKE_BUTTON_DOWN_GPIO) == false)
    {
        /* debounce */
        board_delay(LEVER_DEBOUNCE_TIME_MS);
        button_pressed = gpio_to_button(HANDBRAKE_BUTTON_DOWN_GPIO);
    } else if(gpio_get(HANDBRAKE_BUTTON_UP_GPIO) == false)
    {
        /* debounce */
        board_delay(LEVER_DEBOUNCE_TIME_MS);
        button_pressed = gpio_to_button(HANDBRAKE_BUTTON_UP_GPIO);
    } else if(gpio_get(HANDBRAKE_BUTTON_MODE_TOGGLE_GPIO) == false)
    {
        /* debounce */
        board_delay(MODE_TOGGLE_BUTTON_DEBOUNCE_TIME_MS);
        while(gpio_get(HANDBRAKE_BUTTON_MODE_TOGGLE_GPIO) == false) {
            /* don't register until the button is released */
        }

        button_pressed = HANDBRAKE_ACTION_NONE;
        if(g_mode == MODE_HANDBRAKE) {
            g_mode = MODE_SEQUENTIAL;
            board_led_on();
        } else {
            g_mode = MODE_HANDBRAKE;
            board_led_off();
        }
    }

    return button_pressed;
}