#ifndef	RINGBUFFER_H_H_H
#define	RINGBUFFER_H_H_H

#include <stdint.h>



struct ringbuffer {
	void *data;
	unsigned int size;
	unsigned int read_pos;
	unsigned int write_pos;
};

void ringbuffer_init(struct ringbuffer *ring_buf,uint8_t *buf,uint32_t buf_size);
void ringbuffer_reset(struct ringbuffer *ring_buf);
void ringbuffer_print(struct ringbuffer *ring_buf);
uint32_t ringbuffer_put(struct ringbuffer *ring_buf,uint8_t *buffer, uint32_t len);
uint32_t ringbuffer_get(struct ringbuffer *ring_buf,uint8_t *buffer, uint32_t len);
uint32_t ringbuffer_get_read_pos(struct ringbuffer *ring_buf);
uint32_t ringbuffer_get_write_pos(struct ringbuffer *ring_buf);
uint32_t ringbuffer_is_empty(struct ringbuffer *ring_buf);
uint32_t ringbuffer_is_full(struct ringbuffer *ring_buf);
uint32_t ringbuffer_len(struct ringbuffer *ring_buf);
uint32_t ringbuffer_space_left(struct ringbuffer *ring_buf);

#endif
