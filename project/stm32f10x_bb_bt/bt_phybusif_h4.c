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
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    /* Enable USART2,GPIOA,DMA1 RCC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    USART_DeInit(USART2);
    USART_Cmd(USART2, DISABLE);
    /* Initialize the GPIOA0,GPIOA1,GPIOA2,GPIOA3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Data format :1:8:1, no parity check, hardware flow control */
    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    /* Enable USART interrupts, mainly for idle interrupts */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=BT_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = BT_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Initializes USART2 to enable USART,USART idle interrupts, and USART RX DMA */
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
    USART_Cmd(USART2, ENABLE);

    /* Initializes DMA and enables it */
    hw_memset(&DMA_UART2,0,sizeof(DMA_InitTypeDef));
    DMA_DeInit(DMA1_Channel6);
    DMA_UART2.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_UART2.DMA_MemoryBaseAddr = (uint32_t)bt_dma_rx_buf;
    DMA_UART2.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_UART2.DMA_BufferSize = BT_DMA_BUF_SIZE;
    DMA_UART2.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_UART2.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_UART2.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_UART2.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_UART2.DMA_Mode = DMA_Mode_Normal;
    DMA_UART2.DMA_Priority = DMA_Priority_Medium;
    DMA_UART2.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_UART2);

    DMA_Cmd(DMA1_Channel6, ENABLE);

    return BT_ERR_OK;

}

/******************************************************************************
 * func name   : uart2_dma_enable
 * para        : DMA_CHx(IN)  --> DMA channel
 * return      : NULL
 * description : reset DMA
******************************************************************************/
void uart2_dma_enable(DMA_Channel_TypeDef*DMA_CHx)
{
    DMA_Cmd(DMA_CHx, DISABLE );
    DMA_UART2.DMA_MemoryBaseAddr = (uint32_t)bt_dma_rx_buf;
    DMA_UART2.DMA_BufferSize = BT_DMA_BUF_SIZE;
    DMA_Init(DMA1_Channel6, &DMA_UART2);
    DMA_Cmd(DMA_CHx, ENABLE);
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
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        USART_SendData(USART2,buf[index]);
    }
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
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
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        /* Without this, the interrupt cannot be cleared and continues into the interrupt */
        USART_ReceiveData(USART2);

        /* Clear the interrupt and reset DMA */
        USART_ClearITPendingBit(USART2,USART_IT_IDLE);
        recv_len = BT_DMA_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);

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
            uart2_dma_enable(DMA1_Channel6);
        }
    }
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
	USART_Cmd(USART2, DISABLE);
	DMA_DeInit(DMA1_Channel6);
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
			//printf("W4_EVENT_PARAM %d\n",cb->evhdr->len);
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
