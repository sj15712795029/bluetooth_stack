/******************************************************************************
  * @file           bt_hfp_hf.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt hfp hf role source file
******************************************************************************/

#include "bt_hfp_hf.h"


#if PROFILE_HFP_HF_ENABLE


/* TODO */
/*
* 测试安卓，iphone手机主动被动连接，至少10只(OPPO A3/IPHONE8)--> DONE
* 测试安卓，iphone手机多连接，至少10次(OPPO A3 & IPHONE8)--> DONE

************************1. SLC ***************************************************************************
BRSF中HFP_HFSF_THREE_WAY_CALLING HFP_HFSF_CODEC_NEGOTIATION HFP_HFSF_HF_INDICATORS HFP_HFSF_ESCO_S4添加
* AT+BAC添加 							-->DONE
* AT+CIND?的callback处理				-->DONE
* AT+CHLD的处理							-->DONE
* AT+BIND的处理							-->DONE
* AT+BIEV的处理							-->DONE
* 主动断开有bug，断开后不能重新连接	-->DONE
************************ SLC   END  ********************************************************************

************************ 2. Phone status info **********************************************************
* AT+BIA处理  									-->DONE
* +CIEV callback处理(信号/电量/servers/roam/call/callsetup/callheld/COP)	-->DONE
* CMEE的处理										-->DONE
************************ Phone status info  *************************************************************

************************ 3. Audio connection handing **********************************************************
* AT+BAC(Bluetooth Available Codecs)处理  -->DONE
* AT+BCC(Bluetooth Codec Connection) 处理 -->DONE
* +BCS (Bluetooth Codec Selection）处理	 -->DONE
* AT+BCS 处理									 -->DONE
* SCO建立处理									-->DONE,并且通过support feature来决定是否用BCC还是HCI sco con
* SCO断开处理									-->DONE
* SCO transfer的封装							-->DONE
SCO建立连接后是否需要write voice setting
WM8978 NBS出声音
CSR8811 WBS enable audio dsp
WM8978 WBS出声音
************************ Audio connection handing  *************************************************************

************************ 4. Answer an Incoming Call **********************************************************
* 判断AG是否支持in band ring					-->DONE   但是还不确定什么时候会用到
* RING command的处理							-->DONE
* CLIP command的处理							-->DONE
* ATA  command的处理							-->DONE
* BSIR command的处理							-->DONE
************************ Answer an Incoming Call  *************************************************************

************************ 11. call waiting **********************************************************
* AT+CCWA 处理          						-->DONE   使能后就可以收到+CCWA,disable后就收不到
* +CCWA解析 callback处理						-->DONE
************************ call waiting  *************************************************************

************************ 12. Three-Way Call Handling **********************************************************
* 三方通话的处理								-->DONE
************************ Three-Way Call Handling  *************************************************************


************************ 15. Voice Recognition Activation **********************************************************
* 语音识别的callback							-->DONE 但是不建议用
************************ Voice Recognition Activation  *************************************************************

************************ 16. Attach a Phone Number to a Voice Tag **********************************************************
获取号码的callback --> 到底格式是什么样子
************************ Attach a Phone Number to a Voice Tag  *************************************************************

************************ 18. Remote Audio Volume Control **********************************************************
*测试是否只有在SCO的时候AT+VGS,VGM管用 -->在没有SCO建立的情况下也管用
*测试关闭掉feature是否管用					-->不注册这个feature也管用
* VGS,VGM的callback处理						-->DONE
************************ Remote Audio Volume Control  *************************************************************


************************ 19. Response and Hold **********************************************************
* 整个处理										-->DONE
************************ Response and Hold  *************************************************************

************************ 20. Subscriber Number Information **********************************************************
* 获取到号码的处理								-->DONE
************************ Subscriber Number Information  *************************************************************

************************ 20. Enhanced Call Status Mechanisms **********************************************************
* 获取到号码的处理								-->DONE
************************ Enhanced Call Status Mechanisms  *************************************************************

************************ 21. Enhanced Call control Mechanisms **********************************************************
* 后面再添加										-->DONE
************************ Enhanced Call control Mechanisms  *************************************************************

************************ 25. HF Indicators **********************************************************
* 													-->DONE
************************ HF Indicators  *************************************************************

************************ 3GPP **********************************************************
* static const uint8_t HF_AT_CGMI_RSP[] = "+CGMI:";	-->DONE
* static const uint8_t HF_AT_CGMM_RSP[] = "+CGMM:";	-->DONE
* static const uint8_t HF_AT_CGMR_RSP[] = "+CGMR:";	-->DONE
* static const uint8_t HF_AT_CGSN_RSP[] = "+CGSN:";	-->DONE
************************ 3GPP  *************************************************************/

/* 文档 */
/* BQB + 文档 */

/* BUG TRACE */
/*
* 断开后连接不上，0x17 event没有处理，我们的linkkey没有处理好(DONE)-->需要flash好的时候去存储到flash里面
* SLC在彻底断开前，继续下连接，会出现连接不上的情况
*/


static  uint8_t hfp_hf_service_record[] =
{
    SDP_DES_SIZE8, 0xb,
    SDP_UINT16, BT_ATTRIBUTE_SERVICE_CLASS_ID_LIST>>8&0xff, BT_ATTRIBUTE_SERVICE_CLASS_ID_LIST&0xff, /* Service class ID list attribute */
            SDP_DES_SIZE8, 6,
            SDP_UUID16, BT_SERVICE_CLASS_HANDSFREE>>8&0xff, BT_SERVICE_CLASS_HANDSFREE&0xff,
            SDP_UUID16, BT_SERVICE_CLASS_GENERIC_AUDIO>>8&0xff, BT_SERVICE_CLASS_GENERIC_AUDIO&0xff,

            SDP_DES_SIZE8, 0x11,
            SDP_UINT16, BT_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST>>8&0xff, BT_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST&0xff,/* Protocol descriptor list attribute */
            SDP_DES_SIZE8, 0xc,
            SDP_DES_SIZE8, 0x3,
            SDP_UUID16, BT_PROTOCOL_L2CAP>>8&0xff, BT_PROTOCOL_L2CAP&0xff, /*L2CAP*/
            SDP_DES_SIZE8, 0x5,
            SDP_UUID16, BT_PROTOCOL_RFCOMM>>8&0xff, BT_PROTOCOL_RFCOMM&0xff, /*RFCOMM*/
            SDP_UINT8, RFCOMM_HFP_HF_CHNL, /*RFCOMM channel*/

            SDP_DES_SIZE8, 0xd,
            SDP_UINT16, BT_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST>>8&0xff, BT_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST&0xff, /* profile descriptor List */
            SDP_DES_SIZE8, 0x8,
            SDP_DES_SIZE8,0x06,
            SDP_UUID16,BT_SERVICE_CLASS_HANDSFREE>>8&0xff, BT_SERVICE_CLASS_HANDSFREE&0xff,
            SDP_UINT16,0x01,0x08,			/* V1.8 */

            SDP_DES_SIZE8, 0x6,
            SDP_UINT16, BT_ATTRIBUTE_SUPPORTED_FEATURES>>8&0xff, BT_ATTRIBUTE_SUPPORTED_FEATURES&0xff,  /* support feature */
            SDP_UINT16,0x00,0x27,

};

struct hfp_pcb_t *hfp_active_pcbs;  /* List of all active HFP PCBs */
struct hfp_pcb_t *hfp_tmp_pcb;

uint16_t hfp_hf_local_support_feature;
uint16_t hf_hf_local_sdp_feature;
hfp_hf_cbs_t *hfp_hf_cbs;

/* Bluetooth HFP spec V1.7.2 define */
static const uint8_t HF_AT_OK_RSP[] = "OK";
static const uint8_t HF_AT_ERROR_RSP[] = "ERROR";
static const uint8_t HF_AT_BRSF[] = "AT+BRSF=";
static const uint8_t HF_AT_BRSF_RSP[] = "+BRSF:";
static const uint8_t HF_AT_BAC[] = "AT+BAC=";
static const uint8_t HF_AT_CIND_TEST[] = "AT+CIND=?";
static const uint8_t HF_AT_CIND[] = "AT+CIND?";
static const uint8_t HF_AT_CIND_RSP[] = "+CIND:";
static const uint8_t HF_AT_CMER_ENABLE[] = "AT+CMER=3,0,0,1";
static const uint8_t HF_AT_CMER_DISABLE[] = "AT+CMER=3,0,0,0";
static const uint8_t HF_AT_CHLD_TEST[] = "AT+CHLD=?";
static const uint8_t HF_AT_CHLD[] = "AT+CHLD=";
static const uint8_t HF_AT_CHLD_RSP[] = "+CHLD:";
static const uint8_t HFP_AT_BIND[] = "AT+BIND=";
static const uint8_t HFP_AT_BIND_TEST[] = "AT+BIND?";
static const uint8_t HFP_AT_BIND_RSP[] = "+BIND:";
static const uint8_t HFP_AT_BIEV[] = "AT+BIEV=";
static const uint8_t HF_AT_BCS[] = "+BCS:";
static const uint8_t HF_AT_BCS_RSP[] = "AT+BCS=";
static const uint8_t HF_AT_BCC[] = "AT+BCC";
static const uint8_t HF_AT_CIEV[] = "+CIEV:";
static const uint8_t HFP_AT_CMEE[] = "AT+CMEE=";
static const uint8_t HFP_AT_CME[] ="+CME ERROR";
static const uint8_t HF_AT_ATA[] = "ATA";
static const uint8_t HF_AT_CHUP[] = "AT+CHUP";
static const uint8_t HF_AT_ATD[] = "ATD";
static const uint8_t HF_AT_BLDN[] = "AT+BLDN";
static const uint8_t HF_AT_VTS[] = "AT+VTS=";
static const uint8_t HF_AT_CLCC[] = "AT+CLCC";
static const uint8_t HF_AT_CLCC_RSP[] = "+CLCC:";
static const uint8_t HF_AT_COPS_F[] = "AT+COPS=3,0";
static const uint8_t HF_AT_COPS[] = "AT+COPS?";
static const uint8_t HF_AT_COPS_RSP[] = "+COPS:";
static const uint8_t HF_AT_RING[] = "RING";
static const uint8_t HF_AT_CLIP[] = "AT+CLIP=";
static const uint8_t HF_AT_CLIP_RSP[] = "+CLIP:";
static const uint8_t HF_AT_BSIR[] = "+BSIR:";
static const uint8_t HF_AT_CCWA[] = "AT+CCWA=";
static const uint8_t HF_AT_CCWA_RSP[] = "+CCWA:";
static const uint8_t HF_AT_NREC_DISABLE[] = "AT+NREC=0";
static const uint8_t HF_AT_BVRA[] = "AT+BVRA=";
static const uint8_t HF_AT_BVRA_RSP[] = "+BVRA:";
static const uint8_t HF_AT_BINP[] = "AT+BINP=1";
static const uint8_t HF_AT_BINP_RSP[] = "+BINP:";
static const uint8_t HF_AT_VGM[] = "AT+VGM=";
static const uint8_t HF_AT_VGM_RSP[] = "+VGM:";
static const uint8_t HF_AT_VGS[] = "AT+VGS=";
static const uint8_t HF_AT_VGS_RSP[] = "+VGS:";
static const uint8_t HF_AT_CNUM[] = "AT+CNUM";
static const uint8_t HF_AT_CNUM_RSP[] = "+CNUM:";
static const uint8_t HF_AT_BIA[] = "AT+BIA=";
static const uint8_t HF_AT_BTRH_TEST[] = "AT+BTRH?";
static const uint8_t HF_AT_BTRH[] = "AT+BTRH=";
static const uint8_t HF_AT_BTRH_RSP[] = "+BTRH:";
/* 3GPP AT command */
static const uint8_t HF_AT_CGMI[] = "AT+CGMI";
static const uint8_t HF_AT_CGMI_RSP[] = "+CGMI:";
static const uint8_t HF_AT_CGMM[] = "AT+CGMM";
static const uint8_t HF_AT_CGMM_RSP[] = "+CGMM:";
static const uint8_t HF_AT_CGMR[] = "AT+CGMR";
static const uint8_t HF_AT_CGMR_RSP[] = "+CGMR:";
static const uint8_t HF_AT_CGSN[] = "AT+CGSN";
static const uint8_t HF_AT_CGSN_RSP[] = "+CGSN:";

