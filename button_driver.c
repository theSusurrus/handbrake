#include "button_driver.h"

#include "bsp/board.h"

HANDBRAKE_BUTTON get_handbrake_button()
{
    HANDBRAKE_BUTTON button_pressed = HANDBRAKE_BUTTON_NONE;
    int button_state = board_button_read();

    if(button_state == 1)
    {
        button_pressed = HANDBRAKE_BUTTON_UP;
    }

    return button_pressed;
}