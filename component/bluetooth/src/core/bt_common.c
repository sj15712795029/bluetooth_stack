/******************************************************************************
  * @file           bt_common.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-14
  * @brief          bt common source file
******************************************************************************/

#include "bt_common.h"


void bt_reverse_bytes(uint8_t *src, uint8_t *dst, int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[len - 1 - i] = src[i];
}


uint16_t bt_le_read_16(const uint8_t * buffer, int pos)
{
    return (uint16_t)(((uint16_t) buffer[pos]) | (((uint16_t)buffer[(pos)+1]) << 8));
}
uint32_t bt_le_read_24(const uint8_t * buffer, int pos)
{
    return ((uint32_t) buffer[pos]) | (((uint32_t)buffer[(pos)+1]) << 8) | (((uint32_t)buffer[(pos)+2]) << 16);
}
uint32_t bt_le_read_32(const uint8_t * buffer, int pos)
{
    return ((uint32_t) buffer[pos]) | (((uint32_t)buffer[(pos)+1]) << 8) | (((uint32_t)buffer[(pos)+2]) << 16) | (((uint32_t) buffer[(pos)+3]) << 24);
}

void bt_le_store_16(uint8_t *buffer, uint16_t pos, uint16_t value)
{
    buffer[pos++] = (uint8_t)value;
    buffer[pos++] = (uint8_t)(value >> 8);
}

void bt_le_store_24(uint8_t *buffer, uint16_t pos, uint32_t value)
{
    buffer[pos++] = (uint8_t)(value);
    buffer[pos++] = (uint8_t)(value >> 8);
    buffer[pos++] = (uint8_t)(value >> 16);
}

void bt_le_store_32(uint8_t *buffer, uint16_t pos, uint32_t value)
{
    buffer[pos++] = (uint8_t)(value);
    buffer[pos++] = (uint8_t)(value >> 8);
    buffer[pos++] = (uint8_t)(value >> 16);
    buffer[pos++] = (uint8_t)(value >> 24);
}

uint32_t bt_be_read_16( const uint8_t * buffer, int pos)
{
    return (uint16_t)(((uint16_t) buffer[(pos)+1]) | (((uint16_t)buffer[ pos]) << 8));
}

uint32_t bt_be_read_24( const uint8_t * buffer, int pos)
{
    return ( ((uint32_t)buffer[(pos)+2]) | (((uint32_t)buffer[(pos)+1]) << 8) | (((uint32_t) buffer[pos]) << 16));
}

uint32_t bt_be_read_32( const uint8_t * buffer, int pos)
{
    return ((uint32_t) buffer[(pos)+3]) | (((uint32_t)buffer[(pos)+2]) << 8) | (((uint32_t)buffer[(pos)+1]) << 16) | (((uint32_t) buffer[pos]) << 24);
}

void bt_be_store_16(uint8_t *buffer, uint16_t pos, uint16_t value)
{
    buffer[pos++] = (uint8_t)(value >> 8);
    buffer[pos++] = (uint8_t)(value);
}

void bt_be_store_24(uint8_t *buffer, uint16_t pos, uint32_t value)
{
    buffer[pos++] = (uint8_t)(value >> 16);
    buffer[pos++] = (uint8_t)(value >> 8);
    buffer[pos++] = (uint8_t)(value);
}

void bt_be_store_32(uint8_t *buffer, uint16_t pos, uint32_t value)
{
    buffer[pos++] = (uint8_t)(value >> 24);
    buffer[pos++] = (uint8_t)(value >> 16);
    buffer[pos++] = (uint8_t)(value >> 8);
    buffer[pos++] = (uint8_t)(value);
}

void bt_be_store_64(uint8_t *buffer,uint16_t pos,uint64_t value)
{
    buffer[pos++] = (uint8_t)(value >> 56);
    buffer[pos++] = (uint8_t)(value >> 48);
    buffer[pos++] = (uint8_t)(value >> 40);
    buffer[pos++] = (uint8_t)(value >> 32);
    buffer[pos++] = (uint8_t)(value >> 24);
    buffer[pos++] = (uint8_t)(value >> 16);
    buffer[pos++] = (uint8_t)(value >> 8);
    buffer[pos++] = (uint8_t)(value);
}

uint32_t bt_atoi_spec_size(const uint8_t *str,uint8_t size)
{
    uint32_t val = 0;
    uint8_t temp_size = size;
    while (temp_size--)
    {
        char chr = *str;
        if (!chr || chr < '0' || chr > '9')
        {
            str++;
            continue;
        }
        val = (val * 10) + (chr - '0');
        str++;
    }
    return val;
}

uint32_t bt_atoi(const uint8_t *str)
{
    uint32_t val = 0;
    while (1)
    {
        char chr = *str;
        if (!chr || chr < '0' || chr > '9')
            return val;
        val = (val * 10) + (chr - '0');
        str++;
    }
}