static uint8_t wbs_codecs[] = {HFP_CODEC_CVSD, HFP_CODEC_MSBC};
static uint8_t nbs_codecs[] = {HFP_CODEC_CVSD};

#define HFP_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define HFP_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(hfp_tmp_pcb = *pcbs; hfp_tmp_pcb != NULL; hfp_tmp_pcb = hfp_tmp_pcb->next) { \
                               if(hfp_tmp_pcb->next != NULL && hfp_tmp_pcb->next == npcb) { \
                                  hfp_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)



static struct hfp_pcb_t *hfp_new(rfcomm_pcb_t *rfcommpcb);
static struct hfp_pcb_t *hfp_get_active_pcb(struct bd_addr_t *bdaddr);
static void hfp_close(struct hfp_pcb_t *pcb);
static err_t hfp_hf_run(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_recv(void *arg, rfcomm_pcb_t *pcb, struct bt_pbuf_t *p, err_t err);
static uint8_t hfp_has_codec_negotiation_feature(struct hfp_pcb_t *pcb);
static uint16_t hfp_has_three_call_feature(struct hfp_pcb_t *pcb);
static uint16_t hfp_has_hf_indicators_feature(struct hfp_pcb_t *pcb);
static uint16_t hfp_has_voice_recognition_feature(struct hfp_pcb_t *pcb);
static uint16_t hfp_has_call_status_feature(struct hfp_pcb_t *pcb);
static uint16_t hfp_ag_has_inbandring_feature(struct hfp_pcb_t *pcb);
static uint16_t hfp_ag_has_nrec_feature(struct hfp_pcb_t *pcb);
static err_t hfp_hf_exchange_supported_features(struct hfp_pcb_t *pcb);
static err_t hfp_hf_notify_on_codecs(struct hfp_pcb_t *pcb);
static err_t hfp_hf_codec_select_rsp(struct hfp_pcb_t *pcb,uint8_t bcs);
static err_t hfp_hf_codec_connect(struct hfp_pcb_t *pcb);
static err_t hfp_hf_retrieve_indicators(struct hfp_pcb_t *pcb);
static err_t hfp_hf_retrieve_indicators_status(struct hfp_pcb_t *pcb);
static err_t hfp_hf_activate_ind_report(struct hfp_pcb_t *pcb);
static err_t hfp_hf_deactivate_ind_report(struct hfp_pcb_t *pcb);
static err_t hfp_hf_retrieve_can_hold_call(struct hfp_pcb_t *pcb);
static err_t hfp_hf_hold_call(struct hfp_pcb_t *pcb,uint8_t cmd,uint8_t index);
static err_t hfp_hf_tx_hf_indicator(struct hfp_pcb_t *pcb);
static err_t hfp_hf_tx_hf_indicator_value(struct hfp_pcb_t *pcb,uint8_t indicator,uint8_t value);
static err_t hfp_hf_rx_ag_indicator(struct hfp_pcb_t *pcb);
static err_t hfp_hf_get_ag_indicator_enable(struct hfp_pcb_t *pcb);
static err_t hfp_hf_cmd_ata(struct hfp_pcb_t *pcb);
static err_t hfp_hf_cmd_chup(struct hfp_pcb_t *pcb);
static err_t hfp_hf_cmd_atd_with_phone_number(struct hfp_pcb_t *pcb,uint8_t *number);
static err_t hfp_hf_cmd_atd_with_memory(struct hfp_pcb_t *pcb,uint8_t memory_id);
static err_t hfp_hf_cmd_bldn(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_clcc(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_cops_format(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_cops(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_ccwa(struct hfp_pcb_t *pcb,uint8_t value);
static err_t hfp_hf_send_cmee(struct hfp_pcb_t *pcb,uint8_t value);
static err_t hfp_hf_send_clip(struct hfp_pcb_t *pcb,uint8_t value);
static err_t hfp_hf_send_nrec(struct hfp_pcb_t *pcbs);
static err_t hfp_hf_send_bvra(struct hfp_pcb_t *pcb,uint8_t value);
static err_t hfp_hf_send_binp(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_vts(struct hfp_pcb_t *pcb,uint8_t value);
static err_t hfp_hf_send_vgm(struct hfp_pcb_t *pcb,uint8_t value);
static err_t hfp_hf_send_vgs(struct hfp_pcb_t *pcb,uint8_t value);
static err_t hfp_hf_send_cnum(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_query_btrh(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_control_btrh(struct hfp_pcb_t *pcb,uint8_t value);
static err_t hfp_hf_send_bia(struct hfp_pcb_t *pcb,uint8_t index,uint8_t value);
static err_t hfp_hf_send_cgmi(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_cgmm(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_cgmr(struct hfp_pcb_t *pcb);
static err_t hfp_hf_send_cgsn(struct hfp_pcb_t *pcb);
static err_t hfp_hf_handle_at_brsf(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_extract_indicator_order(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_get_ind_value(uint8_t *data,uint16_t data_len,uint8_t ind_index,uint8_t *ret_value);
static err_t hfp_hf_update_indicator_status(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_cind(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_chld(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_bind(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_ring(struct hfp_pcb_t *pcb);
static err_t hfp_hf_handle_at_clip(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_bsir(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_ccwa(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_bvra(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_binp(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_vgm(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_vgs(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_cnum(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_btrh(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_cgmi(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_cgmm(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_cgmr(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_get_ciev_index_value(uint8_t *data,uint8_t *index,uint8_t* values);
static err_t hfp_hf_handle_at_ciev(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_cops(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_bcs(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_clcc(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static err_t hfp_hf_handle_at_error(struct hfp_pcb_t *pcb);
static err_t hfp_hf_handle_at_ok(struct hfp_pcb_t *pcb);
static err_t hfp_hf_parse_receive_data(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len);
static void hfp_hf_sdp_attributes_recv(void *arg, sdp_pcb_t *sdppcb, uint16_t attribl_bc, struct bt_pbuf_t *p);
static uint8_t hfp_hf_get_rfcomm_cn(uint16_t attribl_bc, struct bt_pbuf_t *attribute_list);
static err_t l2cap_connect_cfm(void *arg, l2cap_pcb_t *l2cappcb, uint16_t result, uint16_t status);
static err_t l2cap_disconnect_cfm(void *arg, l2cap_pcb_t *pcb);
static err_t hfp_connect_cfm(void *arg, rfcomm_pcb_t *pcb, err_t err);
static err_t hfp_connect_ind(void *arg, rfcomm_pcb_t *pcb, err_t err);
static err_t hfp_hf_disconnected(void *arg, rfcomm_pcb_t *pcb, err_t err);
static err_t hfp_hf_sco_connect_ind(void *arg, struct bd_addr_t *bdaddr);
static err_t hfp_hf_sco_conn_complete(void *arg, uint8_t status,struct bd_addr_t *bdaddr);
static err_t  sco_hfp_disconn_complete(void *arg, uint8_t status,struct bd_addr_t *bdaddr);
static err_t hfp_hf_run(struct hfp_pcb_t *pcb);




err_t hfp_hf_init(uint16_t hf_support_feature,uint8_t support_wbs,hfp_hf_cbs_t *cb)
{
    sdp_record_t *record;

    uint8_t hf_sdp_size = sizeof(hfp_hf_service_record);
    uint32_t hf_record_hdl = sdp_next_rhdl();
    hf_hf_local_sdp_feature = hf_support_feature & HFP_HF_SDP_SF_MASK;

    if(support_wbs == HFP_HF_SDP_SUPPORT_WBS)
    {
        hf_hf_local_sdp_feature |= HFP_HF_SDP_SF_WBS;
    }


    hfp_hf_local_support_feature = hf_support_feature;
    bt_be_store_16((uint8_t *)hfp_hf_service_record,hf_sdp_size-2,hf_hf_local_sdp_feature);

    if((record = sdp_record_new((uint8_t *)hfp_hf_service_record,hf_sdp_size,hf_record_hdl)) == NULL)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_init: Could not alloc SDP record\n");

        return BT_ERR_MEM;
    }
    else
    {
        sdp_register_service(record);
    }
    bt_hex_dump((uint8_t *)hfp_hf_service_record,sizeof(hfp_hf_service_record));


    rfcomm_listen(RFCOMM_HFP_HF_CHNL, hfp_connect_ind);

    hfp_hf_cbs = cb;
    hci_register_sco_req(hfp_hf_sco_connect_ind);
    hci_register_sco_conn_complete(hfp_hf_sco_conn_complete);
    hci_register_sco_disconn_complete(sco_hfp_disconn_complete);

    return BT_ERR_OK;
}

err_t hfp_hf_connect(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb;

    if((hfppcb = hfp_new(NULL)) == NULL)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_connect: Could not alloc rfcomm pcb\n");
        return BT_ERR_MEM;
    }

    hfppcb->state = HFP_W2_SDP_CONNECTED;

    bd_addr_set(&(hfppcb->remote_addr),addr );
    hfp_hf_run(hfppcb);

    return BT_ERR_OK;
}

err_t hfp_hf_disconnect(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(!hfppcb)
        return BT_ERR_CONN;

    hfppcb->state = HFP_W2_DISCONNECT_RFCOMM;
    hfp_hf_run(hfppcb);

    return BT_ERR_OK;
}

err_t hfp_hf_set_format_network(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_cops_format(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_get_network(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_cops(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_audio_connect(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(!hfppcb)
        return BT_ERR_CONN;

    hfppcb->state = HFP_W2_CONNECT_SCO;
    hfp_hf_run(hfppcb);
    return BT_ERR_OK;
}

err_t hfp_hf_audio_disconnect(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(!hfppcb)
        return BT_ERR_CONN;

    if(hfppcb->state == HFP_AUDIO_CONNECTION_ESTABLISHED)
    {
        hfppcb->state = HFP_W2_DISCONNECT_SCO;
        hfp_hf_run(hfppcb);
    }
    return BT_ERR_OK;
}

err_t hfp_hf_set_ag_extended_error(struct bd_addr_t *addr,uint8_t value)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(!hfppcb)
        return BT_ERR_CONN;

    hfp_hf_send_cmee(hfppcb,value);

    return BT_ERR_OK;
}

err_t hfp_hf_answer_incoming_call(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(!hfppcb)
        return BT_ERR_CONN;

    hfp_hf_cmd_ata(hfppcb);

    return BT_ERR_OK;
}

err_t hfp_hf_callout_with_phone_number(struct bd_addr_t *addr,uint8_t *number)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_cmd_atd_with_phone_number(hfppcb,number);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_callout_with_memory(struct bd_addr_t *addr,uint8_t memory_id)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_cmd_atd_with_memory(hfppcb,memory_id);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_callout_with_last_number(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_cmd_bldn(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_set_call_waiting_notification(struct bd_addr_t *addr,uint8_t value)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_ccwa(hfppcb,value);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_three_call_control(struct bd_addr_t *addr,uint8_t cmd,uint8_t index)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(!hfppcb)
        return BT_ERR_CONN;

    if((cmd > THREE_CALL_CMD_CONNECT_CALL) || (index > 5))
        return BT_ERR_ARG;

    if((cmd == THREE_CALL_CMD_REL_ACTIVE_CALL) && (index > 0))
    {
        if((hfppcb->hfp_hf_remote_three_call_support_bitmap & THREE_CALL_SERVICE_1x) != THREE_CALL_SERVICE_1x)
            return BT_ERR_ARG;
    }

    if((cmd == THREE_CALL_CMD_SWAP_CALL) && (index > 0))
    {
        if((hfppcb->hfp_hf_remote_three_call_support_bitmap & THREE_CALL_SERVICE_2x) != THREE_CALL_SERVICE_2x)
            return BT_ERR_ARG;
    }

    if((cmd == THREE_CALL_CMD_USER_BUSY ) || (cmd == THREE_CALL_CMD_JOHN_HELD_CALL )  || (cmd == THREE_CALL_CMD_CONNECT_CALL ))
        return hfp_hf_hold_call(hfppcb,cmd,0);
    else
        return hfp_hf_hold_call(hfppcb,cmd,index);
}

err_t hfp_hf_set_call_line_identification_notification(struct bd_addr_t *addr,uint8_t value)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_clip(hfppcb,value);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_disable_ag_nrec(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
    {
        if(hfp_ag_has_nrec_feature(hfppcb))
            return hfp_hf_send_nrec(hfppcb);
        else
            return BT_ERR_ARG;
    }
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_set_voice_recognition(struct bd_addr_t *addr,uint8_t value)
{
    err_t result;
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
    {
        if(hfp_has_voice_recognition_feature(hfppcb))
        {
            result =  hfp_hf_send_bvra(hfppcb,value);
            if(result == BT_ERR_OK)
            {
                if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_voice_recognition)
                    hfp_hf_cbs->hfp_hf_voice_recognition(addr,value);
                return BT_ERR_OK;
            }
        }
        else
            return BT_ERR_ARG;
    }
    else
        return BT_ERR_CONN;

    return BT_ERR_OK;
}

err_t hfp_hf_get_phone_number_via_voice_tag(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_binp(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_transmit_dtmf(struct bd_addr_t *addr,uint8_t value)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_vts(hfppcb,value);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_set_mic_volume(struct bd_addr_t *addr,uint8_t value)
{
    err_t result;
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
    {
        result =  hfp_hf_send_vgm(hfppcb,value);
        if(result == BT_ERR_OK)
        {
            if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_mic_vol)
                hfp_hf_cbs->hfp_hf_mic_vol(addr,value);
            return BT_ERR_OK;
        }
    }
    else
        return BT_ERR_CONN;

    return BT_ERR_OK;
}

err_t hfp_hf_set_spk_volume(struct bd_addr_t *addr,uint8_t value)
{
    err_t result;
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
    {
        result =  hfp_hf_send_vgs(hfppcb,value);
        if(result == BT_ERR_OK)
        {
            if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_spk_vol)
                hfp_hf_cbs->hfp_hf_spk_vol(addr,value);
            return BT_ERR_OK;
        }
    }
    else
        return BT_ERR_CONN;

    return BT_ERR_OK;
}

err_t hfp_hf_query_hold_status(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_query_btrh(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_control_call_hold(struct bd_addr_t *addr,uint8_t value)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_control_btrh(hfppcb,value);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_get_local_phone_number(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_cnum(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_query_call_list(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
    {
        if(hfp_has_call_status_feature(hfppcb))
            return hfp_hf_send_clcc(hfppcb);
        else
            return BT_ERR_ARG;
    }
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_transfer_hf_indicator_value(struct bd_addr_t *addr,uint8_t indicator,uint8_t value)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
    {
        if(hfp_has_hf_indicators_feature(hfppcb))
            return hfp_hf_tx_hf_indicator_value(hfppcb,indicator,value);
        else
            return BT_ERR_ARG;
    }
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_hangup(struct bd_addr_t *addr)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_cmd_chup(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_set_indicator_enable_value(struct bd_addr_t *addr,uint8_t *indicator_name,uint8_t value)
{
    int8_t indicator_index = -1;
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(!hfppcb)
        return BT_ERR_CONN;

    /**
    * Call, Call Setup, and Held Call indicators have been defined as mandatory indicators. This
    * 	implies that whatever the reporting state the HF gives, these indicators shall always been
    * 	keptactivated by the AG
    **/
    if(strcmp("service",(const char*)indicator_name) == 0)
    {
        indicator_index = hfppcb->service_index;
    }

    if(strcmp("signal",(const char*)indicator_name) == 0)
    {
        indicator_index = hfppcb->service_index;
    }

    if(strcmp("roam",(const char*)indicator_name) == 0)
    {
        indicator_index = hfppcb->roam_index;
    }

    if(strcmp("battchg",(const char*)indicator_name) == 0)
    {
        indicator_index = hfppcb->battchg_index;
    }

    if(indicator_index == -1)
        return BT_ERR_ARG;

    return hfp_hf_send_bia(hfppcb,indicator_index,value);


}

err_t hfp_hf_get_manufacturer_id(struct bd_addr_t *addr)
{
    /* +CGMI: "Apple Inc."*/
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_cgmi(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_get_model_id(struct bd_addr_t *addr)
{
    /* +CGMM: iPhone10,3 */
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_cgmm(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_get_revision_id(struct bd_addr_t *addr)
{
    /* +CGMR: Version 12.1.2 (Build 16C101) */
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_cgmr(hfppcb);
    else
        return BT_ERR_CONN;
}

err_t hfp_hf_get_pid(struct bd_addr_t *addr)
{
    /* +CGSN: 35484609376xxxx */
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(addr);
    if(hfppcb)
        return hfp_hf_send_cgsn(hfppcb);
    else
        return BT_ERR_CONN;
}

static struct hfp_pcb_t *hfp_new(rfcomm_pcb_t *rfcommpcb)
{
    struct hfp_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_HFP_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(struct hfp_pcb_t));
        pcb->rfcommpcb = rfcommpcb;
        return pcb;
    }

    BT_HFP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

static struct hfp_pcb_t *hfp_get_active_pcb(struct bd_addr_t *bdaddr)
{
    struct hfp_pcb_t *pcb = NULL;
    for(pcb = hfp_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(bd_addr_cmp(&(pcb->remote_addr),bdaddr))
        {
            break;
        }
    }
    return pcb;
}

static void hfp_close(struct hfp_pcb_t *pcb)
{
    if(pcb != NULL)
    {
        bt_memp_free(MEMP_HFP_PCB, pcb);
        pcb = NULL;
    }
}

static err_t hfp_hf_send(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    struct bt_pbuf_t *p = NULL;
    if((p = bt_pbuf_alloc(BT_PBUF_RAW, data_len, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_HFP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    memcpy((uint8_t *)p->payload, data, data_len);

    rfcomm_uih(pcb->rfcommpcb, rfcomm_cn(pcb->rfcommpcb), p);

    bt_pbuf_free(p);

    return BT_ERR_OK;
}

static err_t hfp_hf_recv(void *arg, rfcomm_pcb_t *pcb, struct bt_pbuf_t *p, err_t err)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(&(pcb->l2cappcb->remote_bdaddr));
    if(!hfppcb)
        return BT_ERR_CONN;
    /* TODO:注意此部分p是否需要内存释放*/
    BT_HFP_TRACE_DEBUG("hfp_hf_recv: p->len == %d p->tot_len == %d\n", p->len, p->tot_len);
    bt_hex_dump(p->payload,p->len);
    BT_HFP_TRACE_DEBUG("***********************************\n");
    BT_HFP_TRACE_DEBUG("%s\n",(char *)p->payload);
    BT_HFP_TRACE_DEBUG("***********************************\n");

    hfp_hf_parse_receive_data(hfppcb,(uint8_t *)p->payload,p->len);

    return BT_ERR_OK;
}

static uint8_t hfp_has_codec_negotiation_feature(struct hfp_pcb_t *pcb)
{
    uint16_t hf_support_codec = pcb->hfp_hf_local_support_feature & HFP_HFSF_CODEC_NEGOTIATION;
    uint16_t ag_support_codec = pcb->hfp_hf_remote_support_feature & HFP_AGSF_CODEC_NEGOTIATION;
    return hf_support_codec && ag_support_codec;
}

static uint16_t hfp_has_three_call_feature(struct hfp_pcb_t *pcb)
{
    uint16_t hf_support_three_call = pcb->hfp_hf_local_support_feature & HFP_HFSF_THREE_WAY_CALLING;
    uint16_t ag_support_three_call = pcb->hfp_hf_remote_support_feature & HFP_AGSF_THREE_WAY_CALLING;
    return hf_support_three_call && ag_support_three_call;
}

static uint16_t hfp_has_hf_indicators_feature(struct hfp_pcb_t *pcb)
{
    uint16_t hf_support_hf_indicators = pcb->hfp_hf_local_support_feature & HFP_HFSF_HF_INDICATORS;
    uint16_t ag_support_hf_indicators = pcb->hfp_hf_remote_support_feature & HFP_AGSF_HF_INDICATORS;
    return hf_support_hf_indicators && ag_support_hf_indicators;
}

static uint16_t hfp_has_voice_recognition_feature(struct hfp_pcb_t *pcb)
{
    uint16_t hf_support_voice_recognition = pcb->hfp_hf_local_support_feature & HFP_HFSF_VOICE_RECOGNITION_FUNCTION;
    uint16_t ag_support_voice_recognition = pcb->hfp_hf_remote_support_feature & HFP_AGSF_VOICE_RECOGNITION_FUNCTION;
    return hf_support_voice_recognition && ag_support_voice_recognition;
}

static uint16_t hfp_has_call_status_feature(struct hfp_pcb_t *pcb)
{
    uint16_t hf_support_call_status = pcb->hfp_hf_local_support_feature & HFP_HFSF_ENHANCED_CALL_STATUS;
    uint16_t ag_support_call_status = pcb->hfp_hf_remote_support_feature & HFP_AGSF_ENHANCED_CALL_STATUS;
    return hf_support_call_status && ag_support_call_status;
}

static uint16_t hfp_ag_has_inbandring_feature(struct hfp_pcb_t *pcb)
{
    uint16_t ag_support_inbandring = pcb->hfp_hf_remote_support_feature & HFP_AGSF_IN_BAND_RING_TONE;
    return ag_support_inbandring;
}

static uint16_t hfp_ag_has_nrec_feature(struct hfp_pcb_t *pcb)
{
    uint16_t ag_support_nrec = pcb->hfp_hf_remote_support_feature & HFP_AGSF_EC_NR_FUNCTION;
    return ag_support_nrec;
}

static uint16_t hfp_has_hf_indicator_feature(struct hfp_pcb_t *pcb)
{
    return pcb->ag_battery_level_indicator_enable;
}


static err_t hfp_hf_exchange_supported_features(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HF_AT_BRSF, pcb->hfp_hf_local_support_feature);
    BT_HFP_TRACE_DEBUG("hfp_hf_exchange_supported_features:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_notify_on_codecs(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    if((hf_hf_local_sdp_feature & HFP_HF_SDP_SF_WBS) == HFP_HF_SDP_SF_WBS)
        sprintf((char *)buffer, "%s%d,%d\r\n", HF_AT_BAC, wbs_codecs[0],wbs_codecs[1]);
    else
        sprintf((char *)buffer, "%s%d\r\n", HF_AT_BAC, nbs_codecs[0]);
    BT_HFP_TRACE_DEBUG("hfp_hf_notify_on_codecs:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_codec_select_rsp(struct hfp_pcb_t *pcb,uint8_t bcs)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HF_AT_BCS_RSP,bcs);
    BT_HFP_TRACE_DEBUG("hfp_hf_codec_select_rsp:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_codec_connect(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_BCC);
    BT_HFP_TRACE_DEBUG("hfp_hf_codec_connect:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_retrieve_indicators(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CIND_TEST);
    BT_HFP_TRACE_DEBUG("hfp_hf_retrieve_indicators:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_retrieve_indicators_status(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CIND);
    BT_HFP_TRACE_DEBUG("hfp_hf_retrieve_indicators_status:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_activate_ind_report(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CMER_ENABLE);
    BT_HFP_TRACE_DEBUG("hfp_hf_activate_ind_report:%s\n", buffer);

    memset(pcb->indicator_enable_map,1,pcb->number_of_indicator);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_deactivate_ind_report(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CMER_DISABLE);
    BT_HFP_TRACE_DEBUG("hfp_hf_deactivate_ind_report:%s\n", buffer);
    memset(pcb->indicator_enable_map,0,pcb->number_of_indicator);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}


static err_t hfp_hf_retrieve_can_hold_call(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CHLD_TEST);
    BT_HFP_TRACE_DEBUG("hfp_hf_retrieve_can_hold_call:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}


static err_t hfp_hf_hold_call(struct hfp_pcb_t *pcb,uint8_t cmd,uint8_t index)
{
    uint8_t buffer[20] = {0};
    if(index > 0)
        sprintf((char *)buffer, "%s%d%d\r\n", HF_AT_CHLD,cmd,index);
    else
        sprintf((char *)buffer, "%s%d\r\n", HF_AT_CHLD,cmd);
    BT_HFP_TRACE_DEBUG("hfp_hf_hold_call:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_tx_hf_indicator(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d,%d\r\n", HFP_AT_BIND,HFP_HF_INDICATOR_ENHANDCED_SAFETY,HFP_HF_INDICATOR_BATTERY_LEVEL);
    BT_HFP_TRACE_DEBUG("hfp_hf_tx_hf_indicator:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_tx_hf_indicator_value(struct hfp_pcb_t *pcb,uint8_t indicator,uint8_t value)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d,%d\r\n", HFP_AT_BIEV,indicator,value);
    BT_HFP_TRACE_DEBUG("hfp_hf_tx_hf_indicator_value:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_rx_ag_indicator(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s?\r\n", HFP_AT_BIND);
    BT_HFP_TRACE_DEBUG("hfp_hf_rx_ag_indicator:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_get_ag_indicator_enable(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HFP_AT_BIND_TEST);
    BT_HFP_TRACE_DEBUG("hfp_hf_get_ag_indicator_enable:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_cmd_ata(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[10] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_ATA);
    BT_HFP_TRACE_DEBUG("hfp_hf_cmd_ata:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_cmd_chup(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[10] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CHUP);
    BT_HFP_TRACE_DEBUG("hfp_hf_cmd_chup:%s\n", buffer)
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_cmd_atd_with_phone_number(struct hfp_pcb_t *pcb,uint8_t *number)
{
    uint8_t buffer[30] = {0};
    sprintf((char *)buffer, "%s%s;\r\n", HF_AT_ATD,number);
    BT_HFP_TRACE_DEBUG("hfp_hf_cmd_atd_with_phone_number:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_cmd_atd_with_memory(struct hfp_pcb_t *pcb,uint8_t memory_id)
{
    uint8_t buffer[30] = {0};
    sprintf((char *)buffer, "%s>%d;\r\n", HF_AT_ATD, memory_id);
    BT_HFP_TRACE_DEBUG("hfp_hf_cmd_atd_with_memory:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}


static err_t hfp_hf_cmd_bldn(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_BLDN);
    BT_HFP_TRACE_DEBUG("hfp_hf_cmd_bldn:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}


static err_t hfp_hf_send_clcc(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CLCC);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_clcc:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_cops_format(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_COPS_F);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_cops_format:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_cops(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_COPS);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_cops:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_ccwa(struct hfp_pcb_t *pcb,uint8_t value)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HF_AT_CCWA,value);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_ccwa:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_cmee(struct hfp_pcb_t *pcb,uint8_t value)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HFP_AT_CMEE,value);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_cmee:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}


static err_t hfp_hf_send_clip(struct hfp_pcb_t *pcb,uint8_t value)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HF_AT_CLIP,value);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_clip:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_nrec(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_NREC_DISABLE);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_nrec:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_bvra(struct hfp_pcb_t *pcb,uint8_t value)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HF_AT_BVRA,value);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_bvra:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}


static err_t hfp_hf_send_binp(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_BINP);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_binps:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}


static err_t hfp_hf_send_vts(struct hfp_pcb_t *pcb,uint8_t value)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HF_AT_VTS,value);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_vts:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_vgm(struct hfp_pcb_t *pcb,uint8_t value)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HF_AT_VGM,value);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_vgm:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_vgs(struct hfp_pcb_t *pcb,uint8_t value)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HF_AT_VGS,value);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_vgs:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_cnum(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CNUM);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_cnum:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_query_btrh(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_BTRH_TEST);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_query_btrh:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_control_btrh(struct hfp_pcb_t *pcb,uint8_t value)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s%d\r\n", HF_AT_BTRH,value);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_control_btrh:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_bia(struct hfp_pcb_t *pcb,uint8_t index,uint8_t value)
{
    uint8_t temp_index = 0;
    uint8_t buffer[30] = {0};
    uint8_t buffer_offset = 0;
    sprintf((char *)buffer, "%s", HF_AT_BIA);
    buffer_offset += strlen((const char*)HF_AT_BIA);
    pcb->indicator_enable_map[index-1] = value;
    for(temp_index = 0; temp_index < pcb->number_of_indicator-1; temp_index++)
    {
        sprintf((char *)buffer+buffer_offset, "%d,", pcb->indicator_enable_map[temp_index]);
        buffer_offset += 2;
    }
    sprintf((char *)buffer+buffer_offset, "%d\r\n", pcb->indicator_enable_map[temp_index]);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_cnum:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_cgmi(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CGMI);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_cgmi:%s\n", buffer);
    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_cgmm(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CGMM);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_cgmm:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_cgmr(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CGMR);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_cgmr:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}

static err_t hfp_hf_send_cgsn(struct hfp_pcb_t *pcb)
{
    uint8_t buffer[20] = {0};
    sprintf((char *)buffer, "%s\r\n", HF_AT_CGSN);
    BT_HFP_TRACE_DEBUG("hfp_hf_send_cgsn:%s\n", buffer);

    return hfp_hf_send(pcb,buffer, strlen((const char*)buffer));
}


static err_t hfp_hf_handle_at_brsf(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    pcb->hfp_hf_remote_support_feature = bt_atoi_spec_size(data,data_len);

    if(hfp_ag_has_inbandring_feature(pcb))
        pcb->ag_support_inband_ring = 1;
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_brsf: remote feature 0x%02x ,support in band ring %d\n", \
                       pcb->hfp_hf_remote_support_feature,pcb->ag_support_inband_ring);


    return BT_ERR_OK;
}

static err_t hfp_hf_extract_indicator_order(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint16_t data_pos = 0;
    uint8_t indicator_index = 1;

    while(data_pos < data_len)
    {
        /* Get next indicator */
        while ((data[data_pos] != '"') && (data_pos < data_len))
            data_pos++;

        /* skip to true data */
        data_pos++;

        switch(data[data_pos])
        {
        case 'c':
        {
            if(data[data_pos+1] == 'a' && data[data_pos+2] == 'l' && data[data_pos+3] == 'l' && \
                    data[data_pos+4] == 's' && data[data_pos+5] == 'e' && data[data_pos+6] == 't' && \
                    data[data_pos+7] == 'u' && data[data_pos+8] == 'p')
            {
                pcb->call_setup_index = indicator_index++;
                BT_HFP_TRACE_DEBUG("hfp_hf_extract_indicator_order: callsetup ind index %d\n", pcb->call_setup_index);
            }
            else if(data[data_pos+1] == 'a' && data[data_pos+2] == 'l' && data[data_pos+3] == 'l' && \
                    data[data_pos+4] == 'h' && data[data_pos+5] == 'e' && data[data_pos+6] == 'l' && \
                    data[data_pos+7] == 'd')
            {
                pcb->call_held_index = indicator_index++;
                BT_HFP_TRACE_DEBUG("hfp_hf_extract_indicator_order: call_held_index ind index %d\n", pcb->call_held_index);

            }
            else if(data[data_pos+1] == 'a' && data[data_pos+2] == 'l' && data[data_pos+3] == 'l')
            {
                pcb->call_index = indicator_index++;
                BT_HFP_TRACE_DEBUG("hfp_hf_extract_indicator_order: call_index ind index %d\n", pcb->call_index);

            }
            break;
        }
        case 's':
        {
            if(data[data_pos+1] == 'e' && data[data_pos+2] == 'r' && data[data_pos+3] == 'v' &&\
                    data[data_pos+4] == 'i' && data[data_pos+5] == 'c' && data[data_pos+6] == 'e')
            {
                pcb->service_index = indicator_index++;
                BT_HFP_TRACE_DEBUG("hfp_hf_extract_indicator_order: service_index ind index %d\n", pcb->service_index);
            }
            else if(data[data_pos+1] == 'i' && data[data_pos+2] == 'g' && data[data_pos+3] == 'n' && \
                    data[data_pos+4] == 'a' && data[data_pos+5] == 'l')
            {
                pcb->signal_index = indicator_index++;
                BT_HFP_TRACE_DEBUG("hfp_hf_extract_indicator_order: signal_index ind index %d\n", pcb->signal_index);
            }
            break;
        }
        case 'r':
        {
            if(data[data_pos+1] == 'o' && data[data_pos+2] == 'a' && data[data_pos+3] == 'm')
            {
                pcb->roam_index = indicator_index++;
                BT_HFP_TRACE_DEBUG("hfp_hf_extract_indicator_order: roam_index ind index %d\n", pcb->roam_index);
            }
            break;
        }
        case 'b':
        {
            if(data[data_pos+1] == 'a' && data[data_pos+2] == 't' && data[data_pos+3] == 't' &&\
                    data[data_pos+4] == 'c' && data[data_pos+5] == 'h' && data[data_pos+6] == 'g')
            {
                pcb->battchg_index = indicator_index++;
                BT_HFP_TRACE_DEBUG("hfp_hf_extract_indicator_order: battchg_index ind index %d\n", pcb->battchg_index);
            }
            break;
        }
        default:
            break;
        }
    }
    pcb->number_of_indicator = indicator_index -1;

    return BT_ERR_OK;
}
static err_t hfp_hf_get_ind_value(uint8_t *data,uint16_t data_len,uint8_t ind_index,uint8_t *ret_value)
{
    if(ind_index == 1)
    {
        *ret_value = bt_atoi_spec_size(data,2);
    }
    else
    {
        uint8_t loop_cnt = ind_index -1;
        uint8_t *temp_data = data;
        while(loop_cnt--)
        {
            temp_data = (uint8_t *)strchr((const char*)temp_data,',');
            temp_data += 1;
        }
        *ret_value = bt_atoi_spec_size(temp_data,2);
    }

    return BT_ERR_OK;
}

static err_t hfp_hf_update_indicator_status(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t ind_value = 0;
    BT_HFP_TRACE_DEBUG("hfp_hf_update_indicator_status: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    if(pcb->call_index)
    {
        hfp_hf_get_ind_value(data,data_len,pcb->call_index,&ind_value);
        BT_HFP_TRACE_DEBUG("hfp_hf_update_indicator_status: call ind value %d\n", ind_value);

        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_call_status)
            hfp_hf_cbs->hfp_hf_call_status(&pcb->remote_addr,ind_value);
    }

    if(pcb->call_setup_index)
    {
        hfp_hf_get_ind_value(data,data_len,pcb->call_setup_index,&ind_value);
        BT_HFP_TRACE_DEBUG("hfp_hf_update_indicator_status: call_setup ind value %d\n", ind_value);

        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_call_setup_status)
            hfp_hf_cbs->hfp_hf_call_setup_status(&pcb->remote_addr,ind_value);
    }

    if(pcb->call_held_index)
    {
        hfp_hf_get_ind_value(data,data_len,pcb->call_held_index,&ind_value);
        BT_HFP_TRACE_DEBUG("hfp_hf_update_indicator_status: call_held ind value %d\n", ind_value);

        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_call_held_status)
            hfp_hf_cbs->hfp_hf_call_held_status(&pcb->remote_addr,ind_value);
    }

    if(pcb->service_index)
    {
        hfp_hf_get_ind_value(data,data_len,pcb->service_index,&ind_value);
        BT_HFP_TRACE_DEBUG("hfp_hf_update_indicator_status: service ind value %d\n", ind_value);
        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_server_status)
            hfp_hf_cbs->hfp_hf_server_status(&pcb->remote_addr,ind_value);
    }

    if(pcb->signal_index)
    {
        hfp_hf_get_ind_value(data,data_len,pcb->signal_index,&ind_value);
        BT_HFP_TRACE_DEBUG("hfp_hf_update_indicator_status: signal ind value %d\n", ind_value);
        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_signal_status)
            hfp_hf_cbs->hfp_hf_signal_status(&pcb->remote_addr,ind_value);
    }

    if(pcb->roam_index)
    {
        hfp_hf_get_ind_value(data,data_len,pcb->roam_index,&ind_value);
        BT_HFP_TRACE_DEBUG("hfp_hf_update_indicator_status: roam ind value %d\n", ind_value);

        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_roam_status)
            hfp_hf_cbs->hfp_hf_roam_status(&pcb->remote_addr,ind_value);
    }

    if(pcb->battchg_index)
    {
        hfp_hf_get_ind_value(data,data_len,pcb->battchg_index,&ind_value);
        BT_HFP_TRACE_DEBUG("hfp_hf_update_indicator_status: battchg ind value %d\n", ind_value);

        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_battchg_status)
            hfp_hf_cbs->hfp_hf_battchg_status(&pcb->remote_addr,ind_value);
    }

    return BT_ERR_OK;

}

static err_t hfp_hf_handle_at_cind(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{

    if(pcb->state == HFP_W4_RETRIEVE_INDICATORS)
    {
        hfp_hf_extract_indicator_order(pcb,data,data_len);
    }
    else if(pcb->state == HFP_W4_RETRIEVE_INDICATORS_STATUS)
    {
        hfp_hf_update_indicator_status(pcb,data,data_len);
    }

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_chld(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t *temp_data = data;
    uint16_t index = 0;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_chld: \n");
    bt_hex_dump(data,data_len);

    for(index = 0; index < data_len; index++)
    {
        if(*temp_data == '0')
            pcb->hfp_hf_remote_three_call_support_bitmap |= THREE_CALL_SERVICE_0;

        if(*temp_data == '1')
        {
            if(*(temp_data+1) == 'x')
                pcb->hfp_hf_remote_three_call_support_bitmap |= THREE_CALL_SERVICE_1x;
            else
                pcb->hfp_hf_remote_three_call_support_bitmap |= THREE_CALL_SERVICE_1;
        }

        if(*temp_data == '2')
        {
            if(*(temp_data+1) == 'x')
                pcb->hfp_hf_remote_three_call_support_bitmap |= THREE_CALL_SERVICE_2x;
            else
                pcb->hfp_hf_remote_three_call_support_bitmap |= THREE_CALL_SERVICE_2;
        }

        if(*temp_data == '3')
            pcb->hfp_hf_remote_three_call_support_bitmap |= THREE_CALL_SERVICE_3;

        if(*temp_data == '4')
            pcb->hfp_hf_remote_three_call_support_bitmap |= THREE_CALL_SERVICE_4;
        temp_data += 1;
    }

    BT_HFP_TRACE_DEBUG("hfp_hf_remote_three_call_support_bitmap: 0x%x\n",pcb->hfp_hf_remote_three_call_support_bitmap);
    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_bind(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_bind: \n");

    bt_hex_dump(data,data_len);

    if(pcb->state <= HFP_W4_RETRIEVE_GENERIC_STATUS_INDICATORS)
    {
        uint8_t *temp_start = (uint8_t *)strchr((const char *)data,'(');
        uint8_t *temp_end = (uint8_t *)strchr((const char *)data,',');
        if(temp_end != NULL)
        {
            uint8_t temp_len = temp_end - temp_start;
            uint8_t indicator = bt_atoi_spec_size(temp_start,temp_len);
            if(indicator == HFP_HF_INDICATOR_ENHANDCED_SAFETY)
                pcb->ag_support_enhandced_safety_indicator = 1;
            else if(indicator == HFP_HF_INDICATOR_BATTERY_LEVEL)
                pcb->ag_support_battery_level_indicator = 1;

            temp_start = temp_end;
            temp_end = (uint8_t *)strchr((const char *)data,')');
            indicator = bt_atoi_spec_size(temp_start,temp_len);
            if(indicator == HFP_HF_INDICATOR_ENHANDCED_SAFETY)
                pcb->ag_support_enhandced_safety_indicator = 1;
            else if(indicator == HFP_HF_INDICATOR_BATTERY_LEVEL)
                pcb->ag_support_battery_level_indicator = 1;

            BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_bind: %d %d\n",pcb->ag_support_enhandced_safety_indicator,pcb->ag_support_battery_level_indicator);
        }
    }
    else
    {
        uint8_t indicator;
        uint8_t value;
        uint8_t *temp_data = data;

        indicator = bt_atoi_spec_size(temp_data,2);

        temp_data = (uint8_t *)strchr((const char *)temp_data,',');
        value = bt_atoi_spec_size(temp_data,2);

        if(indicator == HFP_HF_INDICATOR_ENHANDCED_SAFETY)
            pcb->ag_enhandced_safety_indicator_enable = value;
        else if(indicator == HFP_HF_INDICATOR_BATTERY_LEVEL)
            pcb->ag_battery_level_indicator_enable = value;

        BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_bind1: %d %d\n",pcb->ag_enhandced_safety_indicator_enable,pcb->ag_battery_level_indicator_enable);


    }
    return BT_ERR_OK;
}

static err_t hfp_hf_get_ciev_index_value(uint8_t *data,uint8_t *index,uint8_t* values)
{
    uint8_t *temp_data = data;

    *index = bt_atoi_spec_size(temp_data,2);

    temp_data = (uint8_t *)strchr((const char *)temp_data,',');
    *values = bt_atoi_spec_size(temp_data,2);

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_ciev(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t ciev_index = 0;
    uint8_t ciev_value = 0;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ciev: len %d\n", data_len);

    bt_hex_dump(data,data_len);

    hfp_hf_get_ciev_index_value(data,&ciev_index,&ciev_value);
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ciev: index %d, value %d\n", ciev_index,ciev_value);

    if(pcb->call_index == ciev_index)
    {

        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_call_status)
            hfp_hf_cbs->hfp_hf_call_status(&pcb->remote_addr,ciev_value);
    }

    if(pcb->call_setup_index == ciev_index)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ciev: call_setup index value %d\n",ciev_value);

        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_call_setup_status)
            hfp_hf_cbs->hfp_hf_call_setup_status(&pcb->remote_addr,ciev_value);
    }

    if(pcb->call_held_index == ciev_index)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ciev: call_held index value %d\n",ciev_value);

        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_call_held_status)
            hfp_hf_cbs->hfp_hf_call_held_status(&pcb->remote_addr,ciev_value);
    }

    if(pcb->service_index == ciev_index)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ciev: service index value %d\n",ciev_value);

        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_server_status)
            hfp_hf_cbs->hfp_hf_server_status(&pcb->remote_addr,ciev_value);
    }

    if(pcb->signal_index == ciev_index)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ciev: signal index value %d\n",ciev_value);
        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_signal_status)
            hfp_hf_cbs->hfp_hf_signal_status(&pcb->remote_addr,ciev_value);
    }

    if(pcb->roam_index == ciev_index)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ciev: roam index value %d\n",ciev_value);
        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_roam_status)
            hfp_hf_cbs->hfp_hf_roam_status(&pcb->remote_addr,ciev_value);
    }

    if(pcb->battchg_index == ciev_index)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ciev: battchg index value %d\n",ciev_value);
        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_battchg_status)
            hfp_hf_cbs->hfp_hf_battchg_status(&pcb->remote_addr,ciev_value);
    }

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_cme(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t cme_err = bt_atoi_spec_size(data,3);
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_cops: len %d,err %d\n", data_len,cme_err);

    return BT_ERR_OK;
}


static err_t hfp_hf_handle_at_cops(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t *temp_data = data;
    uint8_t format,name_len;
    uint8_t *name_start;
    uint8_t *name_end;
    uint8_t mode = bt_atoi_spec_size(temp_data,2);
    temp_data += 2;
    format = bt_atoi_spec_size(temp_data,2);
    name_start = (uint8_t *)strchr((const char *)data,'"');
    name_start++;
    name_end = (uint8_t *)strchr((const char *)name_start+1,'"');
    name_len = name_end - name_start;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_cops: len %d\n", data_len);

    bt_hex_dump(data,data_len);

    memcpy(pcb->hfp_hf_remote_network_name,name_start,name_len);
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_cops: network name %s\n", pcb->hfp_hf_remote_network_name);
    bt_hex_dump(pcb->hfp_hf_remote_network_name,name_len);

    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_network)
        hfp_hf_cbs->hfp_hf_network(&pcb->remote_addr,mode,format,pcb->hfp_hf_remote_network_name,name_len);
    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_bcs(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t codec_select = 0;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_bcs: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    codec_select = bt_atoi_spec_size(data,2);
    pcb->audio_codec_select = codec_select;
    hfp_hf_codec_select_rsp(pcb,codec_select);
    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_ring(struct hfp_pcb_t *pcb)
{
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ring: \n");

    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_ring)
        hfp_hf_cbs->hfp_hf_ring(&pcb->remote_addr);
    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_clip(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    /* +CLIP: "10086",129 */
    uint8_t type;
    uint8_t *number_start = (uint8_t *)strchr((const char *)data,'"');
    uint8_t *number_end;
    uint8_t number_len;
    number_start++;
    number_end = (uint8_t *)strchr((const char *)number_start+1,'"');
    number_len = number_end - number_start;
    number_end += 2;
    type = bt_atoi_spec_size(number_end,3);
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_clip: len %d,type %d\n", data_len,type);

    bt_hex_dump(data,data_len);
    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_clip)
        hfp_hf_cbs->hfp_hf_clip(&pcb->remote_addr,number_start,number_len,type);

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_bsir(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t support_inband_ring = 0;
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_bsir: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    support_inband_ring = bt_atoi_spec_size(data,2);
    pcb->ag_support_inband_ring = support_inband_ring;
    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_ccwa(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t type;
    uint8_t *number_start = (uint8_t *)strchr((const char *)data,'"');
    uint8_t *number_end;
    uint8_t number_len;
    number_start++;
    number_end = (uint8_t *)strchr((const char *)number_start+1,'"');
    number_len = number_end - number_start;
    number_end += 2;
    type = bt_atoi_spec_size(number_end,3);

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_ccwa: len %d,type %d\n", data_len,type);

    bt_hex_dump(data,data_len);
    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_call_waiting)
        hfp_hf_cbs->hfp_hf_call_waiting(&pcb->remote_addr,number_start,number_len,type);

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_bvra(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t voice_reg_value = 0;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_bvra: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    voice_reg_value = bt_atoi_spec_size(data,2);
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_bvra: voice_reg_value %d\n", voice_reg_value);

    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_voice_recognition)
        hfp_hf_cbs->hfp_hf_voice_recognition(&pcb->remote_addr,voice_reg_value);
    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_binp(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_binp: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_vgm(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t mic_volume = 0;
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_vgm: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    mic_volume = bt_atoi_spec_size(data,2);

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_vgm: mic_volume %d\n", mic_volume);

    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_mic_vol)
        hfp_hf_cbs->hfp_hf_mic_vol(&pcb->remote_addr,mic_volume);
    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_vgs(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t spk_volume = 0;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_vgs: len %d\n", data_len);

    bt_hex_dump(data,data_len);

    spk_volume = bt_atoi_spec_size(data,2);
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_vgs: spk_volume %d\n", spk_volume);
    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_spk_vol)
        hfp_hf_cbs->hfp_hf_spk_vol(&pcb->remote_addr,spk_volume);
    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_cnum(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    /* +CNUM: ,"xxxxx",145,,4 */
    uint8_t type;
    uint8_t service;
    uint8_t *name_start = (uint8_t *)strchr((const char *)data,'"');
    uint8_t *name_end;
    uint8_t name_len;
    name_start++;

	if(*name_start != '"')
	{
	    name_end = (uint8_t *)strchr((const char *)name_start+1,'"');
	    name_len = name_end - name_start;
	    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_cnum: len %d\n", data_len);
	    bt_hex_dump(data,data_len);
	}
	else
	{
		name_end = name_start;
		name_len = 0;
		
	}

    name_end += 1;
    type = bt_atoi_spec_size(name_end,4);
    name_end += 5;
    service = bt_atoi_spec_size(name_end,2);
    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_local_number)
        hfp_hf_cbs->hfp_hf_local_number(&pcb->remote_addr,name_start,name_len,type,service);

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_btrh(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t hold_status = 0;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_btrh: len %d\n", data_len);

    bt_hex_dump(data,data_len);

    hold_status = bt_atoi_spec_size(data,2);

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_btrh: hold_status %d\n", hold_status);
    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_hold_status)
        hfp_hf_cbs->hfp_hf_hold_status(&pcb->remote_addr,hold_status);
    return BT_ERR_OK;
}


static err_t hfp_hf_handle_at_cgmi(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    /* +CGMI: "Apple Inc." */
    uint8_t *mid_start = (uint8_t *)strchr((const char *)data,'"');
    uint8_t *mid_end;
    uint8_t mid_len;
    mid_start++;
    mid_end = (uint8_t *)strchr((const char *)mid_start+1,'"');
    mid_len = mid_end - mid_start;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_cgmi: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_manufacturer_id)
        hfp_hf_cbs->hfp_hf_manufacturer_id(&pcb->remote_addr,mid_start,mid_len);

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_cgmm(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    /* +CGMM: iPhone10,3 */
    uint8_t *mid_start = (uint8_t *)strchr((const char *)data,' ');
    uint8_t *mid_end;
    uint8_t mid_len;
    mid_start++;
    mid_end = (uint8_t *)strchr((const char *)mid_start+1,0x0d);
    mid_len = mid_end - mid_start;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_cgmm: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_model_id)
        hfp_hf_cbs->hfp_hf_model_id(&pcb->remote_addr,mid_start,mid_len);

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_cgmr(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    /* +CGMR: Version 12.1.2 (Build 16C101) */
    uint8_t *rid_start = (uint8_t *)strchr((const char *)data,' ');
    uint8_t *rid_end;
    uint8_t rid_len;
    rid_start++;
    rid_end = (uint8_t *)strchr((const char *)rid_start+1,0x0d);
    rid_len = rid_end - rid_start;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_cgmr: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_revision_id)
        hfp_hf_cbs->hfp_hf_revision_id(&pcb->remote_addr,rid_start,rid_len);

    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_cgsn(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    /* +CGSN: 35484609376xxxx */
    uint8_t *pid_start = (uint8_t *)strchr((const char *)data,' ');
    pid_start++;
    uint8_t *pid_end = (uint8_t *)strchr((const char *)pid_start+1,0x0d);
    uint8_t pid_len = pid_end - pid_start;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_cgsn: len %d\n", data_len);
    bt_hex_dump(data,data_len);

    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_product_id)
        hfp_hf_cbs->hfp_hf_product_id(&pcb->remote_addr,pid_start,pid_len);

    return BT_ERR_OK;
}


static err_t hfp_hf_handle_at_clcc(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t idx,dir,status,mode,mpty,type;
    uint8_t *number;
    uint8_t *number_start;
    uint8_t *number_end;
    uint8_t number_len;
    uint8_t *temp_data = data;

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_clcc: len %d\n", data_len);

    bt_hex_dump(data,data_len);

    idx = bt_atoi_spec_size(temp_data,2);
    temp_data += 2;
    dir = bt_atoi_spec_size(temp_data,2);
    temp_data += 2;
    status = bt_atoi_spec_size(temp_data,2);
    temp_data += 2;
    mode = bt_atoi_spec_size(temp_data,2);
    temp_data += 2;
    mpty = bt_atoi_spec_size(temp_data,2);

    temp_data = (uint8_t *)strchr((const char*)temp_data,'"');
    number = number_start = temp_data+1;
    number_end = (uint8_t *)strchr((const char*)number_start,'"');
    number_len = number_end - number_start;

    type = bt_atoi_spec_size(number_end,5);

    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_clcc: idx:%d dir:%d status:%d mode:%d mpty:%d type:%d\n", idx,dir,status,mode,mpty,type);
    BT_HFP_TRACE_DEBUG("hfp_hf_handle_at_clcc: number len %d\n", number_len);


    bt_hex_dump(number,number_len);

    if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_call_list)
        hfp_hf_cbs->hfp_hf_call_list(&pcb->remote_addr,number,number_len,idx,dir,status,mode,mpty,type);
    return BT_ERR_OK;

}
static err_t hfp_hf_handle_at_error(struct hfp_pcb_t *pcb)
{
    return BT_ERR_OK;
}

static err_t hfp_hf_handle_at_ok(struct hfp_pcb_t *pcb)
{
    if(pcb->hfp_hf_slc_done == 0)
    {
        switch(pcb->state)
        {
        case HFP_W4_EXCHANGE_SUPPORTED_FEATURES:
            if(hfp_has_codec_negotiation_feature(pcb))
                pcb->state = HFP_NOTIFY_ON_CODECS;
            else
            {
                pcb->state = HFP_RETRIEVE_INDICATORS;
                pcb->audio_codec_select = HFP_CODEC_CVSD;
            }
            break;
        case HFP_W4_NOTIFY_ON_CODECS:
            pcb->state = HFP_RETRIEVE_INDICATORS;
            break;
        case HFP_W4_RETRIEVE_INDICATORS:
            pcb->state = HFP_RETRIEVE_INDICATORS_STATUS;
            break;
        case HFP_W4_RETRIEVE_INDICATORS_STATUS:
            pcb->state = HFP_ENABLE_INDICATORS_STATUS_UPDATE;
            break;
        case HFP_W4_ENABLE_INDICATORS_STATUS_UPDATE:
            if(hfp_has_three_call_feature(pcb))
                pcb->state = HFP_RETRIEVE_CAN_HOLD_CALL;
            else
            {
                if(hfp_has_hf_indicators_feature(pcb))
                    pcb->state = HFP_LIST_GENERIC_STATUS_INDICATORS;
                else
                    pcb->state = HFP_SERVICE_LEVEL_CONNECTION_ESTABLISHED;
            }
            break;
        case HFP_W4_RETRIEVE_CAN_HOLD_CALL:
            if(hfp_has_hf_indicators_feature(pcb))
                pcb->state = HFP_LIST_GENERIC_STATUS_INDICATORS;
            else
                pcb->state = HFP_SERVICE_LEVEL_CONNECTION_ESTABLISHED;
            break;
        case HFP_W4_LIST_GENERIC_STATUS_INDICATORS:
            pcb->state = HFP_RETRIEVE_GENERIC_STATUS_INDICATORS;
            break;
        case HFP_W4_RETRIEVE_GENERIC_STATUS_INDICATORS:
            pcb->state = HFP_RETRIEVE_INITITAL_STATE_GENERIC_STATUS_INDICATORS;
            break;
        case HFP_W4_RETRIEVE_INITITAL_STATE_GENERIC_STATUS_INDICATORS:
            pcb->state = HFP_SERVICE_LEVEL_CONNECTION_ESTABLISHED;
        default:
            break;
        }
        hfp_hf_run(pcb);
    }

    return BT_ERR_OK;
}

static err_t hfp_hf_parse_receive_data(struct hfp_pcb_t *pcb,uint8_t *data,uint16_t data_len)
{
    uint8_t *sub_str = NULL;

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_OK_RSP)) != NULL)
    {
        hfp_hf_handle_at_ok(pcb);
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_ERROR_RSP)) != NULL)
    {
        hfp_hf_handle_at_error(pcb);
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_BRSF_RSP)) != NULL)
    {
        hfp_hf_handle_at_brsf(pcb,sub_str+strlen((const char *)HF_AT_BRSF_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_BRSF_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CIND_RSP)) != NULL)
    {
        hfp_hf_handle_at_cind(pcb,sub_str+strlen((const char *)HF_AT_CIND_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CIND_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CHLD_RSP)) != NULL)
    {
        hfp_hf_handle_at_chld(pcb,sub_str+strlen((const char *)HF_AT_CHLD_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CHLD_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HFP_AT_BIND_RSP)) != NULL)
    {
        hfp_hf_handle_at_bind(pcb,sub_str+strlen((const char *)HFP_AT_BIND_RSP),data_len-(sub_str-data)-strlen((const char *)HFP_AT_BIND_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CIEV)) != NULL)
    {
        hfp_hf_handle_at_ciev(pcb,sub_str+strlen((const char *)HF_AT_CIEV),data_len-(sub_str-data)-strlen((const char *)HF_AT_CIEV));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HFP_AT_CME)) != NULL)
    {
        hfp_hf_handle_at_cme(pcb,sub_str+strlen((const char *)HFP_AT_CME),data_len-(sub_str-data)-strlen((const char *)HFP_AT_CME));
    }


    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_COPS_RSP)) != NULL)
    {
        hfp_hf_handle_at_cops(pcb,sub_str+strlen((const char *)HF_AT_COPS_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_COPS_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CLCC_RSP)) != NULL)
    {
        hfp_hf_handle_at_clcc(pcb,sub_str+strlen((const char *)HF_AT_CLCC_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CLCC_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_BCS)) != NULL)
    {
        hfp_hf_handle_at_bcs(pcb,sub_str+strlen((const char *)HF_AT_BCS),data_len-(sub_str-data)-strlen((const char *)HF_AT_BCS));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_RING)) != NULL)
    {
        hfp_hf_handle_at_ring(pcb);
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CLIP_RSP)) != NULL)
    {
        hfp_hf_handle_at_clip(pcb,sub_str+strlen((const char *)HF_AT_CLIP_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CLIP_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_BSIR)) != NULL)
    {
        hfp_hf_handle_at_bsir(pcb,sub_str+strlen((const char *)HF_AT_BSIR),data_len-(sub_str-data)-strlen((const char *)HF_AT_BSIR));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CCWA_RSP)) != NULL)
    {
        hfp_hf_handle_at_ccwa(pcb,sub_str+strlen((const char *)HF_AT_CCWA_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CCWA_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_BVRA_RSP)) != NULL)
    {
        hfp_hf_handle_at_bvra(pcb,sub_str+strlen((const char *)HF_AT_BVRA_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_BVRA_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_BINP_RSP)) != NULL)
    {
        hfp_hf_handle_at_binp(pcb,sub_str+strlen((const char *)HF_AT_BINP_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_BINP_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_VGM_RSP)) != NULL)
    {
        hfp_hf_handle_at_vgm(pcb,sub_str+strlen((const char *)HF_AT_VGM_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_VGM_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_VGS_RSP)) != NULL)
    {
        hfp_hf_handle_at_vgs(pcb,sub_str+strlen((const char *)HF_AT_VGS_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_VGS_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CNUM_RSP)) != NULL)
    {
        hfp_hf_handle_at_cnum(pcb,sub_str+strlen((const char *)HF_AT_CNUM_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CNUM_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_BTRH_RSP)) != NULL)
    {
        hfp_hf_handle_at_btrh(pcb,sub_str+strlen((const char *)HF_AT_BTRH_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_BTRH_RSP));
    }


    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CGMI_RSP)) != NULL)
    {
        hfp_hf_handle_at_cgmi(pcb,sub_str+strlen((const char *)HF_AT_CGMI_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CGMI_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CGMM_RSP)) != NULL)
    {
        hfp_hf_handle_at_cgmm(pcb,sub_str+strlen((const char *)HF_AT_CGMM_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CGMM_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CGMR_RSP)) != NULL)
    {
        hfp_hf_handle_at_cgmr(pcb,sub_str+strlen((const char *)HF_AT_CGMR_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CGMR_RSP));
    }

    if((sub_str = (uint8_t *)strstr((const char*)data,(const char*)HF_AT_CGSN_RSP)) != NULL)
    {
        hfp_hf_handle_at_cgsn(pcb,sub_str+strlen((const char *)HF_AT_CGSN_RSP),data_len-(sub_str-data)-strlen((const char *)HF_AT_CGSN_RSP));
    }

    return BT_ERR_OK;

}


static void hfp_hf_sdp_attributes_recv(void *arg, sdp_pcb_t *sdppcb, uint16_t attribl_bc, struct bt_pbuf_t *p)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(&(sdppcb->l2cappcb->remote_bdaddr));

    if(!hfppcb)
        return;

    /* Get the RFCOMM channel identifier from the protocol descriptor list */
    if((hfppcb->remote_cn = hfp_hf_get_rfcomm_cn(attribl_bc, p)) != 0)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_sdp_attributes_recv:remote cn %d\n",hfppcb->remote_cn);
    }

    hfppcb->state = HFP_W2_SDP_DISCONNECTD;
    hfp_hf_run(hfppcb);
}

static uint8_t hfp_hf_get_rfcomm_cn(uint16_t attribl_bc, struct bt_pbuf_t *attribute_list)
{
    uint8_t i;
    for(i = 0; i < attribl_bc; i++)
    {
        if(((uint8_t *)attribute_list->payload)[i] == (SDP_DE_TYPE_UUID | SDP_DE_SIZE_16))
        {
            if(bt_ntohs(*((uint16_t *)(((uint8_t *)attribute_list->payload)+i+1))) == 0x0003)
            {
                return *(((uint8_t *)attribute_list->payload)+i+4);
            }
        }
    }
    return 0;
}

static err_t l2cap_connect_cfm(void *arg, l2cap_pcb_t *l2cappcb, uint16_t result, uint16_t status)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(&(l2cappcb->remote_bdaddr));

    if(hfppcb)
    {

        if(result == L2CAP_CONN_SUCCESS)
        {
            BT_HFP_TRACE_DEBUG("l2cap_connect_cfm: L2CAP connected pcb->state = %d,psm %d\n", l2cappcb->state,l2cap_psm(l2cappcb));

            /* Tell L2CAP that we wish to be informed of a disconnection request */
            switch(l2cap_psm(l2cappcb))
            {
            case SDP_PSM:
                BT_HFP_TRACE_DEBUG("l2cap_connect_cfm: SDP L2CAP configured. Result = %d\n", result);

                if((hfppcb->sdppcb= sdp_new(l2cappcb)) == NULL)
                {
                    BT_HFP_TRACE_DEBUG("l2cap_connect_cfm: Failed to create a SDP PCB\n");
                    return BT_ERR_MEM;
                }
                hfppcb->state = HFP_W2_SDP_QUERY_RFCOMM_CHANNEL;
                hfppcb->sdppcb->l2cappcb = l2cappcb;
                l2cap_register_recv(l2cappcb, sdp_recv);
                hfp_hf_run(hfppcb);
                return BT_ERR_OK;
            case RFCOMM_PSM:
                if((hfppcb->rfcommpcb = rfcomm_new(l2cappcb)) == NULL)
                {
                    BT_HFP_TRACE_DEBUG("l2cap_connect_cfm: Failed to create a RFCOMM PCB\n");
                    return BT_ERR_MEM;
                }
                hfppcb->rfcommpcb->l2cappcb = l2cappcb;
                l2cap_register_recv(l2cappcb, rfcomm_input);
                hfppcb->state = HFP_W2_SERVER_CN_CONNECTED;
                hfp_hf_run(hfppcb);
                break;
            default:
                return BT_ERR_VAL;
            }
        }
        else
        {
            BT_HFP_TRACE_DEBUG("l2cap_connect_cfm: L2CAP not connected. Redo inquiry\n");

            l2cap_close(l2cappcb);
        }

        return BT_ERR_OK;
    }
    else
    {
        BT_HFP_TRACE_DEBUG("l2cap_connect_cfm: connect fail\n");


        HFP_PCB_RMV(&hfp_active_pcbs, hfppcb);
        hfp_close(hfppcb);
        return BT_ERR_CONN;
    }

}

static err_t l2cap_disconnect_cfm(void *arg, l2cap_pcb_t *pcb)
{
    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(&(pcb->remote_bdaddr));

    if(!hfppcb)
        return BT_ERR_CONN;

    BT_HFP_TRACE_DEBUG("l2cap_disconnected_cfm psm 0x%02x\n",l2cap_psm(pcb));

    switch(l2cap_psm(pcb))
    {
    case SDP_PSM:
        hfppcb->state = HFP_W2_RFCOMM_CONNECTED;
        hfp_hf_run(hfppcb);
        break;
    case RFCOMM_PSM:
        HFP_PCB_RMV(&hfp_active_pcbs, hfppcb);
        rfcomm_close(hfppcb->rfcommpcb);
        hfp_close(hfppcb);
        break;
    default:
        break;
    }
    l2cap_close(pcb);
    return BT_ERR_OK;
}

static err_t hfp_connect_cfm(void *arg, rfcomm_pcb_t *pcb, err_t err)
{

    struct hfp_pcb_t *hfppcb = hfp_get_active_pcb(&(pcb->l2cappcb->remote_bdaddr));

    if(err == BT_ERR_OK)
    {
        BT_HFP_TRACE_DEBUG("hfp_connect_cfm. CN = %d\n", rfcomm_cn(pcb));
        rfcomm_register_disc(pcb, hfp_hf_disconnected);
        rfcomm_register_recv(pcb, hfp_hf_recv);
        hfppcb->rfcommpcb = pcb;
        hfppcb->state = HFP_EXCHANGE_SUPPORTED_FEATURES;
        hfp_hf_run(hfppcb);
    }
    else
    {
        BT_HFP_TRACE_DEBUG("hfp_connect_cfm. Connection attempt failed CN = %d\n", rfcomm_cn(pcb));
        l2cap_close(pcb->l2cappcb);
        rfcomm_close(pcb);
    }
    return BT_ERR_OK;
}

static err_t hfp_connect_ind(void *arg, rfcomm_pcb_t *pcb, err_t err)
{
    struct hfp_pcb_t *hfppcb;

    BT_HFP_TRACE_DEBUG("hfp_connect_ind: CN = %d\n", rfcomm_cn(pcb));

    if((hfppcb = hfp_new(NULL)) == NULL)
    {
        BT_HFP_TRACE_DEBUG("hfp_connect_ind: Could not alloc rfcomm pcb\n");

        return BT_ERR_MEM;
    }
    hfppcb->hfp_hf_local_support_feature = hfp_hf_local_support_feature;
    bd_addr_set(&(hfppcb->remote_addr),&(pcb->l2cappcb->remote_bdaddr));
    HFP_PCB_REG(&hfp_active_pcbs, hfppcb);


    rfcomm_register_disc(pcb, hfp_hf_disconnected);
    if(pcb->cn == RFCOMM_HFP_HF_CHNL)
    {
        rfcomm_register_recv(pcb, hfp_hf_recv);
    }
    hfppcb->rfcommpcb = pcb;
    hfppcb->state = HFP_EXCHANGE_SUPPORTED_FEATURES;
    hfp_hf_run(hfppcb);
    return BT_ERR_OK;
}

static err_t hfp_hf_disconnected(void *arg, rfcomm_pcb_t *pcb, err_t err)
{
    struct hfp_pcb_t *hfppcb;

    BT_HFP_TRACE_DEBUG("hfp_hf_disconnected: CN = %d\n", rfcomm_cn(pcb));
    rfcomm_close(pcb);

    hfppcb = hfp_get_active_pcb(&(pcb->l2cappcb->remote_bdaddr));
    if(hfppcb)
    {
        BT_HFP_TRACE_DEBUG("hfp_hf_disconnected: close hfp active pcb\n");

        hfppcb->state = HFP_W4_RFCOMM_DISCONNECTED_AND_RESTART;
        hfp_hf_run(hfppcb);


    }
    return BT_ERR_OK;
}

static err_t hfp_hf_sco_connect_ind(void *arg, struct bd_addr_t *bdaddr)
{
    struct hfp_pcb_t *hfppcb;

    BT_HFP_TRACE_DEBUG("hfp_hf_sco_connect_ind: hfp sco req handle\n");

    bt_addr_dump(bdaddr->addr);

    hfppcb = hfp_get_active_pcb(bdaddr);
    if(!hfppcb)
        return BT_ERR_CONN;

    hfppcb->state = HFP_W2_ACCPET_CONNECT_SCO;
    hfp_hf_run(hfppcb);

    return BT_ERR_OK;
}

static err_t hfp_hf_sco_conn_complete(void *arg, uint8_t status,struct bd_addr_t *bdaddr)
{
    struct hfp_pcb_t *hfppcb;

    BT_HFP_TRACE_DEBUG("hfp_hf_sco_conn_complete: hfp sco conn complete status %d\n",status);

    bt_addr_dump(bdaddr->addr);
    hfppcb = hfp_get_active_pcb(bdaddr);
    if(!hfppcb)
        return BT_ERR_CONN;

    hfp_hf_run(hfppcb);

    return BT_ERR_OK;
}

static err_t  sco_hfp_disconn_complete(void *arg, uint8_t status,struct bd_addr_t *bdaddr)
{
    struct hfp_pcb_t *hfppcb;

    BT_HFP_TRACE_DEBUG("sco_hfp_disconn_complete: hfp sco disconn complete status %d\n",status);

    bt_addr_dump(bdaddr->addr);
    hfppcb = hfp_get_active_pcb(bdaddr);
    if(!hfppcb)
        return BT_ERR_CONN;

    hfppcb->state = HFP_SCO_DISCONNECTED;
    hfp_hf_run(hfppcb);

    return BT_ERR_OK;
}

static err_t hfp_hf_run(struct hfp_pcb_t *pcb)
{
    BT_HFP_TRACE_DEBUG("hfp_hf_run: state %d\n", pcb->state);

    switch(pcb->state)
    {
    case HFP_W2_SDP_CONNECTED:
        pcb->hfp_hf_local_support_feature = hfp_hf_local_support_feature;
        HFP_PCB_REG(&hfp_active_pcbs, pcb);
        if((pcb->l2cappcb = l2cap_new()) == NULL)
        {
            BT_HFP_TRACE_DEBUG("hfp_hf_run: Could not alloc L2CAP pcb\n");
            return BT_ERR_MEM;
        }
        l2cap_connect_req(pcb->l2cappcb, &(pcb->remote_addr), SDP_PSM, HCI_ALLOW_ROLE_SWITCH, l2cap_connect_cfm);
        break;
    case HFP_W2_SDP_QUERY_RFCOMM_CHANNEL:
    {
        uint8_t hfp[] = {0x35, 0x03, 0x19, 0x11, 0x1f}; /* Service search pattern with HFP UUID is default */

        uint8_t attrids[] = {0x35, 0x03, 0x09, 0x00, 0x04}; /* Attribute IDs to search for in data element
		sequence form */

        sdp_service_search_attrib_req(pcb->sdppcb, 0xFFFF, hfp, sizeof(hfp),
                                      attrids, sizeof(attrids), hfp_hf_sdp_attributes_recv);
        break;
    }
    case HFP_W2_SDP_DISCONNECTD:
        l2cap_disconnect_req(pcb->sdppcb->l2cappcb, l2cap_disconnect_cfm);
        sdp_free(pcb->sdppcb);
        break;
    case HFP_W2_RFCOMM_CONNECTED:

        if((pcb->l2cappcb = l2cap_new()) == NULL)
        {
            BT_HFP_TRACE_DEBUG("hfp_hf_run: Could not alloc L2CAP pcb\n");
            return BT_ERR_MEM;
        }
        l2cap_connect_req(pcb->l2cappcb, &(pcb->remote_addr), RFCOMM_PSM, HCI_ALLOW_ROLE_SWITCH, l2cap_connect_cfm);
        break;
    case HFP_W2_SERVER_CN_CONNECTED:
        rfcomm_connect(pcb->rfcommpcb, pcb->remote_cn, hfp_connect_cfm);
        break;
    case HFP_EXCHANGE_SUPPORTED_FEATURES:
        pcb->state = HFP_W4_EXCHANGE_SUPPORTED_FEATURES;
        hfp_hf_exchange_supported_features(pcb);
        break;
    case HFP_NOTIFY_ON_CODECS:
        pcb->state = HFP_W4_NOTIFY_ON_CODECS;
        hfp_hf_notify_on_codecs(pcb);
        break;
    case HFP_RETRIEVE_INDICATORS:
        pcb->state = HFP_W4_RETRIEVE_INDICATORS;
        hfp_hf_retrieve_indicators(pcb);
        break;
    case HFP_RETRIEVE_INDICATORS_STATUS:
        pcb->state = HFP_W4_RETRIEVE_INDICATORS_STATUS;
        hfp_hf_retrieve_indicators_status(pcb);
        break;
    case HFP_ENABLE_INDICATORS_STATUS_UPDATE:
        pcb->state = HFP_W4_ENABLE_INDICATORS_STATUS_UPDATE;
        hfp_hf_activate_ind_report(pcb);
        break;
    case HFP_RETRIEVE_CAN_HOLD_CALL:
        pcb->state = HFP_W4_RETRIEVE_CAN_HOLD_CALL;
        hfp_hf_retrieve_can_hold_call(pcb);
        break;
    case HFP_LIST_GENERIC_STATUS_INDICATORS:
        pcb->state = HFP_W4_LIST_GENERIC_STATUS_INDICATORS;
        hfp_hf_tx_hf_indicator(pcb);
        break;
    case HFP_RETRIEVE_GENERIC_STATUS_INDICATORS:
        pcb->state = HFP_W4_RETRIEVE_GENERIC_STATUS_INDICATORS;
        hfp_hf_rx_ag_indicator(pcb);
        break;
    case HFP_RETRIEVE_INITITAL_STATE_GENERIC_STATUS_INDICATORS:
        pcb->state = HFP_W4_RETRIEVE_INITITAL_STATE_GENERIC_STATUS_INDICATORS;
        hfp_hf_get_ag_indicator_enable(pcb);
        break;
    case HFP_SERVICE_LEVEL_CONNECTION_ESTABLISHED:
        pcb->hfp_hf_slc_done = 1;
        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_connect_set_up)
            hfp_hf_cbs->hfp_hf_connect_set_up(&pcb->remote_addr,BT_ERR_OK);
        break;
    case HFP_W2_DISCONNECT_RFCOMM:
        rfcomm_disconnect(pcb->rfcommpcb);
        //l2cap_disconnect_req(pcb->rfcommpcb->l2cappcb, l2cap_disconnect_cfm);
        break;
    case HFP_W4_RFCOMM_DISCONNECTED_AND_RESTART:
        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_connect_realease)
            hfp_hf_cbs->hfp_hf_connect_realease(&pcb->remote_addr,BT_ERR_OK);
        pcb->state = HFP_IDLE;
        HFP_PCB_RMV(&hfp_active_pcbs, pcb);
        hfp_close(pcb);
        break;
    case HFP_W2_CONNECT_SCO:
        if(hfp_has_codec_negotiation_feature(pcb))
            hfp_hf_codec_connect(pcb);
        else
            hci_connect_sco(&(pcb->remote_addr),8000,8000,0x0c,0x60,0x01,0x2bf);
        pcb->state = HFP_W4_SCO_CONNECTED;
        break;
    case HFP_W2_ACCPET_CONNECT_SCO:
        if(pcb->audio_codec_select == HFP_CODEC_CVSD)
            hci_accept_sco_req(&(pcb->remote_addr),8000,8000,0x0c,0x60,0x01,0x2bf);
        else
            hci_accept_sco_req(&(pcb->remote_addr),8000,8000,0x0d,0x63,0x02,0x2bf);
        pcb->state = HFP_W4_SCO_CONNECTED;
        break;
    case HFP_W4_SCO_CONNECTED:
        /* TODO；write voice setting */
        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_sco_connect_set_up)
            hfp_hf_cbs->hfp_hf_sco_connect_set_up(&pcb->remote_addr,BT_ERR_OK);
        pcb->state = HFP_AUDIO_CONNECTION_ESTABLISHED;
        break;
    case HFP_W2_DISCONNECT_SCO:
        hci_disconnect_sco(&(pcb->remote_addr), HCI_OTHER_END_TERMINATED_CONN_USER_ENDED);
        pcb->state = HFP_W4_SCO_DISCONNECTED;
        break;
    case HFP_SCO_DISCONNECTED:
        if(hfp_hf_cbs && hfp_hf_cbs->hfp_hf_sco_connect_realease)
            hfp_hf_cbs->hfp_hf_sco_connect_realease(&pcb->remote_addr,BT_ERR_OK);
        pcb->state = HFP_SERVICE_LEVEL_CONNECTION_ESTABLISHED;
        break;
    default:
        break;
    }

    return BT_ERR_OK;
}

#endif


