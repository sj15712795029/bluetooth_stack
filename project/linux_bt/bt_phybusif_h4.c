/******************************************************************************
  * @file           bt_phybusif.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt phybusif source file
******************************************************************************/


#include "bt_phybusif_h4.h"
#include "bt_hci.h"
#include "bt_common.h"
#include "ringbuffer.h"
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "bt_snoop.h"

#define PHYBUSIF_NAME "/dev/ttyUSB0"

struct phybusif_cb uart_if;


#define BT_DMA_BUF_SIZE	(4*1024)
#define BT_RX_BUF_SIZE (8*1024)
struct ringbuffer bt_ring_buf;
uint8_t bt_rx_buf[BT_RX_BUF_SIZE];
uint8_t bt_dma_rx_buf[BT_DMA_BUF_SIZE];

#define BT_TX_BUF_SIZE	1024
uint8_t bt_tx_buff[BT_TX_BUF_SIZE];

uint8_t uart_thread_process = 1;
uint8_t* bt_get_tx_buffer()
{
    return bt_tx_buff;
}


uint8_t hw_uart_bt_init(uint32_t baud_rate)
{
    speed_t baudrate;
    int flags = O_RDWR | O_NOCTTY;

    struct termios toptions;
    if(baud_rate == 115200)
        baudrate =  B115200;
    else
        baudrate =  B921600;

#if 0
    {
        if (tcgetattr(uart_if.phyuart_fd, &toptions) < 0)
        {
            printf("ERROR:Couldn't get term attributes\n");
            return -1;
        }
        cfsetospeed(&toptions, baudrate);
        cfsetispeed(&toptions, baudrate);


        if( tcsetattr(uart_if.phyuart_fd, TCSANOW, &toptions) < 0)
        {
            printf("ERROR:Couldn't set term attributes\n");
            return -1;
        }

        return 0;
    }
#endif

    printf("phybusif_open %s\n",PHYBUSIF_NAME);

    uart_if.phyuart_fd = open(PHYBUSIF_NAME, flags);
    if (uart_if.phyuart_fd == -1)
    {
        printf("ERROR:Unable to open port %s\n",PHYBUSIF_NAME);
        return -1;
    }

    printf("uart_if.phyuart_fd %d\n",uart_if.phyuart_fd);
    if (tcgetattr(uart_if.phyuart_fd, &toptions) < 0)
    {
        printf("ERROR:Couldn't get term attributes\n");
        return -1;
    }

    cfmakeraw(&toptions);

    // 8N1
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag |= CS8;

    toptions.c_cflag |= CREAD | CLOCAL | CRTSCTS;
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
    toptions.c_cflag &= ~PARENB;

    toptions.c_cc[VMIN]  = 1;
    toptions.c_cc[VTIME] = 0;

    if(tcsetattr(uart_if.phyuart_fd, TCSANOW, &toptions) < 0)
    {
        printf("ERROR:Couldn't set term attributes\n");
        return -1;
    }

    if (tcgetattr(uart_if.phyuart_fd, &toptions) < 0)
    {
        printf("ERROR:Couldn't get term attributes\n");
        return -1;
    }
    cfsetospeed(&toptions, baudrate);
    cfsetispeed(&toptions, baudrate);


    if( tcsetattr(uart_if.phyuart_fd, TCSANOW, &toptions) < 0)
    {
        printf("ERROR:Couldn't set term attributes\n");
        return -1;
    }

    return BT_ERR_OK;

}


void uart_bt_send(uint8_t *buf,uint16_t len)
{
    write(uart_if.phyuart_fd,buf,len);
}



/******************************************************************************
 * func name   : bt_uart_test
 * para        : NULL
 * return      : NULL
 * description : Bluetooth HCI reset command over H4 transport test
******************************************************************************/
void bt_uart_test()
{
    uint8_t hci_reset[4] = {0x01,0x03,0x0c,0x00};
    uart_bt_send(hci_reset,4);
}



