/******************************************************************************
  * @file    bsp_24c020.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-12-22
  * @brief   eeprom 24c02 source file
******************************************************************************/
#include "hw_24c02.h"


static uint8_t _24c02_read_byte(uint16_t addr);
static void _24c02_write_byte(uint16_t addr,uint8_t data);

uint8_t hw_24c02_init(void)
{
	bsp_iic_init();

	return 0;
}


uint8_t hw_24c02_write(uint16_t addr,uint8_t *data,uint16_t data_len)
{
	uint16_t index;
	for(index=0;index<data_len;index++)
	{
		_24c02_write_byte(addr+index,data[index]);
	}	

	return 0;
}

uint8_t hw_24c02_read(uint16_t addr,uint8_t *data,uint16_t data_len)
{
	while(data_len)
	{
		*data++=_24c02_read_byte(addr++);	
		data_len--;
	}

	return 0;
}

static uint8_t _24c02_read_byte(uint16_t addr)
{				  
	uint8_t data=0;		  	    																 
    bsp_iic_start();
	bsp_iic_send_byte(0XA0+((addr/256)<<1)); 
	bsp_iic_wait_ack(); 
    bsp_iic_send_byte(addr%256);
	bsp_iic_wait_ack();	    
	bsp_iic_start();  	 	   
	bsp_iic_send_byte(0XA1);	   
	bsp_iic_wait_ack();	 
    data=bsp_iic_read_byte(0);		   
    bsp_iic_stop();   
	return data;
}

static void _24c02_write_byte(uint16_t addr,uint8_t data)
{				   	  	    																 
    bsp_iic_start();  
	bsp_iic_send_byte(0XA0+((addr/256)<<1));
	bsp_iic_wait_ack();	   
    bsp_iic_send_byte(addr%256);
	bsp_iic_wait_ack(); 	 										  		   
	bsp_iic_send_byte(data);						   
	bsp_iic_wait_ack();  		    	   
    bsp_iic_stop();
	hw_loop_wait(20000);	 
}

