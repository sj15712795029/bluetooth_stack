/******************************************************************************
  * @file           bt_a2dp_sink.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-11-09
  * @brief          bt a2dp sink header file
******************************************************************************/
#ifndef A2DP_SINK_H_H_H
#define A2DP_SINK_H_H_H

#include "bt_common.h"

#if PROFILE_A2DP_ENABLE


#include "bt_avdtp.h"

typedef enum{
    A2DP_SBC_48000 = 0x10,
    A2DP_SBC_44100 = 0x20,
    A2DP_SBC_32000 = 0x40,
    A2DP_SBC_16000 = 0x80,
} sbc_sampling_frequency_t;

typedef enum{
    A2DP_SBC_JOINT_STEREO  = 0x1,
    A2DP_SBC_STEREO        = 0x2,
    A2DP_SBC_DUAL_CHANNEL  = 0x4,
    A2DP_SBC_MONO          = 0x8
} sbc_channel_mode_t;

typedef enum{
    A2DP_SBC_BLOCK_LENGTH_16 = 0x10,
    A2DP_SBC_BLOCK_LENGTH_12 = 0x20,
    A2DP_SBC_BLOCK_LENGTH_8  = 0x40,
    A2DP_SBC_BLOCK_LENGTH_4  = 0x80,
} sbc_block_length_t;

typedef enum{
    A2DP_SBC_SUBBANDS_8 = 0x4,
    A2DP_SBC_SUBBANDS_4 = 0x8,
} sbc_subbands_t;

typedef enum{
    A2DP_SBC_ALLOCATION_METHOD_LOUDNESS = 1,
    A2DP_SBC_ALLOCATION_METHOD_SNR      = 2,
} sbc_allocation_method_t;

struct a2dp_pcb_t
{
    struct a2dp_pcb_t *next; /* For the linked list */
	struct avdtp_pcb_t *avdtppcb;
	struct bd_addr_t remote_addr;
	uint8_t codec_type;
	uint8_t media_codec_cap[16];
	uint16_t media_codec_cap_len;
};

typedef struct
{
    void (*a2dp_sink_signal_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*a2dp_sink_signal_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*a2dp_sink_stream_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*a2dp_sink_stream_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*a2dp_sink_stream_start)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*a2dp_sink_stream_realease)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*a2dp_sink_stream_suspend)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*a2dp_sink_stream_abort)(struct bd_addr_t *remote_addr,uint8_t value);
} a2dp_sink_cbs_t;


err_t a2dp_sink_init(a2dp_sink_cbs_t *cb);

#endif

#endif