err_t phybusif_reset(struct phybusif_cb *cb)
{
    /* Init new ctrl block */
    /* Alloc new bt_pbuf_t. bt will handle dealloc */
    if((cb->p = bt_pbuf_alloc(BT_PBUF_RAW, PBUF_POOL_BUFSIZE, BT_PBUF_POOL)) == NULL)
    {
        BT_TRANSPORT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }
    cb->q = cb->p; /* Make p the pointer to the head of the bt_pbuf_t chain and q to the tail */

    cb->state = W4_PACKET_TYPE;
    return BT_ERR_OK;
}


void *uart_rx_thread(void *data)
{
    while(uart_thread_process)
    {
        fd_set read_fd;
        int result = 0;
        struct timeval tv;
        tv.tv_sec = 1; /* 1s */
        tv.tv_usec = 0;

        /* monitor uart rx */
        FD_ZERO (&read_fd);
        FD_SET (uart_if.phyuart_fd, &read_fd);

        if ((result = select (1 + uart_if.phyuart_fd, &read_fd, NULL, NULL, &tv)) == -1)
        {
            BT_TRANSPORT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] select fail\n",__FILE__,__FUNCTION__,__LINE__);
        }

        if(result > 0 && FD_ISSET ( uart_if.phyuart_fd, &read_fd))
        {

            int read_result = read(uart_if.phyuart_fd,bt_dma_rx_buf,BT_DMA_BUF_SIZE);
#if 0
            printf("--------1---------len %d\n",read_result);
            bt_hex_dump(bt_dma_rx_buf,read_result);
            printf("--------2----------\n");
#endif

            if(ringbuffer_space_left(&bt_ring_buf) > read_result)
                ringbuffer_put(&bt_ring_buf,bt_dma_rx_buf,read_result);
            else
                BT_TRANSPORT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] ring buffer left %d < %d\n",__FILE__,__FUNCTION__,__LINE__,ringbuffer_space_left(&bt_ring_buf),read_result);

        }
    }

	pthread_exit((void *)0);
}

pthread_t thread_rx_id;

void phybusif_open(uint32_t baud_rate)
{
	uart_thread_process = 1;
	ringbuffer_init(&bt_ring_buf,bt_rx_buf,BT_RX_BUF_SIZE);
    hw_uart_bt_init(baud_rate);
  
    pthread_create(&thread_rx_id, NULL, uart_rx_thread, NULL);

}

void phybusif_reopen(uint32_t baud_rate)
{
	uart_thread_process = 0;
	pthread_join(thread_rx_id, NULL);
	close(uart_if.phyuart_fd);

	ringbuffer_reset(&bt_ring_buf);

	phybusif_open(baud_rate);
	
}

void phybusif_close(void)
{
	uart_thread_process = 0;
	pthread_join(thread_rx_id, NULL);
	close(uart_if.phyuart_fd);

	ringbuffer_reset(&bt_ring_buf);
}

void phybusif_output(struct bt_pbuf_t *p, uint16_t len,uint8_t packet_type)
{
    bt_pbuf_header(p, 1);
    ((uint8_t *)p->payload)[0] = packet_type;

#if 0
    switch(packet_type)
    {
    case PHYBUSIF_PACKET_TYPE_CMD
    {
        ((uint8_t *)p->payload)[3] = ;
        break;
    }
    case PHYBUSIF_PACKET_TYPE_ACL_DATA
    {
        break;
    }
    case PHYBUSIF_PACKET_TYPE_SCO_DATA
    {
        break;
    }
    case PHYBUSIF_PACKET_TYPE_EVT
    {
        break;
    }
    default:
    {
        BT_TRANSPORT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] packet_type is unvalid\n",__FILE__,__FUNCTION__,__LINE__);
        break;
    }
    }
#endif

    uint8_t *tx_buffer = bt_get_tx_buffer();
    bt_pbuf_copy_partial(p, tx_buffer, p->tot_len, 0);

    BT_TRANSPORT_TRACE_DEBUG("BT TX LEN:totol len:%d, len:%d\n",p->tot_len,len);
    //bt_hex_dump(tx_buffer,len+1);

#if BT_ENABLE_SNOOP > 0
    bt_snoop_write(packet_type,0,tx_buffer+1,len);
#endif

    uart_bt_send(tx_buffer,len+1);
}



