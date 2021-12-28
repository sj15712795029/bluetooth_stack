/******************************************************************************
  * @file           bt_smp.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2021-11-27
  * @brief          bt ble smp header file
******************************************************************************/

#ifndef BT_SMP_H_H_H
#define BT_SMP_H_H_H

#include "bt_common.h"
#include "bt_l2cap.h"
#include "bt_hci.h"

/* SMP command code */
#define SMP_OPCODE_PAIRING_REQ            0x01
#define SMP_OPCODE_PAIRING_RSP            0x02
#define SMP_OPCODE_CONFIRM                0x03
#define SMP_OPCODE_RAND                   0x04
#define SMP_OPCODE_PAIRING_FAILED         0x05
#define SMP_OPCODE_ENCRYPT_INFO           0x06
#define SMP_OPCODE_MASTER_ID              0x07
#define SMP_OPCODE_IDENTITY_INFO          0x08
#define SMP_OPCODE_ID_ADDR                0x09
#define SMP_OPCODE_SIGN_INFO              0x0A
#define SMP_OPCODE_SEC_REQ                0x0B
#define SMP_OPCODE_PAIR_PUBLIC_KEY        0x0C
#define SMP_OPCODE_PAIR_DHKEY_CHECK       0x0D
#define SMP_OPCODE_PAIR_KEYPR_NOTIF       0x0E

/* pairing failure reason code */
#define SMP_PASSKEY_ENTRY_FAIL      0x01
#define SMP_OOB_FAIL                0x02
#define SMP_PAIR_AUTH_FAIL          0x03
#define SMP_CONFIRM_VALUE_ERR       0x04
#define SMP_PAIR_NOT_SUPPORT        0x05
#define SMP_ENC_KEY_SIZE            0x06
#define SMP_INVALID_CMD             0x07
#define SMP_PAIR_FAIL_UNKNOWN       0x08
#define SMP_REPEATED_ATTEMPTS       0x09
#define SMP_INVALID_PARAMETERS      0x0A
#define SMP_DHKEY_CHK_FAIL          0x0B
#define SMP_NUMERIC_COMPAR_FAIL     0x0C
#define SMP_BR_PARING_IN_PROGR      0x0D
#define SMP_XTRANS_DERIVE_NOT_ALLOW 0x0E
#define SMP_KEY_REJECTED 0x0F

#define SMP_MODEL_ENCRYPTION_ONLY  0   /* Legacy mode, Just Works model */
#define SMP_MODEL_PASSKEY       1   /* Legacy mode, Passkey Entry model, this side inputs the key */
#define SMP_MODEL_OOB           2   /* Legacy mode, OOB model */
#define SMP_MODEL_KEY_NOTIF     3   /* Legacy mode, Passkey Entry model, this side displays the key */
#define SMP_MODEL_SEC_CONN_JUSTWORKS  4  /* Secure Connections mode, Just Works model */
#define SMP_MODEL_SEC_CONN_NUM_COMP   5  /* Secure Connections mode, Numeric Comparison model */
#define SMP_MODEL_SEC_CONN_PASSKEY_ENT 6 /* Secure Connections mode, Passkey Entry model, this side inputs the key */
#define SMP_MODEL_SEC_CONN_PASSKEY_DISP 7   /* Secure Connections mode, Passkey Entry model, this side displays the key */
#define SMP_MODEL_SEC_CONN_OOB  8   /* Secure Connections mode, OOB model */
#define SMP_MODEL_OUT_OF_RANGE  9


/* IO capability */
#define SMP_IO_CAP_DISPLAY_ONLY 0
#define SMP_IO_CAP_DISPLAY_YESNO  1
#define SMP_IO_CAP_KEYBORAD_ONLY 2
#define SMP_IO_CAP_NOIO 3
#define SMP_IO_CAP_KBDISP 4

/* OOB data flag */
#define SMP_OOB_NONE 0
#define SMP_OOB_PRESENT 1

/* AuthReq */
#define SMP_NO_BONDING (0 << 0)
#define SMP_BONDING (1 << 0)
#define SMP_AUTH_MIMT_BIT (1 << 2)
#define SMP_SC_SUPPORT_BIT (1 << 3)
#define SMP_KP_SUPPORT_BIT (1 << 4)
#define SMP_H7_SUPPORT_BIT (1 << 5)


