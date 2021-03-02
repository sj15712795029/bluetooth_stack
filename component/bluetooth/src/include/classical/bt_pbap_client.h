/******************************************************************************
  * @file           bt_pbap_client.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt pbap client header file
******************************************************************************/

#ifndef BT_PBAP_CLIENT_H_H_H
#define BT_PBAP_CLIENT_H_H_H

#include "bt_common.h"

#if PROFILE_PBAP_ENABLE


#include "bt_l2cap.h"
#include "bt_sdp.h"
#include "bt_rfcomm.h"
#include "bt_obex_client.h"


#define PBAP_NONE_REPOSITORY 0
#define PB_LOCAL_REPOSITORY 1
#define PB_SIM_REPOSITORY 2

#define PBAP_NONE_TYPE 0
#define PB_PHONEBOOK_TYPE 1
#define PB_INCOMING_BOOK_TYPE 2
#define PB_OUTGOING_BOOK_TYPE 3
#define PB_MISSING_BOOK_TYPE 4
#define PB_COMBINE_BOOK_TYPE 5

#define PBAP_DN_PB_NONE 0
#define PBAP_DN_PB_START 1
#define PBAP_DN_PB_CONTINUE 2
#define PBAP_DN_PB_END 3
#define PBAP_DN_PB_ABORT 4
#define PBAP_DN_VCARD_LIST_NONE 0
#define PBAP_DN_VCARD_LIST_START 1
#define PBAP_DN_VCARD_LIST_CONTINUE 2
#define PBAP_DN_VCARD_LIST_END 3
#define PBAP_DN_VCARD_LIST_ABORT 4



#define PBAP_APP_PARAM_ORDER 0x01 /*Order - 0x01 - 1 byte: 0x00 = indexed 0x01 = alphanumeric 0x02 = phonetic */
#define PBAP_APP_PARAM_SEARCH_VALUE 0x02 /* SearchValue - 0x02 - variable - Text */
#define PBAP_APP_PARAM_SEARCH_PROPERTY 0x03 /* SearchProperty - 0x03 - 1 byte - 0x00= Name 0x01= Number 0x02= Sound */
#define PBAP_APP_PARAM_MAX_LIST_COUNT 0x04 /* MaxListCount - 0x04 - 2 bytes - 0x0000 to 0xFFFF */
#define PBAP_APP_PARAM_LIST_START_OFFSET 0x05 /* ListStartOffset - 0x05 - 2 bytes - 0x0000 to 0xFFFF */
#define PBAP_APP_PARAM_PROPERTY_SELECTOR 0x06 /* PropertySelector - 0x06 - 8 bytes - 64 bits mask */
#define PBAP_APP_PARAM_FORMAT 0x07 /* Format - 0x07 - 1 byte - 0x00 = 2.1 0x01 = 3.0 */
#define PBAP_APP_PARAM_PHONEBOOK_SIZE 0x08 /* PhonebookSize - 0x08 - 2 bytes - 0x0000 to 0xFFFF */
#define PBAP_APP_PARAM_NEW_MISSED_CALLS 0x09 /* NewMissedCalls - 0x09 - 1 byte - 0x00 to 0xFF */
#define PBAP_APP_PARAM_PRIMARY_VERSION_COUNTER 0x0A /* PrimaryVersionCounter - 0x0A - 16 bytes - 0 to (2pow128 - 1) */
#define PBAP_APP_PARAM_SECONDARY_VERSION_COUNTER 0x0B /* SecondaryVersionCounter - 0x0B - 16 bytes - 0 to (2pow128 - 1) */
#define PBAP_APP_PARAM_VCARD_SELECTOR 0x0C /* vCardSelector - 0x0C - 8 bytes - 64 bits mask */
#define PBAP_APP_PARAM_DATABASE_IDENTIFIER 0x0D /* DatabaseIdentifier - 0x0D - 16 bytes - 0 to (2pow128 - 1) */
#define PBAP_APP_PARAM_VCARD_SELECTOR_OPERATOR 0x0E /* vCardSelectorOperator - 0x0E - 1 byte - 0x00 = OR 0x01 = AND */
#define PBAP_APP_PARAM_RESET_NEW_MISSED_CALLS 0x0F /* ResetNewMissedCalls -   0x0F -  1 byte */
#define PBAP_APP_PARAM_PBAP_SUPPORTED_FEATURES 0x10 /* PbapSupportedFeatures - 0x10 - 4 bytes */

