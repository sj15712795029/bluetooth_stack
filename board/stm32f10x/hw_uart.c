/******************************************************************************
  * @file    hw_usart.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-6-26
  * @brief   usart source file
******************************************************************************/

#include "hw_uart.h"


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
            while ((USART1->SR & USART_SR_TXE) == 0);
            USART1->DR = '\r';
        }
        /* Output characters to USART1 */
        while ((USART1->SR & USART_SR_TXE) == 0);
        USART1->DR = ch;
    }
    return ch;
}

/******************************************************************************
 * func name   : uart1_send
 * para        : buf(IN)  --> Buffer to send
                 len(IN)  --> The buffer length to be sent
 * return      : NULL
 * description : The send function of USART1
******************************************************************************/
void uart1_send(uint8_t *buf,uint8_t len)
{
    uint8_t index;
    for(index = 0; index < len ; index++)
    {
        /* wait transport complete */
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1,buf[index]);
    }
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}


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
    DMA_InitTypeDef DMA_InitStructure;

    /* Enable RCC clock for USART1,GPIOA,DMA1 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* Initialization GPIOA9 GPIOA10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Data format :1:8:1, no parity check, no hardware flow control */
    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    /* Enable USART interrupts, mainly for idle interrupts */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=DEBUG_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEBUG_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Initializes USART1 to enable USART, USART idle interrupts and USART RX DMA */
    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
    USART_Cmd(USART1, ENABLE);

    /* Initializes DMA and enables it */
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart1_rev_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = UART1_MAX_REV;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel5, ENABLE);

    return HW_ERR_OK;

}


/******************************************************************************
 * func name   : uart1_dma_enable
 * para        : DMA_CHx(IN)  --> DMA channel
 * return      : NULL
 * description : reset DMA
******************************************************************************/
void uart1_dma_enable(DMA_Channel_TypeDef*DMA_CHx)
{
    DMA_Cmd(DMA_CHx, DISABLE );
    DMA_SetCurrDataCounter(DMA_CHx,UART1_MAX_REV);
    DMA_Cmd(DMA_CHx, ENABLE);
}

/******************************************************************************
 * func name   : USART1_IRQHandler
 * para        : NULL
 * return      : NULL
 * description : Interrupt handler for usart1
******************************************************************************/
void USART1_IRQHandler(void)
{

    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        /* Without this, the interrupt cannot be cleared and continues into the interrupt */
        USART_ReceiveData(USART1);
        uart1_rev_len =UART1_MAX_REV-DMA_GetCurrDataCounter(DMA1_Channel5);

        if(uart1_rev_len != 0)
        {
            /* Call the parse function */
            shell_parse(uart1_rev_buffer);
            hw_memset(uart1_rev_buffer,0,sizeof(uart1_rev_buffer));
        }
        /* Clear the interrupt and reset DMA */
        USART_ClearITPendingBit(USART1,USART_IT_IDLE);
        uart1_dma_enable(DMA1_Channel5);
    }
}


