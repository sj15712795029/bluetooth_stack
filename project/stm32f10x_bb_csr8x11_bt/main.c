#include <stdio.h>
#include "stm32f10x_conf.h"
#include "board_wrapper.h"
#include "bt_timer.h"
#include "bt_l2cap.h"
#include "bt_rfcomm.h"
#include "bt_phybusif_h4.h"

uint32_t sys_time = 0;
uint32_t last_sys_time = 0;

#define CONF_BSP_TICKS_PER_SEC 100


void board_init()
{
    last_sys_time = sys_time;
    utimer_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    hw_uart_debug_init(115200);
    hw_systick_init(SystemCoreClock/CONF_BSP_TICKS_PER_SEC);
}



void SysTick_Handler(void)
{
    sys_time += 1000/CONF_BSP_TICKS_PER_SEC;
    utimer_polling();
}


extern struct phybusif_cb uart_if;
int main()
{
    board_init();

    while(1)
    {

        	//NVIC_DisableIRQ(USART2_IRQn);
            phybusif_input(&uart_if);
		//NVIC_EnableIRQ(USART2_IRQn);

        if(sys_time - last_sys_time > 1000)
        {
            printf("bt stack running\n");
            last_sys_time = sys_time;
            l2cap_tmr();
            rfcomm_tmr();
        }
    }
}
