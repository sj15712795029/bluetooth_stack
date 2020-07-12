
#ifndef __SBC_H__
#define __SBC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/* sampling frequency */
#define SBC_FREQ_16000		0x00
#define SBC_FREQ_32000		0x01
#define SBC_FREQ_44100		0x02
#define SBC_FREQ_48000		0x03

/* blocks */
#define SBC_BLK_4		0x00
#define SBC_BLK_8		0x01
#define SBC_BLK_12		0x02
#define SBC_BLK_16		0x03

/* channel mode */
#define SBC_MODE_MONO		0x00
#define SBC_MODE_DUAL_CHANNEL	0x01
#define SBC_MODE_STEREO		0x02
#define SBC_MODE_JOINT_STEREO	0x03

/* allocation method */
#define SBC_AM_LOUDNESS		0x00
#define SBC_AM_SNR		0x01

/* subbands */
#define SBC_SB_4		0x00
#define SBC_SB_8		0x01

/* Data endianness */
#define SBC_LE			0x00
#define SBC_BE			0x01
#if BTHOST_BIGENDIAN
#    define SBC_ENDIANESS   SBC_BE
#else
#    define SBC_ENDIANESS   SBC_LE
#endif
#define SBC_DECODER     1
#define SBC_ENCODER     0

#define SBC_FLAGS_ENCODER	0x00
#define SBC_FLAGS_DECODER	0x01

/* Error Message */
#define EIO		5
#define ENOMEM	12
#define ENOSPC	28

struct sbc_struct {
	unsigned long flags;

	uint8_t frequency;
	uint8_t blocks;
	uint8_t subbands;
	uint8_t mode;
	uint8_t allocation;
	uint8_t bitpool;
	//uint8_t endian;

	void *priv;
	void *priv_alloc_base;
};

typedef struct sbc_struct sbc_t;

int sbc_init(sbc_t *sbc, unsigned long flags);
int sbc_reinit(sbc_t *sbc, unsigned long flags);

int32_t sbc_parse(sbc_t *sbc, const void *input, uint32_t input_len);

/* Decodes ONE input block into ONE output block */
int32_t sbc_decode(sbc_t *sbc, const void *input, uint32_t input_len, void *output, uint32_t output_len, uint32_t *written);

/* Encodes ONE input block into ONE output block */
int32_t sbc_encode(sbc_t *sbc, const void *input, uint32_t input_len, void *output, uint32_t output_len, uint32_t *written);

/* Returns the output block size in bytes */
uint32_t sbc_get_frame_length(sbc_t *sbc);

/* Returns the time one input/output block takes to play in msec*/
unsigned sbc_get_frame_duration(sbc_t *sbc);

/* Returns the input block size in bytes */
uint32_t sbc_get_codesize(sbc_t *sbc);

//const char *sbc_get_implementation_info(sbc_t *sbc);
void sbc_finish(sbc_t *sbc);

#ifdef __cplusplus
}
#endif

#endif /* __SBC_H__ */
