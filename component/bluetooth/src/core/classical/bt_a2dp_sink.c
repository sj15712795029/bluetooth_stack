#include "bt_a2dp_sink.h"

#include "bt_common.h"
#include "sbc.h"
#include "bt_pbuf.h"
#include "bt_config.h"

sbc_t sbc_context;

struct avdtp_pcb_t *temp_a2dp_sink;

static  uint8_t a2dp_sink_service_record[] =
{
    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, 0x0, 0x0, /* Service record handle attribute */
    SDP_UINT32, 0x00, 0x01, 0x00, 0x01, /*dummy vals, filled in on xmit*/

    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, 0x0, 0x1, /* Service class ID list attribute */
    SDP_DES_SIZE8, 0x3,
    SDP_UUID16, 0x11,0x0b,

    SDP_DES_SIZE8, 0x15,
    SDP_UINT16, 0x0, 0x4, /* Protocol descriptor list attribute */
    SDP_DES_SIZE8, 0x10,
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x1, 0x0, /*L2CAP*/
    SDP_UINT16, 0x0, 0x19,/* AVDTP */
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x0, 0x19, /* AVDTP UUID */
    SDP_UINT16, 0x1, 0x03,/* AVDTP version */

    SDP_DES_SIZE8, 0xd,
    SDP_UINT16, 0x0, 0x9, /* profile descriptor List */
    SDP_DES_SIZE8, 0x8,
    SDP_DES_SIZE8,0x06,
    SDP_UUID16,0x11,0x0d,
    SDP_UINT16,0x01,0x03,

    SDP_DES_SIZE8, 0x6,
    SDP_UINT16, 0x3, 0x11, /* support feature 1:speaker */
    SDP_UINT16,0x00,0x01,

};

struct a2dp_pcb *a2dp_sink_active_pcbs;  /* List of all active A2DP PCBs */
struct a2dp_pcb *a2dp_sink_tmp_pcb;
uint8_t media_codec_cap[20];
uint16_t media_codec_cap_len;

static const uint8_t sbc_snk_codec_caps[] =
{
    AVDTP_AUDIO<<4,
               AVDTP_CODEC_SBC,

               AVDTP_SBC_48000|AVDTP_SBC_44100|AVDTP_SBC_32000|AVDTP_SBC_16000|\
               AVDTP_SBC_JOINT_STEREO|AVDTP_SBC_STEREO|AVDTP_SBC_DUAL_CHANNEL|AVDTP_SBC_MONO,

               AVDTP_SBC_BLOCK_LENGTH_16|AVDTP_SBC_BLOCK_LENGTH_12|AVDTP_SBC_BLOCK_LENGTH_8|AVDTP_SBC_BLOCK_LENGTH_4|\
               AVDTP_SBC_SUBBANDS_8|AVDTP_SBC_SUBBANDS_4|AVDTP_SBC_ALLOCATION_METHOD_LOUDNESS|AVDTP_SBC_ALLOCATION_METHOD_SNR,

               SBC_SNK_MIN_BITPOOL,
               SBC_SNK_MAX_BITPOOL,
};

