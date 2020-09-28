/******************************************************************************
  * @file    memory_manager.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-9-28
  * @brief   memory manager header file
******************************************************************************/
#ifndef MEMORY_MANAGER_H_H_H
#define MEMORY_MANAGER_H_H_H

#include "hw_misc.h"


#define SRAMBANK 	1

#define MEM_EXTERN_SRAM_INDEX 0
#define MEM_BLOCK_SIZE	32 /* Block size */
#define MEM_MAX_SIZE		960 *1024 /* Total memory size */
#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE /* Total block count */
		 
typedef struct
{
	void (*init)(uint8_t );	
	uint8_t (*perused)(uint8_t);
	uint8_t 	*membase[SRAMBANK];	
	uint16_t *memmap[SRAMBANK];
	uint8_t  memrdy[SRAMBANK];
}memory_manager_t;

void mm_meminit(uint8_t mem_index);
void mm_memset(void *data,uint8_t value,uint32_t count);
void mm_memcpy(void *des,void *src,uint32_t size);   
void *mm_malloc(uint8_t mem_index,uint32_t size);
void *mm_realloc(uint8_t mem_index,void *ptr,uint32_t size);
void mm_free(uint8_t mem_index,void *ptr);  
uint8_t mm_perused(uint8_t mem_index);

#endif