/* SMP key types */
#define SMP_SEC_KEY_TYPE_ENC (1 << 0)    /* encryption key */
#define SMP_SEC_KEY_TYPE_ID (1 << 1)    /* identity key */
#define SMP_SEC_KEY_TYPE_CSRK (1 << 2)    /* slave CSRK */
#define SMP_SEC_KEY_TYPE_LK (1 << 3)    /* BR/EDR link key */

#define SMP_PAIR_REQ_PACK_LEN 7
#define SMP_PAIR_RSP_PACK_LEN 7
#define SMP_PAIR_CONFIRM_PACK_LEN 17
#define SMP_PAIR_RANDOM_PACK_LEN 17
#define SMP_PAIR_PUBLIC_KEY_PLEN 65
#define SMP_PAIR_DHKEY_CHECK 17
#define SMP_PAIR_FAIL_PACK_LEN 2
#define SMP_ENC_INFO_PACK_LEN 17
#define SMP_MASTER_ID_PACK_LEN 11
#define SMP_ID_INFO_PACK_LEN 17
#define SMP_ID_ADDR_INFO_PACK_LEN 8
#define SMP_SECURITY_REQ_PLEN 2

#define SMP_FLAG_PAIRING (1<<0)


typedef struct 
{
	uint8_t is_sc;
	struct bd_addr_t remote_addr;
	uint8_t ltk[16];
}smp_device_info_t;


typedef struct _smp_pcb_t
{
    struct _smp_pcb_t *next; /* For the linked list */
    struct bd_addr_t remote_addr;
    l2cap_pcb_t*l2cappcb;
	
	smp_device_info_t device_info;
	
	uint8_t flag;
	
	/* SMP pairing phase 1 step */
	uint8_t pair_req_buf[SMP_PAIR_REQ_PACK_LEN];
	uint8_t pair_rsp_buf[SMP_PAIR_RSP_PACK_LEN];
	
	uint8_t remote_io_cap;
    uint8_t remote_oob_flag;
    uint8_t remote_auth_req;
    uint8_t remote_enc_size;
    uint8_t remote_i_key;
    uint8_t remote_r_key;

    uint8_t local_io_cap;
    uint8_t local_oob_flag;
    uint8_t local_auth_req;
    uint8_t local_enc_size;
    uint8_t local_i_key;
    uint8_t local_r_key;
	
	uint8_t use_sc;
	uint8_t pairing_method;

	/* SMP pairing phase 2 step:legacy */	
	uint8_t tk[16];					/* Legacy pairing tk */
	uint8_t local_random[16];		/* Legacy pairing local random */
	uint8_t local_confirm[16]; 	/* Legacy pairing local confirm */
	uint8_t remote_random[16];	/* Legacy pairing remote random */
	uint8_t remote_confirm[16]; /* Legacy pairing remote confirm */
	uint8_t stk[16];					/* Legacy pairing short term key */
	uint8_t ltk[16];					/* Legacy pairing long term key */

	/* SMP pairing phase 2 step:security connection */	
	uint8_t local_sc_public_key[64];
	uint8_t remote_sc_public_key[64];

	uint32_t sc_passkey;
	uint8_t sc_mackey[16];
	uint8_t sc_ltk[16];
	uint8_t remote_dhkey_check[16];

	uint8_t sc_na[16];
	uint8_t sc_nb[16];
	uint32_t sc_vb;

	uint8_t sc_local_dhkey[32];
	
} smp_pcb_t;


typedef struct
{
    void (*smp_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*smp_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*smp_passkey_display)(struct bd_addr_t *remote_addr,uint32_t passkey);
	void (*smp_passkey_input)(struct bd_addr_t *remote_addr,uint32_t *passkey);
	void (*smp_ltk_generate)(smp_device_info_t *device_info);
	void (*smp_ltk_request)(smp_device_info_t *device_info,uint8_t *has_device);
} smp_cbs_t;


err_t smp_init(smp_cbs_t *cb);
err_t smp_security_request(struct bd_addr_t *remote_addr);


#endif

