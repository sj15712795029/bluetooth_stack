/******************************************************************************
  * @file           bt_memp.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-14
  * @brief          bt memory pool source file
******************************************************************************/

#include "bt_memp.h"
#include "bt_hci.h"
#include "bt_l2cap.h"
#include "bt_sdp.h"
#include "bt_rfcomm.h"
#include "bt_obex_client.h"
#include "bt_avdtp.h"
#include "bt_avctp.h"
#include "bt_hfp_hf.h"
#include "bt_a2dp_sink.h"
#include "bt_avrcp_controller.h"
#include "bt_spp.h"
#include "bt_pbap_client.h"
#include "bt_hid_device.h"


struct bt_memp_t
{
    struct bt_memp_t *next;
};

static struct bt_memp_t *memp_tab[MEMP_BT_MAX];

static const uint16_t memp_sizes[MEMP_BT_MAX] =
{
    sizeof(hci_pcb_t),
    sizeof(hci_link_t),
    sizeof(hci_inq_res_t),
    sizeof(l2cap_pcb_t),
    sizeof(l2cap_pcb_listen_t),
    sizeof(l2cap_sig_t),
    sizeof(l2cap_seg_t),
    sizeof(struct sdp_pcb_t),
    sizeof(struct sdp_record_t),
    sizeof(struct rfcomm_pcb_t),
    sizeof(struct rfcomm_pcb_listen_t),
    sizeof(struct obex_pcb_t),
    sizeof(struct avdtp_pcb_t),
    sizeof(struct avdtp_sep_t),
    sizeof(struct avctp_pcb_t),
    sizeof(struct hfp_pcb_t),
    sizeof(struct a2dp_pcb_t),
    sizeof(struct avrcp_pcb_t),
    sizeof(struct spp_pcb_t),
    sizeof(struct pbap_pcb_t),
    sizeof(struct hid_device_pcb_t),
    PBUF_POOL_BUFSIZE,
};

static const uint16_t memp_num[MEMP_BT_MAX] =
{
    MEMP_NUM_HCI_PCB,
    MEMP_NUM_HCI_LINK,
    MEMP_NUM_HCI_INQ,
    MEMP_NUM_L2CAP_PCB,
    MEMP_NUM_L2CAP_PCB_LISTEN,
    MEMP_NUM_L2CAP_SIG,
    MEMP_NUM_L2CAP_SEG,
    MEMP_NUM_SDP_PCB,
    MEMP_NUM_SDP_RECORD,
    MEMP_NUM_RFCOMM_PCB,
    MEMP_NUM_RFCOMM_PCB_LISTEN,
    MEMP_NUM_OBEX,
    MEMP_NUM_AVDTP,
    MEMP_NUM_AVDTP_SEP,
    MEMP_NUM_AVCTP,
    MEMP_NUM_HFP,
    MEMP_NUM_A2DP,
    MEMP_NUM_AVRCP,
    MEMP_NUM_SPP,
    MEMP_NUM_PBAP,
    MEMP_NUM_HID,
    MEMP_NUM_BT_PBUF,
};

