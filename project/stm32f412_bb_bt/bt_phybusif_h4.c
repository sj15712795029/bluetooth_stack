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
#include "board_wrapper.h"
#include "ringbuffer.h"
struct phybusif_cb uart_if;
UART_HandleTypeDef huart2;



#define BT_DMA_BUF_SIZE	(2*1024)
#define BT_RX_BUF_SIZE (4*1024)
struct ringbuffer bt_ring_buf;
uint8_t bt_rx_buf[BT_RX_BUF_SIZE];
uint8_t bt_dma_rx_buf[BT_DMA_BUF_SIZE];

#define BT_TX_BUF_SIZE	1024
uint8_t bt_tx_buff[BT_TX_BUF_SIZE];

uint8_t* bt_get_tx_buffer()
{
    return bt_tx_buff;
}

DMA_InitTypeDef DMA_UART2;
/******************************************************************************
 * func name   : hw_uart_bt_init
 * para        : baud_rate(IN)  --> Baud rate of uart1
 * return      : hw_uart_bt_init result
 * description : Initialization of USART2.PA0->CTS PA1->RTS PA2->TX PA3->RX
******************************************************************************/
uint8_t hw_uart_bt_init(uint32_t baud_rate)
{
	__HAL_RCC_DMA1_CLK_ENABLE();

	huart2.Instance = USART2;
  huart2.Init.BaudRate = baud_rate;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    return HW_HAL_EXCU_ERR;
  }

	HAL_UART_Receive_DMA(&huart2, bt_dma_rx_buf, BT_DMA_BUF_SIZE);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);

    return HW_ERR_OK;

}


/******************************************************************************
 * func name   : uart_bt_send
 * para        : buf(IN)  --> Buffer to send
                 len(IN)  --> The buffer length to be sent
 * return      : NULL
 * description : The send function of USART2
******************************************************************************/
void uart_bt_send(uint8_t *buf,uint16_t len)
{
    uint16_t index;
    for(index = 0; index < len ; index++)
    {
        /* Wait until the last send is complete, then send the data */
		while ((USART2->SR & USART_SR_TXE) == 0);
        USART2->DR = buf[index];
    }
}

/******************************************************************************
 * func name   : USART2_IRQHandler
 * para        : NULL
 * return      : NULL
 * description : Interrupt handler for usart2
******************************************************************************/
void USART2_IRQHandler(void)
{

    uint32_t recv_len;
    if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) != RESET))
    {

        /* Clear the interrupt and reset DMA */
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        HAL_UART_DMAStop(&huart2);
		
        recv_len = BT_DMA_BUF_SIZE - huart2.hdmarx->Instance->NDTR;

        if(recv_len > 500)
        {
            printf("------ %d ----\n",recv_len);
        }

        if(recv_len > 0)
        {
            if(ringbuffer_space_left(&bt_ring_buf) < recv_len)
            {
                HW_DEBUG("+++++++++++++++ring buffer is full,recv_len %d,left %d\n",recv_len,ringbuffer_space_left(&bt_ring_buf));
            }
            else
            {
                ringbuffer_put(&bt_ring_buf,bt_dma_rx_buf,recv_len);
            }
            HAL_UART_Receive_DMA(&huart2,bt_dma_rx_buf,BT_DMA_BUF_SIZE);
        }
    }

	HAL_UART_IRQHandler(&huart2);
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

void phybusif_open(uint32_t baud_rate)
{
    ringbuffer_init(&bt_ring_buf,bt_rx_buf,BT_RX_BUF_SIZE);
    hw_uart_bt_init(baud_rate);
}

void phybusif_reopen(uint32_t baud_rate)
{

	ringbuffer_reset(&bt_ring_buf);

	phybusif_open(baud_rate);
	
}

void phybusif_close()
{
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
    bt_hex_dump(tx_buffer,len+1);

    uart_bt_send(tx_buffer,len+1);
}



err_t phybusif_input(struct phybusif_cb *cb)
{

    while(!ringbuffer_is_empty(&bt_ring_buf))
    {
        //printf("state %d\n",cb->state);

        switch(cb->state)
        {
        case W4_PACKET_TYPE:
        {
            uint8_t packet_type = 0;

            ringbuffer_get(&bt_ring_buf,&packet_type,1);
            //printf("+++++++++++++packet_type 0x%x\n",packet_type);
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
                return BT_ERR_BUF;
            }

            ringbuffer_get(&bt_ring_buf,(uint8_t *)cb->p->payload,HCI_EVT_HDR_LEN);
            cb->evhdr = cb->p->payload;
            if(cb->evhdr->len > PBUF_POOL_BUFSIZE)
            {
                bt_pbuf_free(cb->p);
                printf("!!!!!!!!!left %d\n",ringbuffer_space_left(&bt_ring_buf));
                printf("!!!!!!!!!!!wp %d,rp %d\n",ringbuffer_get_write_pos(&bt_ring_buf),ringbuffer_get_read_pos(&bt_ring_buf));
                phybusif_reset(cb);
            }
            cb->state = W4_EVENT_PARAM;
            break;
        }
        case W4_ACL_HDR:
        {
            if(ringbuffer_space_left(&bt_ring_buf) < HCI_ACL_HDR_LEN)
            {
                return BT_ERR_BUF;
            }

            ringbuffer_get(&bt_ring_buf,(uint8_t *)cb->p->payload,HCI_ACL_HDR_LEN);
            cb->aclhdr = cb->p->payload;
            if(cb->aclhdr->len > PBUF_POOL_BUFSIZE)
            {
                bt_pbuf_free(cb->p);

                printf("!!!!!!!!!left %d\n",ringbuffer_space_left(&bt_ring_buf));
                printf("!!!!!!!!!!!wp %d,rp %d\n",ringbuffer_get_write_pos(&bt_ring_buf),ringbuffer_get_read_pos(&bt_ring_buf));
                phybusif_reset(cb);
            }
            cb->state = W4_ACL_DATA;
            break;
        }
        case W4_EVENT_PARAM:
        {
            if(ringbuffer_space_left(&bt_ring_buf) < cb->evhdr->len)
            {
                printf("+++++++W4_EVENT_PARAM left %d,event size %d\n",ringbuffer_space_left(&bt_ring_buf),cb->evhdr->len);
                return BT_ERR_BUF;
            }
            ringbuffer_get(&bt_ring_buf,(uint8_t *)cb->p->payload+HCI_EVT_HDR_LEN,cb->evhdr->len);
            hci_event_input(cb->p);
            bt_pbuf_free(cb->p);
            phybusif_reset(cb);
            break;
        }
        case W4_ACL_DATA:
        {
            if(ringbuffer_space_left(&bt_ring_buf) < cb->aclhdr->len)
            {
                printf("+++++++W4_ACL_DATA left %d,acl size %d\n",ringbuffer_space_left(&bt_ring_buf),cb->aclhdr->len);
                return BT_ERR_BUF;
            }
            ringbuffer_get(&bt_ring_buf,(uint8_t *)cb->p->payload+HCI_ACL_HDR_LEN,cb->aclhdr->len);
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
