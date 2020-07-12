#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ringbuffer.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
uint32_t is_power_of_2(uint32_t n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

void ringbuffer_init(struct ringbuffer *ring_buf,uint8_t *buf,uint32_t buf_size)
{
	if(!is_power_of_2(buf_size))
		return;
    ring_buf->data = buf;
    ring_buf->size = buf_size;
    ring_buf->read_pos = 0;
    ring_buf->write_pos = 0;
}

/**
 * ringbuffer_put - puts some data into the ringbuffer, no locking version
 * @buffer: the data to be added.
 * @len: the length of the data to be added.
 *
 * This function copies at most @len bytes from the @buffer into
 * the ringbuffer depending on the free space, and returns the number of
 * bytes copied.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */

uint32_t ringbuffer_put(struct ringbuffer *ring_buf,uint8_t *buffer, uint32_t len)
{
    uint32_t l;

    len = min(len, ring_buf->size - ring_buf->write_pos + ring_buf->read_pos);

    /* first put the data starting from write_pos to buffer end */
    l = min(len, ring_buf->size - (ring_buf->write_pos & (ring_buf->size - 1)));
    memcpy((uint8_t *)(ring_buf->data) + (ring_buf->write_pos & (ring_buf->size - 1)), buffer, l);

    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(ring_buf->data, buffer + l, len - l);

    ring_buf->write_pos += len;
    return len;
}

/**
 *  ringbuffer_print - print ringbuf data
 *  @ring_buf: the ringbuffer to be used.
 *  @cnt : the number byte to be print.
 */
void ringbuffer_print(struct ringbuffer *ring_buf)
{
	printf("RP:%d,WP:%d\n",ring_buf->read_pos,ring_buf->write_pos);
    return;
}


/**
 *  ringbuffer_get - gets some data from the ringbuffer, no locking version
 *  @ring_buf: the ringbuffer to be used.
 *  @buffer: where the data must be copied.
 *  @len: the size of the destination buffer.
 *
 *  This function copies at most @len bytes from the ringbuffer into the
 *  @buffer and returns the number of copied bytes.
 *
 *  Note that with only one concurrent reader and one concurrent
 *  writer, you don't need extra locking to use these functions.
 */
uint32_t ringbuffer_get(struct ringbuffer *ring_buf,uint8_t *buffer, uint32_t len)
{
    uint32_t l;

    len = min(len, ring_buf->write_pos - ring_buf->read_pos);

    /* first get the data from ring_buf->read_pos until the end of the buffer */
    l = min(len, ring_buf->size - (ring_buf->read_pos & (ring_buf->size - 1)));
    memcpy(buffer, (uint8_t *)(ring_buf->data) + (ring_buf->read_pos & (ring_buf->size - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, ring_buf->data, len - l);

    ring_buf->read_pos += len;
    return len;
}


uint32_t ringbuffer_get_read_pos(struct ringbuffer *ring_buf)
{
    return (ring_buf->read_pos/* & (ring_buf->size - 1)*/);
}
uint32_t ringbuffer_get_write_pos(struct ringbuffer *ring_buf)
{
    return (ring_buf->write_pos/* & (ring_buf->size - 1)*/);
}


/**
 * ringbuffer_reset - removes the entire ringbuffer content
 * @ring_buf: address of the ringbuffer to be used
 *
 * Note: usage of ringbuffer_reset() is dangerous. It should be only called when the
 * ringbuffer is exclusived locked or when it is secured that no other thread is
 * accessing the fifo.
 */
void ringbuffer_reset(struct ringbuffer *ring_buf)
{
    ring_buf->write_pos = ring_buf->read_pos = 0;
}

uint32_t ringbuffer_is_empty(struct ringbuffer *ring_buf)
{
    return ring_buf->read_pos == ring_buf->write_pos;
}

uint32_t ringbuffer_is_full(struct ringbuffer *ring_buf)
{
    return ring_buf->size == ring_buf->write_pos -  ring_buf->read_pos;
}

uint32_t ringbuffer_len(struct ringbuffer *ring_buf)
{
    return ring_buf->write_pos -  ring_buf->read_pos;
}

uint32_t ringbuffer_space_left(struct ringbuffer *ring_buf)
{
    return ring_buf->size - ring_buf->write_pos + ring_buf->read_pos;
}


