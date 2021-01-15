/******************************************************************************
  * @file           bt_att.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2021-1-13
  * @brief          bt ble att server role header file
******************************************************************************/

#ifndef BT_ATT_H_H_H
#define BT_ATT_H_H_H

#include "bt_common.h"
#include "bt_l2cap.h"

#define ATT_RSP_ERROR 0x01
#define ATT_REQ_MTU 0x02
#define ATT_RSP_MTU 0x03
#define ATT_REQ_FIND_INFO 0x04
#define ATT_RSP_FIND_INFO 0x05
#define ATT_REQ_FIND_TYPE_VALUE 0x06
#define ATT_RSP_FIND_TYPE_VALUE 0x07
#define ATT_REQ_READ_BY_TYPE 0x08
#define ATT_RSP_READ_BY_TYPE 0x09
#define ATT_REQ_READ 0x0A
#define ATT_RSP_READ 0x0B
#define ATT_REQ_READ_BLOB 0x0C
#define ATT_RSP_READ_BLOB 0x0D
#define ATT_REQ_READ_MULTI 0x0E
#define ATT_RSP_READ_MULTI 0x0F
#define ATT_REQ_READ_BY_GRP_TYPE 0x10
#define ATT_RSP_READ_BY_GRP_TYPE 0x11
/*                0001-0010 (write)*/
#define ATT_REQ_WRITE 0x12
#define ATT_RSP_WRITE 0x13
/* changed in V4.0 01001-0010(write cmd)*/
#define ATT_CMD_WRITE 0x52
#define ATT_REQ_PREPARE_WRITE 0x16
#define ATT_RSP_PREPARE_WRITE 0x17
#define ATT_REQ_EXEC_WRITE 0x18
#define ATT_RSP_EXEC_WRITE 0x19
#define ATT_HANDLE_VALUE_NOTIF 0x1B
#define ATT_HANDLE_VALUE_IND 0x1D
#define ATT_HANDLE_VALUE_CONF 0x1E
/* changed in V4.0 1101-0010 (signed write)  see write cmd above*/
#define ATT_SIGN_CMD_WRITE 0xD2

/* Success code and error codes */
#define ATT_SUCCESS 0x00
#define ATT_INVALID_HANDLE 0x01
#define ATT_READ_NOT_PERMIT 0x02
#define ATT_WRITE_NOT_PERMIT 0x03
#define ATT_INVALID_PDU 0x04
#define ATT_INSUF_AUTHENTICATION 0x05
#define ATT_REQ_NOT_SUPPORTED 0x06
#define ATT_INVALID_OFFSET 0x07
#define ATT_INSUF_AUTHORIZATION 0x08
#define ATT_PREPARE_Q_FULL 0x09
#define ATT_NOT_FOUND 0x0a
#define ATT_NOT_LONG 0x0b
#define ATT_INSUF_KEY_SIZE 0x0c
#define ATT_INVALID_ATTR_LEN 0x0d
#define ATT_ERR_UNLIKELY 0x0e
#define ATT_INSUF_ENCRYPTION 0x0f
#define ATT_UNSUPPORT_GRP_TYPE 0x10
#define ATT_INSUF_RESOURCE 0x11



typedef struct
{
    void (*att_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*att_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*att_data_recv)(struct bd_addr_t *remote_addr,struct bt_pbuf_t *p);
} att_cbs_t;


err_t att_init(void);
err_t att_register_cb(att_cbs_t *cb);
err_t att_send_data(struct bt_pbuf_t *p);
err_t att_parse_read_req(struct bt_pbuf_t *p,uint16_t *handle);
err_t att_parse_find_info_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle);
err_t att_parse_read_type_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle,uint16_t *uuid);
err_t att_parse_read_group_type_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle,uint16_t *uuid);


#endif

