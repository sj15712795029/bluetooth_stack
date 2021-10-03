
#include "bt_a2dp_sink.h"
#include "bt_a2dp_playback.h"

#if PROFILE_A2DP_ENABLE


#include "bt_sdp.h"
#include "sbc.h"


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

struct a2dp_pcb_t *a2dp_sink_active_pcbs;  /* List of all active A2DP PCBs */
struct a2dp_pcb_t *a2dp_sink_tmp_pcb;
#define A2DP_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define A2DP_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(a2dp_sink_tmp_pcb = *pcbs; a2dp_sink_tmp_pcb != NULL; a2dp_sink_tmp_pcb = a2dp_sink_tmp_pcb->next) { \
                               if(a2dp_sink_tmp_pcb->next != NULL && a2dp_sink_tmp_pcb->next == npcb) { \
                                  a2dp_sink_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)


a2dp_sink_cbs_t *a2dp_sink_cbs;
sbc_t sbc_context;
uint32_t pcm_data_len;
uint8_t pcm_data[1024];


static err_t a2dp_signal_connect_ind(struct avdtp_pcb_t *avdtp_pcb);
static err_t a2dp_stream_connect_ind(struct avdtp_pcb_t *avdtp_pcb);
static err_t a2dp_signal_disconnect_ind(struct avdtp_pcb_t *avdtp_pcb);
static err_t a2dp_stream_disconnect_ind(struct avdtp_pcb_t *avdtp_pcb);
static err_t a2dp_signal_set_config_ind(struct avdtp_pcb_t *avdtp_pcb,struct bt_pbuf_t *p);
static err_t a2dp_signal_open_ind(struct avdtp_pcb_t *avdtp_pcb);
static err_t a2dp_signal_start_ind(struct avdtp_pcb_t *avdtp_pcb);
static err_t a2dp_signal_suspend_ind(struct avdtp_pcb_t *avdtp_pcb);
static err_t a2dp_signal_close_ind(struct avdtp_pcb_t *avdtp_pcb);
static err_t a2dp_signal_abort_ind(struct avdtp_pcb_t *avdtp_pcb);
static err_t a2dp_sink_get_sbc_context_setting(uint8_t *sbc_codec_info);


static const uint8_t sbc_snk_codec_caps[] =
{
    AVDTP_AUDIO<<4,
   AVDTP_CODEC_SBC,

   A2DP_SBC_48000|A2DP_SBC_44100|A2DP_SBC_32000|A2DP_SBC_16000|\
   A2DP_SBC_JOINT_STEREO|A2DP_SBC_STEREO|A2DP_SBC_DUAL_CHANNEL|A2DP_SBC_MONO,

   A2DP_SBC_BLOCK_LENGTH_16|A2DP_SBC_BLOCK_LENGTH_12|A2DP_SBC_BLOCK_LENGTH_8|A2DP_SBC_BLOCK_LENGTH_4|\
   A2DP_SBC_SUBBANDS_8|A2DP_SBC_SUBBANDS_4|A2DP_SBC_ALLOCATION_METHOD_LOUDNESS|A2DP_SBC_ALLOCATION_METHOD_SNR,

   A2DP_SBC_SNK_MIN_BITPOOL,
   A2DP_SBC_SNK_MAX_BITPOOL,
};


static struct a2dp_pcb_t *a2dp_new(struct avdtp_pcb_t *avdtp_pcb)
{
    struct a2dp_pcb_t *a2dp_pcb;

    a2dp_pcb = bt_memp_malloc(MEMP_A2DP_PCB);
    if(a2dp_pcb != NULL)
    {
        memset(a2dp_pcb, 0, sizeof(struct a2dp_pcb_t));
        a2dp_pcb->avdtppcb = avdtp_pcb;
        return a2dp_pcb;
    }
	BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);

    return NULL;
}

static struct a2dp_pcb_t *a2dp_get_active_pcb(struct bd_addr_t *bdaddr)
{
    struct a2dp_pcb_t *a2dp_pcb = NULL;
    for(a2dp_pcb = a2dp_sink_active_pcbs; a2dp_pcb != NULL; a2dp_pcb = a2dp_pcb->next)
    {
        if(bd_addr_cmp(&(a2dp_pcb->remote_addr),bdaddr))
        {
            break;
        }
    }
    return a2dp_pcb;
}

