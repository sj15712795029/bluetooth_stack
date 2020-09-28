/******************************************************************************
  * @file    memory_manager.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-9-28
  * @brief   memory manager source file
******************************************************************************/

#include "memory_manager.h"

static uint32_t _mm_malloc(uint8_t mem_index,uint32_t size);
static uint8_t _mm_free(uint8_t mem_index,uint32_t offset);

__align(32) uint8_t mem_base[MEM_MAX_SIZE] __attribute__((at(0X68000000)));
uint16_t mem_mapbase[MEM_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+MEM_MAX_SIZE)));
const uint32_t memtblsize[SRAMBANK]= {MEM_ALLOC_TABLE_SIZE};
const uint32_t memblksize[SRAMBANK]= {MEM_BLOCK_SIZE};
const uint32_t memsize[SRAMBANK]= {MEM_MAX_SIZE};


memory_manager_t memory_manager=
{
    mm_meminit,
    mm_perused,
    mem_base,
    mem_mapbase,
    0,
};

void mm_memset(void *data,uint8_t value,uint32_t count)
{
    uint8_t *temp_data = data;
    while(count--)*temp_data++=value;
}

void mm_memcpy(void *des,void *src,uint32_t size)
{
    uint8_t *xdes=des;
    uint8_t *xsrc=src;
    while(size--)*xdes++=*xsrc++;
}

void mm_meminit(uint8_t mem_index)
{
    mm_memset(memory_manager.memmap[mem_index], 0,memtblsize[mem_index]*2);
    mm_memset(memory_manager.membase[mem_index], 0,memsize[mem_index]);
    memory_manager.memrdy[mem_index]=1;
}

uint8_t mm_perused(uint8_t mem_index)
{
    uint32_t used=0;
    uint32_t i;
    for(i=0; i<memtblsize[mem_index]; i++)
    {
        if(memory_manager.memmap[mem_index][i])used++;
    }
    return (used*100)/(memtblsize[mem_index]);
}


static uint32_t _mm_malloc(uint8_t mem_index,uint32_t size)
{
    signed long offset=0;
    uint32_t nmemb;
    uint32_t cmemb=0;
    uint32_t i;
    if(!memory_manager.memrdy[mem_index])memory_manager.init(mem_index);
    if(size==0)return 0XFFFFFFFF;
    nmemb=size/memblksize[mem_index];
    if(size%memblksize[mem_index])nmemb++;
    for(offset=memtblsize[mem_index]-1; offset>=0; offset--)
    {
        if(!memory_manager.memmap[mem_index][offset])cmemb++;
        else cmemb=0;
        if(cmemb==nmemb)
        {
            for(i=0; i<nmemb; i++)
            {
                memory_manager.memmap[mem_index][offset+i]=nmemb;
            }
            return (offset*memblksize[mem_index]);
        }
    }
    return 0XFFFFFFFF;
}


static uint8_t _mm_free(uint8_t mem_index,uint32_t offset)
{
    int i;
    if(!memory_manager.memrdy[mem_index])
    {
        memory_manager.init(mem_index);
        return 1;
    }
    if(offset<memsize[mem_index])
    {
        int index=offset/memblksize[mem_index];
        int nmemb=memory_manager.memmap[mem_index][index];
        for(i=0; i<nmemb; i++)
        {
            memory_manager.memmap[mem_index][index+i]=0;
        }
        return 0;
    }
    else return 2;
}

void *mm_malloc(uint8_t mem_index,uint32_t size)
{
    uint32_t offset;
    offset=_mm_malloc(mem_index,size);
    if(offset==0XFFFFFFFF)return NULL;
    else return (void*)((uint32_t)memory_manager.membase[mem_index]+offset);
}

void mm_free(uint8_t mem_index,void *ptr)
{
    uint32_t offset;
    if(ptr==NULL)return;
    offset=(uint32_t)ptr-(uint32_t)memory_manager.membase[mem_index];
    _mm_free(mem_index,offset);
}

void *mm_realloc(uint8_t mem_index,void *ptr,uint32_t size)
{
    uint32_t offset;
    offset=_mm_malloc(mem_index,size);
    if(offset==0XFFFFFFFF)return NULL;
    else
    {
        mm_memcpy((void*)((uint32_t)memory_manager.membase[mem_index]+offset),ptr,size);
        mm_free(mem_index,ptr);
        return (void*)((uint32_t)memory_manager.membase[mem_index]+offset);
    }
}


