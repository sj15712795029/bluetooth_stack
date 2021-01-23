/******************************************************************************
  * @file           bt_phybusif.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt phybusif header file
******************************************************************************/

#ifndef BT_PHYBUSIF_H_H_H
#define BT_PHYBUSIF_H_H_H

#include "bt_pbuf.h"
#include "bt_common.h"
#include "bt_hci.h"


#define PHYBUSIF_PACKET_TYPE_CMD  0x01
#define PHYBUSIF_PACKET_TYPE_ACL_DATA  0x02
#define PHYBUSIF_PACKET_TYPE_SCO_DATA  0x03
#define PHYBUSIF_PACKET_TYPE_EVT  0x04
#define PHYBUSIF_PACKET_TYPE_ISO  0x05

struct phybusif_cb;

/* Application program's interface: */
void phybusif_open(uint32_t baud_rate); /* Must be called first to initialize the physical bus interface */
void phybusif_reopen(uint32_t baud_rate);
void phybusif_close(void);
err_t phybusif_reset(struct phybusif_cb *cb);
err_t phybusif_input(struct phybusif_cb *cb);

/* Upper layer interface: */
void phybusif_output(struct bt_pbuf_t *p, uint16_t len,uint8_t packet_type);

enum phybusif_state
{
    W4_PACKET_TYPE,
    W4_EVENT_HDR,
    W4_ACL_HDR,
    W4_SCO_HDR,
    W4_EVENT_PARAM,
    W4_ACL_DATA,
    W4_SCO_DATA,
};

/* The physical bus interface control block */
struct phybusif_cb
{
	int phyuart_fd;
    enum phybusif_state state;

    uint32_t offset;
    /* p is header node,q is temp node*/
    struct bt_pbuf_t *p;
    struct bt_pbuf_t *q;

    hci_evt_hdr_t *evhdr;
    hci_acl_hdr_t *aclhdr;

};

#endif