static void a2dp_close(struct a2dp_pcb_t *a2dp_pcb)
{
    if(a2dp_pcb != NULL)
    {
        bt_memp_free(MEMP_A2DP_PCB, a2dp_pcb);
        a2dp_pcb = NULL;
    }
}

static err_t a2dp_signal_connect_ind(struct avdtp_pcb_t *avdtp_pcb)
{
    struct a2dp_pcb_t *a2dp_pcb;

    if((a2dp_pcb = a2dp_new(avdtp_pcb)) == NULL)
    {
    	BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not alloc a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    bd_addr_set(&(a2dp_pcb->remote_addr),&(avdtp_pcb->remote_bdaddr));
    A2DP_PCB_REG(&a2dp_sink_active_pcbs, a2dp_pcb);

    if(a2dp_sink_cbs && a2dp_sink_cbs->a2dp_sink_signal_connect_set_up)
        a2dp_sink_cbs->a2dp_sink_signal_connect_set_up(&a2dp_pcb->remote_addr,BT_ERR_OK);
    return BT_ERR_OK;
}

static err_t a2dp_stream_connect_ind(struct avdtp_pcb_t *avdtp_pcb)
{
    struct a2dp_pcb_t *a2dp_pcb = a2dp_get_active_pcb(&avdtp_pcb->remote_bdaddr);

    if(a2dp_pcb == NULL)
    {
    	BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    if(a2dp_sink_cbs && a2dp_sink_cbs->a2dp_sink_stream_connect_set_up)
        a2dp_sink_cbs->a2dp_sink_stream_connect_set_up(&a2dp_pcb->remote_addr,BT_ERR_OK);
    return BT_ERR_OK;
}

static err_t a2dp_signal_disconnect_ind(struct avdtp_pcb_t *avdtp_pcb)
{
    struct a2dp_pcb_t *a2dp_pcb = a2dp_get_active_pcb(&avdtp_pcb->remote_bdaddr);

    if(a2dp_pcb == NULL)
    {
        BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    
    if(a2dp_sink_cbs && a2dp_sink_cbs->a2dp_sink_signal_connect_realease)
        a2dp_sink_cbs->a2dp_sink_signal_connect_realease(&a2dp_pcb->remote_addr,BT_ERR_OK);

	A2DP_PCB_RMV(&a2dp_sink_active_pcbs, a2dp_pcb);
	a2dp_close(a2dp_pcb);
    return BT_ERR_OK;
}

static err_t a2dp_stream_disconnect_ind(struct avdtp_pcb_t *avdtp_pcb)
{
    struct a2dp_pcb_t *a2dp_pcb = a2dp_get_active_pcb(&avdtp_pcb->remote_bdaddr);

    if(a2dp_pcb == NULL)
    {
        BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    if(a2dp_sink_cbs && a2dp_sink_cbs->a2dp_sink_stream_realease)
        a2dp_sink_cbs->a2dp_sink_stream_realease(&a2dp_pcb->remote_addr,BT_ERR_OK);
    return BT_ERR_OK;
}

static err_t a2dp_signal_set_config_ind(struct avdtp_pcb_t *avdtp_pcb,struct bt_pbuf_t *p)
{
	uint8_t * media_codec_cap_temp;
	struct a2dp_pcb_t *a2dp_pcb = a2dp_get_active_pcb(&avdtp_pcb->remote_bdaddr);

    if(a2dp_pcb == NULL)
    {
        BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

	/* Parse media codec cap */
	media_codec_cap_temp = (uint8_t * )avdtp_get_spec_cap_value(AVDTP_MEDIA_CODEC,p->payload,p->len,&a2dp_pcb->media_codec_cap_len);
    memcpy(a2dp_pcb->media_codec_cap,media_codec_cap_temp,a2dp_pcb->media_codec_cap_len);
	BT_A2DP_TRACE_DEBUG("a2dp_signal_set_config_ind media_codec_cap_len %d\n",a2dp_pcb->media_codec_cap_len);
    bt_hex_dump(a2dp_pcb->media_codec_cap,a2dp_pcb->media_codec_cap_len);

	/* Parse content protection */
	
	return BT_ERR_OK;
}


static err_t a2dp_signal_open_ind(struct avdtp_pcb_t *avdtp_pcb)
{
	uint8_t media_type,media_codec_type;
	uint8_t *codec_info_element;
    struct a2dp_pcb_t *a2dp_pcb = a2dp_get_active_pcb(&avdtp_pcb->remote_bdaddr);

    if(a2dp_pcb == NULL)
    {
        BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

	BT_A2DP_TRACE_DEBUG("a2dp_signal_open_ind media_codec_cap_len %d\n",a2dp_pcb->media_codec_cap_len);
    bt_hex_dump(a2dp_pcb->media_codec_cap,a2dp_pcb->media_codec_cap_len);

	codec_info_element = avdtp_parse_media_codec_cap(a2dp_pcb->media_codec_cap,&media_type,&media_codec_type);
	a2dp_pcb->codec_type = media_codec_type;
	
	switch(a2dp_pcb->codec_type)
	{
		case AVDTP_CODEC_SBC:
			a2dp_pcb->codec_type = AVDTP_CODEC_SBC;
			a2dp_sink_get_sbc_context_setting(codec_info_element);
			break;
		case AVDTP_CODEC_MPEG_1_2_AUDIO:
			a2dp_pcb->codec_type = AVDTP_CODEC_MPEG_1_2_AUDIO;
			break;
		case AVDTP_CODEC_MPEG_2_4_AAC:
			a2dp_pcb->codec_type = AVDTP_CODEC_MPEG_2_4_AAC;
			break;
		case AVDTP_CODEC_ATRAC_FAMILY:
			a2dp_pcb->codec_type = AVDTP_CODEC_ATRAC_FAMILY;
			break;
		default:
			break;
	}
	
    return BT_ERR_OK;
}


static err_t a2dp_signal_start_ind(struct avdtp_pcb_t *avdtp_pcb)
{
    struct a2dp_pcb_t *a2dp_pcb = a2dp_get_active_pcb(&avdtp_pcb->remote_bdaddr);

    if(a2dp_pcb == NULL)
    {
        BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

	bt_a2dp_playback_open();
    if(a2dp_sink_cbs && a2dp_sink_cbs->a2dp_sink_stream_start)
        a2dp_sink_cbs->a2dp_sink_stream_start(&a2dp_pcb->remote_addr,BT_ERR_OK);
    return BT_ERR_OK;
}

static err_t a2dp_signal_suspend_ind(struct avdtp_pcb_t *avdtp_pcb)
{
    struct a2dp_pcb_t *a2dp_pcb = a2dp_get_active_pcb(&avdtp_pcb->remote_bdaddr);

    if(a2dp_pcb == NULL)
    {
        BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

	bt_a2dp_playback_close();
    if(a2dp_sink_cbs && a2dp_sink_cbs->a2dp_sink_stream_suspend)
        a2dp_sink_cbs->a2dp_sink_stream_suspend(&a2dp_pcb->remote_addr,BT_ERR_OK);
    return BT_ERR_OK;
}

static err_t a2dp_signal_close_ind(struct avdtp_pcb_t *avdtp_pcb)
{
    struct a2dp_pcb_t *a2dp_pcb = a2dp_get_active_pcb(&avdtp_pcb->remote_bdaddr);

    if(a2dp_pcb == NULL)
    {
        BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

	switch(a2dp_pcb->codec_type)
	{
		case AVDTP_CODEC_SBC:
			sbc_finish(&sbc_context);
			break;
		case AVDTP_CODEC_MPEG_1_2_AUDIO:
			break;
		case AVDTP_CODEC_MPEG_2_4_AAC:
			break;
		case AVDTP_CODEC_ATRAC_FAMILY:
			break;
		default:
			break;
	}
	
	
    if(a2dp_sink_cbs && a2dp_sink_cbs->a2dp_sink_stream_realease)
        a2dp_sink_cbs->a2dp_sink_stream_realease(&a2dp_pcb->remote_addr,BT_ERR_OK);
    return BT_ERR_OK;
}

static err_t a2dp_signal_abort_ind(struct avdtp_pcb_t *avdtp_pcb)
{
    struct a2dp_pcb_t *a2dp_pcb = a2dp_get_active_pcb(&avdtp_pcb->remote_bdaddr);

    if(a2dp_pcb == NULL)
    {
        BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find a2dp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    if(a2dp_sink_cbs && a2dp_sink_cbs->a2dp_sink_stream_realease)
        a2dp_sink_cbs->a2dp_sink_stream_realease(&a2dp_pcb->remote_addr,BT_ERR_OK);
    return BT_ERR_OK;
}


static err_t a2dp_sink_get_sbc_context_setting(uint8_t *sbc_codec_info)
{
	uint32_t sample_rate,channel;
	sbc_init(&sbc_context, 0);
        switch (sbc_codec_info[0] & 0xF0)
        {
        case A2DP_SBC_16000 :
            sbc_context.frequency = SBC_FREQ_16000;
			sample_rate = 16000;
            break;
        case A2DP_SBC_32000 :
            sbc_context.frequency = SBC_FREQ_32000;
			sample_rate = 32000;
            break;
        case A2DP_SBC_44100 :
            sbc_context.frequency= SBC_FREQ_44100;
			sample_rate = 44100;
            break;
        case A2DP_SBC_48000 :
            sbc_context.frequency = SBC_FREQ_48000;
			sample_rate = 48000;
            break;
        }

        switch (sbc_codec_info[0] & 0x0F)
        {
        case A2DP_SBC_MONO :
            sbc_context.mode = SBC_MODE_MONO;
			channel = 1;
            break;
        case A2DP_SBC_DUAL_CHANNEL :
            sbc_context.mode = SBC_MODE_DUAL_CHANNEL;
			channel = 2;
            break;
        case A2DP_SBC_STEREO :
            sbc_context.mode = SBC_MODE_STEREO;
			channel = 2;
            break;
        case A2DP_SBC_JOINT_STEREO :
            sbc_context.mode = SBC_MODE_JOINT_STEREO;
			channel = 2;
            break;
        }

        switch (sbc_codec_info[1] & 0xF0)
        {
        case A2DP_SBC_BLOCK_LENGTH_4 :
            sbc_context.blocks = SBC_BLK_4;
            break;
        case A2DP_SBC_BLOCK_LENGTH_8 :
            sbc_context.blocks = SBC_BLK_8;
            break;
        case A2DP_SBC_BLOCK_LENGTH_12 :
            sbc_context.blocks = SBC_BLK_12;
            break;
        case A2DP_SBC_BLOCK_LENGTH_16 :
            sbc_context.blocks = SBC_BLK_16;
            break;
        }

        switch (sbc_codec_info[1] & 0x0C)
        {
        case A2DP_SBC_SUBBANDS_4 :
            sbc_context.subbands = SBC_SB_4;
            break;
        case A2DP_SBC_SUBBANDS_8 :
            sbc_context.subbands = SBC_SB_8;
            break;
        }

        switch (sbc_codec_info[1] & 0x03)
        {
        case A2DP_SBC_ALLOCATION_METHOD_SNR :
            sbc_context.allocation = SBC_AM_SNR;
            break;
        case A2DP_SBC_ALLOCATION_METHOD_LOUDNESS :
            sbc_context.allocation = SBC_AM_LOUDNESS;
            break;
        }

        sbc_context.bitpool = sbc_codec_info[3];

		bt_a2dp_playback_init(sample_rate,channel);
		return BT_ERR_OK;
}


static err_t a2dp_sink_event_handle(struct avdtp_pcb_t *avdtp_pcb,uint32_t msg_id,struct bt_pbuf_t *p)
{

    switch(msg_id)
    {
    case AVDTP_SI_DISCOVER:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_DISCOVER\n");
        break;
    case AVDTP_SI_GET_CAPABILITIES:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_GET_CAPABILITIES\n");
        break;
    case AVDTP_SI_SET_CONFIGURATION:
    {
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_SET_CONFIGURATION\n");
        a2dp_signal_set_config_ind(avdtp_pcb,p);
        break;
    }
    case AVDTP_SI_GET_CONFIGURATION:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_GET_CONFIGURATION\n");
        break;
    case AVDTP_SI_RECONFIGURE:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_RECONFIGURE\n");
        break;
    case AVDTP_SI_OPEN:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_OPEN\n");
		a2dp_signal_open_ind(avdtp_pcb);
        break;
    case AVDTP_SI_START:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_START\n");
		a2dp_signal_start_ind(avdtp_pcb);
        break;
    case AVDTP_SI_CLOSE:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_CLOSE\n");
		a2dp_signal_close_ind(avdtp_pcb);
        break;
    case AVDTP_SI_SUSPEND:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_SUSPEND\n");
		a2dp_signal_suspend_ind(avdtp_pcb);
        break;
    case AVDTP_SI_ABORT:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_ABORT\n");
		a2dp_signal_abort_ind(avdtp_pcb);
        break;
    case AVDTP_SI_SECURITY_CONTROL:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_SECURITY_CONTROL\n");
        break;
    case AVDTP_SI_GET_ALL_CAPABILITIES:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_GET_ALL_CAPABILITIES\n");
        break;
    case AVDTP_SI_DELAYREPORT:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_DELAYREPORT\n");
        break;
    /* USER DEFINE */
    case AVDTP_SI_SIGNAL_CONNECT_IND:
        BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_SIGNAL_CONNECT_IND\n");
        a2dp_signal_connect_ind(avdtp_pcb);
        break;
    case AVDTP_SI_SIGNAL_DISCON_IND:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_SIGNAL_DISCON_IND\n");
        a2dp_signal_disconnect_ind(avdtp_pcb);
        break;
    case AVDTP_SI_SIGNAL_DISCON_CFM:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_SIGNAL_DISCON_CFM\n");
        break;
    case AVDTP_SI_STREAM_CONNECT_IND:
        BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_STREAM_CONNECT_IND\n");
        a2dp_stream_connect_ind(avdtp_pcb);
        break;
    case AVDTP_SI_STREAM_CONNECT_CFM:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_STREAM_CONNECT_CFM\n");
        break;
    case AVDTP_SI_STREAM_DISCON_IND:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_STREAM_DISCON_IND\n");
        a2dp_stream_disconnect_ind(avdtp_pcb);
        break;
    case AVDTP_SI_STREAM_DISCON_CFM:
		BT_A2DP_TRACE_DEBUG("a2dp_sink_event_handle: AVDTP_SI_STREAM_DISCON_CFM\n");
        break;
    default:
		BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow avdtp signal id\n",__FILE__,__FUNCTION__,__LINE__);
        break;
    }

    return BT_ERR_OK;
}


static err_t a2dp_sink_media_handle(struct avdtp_pcb_t *avdtp_pcb,struct bt_pbuf_t *p)
{
    uint8_t frame_index = 0;
    uint8_t *data = (uint8_t *)p->payload;
    uint32_t data_len = p->tot_len;
    int32_t sbc_frame_size;
    uint8_t fragmentation = (data[0] >> 7) & 0x1;
    uint8_t starting_packet = (data[0] >> 6) & 0x1;
    uint8_t last_packet = (data[0] >> 5) & 0x1;
    uint8_t num_frames = data[0] & 0xf;

    BT_UNUSED_ARG(frame_index);
    BT_UNUSED_ARG(fragmentation);
    BT_UNUSED_ARG(starting_packet);
    BT_UNUSED_ARG(last_packet);
    BT_UNUSED_ARG(num_frames);
    data += 1;
    data_len -= 1;

    while(data_len > 0)
    {
        sbc_frame_size = sbc_decode(&sbc_context, data, data_len,
                                    pcm_data, 1024, &pcm_data_len);

        if (sbc_frame_size <= 0)
            break;

		bt_a2dp_playback_write(pcm_data,pcm_data_len);
        BT_A2DP_INFO_TRACE_DEBUG("sbc_frame_size %d,pcm_data_len %d\n",sbc_frame_size,pcm_data_len);
        data += sbc_frame_size;
        data_len -= sbc_frame_size;
    }

    return BT_ERR_OK;
}


err_t a2dp_sink_init(a2dp_sink_cbs_t *cb)
{
    sdp_record_t *record;

    uint32_t sink_record_hdl = sdp_next_rhdl();

    if((record = sdp_record_new((uint8_t *)a2dp_sink_service_record,sizeof(a2dp_sink_service_record),sink_record_hdl)) == NULL)
    {
		BT_A2DP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not alloc SDP record\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    else
    {
        sdp_register_service(record);
    }
    bt_hex_dump((uint8_t *)a2dp_sink_service_record,sizeof(a2dp_sink_service_record));

    a2dp_sink_cbs = cb;
    avdtp_init(a2dp_sink_event_handle,a2dp_sink_media_handle);
    avdtp_create_sep(AVDTP_CODEC_SBC,(uint8_t *)sbc_snk_codec_caps,sizeof(sbc_snk_codec_caps));
    return BT_ERR_OK;
}

#endif