#define PBAP_VCARD_FORMAT2_1 0x00
#define PBAP_VCARD_FORMAT3_0 0x01

/* PBAP Property Mask - also used for vCardSelector */
#define PBAP_PROPERTY_MASK_VERSION              (1<< 0) /* vCard Version                       */
#define PBAP_PROPERTY_MASK_FN                   (1<< 1) /* Formatted Name                      */
#define PBAP_PROPERTY_MASK_N                    (1<< 2) /* Structured Presentation of Name     */
#define PBAP_PROPERTY_MASK_PHOTO                (1<< 3) /* Associated Image or Photo           */
#define PBAP_PROPERTY_MASK_BDAY                 (1<< 4) /* Birthday                            */
#define PBAP_PROPERTY_MASK_ADR                  (1<< 5) /* Delivery Address                    */
#define PBAP_PROPERTY_MASK_LABEL                (1<< 6) /* Delivery                            */
#define PBAP_PROPERTY_MASK_TEL                  (1<< 7) /* Telephone Number                    */
#define PBAP_PROPERTY_MASK_EMAIL                (1<< 8) /* Electronic Mail Address             */
#define PBAP_PROPERTY_MASK_MAILER               (1<< 9) /* Electronic Mail                     */
#define PBAP_PROPERTY_MASK_TZ                   (1<<10) /* Time Zone                           */
#define PBAP_PROPERTY_MASK_GEO                  (1<<11) /* Geographic Position                 */
#define PBAP_PROPERTY_MASK_TITLE                (1<<12) /* Job                                 */
#define PBAP_PROPERTY_MASK_ROLE                 (1<<13) /* Role within the Organization        */
#define PBAP_PROPERTY_MASK_LOGO                 (1<<14) /* Organization Logo                   */
#define PBAP_PROPERTY_MASK_AGENT                (1<<15) /* vCard of Person Representing        */
#define PBAP_PROPERTY_MASK_ORG                  (1<<16) /* Name of Organization                */
#define PBAP_PROPERTY_MASK_NOTE                 (1<<17) /* Comments                            */
#define PBAP_PROPERTY_MASK_REV                  (1<<18) /* Revision                            */
#define PBAP_PROPERTY_MASK_SOUND                (1<<19) /* Pronunciation of Name               */
#define PBAP_PROPERTY_MASK_URL                  (1<<20) /* Uniform Resource Locator            */
#define PBAP_PROPERTY_MASK_UID                  (1<<21) /* Unique ID                           */
#define PBAP_PROPERTY_MASK_KEY                  (1<<22) /* Public Encryption Key               */
#define PBAP_PROPERTY_MASK_NICKNAME             (1<<23) /* Nickname                            */
#define PBAP_PROPERTY_MASK_CATEGORIES           (1<<24) /* Categories                          */
#define PBAP_PROPERTY_MASK_PROID                (1<<25) /* Product ID                          */
#define PBAP_PROPERTY_MASK_CLASS                (1<<26) /* Class information                   */
#define PBAP_PROPERTY_MASK_SORT_STRING          (1<<27) /* String used for sorting operations  */
#define PBAP_PROPERTY_MASK_X_IRMC_CALL_DATETIME (1<<28) /* Time stamp                          */
#define PBAP_PROPERTY_MASK_X_BT_SPEEDDIALKEY    (1<<29) /* Speed-dial shortcut                 */
#define PBAP_PROPERTY_MASK_X_BT_UCI             (1<<30) /* Uniform Caller Identifier           */
#define PBAP_PROPERTY_MASK_X_BT_UID             (1<<31) /* Bluetooth Contact Unique Identifier */


