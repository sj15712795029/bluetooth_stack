/******************************************************************************
  * @file    hw_usart.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2019-3-12
  * @brief   usart source file
******************************************************************************/

#include "hw_uart.h"
#include "ringbuffer.h"


#define UART1_MAX_REV	256
uint8_t uart1_rev_buffer[UART1_MAX_REV];
uint8_t uart1_rev_len;

extern uint8_t shell_parse(uint8_t *shell_string);

/******************************************************************************
 * func name   : fputc
 * para        : ch(IN)  --> output char
                 fp(IN)  --> file point
 * return      : return output char
 * description : The fputc () function redirects the fputc called by printf
                 For subsequent calls to printf,
                 you can use the PC terminal output from USART1 directly
******************************************************************************/



int fputc(int ch, FILE *fp)
{
    if (fp == stdout)
    {
        /*If you want to output a line feed, you can automatically add a carriage
        return to prevent the horizontal position of some terminal terminals
        from changing after line feed.*/
        if (ch == '\n')
        {
            while ((USART2->SR & USART_SR_TXE) == 0);
            USART2->DR = '\r';
        }
        /* Output characters to USART1 */
        while ((USART2->SR & USART_SR_TXE) == 0);
        USART2->DR = ch;
    }
    return ch;
}


DMA_InitTypeDef DMA_UART_DEBUG;
/******************************************************************************
 * func name   : hw_uart_debug_init
 * para        : baud_rate(IN)  --> Baud rate of uart1
 * return      : hw_uart_debug_init result
 * description : Initialization of USART1.PA9->TX PA10->RX
******************************************************************************/
uint8_t hw_uart_debug_init(uint32_t baud_rate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Enable USART interrupts, mainly for idle interrupts */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=DEBUG_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEBUG_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Initializes USART1 to enable USART, USART idle interrupts and USART RX DMA */
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
    USART_Cmd(USART2, ENABLE);

    /* Initializes DMA and enables it */
    DMA_DeInit(DMA1_Stream5);
    DMA_UART_DEBUG.DMA_Channel = DMA_Channel_4;
    DMA_UART_DEBUG.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_UART_DEBUG.DMA_Memory0BaseAddr = (uint32_t)uart1_rev_buffer;
    DMA_UART_DEBUG.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_UART_DEBUG.DMA_BufferSize = UART1_MAX_REV;
    DMA_UART_DEBUG.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_UART_DEBUG.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_UART_DEBUG.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_UART_DEBUG.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_UART_DEBUG.DMA_Mode = DMA_Mode_Normal;
    DMA_UART_DEBUG.DMA_Priority = DMA_Priority_Medium;
    DMA_UART_DEBUG.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_UART_DEBUG.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_UART_DEBUG.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_UART_DEBUG.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_UART_DEBUG);

    DMA_Cmd(DMA1_Stream5, ENABLE);

    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : uart1_dma_enable
 * para        : DMA_CHx(IN)  --> DMA channel
 * return      : NULL
 * description : reset DMA
******************************************************************************/
void uart2_dma_enable(DMA_Stream_TypeDef*DMA_STREAMx)
{
    DMA_Cmd(DMA_STREAMx, DISABLE );
    DMA_DeInit(DMA_STREAMx);
    DMA_Init(DMA_STREAMx, &DMA_UART_DEBUG);
    DMA_Cmd(DMA_STREAMx, ENABLE);
}

/******************************************************************************
 * func name   : USART1_IRQHandler
 * para        : NULL
 * return      : NULL
 * description : Interrupt handler for usart1
******************************************************************************/
void USART2_IRQHandler(void)
{

    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        /* Without this, the interrupt cannot be cleared and continues into the interrupt */
        USART_ReceiveData(USART2);
        uart1_rev_len =UART1_MAX_REV-DMA_GetCurrDataCounter(DMA1_Stream5);

        if(uart1_rev_len != 0)
        {
            /* Call the parse function */
            shell_parse(uart1_rev_buffer);
            hw_memset(uart1_rev_buffer,0,sizeof(uart1_rev_buffer));
        }
        /* Clear the interrupt and reset DMA */
        USART_ClearITPendingBit(USART2,USART_IT_IDLE);
        uart2_dma_enable(DMA1_Stream5);
    }
}