#define MAX_COL 16
#define SHOW_LINE_SIZE 16
void bt_hex_dump(uint8_t *data,uint32_t len)
{
    uint32_t line;
    uint32_t curline = 0;
    uint32_t curcol = 0;
    char showline[SHOW_LINE_SIZE];
    uint32_t data_pos = 0;

    if(len % MAX_COL)
    {
        line = len/MAX_COL+1;
    }
    else
    {
        line = len/MAX_COL;
    }

    for(curline = 0; curline < line; curline++)
    {
        sprintf(showline,"%08xh:",curline*MAX_COL);
        BT_HEX_TRACE_DEBUG("%s",showline);
        for(curcol = 0; curcol < MAX_COL; curcol++)
        {
            if(data_pos < len)
            {
                BT_HEX_TRACE_DEBUG("%02x ",data[data_pos]);
                data_pos++;
                continue;
            }
            else
            {
                break;
            }
        }
        BT_HEX_TRACE_DEBUG("\n");
    }
}

uint8_t* bt_hex_string(const void* buf, uint16_t len)
{
    static const char hex[] = "0123456789abcdef";
    static char str[129];
    const uint8_t* b = buf;
    uint16_t i;

    //      len = MIN(len, (sizeof(str) - 1) / 2);
    if (len > (((sizeof(str) - 1)) / 2))
    {
        len = (((sizeof(str) - 1)) / 2);
    }

    for (i = 0; i < len; i++)
    {
        str[i * 2] = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return ((uint8_t *)str);
}


void bt_addr_dump(uint8_t *addr)
{
    uint8_t addr_buff[32] = {0};
    sprintf((char*)addr_buff,"BT ADDR(%02x:%02x:%02x:%02x:%02x:%02x)\n",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
    BT_HEX_TRACE_DEBUG("%s",(char*)addr_buff);
}

void bt_uuid128_dump(uint8_t *uuid128)
{
    uint8_t uuid_buf[64] = {0};
    uint8_t *uuid_ptr = uuid_buf;

    for (int i = 0; i < 4; i++)
    {
        uuid_ptr += sprintf((char *)uuid_ptr, "%02x", uuid128[i]);
    }
    uuid_ptr += sprintf((char *)uuid_ptr, "-");
    for (int i = 4; i < 6; i++)
    {
        uuid_ptr += sprintf((char *)uuid_ptr, "%02x", uuid128[i]);
    }
    uuid_ptr += sprintf((char *)uuid_ptr, "-");
    for (int i = 6; i < 8; i++)
    {
        uuid_ptr += sprintf((char *)uuid_ptr, "%02x", uuid128[i]);
    }
    uuid_ptr += sprintf((char *)uuid_ptr, "-");
    for (int i = 8; i < 10; i++)
    {
        uuid_ptr += sprintf((char *)uuid_ptr, "%02x", uuid128[i]);
    }
    uuid_ptr += sprintf((char *)uuid_ptr, "-");
    for (int i = 10; i < 16; i++)
    {
        uuid_ptr += sprintf((char *)uuid_ptr, "%02x", uuid128[i]);
    }

    BT_HEX_TRACE_DEBUG("UUID128(%s)\n",(char*)uuid_buf);
}

void bt_memcpy_swap(void *dst, const void *src, uint16_t length)
{
	uint8_t *pdst = (uint8_t *)dst;
	const uint8_t *psrc = (const uint8_t *)src;

	psrc += length - 1;

	for (; length > 0; length--) {
		*pdst++ = *psrc--;
	}
}

void bt_mem_swap(void *buf, uint16_t length)
{
	uint16_t i;

	for (i = 0; i < (length/2); i++) {
		uint8_t tmp = ((uint8_t *)buf)[i];

		((uint8_t *)buf)[i] = ((uint8_t *)buf)[length - 1 - i];
		((uint8_t *)buf)[length - 1 - i] = tmp;
	}
}


uint8_t bt_parse_cod(uint8_t cod[3],uint16_t *device_service,uint16_t * device_major,uint16_t *device_minor)
{
    uint8_t device_type = BT_COD_TYPE_UNKNOW;
    BT_COD_SERVICE_CLASS(*device_service,cod);
    BT_COD_MAJOR_CLASS(*device_major,cod);
    BT_COD_MINOR_CLASS(*device_minor,cod);

    if(*device_major == BT_COD_MAJOR_AUDIO)
    {
        switch(*device_minor)
        {
        case BT_COD_MINOR_CONFM_HEADSET:
            device_type = BT_COD_TYPE_HEADSET;
            break;
        default:
            break;
        }
    }

    return device_type;

}





