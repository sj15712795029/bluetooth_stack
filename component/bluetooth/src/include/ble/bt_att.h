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

#define ATT_UUID16_FORMAT 1
#define ATT_UUID128_FORMAT 2

#define ATT_ERR_RSP_PACK_LEN 5
#define ATT_EXCHANGE_MTU_PACK_LEN 3
#define ATT_FIND_INFO_REQ_LEN 5
#define ATT_FIND_INFO_RSP_HDR_LEN 2
#define ATT_FIND_INFO_VALUE_TYPE_RSP_PACK_LEN 5
#define ATT_READ_TYPE_RSP_PACK_LEN 2
#define ATT_READ_RSP_HDR_LEN 1
#define ATT_READ_BLOB_RSP_HDR_LEN 1
#define ATT_READ_GROUP_TYPE_RSP_HDR_LEN 2
#define ATT_WRITE_RSP_PACK_LEN 1
#define ATT_SEND_NOTIFICATION_HDR_LEN 3
#define ATT_SEND_INDICATION_HDR_LEN 3
#define ATT_READ_GROUP_UUID16_TYPE_REQ_HDR_LEN 7
#define ATT_READ_GROUP_UUID128_TYPE_REQ_HDR_LEN 21
#define ATT_FIND_TYPE_VALUE_REQ_HDR_LEN 7
#define ATT_READ_TYPE_REQ_HDR_LEN 5
#define ATT_READ_REQ_PACK_LEN 3

typedef struct _att_pcb_t
{
    struct _att_pcb_t *next; /* For the linked list */
	struct bd_addr_t remote_addr;
    l2cap_pcb_t*l2cappcb;

}att_pcb_t;


typedef struct
{
    void (*att_connect_set_up)(att_pcb_t *att_pcb,uint8_t status);
    void (*att_connect_realease)(att_pcb_t *att_pcb,uint8_t status);
    void (*att_data_recv)(att_pcb_t *att_pcb,struct bt_pbuf_t *p);
} att_cbs_t;


/* ATT common API */
err_t att_init(att_cbs_t *cb);


/* ATT server API */
err_t att_err_rsp(uint8_t req_op,uint16_t handle,uint8_t err_code);
err_t att_mtu_rsp(uint16_t server_mtu);
err_t att_find_info_rsp(uint8_t uuid_format,uint8_t *info_data,uint8_t info_len);
err_t att_find_info_value_type_rsp(uint16_t found_handle,uint16_t end_group_handle);
err_t att_read_type_rsp(uint8_t *data_list,uint8_t data_list_len);
err_t att_read_rsp(uint8_t *att_value,uint8_t att_value_len);
err_t att_read_blob_rsp(uint8_t *att_value,uint8_t att_value_len);
err_t att_read_multi_rsp(void);
err_t att_read_group_type_rsp(uint8_t *att_dlist,uint8_t att_dlist_len);
err_t att_multi_var_rsp(void);
err_t att_write_rsp(void);
err_t att_pre_write_rsp(void);
err_t att_exc_write_rsp(void);
err_t att_notification(uint16_t handle,uint8_t *att_value,uint8_t att_value_len);
err_t att_indication(uint16_t handle,uint8_t *att_value,uint8_t att_value_len);

/* ATT Server parse */
err_t att_parse_mtu_req(struct bt_pbuf_t *p,uint16_t *client_mtu);
err_t att_parse_find_info_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle);
err_t att_parse_find_info_type_value_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle,
	uint16_t *att_type,uint8_t *value,uint8_t *value_len);
err_t att_parse_read_type_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle,
	uint8_t *uuid_format,uint16_t *uuid,uint8_t **uuid128);
err_t att_parse_read_req(struct bt_pbuf_t *p,uint16_t *handle);
err_t att_parse_read_blob_req(struct bt_pbuf_t *p,uint16_t *handle,uint16_t *offset);
err_t att_parse_read_multi_req(void);
err_t att_parse_read_group_type_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle,uint16_t *uuid);;
err_t att_parse_multi_var_req(void);
err_t att_parse_write_req(struct bt_pbuf_t *p,uint16_t *handle,uint8_t *att_value,uint8_t *value_len);
err_t att_parse_write_cmd(struct bt_pbuf_t *p,uint16_t *handle,uint8_t *att_value,uint8_t *value_len);
err_t att_parse_sig_write_cmd(void);
err_t att_parse_pre_write_req(void);
err_t att_parse_exc_write_req(void);


/* ATT client API */
err_t att_mtu_req(att_pcb_t *att_pcb,uint16_t client_mtu);
err_t att_find_info_req(uint16_t start_handle,uint16_t end_handle);
err_t att_find_type_value_req(uint16_t start_handle,uint16_t end_handle,uint16_t uuid,uint8_t *value,uint8_t value_len);
err_t att_read_type_req(uint16_t start_handle,uint16_t end_handle,uint8_t *value,uint8_t value_len);
err_t att_read_req(uint16_t handle);
err_t att_read_group_type_req(uint16_t start_handle,uint16_t end_handle,uint16_t uuid16,uint8_t *uuid128);
/* ATT client parse */
err_t att_parse_mtu_rsp(struct bt_pbuf_t *p,uint16_t *server_mtu);
err_t att_parse_mtu_rsp(struct bt_pbuf_t *p,uint16_t *server_mtu);
err_t att_parse_read_type_rsp(struct bt_pbuf_t *p,uint8_t *each_len,uint8_t *data_num,uint8_t **data_list,uint8_t *uuid_type);
err_t att_parse_read_group_type_rsp(struct bt_pbuf_t *p,uint8_t *each_len,uint8_t *data_num,uint8_t **data_list,uint8_t *uuid_type);
err_t att_parse_find_type_value_rsp(struct bt_pbuf_t *p,uint8_t *info_num,uint8_t **info_list);


#endif