#define BT_DMA_BUF_SIZE	(4*1024)
#define BT_RX_BUF_SIZE (12*1024)
struct ringbuffer bt_ring_buf;
uint8_t bt_rx_buf[BT_RX_BUF_SIZE];
uint8_t bt_dma_rx_buf[BT_DMA_BUF_SIZE];

#define BT_TX_BUF_SIZE	1024
uint8_t bt_tx_buff[BT_TX_BUF_SIZE];

uint8_t* bt_get_tx_buffer()
{
    return bt_tx_buff;
}

DMA_InitTypeDef DMA_UART_BT;
/******************************************************************************
 * func name   : hw_uart_bt_init
 * para        : baud_rate(IN)  --> Baud rate of uart1
 * return      : hw_uart_bt_init result
 * description : Initialization of USART2.PA0->CTS PA1->RTS PA2->TX PA3->RX
******************************************************************************/
uint8_t hw_uart_bt_init(uint32_t baud_rate,uint8_t reconfig)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if(reconfig == 0)
        ringbuffer_init(&bt_ring_buf,bt_rx_buf,BT_RX_BUF_SIZE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_USART3);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11|GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Enable USART interrupts, mainly for idle interrupts */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=BT_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = BT_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Initializes USART1 to enable USART, USART idle interrupts and USART RX DMA */
    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
    USART_Cmd(USART3, ENABLE);

    /* Initializes DMA and enables it */
    DMA_DeInit(DMA1_Stream1);
    DMA_UART_BT.DMA_Channel = DMA_Channel_4;
    DMA_UART_BT.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
    DMA_UART_BT.DMA_Memory0BaseAddr = (uint32_t)bt_dma_rx_buf;
    DMA_UART_BT.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_UART_BT.DMA_BufferSize = BT_DMA_BUF_SIZE;
    DMA_UART_BT.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_UART_BT.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_UART_BT.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_UART_BT.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_UART_BT.DMA_Mode = DMA_Mode_Normal;
    DMA_UART_BT.DMA_Priority = DMA_Priority_Medium;
    DMA_UART_BT.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_UART_BT.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_UART_BT.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_UART_BT.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream1, &DMA_UART_BT);

    DMA_Cmd(DMA1_Stream1, ENABLE);

    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : uart2_dma_enable
 * para        : DMA_CHx(IN)  --> DMA channel
 * return      : NULL
 * description : reset DMA
******************************************************************************/
void uart3_dma_enable(DMA_Stream_TypeDef*DMA_STREAMx)
{
    DMA_Cmd(DMA_STREAMx, DISABLE );
    DMA_DeInit(DMA_STREAMx);
    DMA_Init(DMA_STREAMx, &DMA_UART_BT);
    DMA_Cmd(DMA_STREAMx, ENABLE);
}

void uart_bt_send(uint8_t *buf,uint8_t len)
{
    uint8_t index;
    for(index = 0; index < len ; index++)
    {
        /* Wait until the last send is complete, then send the data */
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
        USART_SendData(USART3,buf[index]);
    }
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}

/******************************************************************************
 * func name   : USART3_IRQHandler
 * para        : NULL
 * return      : NULL
 * description : Interrupt handler for usart2
******************************************************************************/
void USART3_IRQHandler(void)
{
    uint32_t recv_len;
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        /* Without this, the interrupt cannot be cleared and continues into the interrupt */
        USART_ReceiveData(USART3);

        /* Clear the interrupt and reset DMA */
        USART_ClearITPendingBit(USART3,USART_IT_IDLE);
        recv_len = BT_DMA_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Stream1);


        if(recv_len > 0)
        {
#if 1
            if(ringbuffer_space_left(&bt_ring_buf) < recv_len)
            {
                HW_DEBUG("+++++++++++++++ring buffer is full,recv_len %d,left %d\n",recv_len,ringbuffer_space_left(&bt_ring_buf));
            }
            else
            {
                ringbuffer_put(&bt_ring_buf,bt_dma_rx_buf,recv_len);
            }
            uart3_dma_enable(DMA1_Stream1);
#else
	static uint32_t total_len = 0;
	static uint32_t total_err = 0;

	total_len += recv_len;

        if(recv_len % 640)
        {
            total_err++;
            printf("++++++++++len %d \n",recv_len);
			//hw_hex_dump(bt_dma_rx_buf,recv_len);
        }

        printf("len %d total len %d,err %d\n",recv_len,total_len,total_err);
        uart3_dma_enable(DMA1_Stream1);
#endif
        }

    }
}