static uint8_t memp_memory[(MEMP_NUM_HCI_PCB *
                         MEM_ALIGN_SIZE(sizeof(hci_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_HCI_LINK *
                         MEM_ALIGN_SIZE(sizeof(hci_link_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_HCI_INQ *
                         MEM_ALIGN_SIZE(sizeof(hci_inq_res_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_L2CAP_PCB *
                         MEM_ALIGN_SIZE(sizeof(l2cap_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_L2CAP_PCB_LISTEN *
                         MEM_ALIGN_SIZE(sizeof(l2cap_pcb_listen_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_L2CAP_SIG *
                         MEM_ALIGN_SIZE(sizeof(l2cap_sig_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_L2CAP_SEG *
                         MEM_ALIGN_SIZE(sizeof(l2cap_seg_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_SDP_PCB *
                         MEM_ALIGN_SIZE(sizeof(struct sdp_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_SDP_RECORD *
                         MEM_ALIGN_SIZE(sizeof(struct sdp_record_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_RFCOMM_PCB *
                         MEM_ALIGN_SIZE(sizeof(struct rfcomm_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_RFCOMM_PCB_LISTEN *
                         MEM_ALIGN_SIZE(sizeof(struct rfcomm_pcb_listen_t) +
                                        sizeof(struct bt_memp_t)) +
                          MEMP_NUM_OBEX *
                         MEM_ALIGN_SIZE(sizeof(struct obex_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_AVDTP *
                         MEM_ALIGN_SIZE(sizeof(struct avdtp_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_AVDTP_SEP *
                         MEM_ALIGN_SIZE(sizeof(struct avdtp_sep_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_AVCTP *
                         MEM_ALIGN_SIZE(sizeof(struct avctp_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_HFP *
                         MEM_ALIGN_SIZE(sizeof(struct hfp_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_A2DP *
                         MEM_ALIGN_SIZE(sizeof(struct a2dp_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_AVRCP *
                         MEM_ALIGN_SIZE(sizeof(struct avrcp_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_SPP *
                         MEM_ALIGN_SIZE(sizeof(struct spp_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_PBAP *
                         MEM_ALIGN_SIZE(sizeof(struct pbap_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_HID *
                         MEM_ALIGN_SIZE(sizeof(struct hid_device_pcb_t) +
                                        sizeof(struct bt_memp_t)) +
                         MEMP_NUM_BT_PBUF *
                         MEM_ALIGN_SIZE(0x500))];

/******************************************************************************
 * func name   : bt_memp_init
 * para        : VOID
 * return      : VOID
 * description : 蓝牙内存池的初始化
******************************************************************************/
void bt_memp_init(void)
{
    struct bt_memp_t *m, *memp;
    uint16_t i, j;
    uint16_t size;

    memp = (struct bt_memp_t *)&memp_memory[0];
    for(i = 0; i < MEMP_BT_MAX; ++i)
    {
        size = MEM_ALIGN_SIZE(memp_sizes[i] + sizeof(struct bt_memp_t));
        if(memp_num[i] > 0)
        {
            memp_tab[i] = memp;
            m = memp;

            for(j = 0; j < memp_num[i]; ++j)
            {
                m->next = (struct bt_memp_t *)MEM_ALIGN((uint8_t *)m + size);
                memp = m;
                m = m->next;
            }
            memp->next = NULL;
            memp = m;
        }
        else
        {
            memp_tab[i] = NULL;
        }
    }
}
/******************************************************************************
 * func name   : bt_memp_malloc
 * para        : type(IN) --> 申请内存池的类型
 * return      : VOID
 * description : 从内存池中申请空间
******************************************************************************/
void *bt_memp_malloc(bt_memp_t type)
{
    struct bt_memp_t *memp;
    void *mem;

    memp = memp_tab[type];

    if(memp != NULL)
    {
        memp_tab[type] = memp->next;
        memp->next = NULL;

        mem = MEM_ALIGN((uint8_t *)memp + sizeof(struct bt_memp_t));
        /* initialize memp memory with zeroes */
        memset(mem, 0, memp_sizes[type]);
        return mem;
    }
    else
    {
        return NULL;
    }
}

/******************************************************************************
 * func name   : bt_memp_free
 * para        : type(IN) --> 申请内存池的类型
 					 mem(IN) --> 要释放的内存
 * return      : VOID
 * description : 释放内存回归到内存池
******************************************************************************/
void bt_memp_free(bt_memp_t type, void *mem)
{
    struct bt_memp_t *memp;

    if(mem == NULL)
    {
        return;
    }
    memp = (struct bt_memp_t *)((uint8_t *)mem - sizeof(struct bt_memp_t));

    memp->next = memp_tab[type];
    memp_tab[type] = memp;

    return;
}