#define PBAP_PROPERTY_MASK 0xffffffffffffffff
#define PBAP_PROPERTY_MASK_DEFAULT ((PBAP_PROPERTY_MASK_VERSION |PBAP_PROPERTY_MASK_FN |\
                               PBAP_PROPERTY_MASK_N | PBAP_PROPERTY_MASK_TEL) & PBAP_PROPERTY_MASK)

typedef enum
{
    PBAP_IDLE = 0,
    PBAP_W2_SDP_CONNECTED,
    PBAP_W2_SDP_QUERY_RFCOMM_CHANNEL,
    PBAP_W4_SDP_QUERY_COMPLETE,
    PBAP_W2_SDP_DISCONNECTD,
    PBAP_W4_SDP_DISCONNECTD,
    PBAP_W2_RFCOMM_CONNECTED,
    PBAP_W4_RFCOMM_CONNECTED,

    PBAP_W2_SERVER_CN_CONNECTED,
    PBAP_W4_SERVER_CN_CONNECTED,

    PBAP_SERVER_CN_CONNECTED,
    PBAP_W2_OBEX_CONNECTED,
    PBAP_W4_OBEX_CONNECTED,
    PBAP_OBEX_CONNECTED,

    PBAP_OPERATE_IDLE,
    PBAP_OPERATE_PULL_PHONEBOOK,
    PBAP_OPERATE_QUERY_PHONEBOOK_SIZE,
    PBAP_OPERATE_SET_PATH,
    PBAP_OPERATE_PULL_VCARD_LIST,
    PBAP_OPERATE_PULL_VCARD_ENTRY,
    PBAP_OPERATE_ABORT,

    PBAP_W2_DISCONNECT_RFCOMM,
    PBAP_W4_RFCOMM_DISCONNECTED,
    PBAP_W4_RFCOMM_DISCONNECTED_AND_RESTART,
    PBAP_W4_CONNECTION_ESTABLISHED_TO_SHUTDOWN
} pbap_state_e;



typedef struct
{
    void (*pbap_client_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*pbap_client_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*pbap_query_repositories_size)(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint16_t size);
	void (*pbap_download_phonebook_status)(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t status);
	void (*pbap_download_phonebook_data)(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t *data,uint16_t data_len);
	void (*pbap_download_vcardlist_status)(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t status);
	void (*pbap_download_vcardlist_data)(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t *data,uint16_t data_len);
	void (*pbap_download_vcardentry_data)(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint16_t entry_number,uint8_t *data,uint16_t data_len);
} pbap_client_cbs_t;

struct pbap_pcb_t
{
    struct pbap_pcb_t *next; /* For the linked list */
    rfcomm_pcb_t *rfcommpcb;
    sdp_pcb_t *sdppcb;
    l2cap_pcb_t*l2cappcb;

    struct bd_addr_t remote_addr;

    uint32_t cid;
    uint8_t remote_cn;
    pbap_state_e state;

	uint8_t dn_operate;

    uint8_t current_repositories;
    uint8_t current_type;

	uint8_t query_repositories;
	uint8_t query_type;

	uint8_t dn_pb_repositories;
	uint8_t dn_pb_type;

	uint16_t dn_ventry_number;
};

err_t pbap_client_init(pbap_client_cbs_t *cb);
err_t pbap_client_connect(struct bd_addr_t *addr);
err_t pbap_client_disconnect(struct bd_addr_t *addr);
err_t pbap_client_download_phonebook(struct bd_addr_t *addr,uint8_t repositories,uint8_t type);
err_t pbap_client_query_phonebook_size(struct bd_addr_t *addr,uint8_t repositories,uint8_t type);
err_t pbap_client_set_path(struct bd_addr_t *addr,uint8_t repositories,uint8_t type);
err_t pbap_client_download_vcard_list(struct bd_addr_t *addr,uint8_t repositories,uint8_t type);
err_t pbap_client_download_vcard_entry(struct bd_addr_t *addr,uint8_t repositories,uint8_t type,uint16_t entry_number);
err_t pbap_client_download_abort(struct bd_addr_t *addr);

#endif

#endif

