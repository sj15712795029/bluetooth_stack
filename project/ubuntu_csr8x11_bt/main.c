#include <stdio.h>
#include "bt_timer.h"
#include "bt_l2cap.h"
#include "bt_rfcomm.h"
#include "bt_phybusif_h4.h"
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

uint32_t sys_time = 0;
uint32_t last_sys_time = 0;

#define CONF_BSP_TICKS_PER_SEC 100


#define UART1_MAX_REV	1024
uint8_t uart_rev_buffer[UART1_MAX_REV];
uint8_t uart_rev_len;

void *timer_thread(void *data)
{
    printf("timer_thread runing...\n");
    while(1)
    {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000/CONF_BSP_TICKS_PER_SEC * 1000;

        select(0, NULL, NULL, NULL, &tv);
        sys_time += 1000/CONF_BSP_TICKS_PER_SEC;

        utimer_polling();
    }
}

#define STDIN_BUF_SIZE 128
uint8_t stdin_buf[STDIN_BUF_SIZE] = {0};
uint8_t stdin_recv_len = 0;

void *stdin_process_thread(void *data)
{

    while(1)
    {
        fd_set read_fd;
        int result = 0;

        /* monitor uart rx */
        FD_ZERO (&read_fd);
        FD_SET (0, &read_fd);

        if ((result = select (1, &read_fd, NULL, NULL, NULL)) == -1)
        {
            printf("ERROR:file[%s],function[%s],line[%d] select fail\n",__FILE__,__FUNCTION__,__LINE__);
        }

        if(result > 0 && FD_ISSET ( 0, &read_fd))
        {

            memset(stdin_buf,0,STDIN_BUF_SIZE);
            int read_result = read(0,stdin_buf,STDIN_BUF_SIZE);
            stdin_recv_len = read_result;
            //printf("--------1---------len %d\n",read_result);
            //bt_hex_dump(stdin_buf,read_result);
            //printf("--------2----------\n");

        }
    }
}


void stdin_process_init()
{

    pthread_t thread_stdin_id;
    pthread_create(&thread_stdin_id, NULL, stdin_process_thread, NULL);
}
void stdin_process_run()
{
    if(stdin_recv_len != 0)
    {
        shell_parse(stdin_buf);
        stdin_recv_len = 0;
    }
}



void board_init()
{
    pthread_t thread_timer_id;

    last_sys_time = sys_time;
    utimer_init();

    bt_start();
    pthread_create(&thread_timer_id, NULL, timer_thread, NULL);


}



extern struct phybusif_cb uart_if;
int main()
{
    stdin_process_init();
    board_init();

    while(1)
    {

        stdin_process_run();
        if(phybusif_input(&uart_if) != BT_ERR_OK)
            usleep(1000);

        if(sys_time - last_sys_time > 1000)
        {
            //printf("bt stack running\n");
            last_sys_time = sys_time;
            l2cap_tmr();
            rfcomm_tmr();
        }


    }
}