static err_t a2dp_sinks_event_handle(struct avdtp_pcb_t *pcb,uint32_t msg_id,struct bt_pbuf_t *p)
{
		
    switch(msg_id)
    {
    case AVDTP_SI_DISCOVER:
        break;
    case AVDTP_SI_GET_CAPABILITIES:
        break;
    case AVDTP_SI_SET_CONFIGURATION:
		{

        uint8_t * media_codec_cap_temp = (uint8_t * )avdtp_get_spec_cap_value(AVDTP_MEDIA_CODEC,p->payload,p->len,&media_codec_cap_len);
        BT_A2DP_TRACE_DEBUG("-------------AVDTP_MEDIA_CODEC---len %d----\n",media_codec_cap_len);

        bt_hex_dump(media_codec_cap,media_codec_cap_len);
        memcpy(media_codec_cap,media_codec_cap_temp,media_codec_cap_len);

        break;
		}
    case AVDTP_SI_GET_CONFIGURATION:
        break;
    case AVDTP_SI_RECONFIGURE:
        break;
    case AVDTP_SI_OPEN:
        sbc_init(&sbc_context, SBC_FLAGS_DECODER);
        switch (media_codec_cap[2] & 0xF0)
        {
        case AVDTP_SBC_16000 :
            sbc_context.frequency = SBC_FREQ_16000;
            break;
        case AVDTP_SBC_32000 :
            sbc_context.frequency = SBC_FREQ_32000;
            break;
        case AVDTP_SBC_44100 :
            sbc_context.frequency= SBC_FREQ_44100;
            break;
        case AVDTP_SBC_48000 :
            sbc_context.frequency = SBC_FREQ_48000;
            break;
        }

        switch (media_codec_cap[2] & 0x0F)
        {
        case AVDTP_SBC_MONO :
            sbc_context.mode = SBC_MODE_MONO;
            break;
        case AVDTP_SBC_DUAL_CHANNEL :
            sbc_context.mode = SBC_MODE_DUAL_CHANNEL;
            break;
        case AVDTP_SBC_STEREO :
            sbc_context.mode = SBC_MODE_STEREO;
            break;
        case AVDTP_SBC_JOINT_STEREO :
            sbc_context.mode = SBC_MODE_JOINT_STEREO;
            break;
        }

        switch (media_codec_cap[3] & 0xF0)
        {
        case AVDTP_SBC_BLOCK_LENGTH_4 :
            sbc_context.blocks = SBC_BLK_4;
            break;
        case AVDTP_SBC_BLOCK_LENGTH_8 :
            sbc_context.blocks = SBC_BLK_8;
            break;
        case AVDTP_SBC_BLOCK_LENGTH_12 :
            sbc_context.blocks = SBC_BLK_12;
            break;
        case AVDTP_SBC_BLOCK_LENGTH_16 :
            sbc_context.blocks = SBC_BLK_16;
            break;
        }

        switch (media_codec_cap[3] & 0x0C)
        {
        case AVDTP_SBC_SUBBANDS_4 :
            sbc_context.subbands = SBC_SB_4;
            break;
        case AVDTP_SBC_SUBBANDS_8 :
            sbc_context.subbands = SBC_SB_8;
            break;
        }

        switch (media_codec_cap[3] & 0x03)
        {
        case AVDTP_SBC_ALLOCATION_METHOD_SNR :
            sbc_context.allocation = SBC_AM_SNR;
            break;
        case AVDTP_SBC_ALLOCATION_METHOD_LOUDNESS :
            sbc_context.allocation = SBC_AM_LOUDNESS;
            break;
        }

        sbc_context.bitpool = media_codec_cap[5];
        BT_A2DP_TRACE_DEBUG("a2dp_sinks_event_handle: AVDTP_SI_OPEN\n");

        break;
    case AVDTP_SI_START:
        break;
    case AVDTP_SI_CLOSE:
        break;
    case AVDTP_SI_SUSPEND:
        break;
    case AVDTP_SI_ABORT:
        break;
    case AVDTP_SI_SECURITY_CONTROL:
        break;
    case AVDTP_SI_GET_ALL_CAPABILITIES:
        break;
    case AVDTP_SI_DELAYREPORT:
        break;
    /* USER DEFINE */
    case AVDTP_SI_SIGNAL_CONNECT_IND:
        temp_a2dp_sink = pcb;
        BT_A2DP_TRACE_DEBUG("a2dp_sinks_event_handle: AVDTP_SI_SIGNAL_CONNECT_IND\n");

        break;
    case AVDTP_SI_SIGNAL_CONNECT_CFM:
        break;
    case AVDTP_SI_SIGNAL_DISCON_IND:
        break;
    case AVDTP_SI_SIGNAL_DISCON_CFM:
        break;
    case AVDTP_SI_STREAM_CONNECT_IND:
        break;
    case AVDTP_SI_STREAM_CONNECT_CFM:
        break;
    case AVDTP_SI_STREAM_DISCON_IND:
        break;
    case AVDTP_SI_STREAM_DISCON_CFM:
        break;
    default:
        break;
    }
		
		return BT_ERR_OK;
}

uint32_t pcm_data_len;
uint8_t pcm_data[1024];

static err_t a2dp_sinks_media_handle(struct avdtp_pcb_t *pcb,struct bt_pbuf_t *p)
{
    uint8_t frame_index = 0;
    uint8_t *data = (uint8_t *)p->payload;
    uint32_t data_len = p->tot_len;
    int32_t sbc_frame_size;
    uint8_t fragmentation = (data[0] >> 7) & 0x1;
    uint8_t starting_packet = (data[0] >> 6) & 0x1;
    uint8_t last_packet = (data[0] >> 5) & 0x1;
    uint8_t num_frames = data[0] & 0xf;

    data += 1;
    data_len -= 1;

    while(data_len > 0)
    {
        sbc_frame_size = sbc_decode(&sbc_context, data, data_len,
                                    pcm_data, 1024, &pcm_data_len);
        if (sbc_frame_size <= 0)
            break;

	BT_A2DP_TRACE_DEBUG("sbc_frame_size %d,pcm_data_len %d\n",sbc_frame_size,pcm_data_len);
        data += sbc_frame_size;
        data_len -= sbc_frame_size;
    }
		
		return BT_ERR_OK;
}


err_t a2dp_sink_init()
{
    struct sdp_record_t *record;

    uint32_t sink_record_hdl = sdp_next_rhdl();

    if((record = sdp_record_new((uint8_t *)a2dp_sink_service_record,sizeof(a2dp_sink_service_record),sink_record_hdl)) == NULL)
    {
        BT_A2DP_TRACE_DEBUG("a2dp_sink_init: Could not alloc SDP record\n");


        return BT_ERR_MEM;
    }
    else
    {
        sdp_register_service(record);
    }
    bt_hex_dump((uint8_t *)a2dp_sink_service_record,sizeof(a2dp_sink_service_record));

    avdtp_init(a2dp_sinks_event_handle,a2dp_sinks_media_handle);
    avdtp_create_sep((uint8_t *)sbc_snk_codec_caps,sizeof(sbc_snk_codec_caps));
    return BT_ERR_OK;
}
