#include "board.h"

/*
    指示灯:
        正常工作    常亮
        有操作发生  快速、单次闪烁
        接收广播    快速闪烁
        可以考虑使用pwm输出

    按钮:
        开启/关闭接收广播:  长按(使用定时器)
*/

static void board_led_init(void)
{
    //  如果可以，使用pwm输出
}

static void board_button_init(void)
{
    //  引入定时器，检测按下时间
}

void board_init(void)
{
    board_led_init();
    board_button_init();
}