err_t phybusif_input(struct phybusif_cb *cb)
{

    while(!ringbuffer_is_empty(&bt_ring_buf))
    {

        switch(cb->state)
        {
        case W4_PACKET_TYPE:
        {
            uint8_t packet_type = 0;

            ringbuffer_get(&bt_ring_buf,&packet_type,1);
            switch(packet_type)
            {
            case PHYBUSIF_PACKET_TYPE_EVT:
            {
                cb->state = W4_EVENT_HDR;
                break;
            }
            case PHYBUSIF_PACKET_TYPE_ACL_DATA:
            {
                cb->state = W4_ACL_HDR;
                break;
            }
            default:
                break;
            }
            break;
        }
        case W4_EVENT_HDR:
        {
            if(ringbuffer_len(&bt_ring_buf) < HCI_EVT_HDR_LEN)
            {
                BT_TRANSPORT_TRACE_WARNING("+++++++W4_EVENT_HDR left %d<HCI_EVENT_HDR_LEN\n",ringbuffer_len(&bt_ring_buf));
                return BT_ERR_BUF;
            }

            ringbuffer_get(&bt_ring_buf,(uint8_t *)cb->p->payload,HCI_EVT_HDR_LEN);
            cb->evhdr = cb->p->payload;
            if(cb->evhdr->len > PBUF_POOL_BUFSIZE)
            {
                bt_pbuf_free(cb->p);
                phybusif_reset(cb);
            }
            cb->state = W4_EVENT_PARAM;
            break;
        }
        case W4_ACL_HDR:
        {
            if(ringbuffer_len(&bt_ring_buf) < HCI_ACL_HDR_LEN)
            {
                BT_TRANSPORT_TRACE_WARNING("+++++++W4_ACL_HDR left %d<HCI_ACL_HDR_LEN\n",ringbuffer_len(&bt_ring_buf));

                return BT_ERR_BUF;
            }

            ringbuffer_get(&bt_ring_buf,(uint8_t *)cb->p->payload,HCI_ACL_HDR_LEN);
            cb->aclhdr = cb->p->payload;
            if(cb->aclhdr->len > PBUF_POOL_BUFSIZE)
            {
                bt_pbuf_free(cb->p);

                phybusif_reset(cb);
            }
            cb->state = W4_ACL_DATA;
            break;
        }
        case W4_EVENT_PARAM:
        {
            if(ringbuffer_len(&bt_ring_buf) < cb->evhdr->len)
            {
                BT_TRANSPORT_TRACE_WARNING("+++++++W4_EVENT_PARAM left %d< %d\n",ringbuffer_len(&bt_ring_buf),cb->evhdr->len);

                return BT_ERR_BUF;
            }
            ringbuffer_get(&bt_ring_buf,(uint8_t *)cb->p->payload+HCI_EVT_HDR_LEN,cb->evhdr->len);

#if BT_ENABLE_SNOOP > 0
            bt_snoop_write(BT_SNOOP_PACKET_TYPE_EVT,1,cb->p->payload,cb->evhdr->len+HCI_EVT_HDR_LEN);
#endif

            hci_event_input(cb->p);
            bt_pbuf_free(cb->p);
            phybusif_reset(cb);
            break;
        }
        case W4_ACL_DATA:
        {
            if(ringbuffer_len(&bt_ring_buf) < cb->aclhdr->len)
            {
                BT_TRANSPORT_TRACE_WARNING("+++++++W4_ACL_DATA left %d< %d\n",ringbuffer_len(&bt_ring_buf),cb->aclhdr->len);

                return BT_ERR_BUF;
            }
            ringbuffer_get(&bt_ring_buf,(uint8_t *)cb->p->payload+HCI_ACL_HDR_LEN,cb->aclhdr->len);

#if BT_ENABLE_SNOOP > 0
            bt_snoop_write(BT_SNOOP_PACKET_TYPE_ACL_DATA,1,cb->p->payload,cb->aclhdr->len+HCI_ACL_HDR_LEN);
#endif

            hci_acl_input(cb->p);
            bt_pbuf_free(cb->p);
            phybusif_reset(cb);
            break;
        }
        default:
            break;
        }

    }
    return BT_ERR_OK;
}
