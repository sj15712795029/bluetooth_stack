/******************************************************************************
  * @file           bt_hci.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-15
  * @brief          bt hci source file
******************************************************************************/

#include "bt_phybusif_h4.h"
#include "bt_hci.h"
#include "bt_l2cap.h"
#include "bt_timer.h"
#if BT_VENDOR_CSR8X11_SUPPORT
#include "bt_vendor_csr8x11.h"
#endif
#if BT_VENDOR_BCM43430A1_SUPPORT
#include "bt_vendor_bcm43430a1.h"
#endif
#if BT_VENDOR_BCM4345C5_SUPPORT
#include "bt_vendor_bcm4345c5.h"
#endif

/* The HCI LINK lists. */
static hci_link_t *hci_active_links;  /* List of all active HCI LINKs */
static hci_link_t *hci_tmp_link;
static hci_pcb_t *hci_pcb = NULL;

#define HCI_EVENT_PIN_REQ(pcb,bdaddr) \
                         if((pcb)->pin_req != NULL) { \
                           ((pcb)->pin_req((pcb)->callback_arg,(bdaddr))); \
                         } else { \
                           hci_pin_code_request_neg_reply(bdaddr); \
			 }
#define HCI_BT_WORKING(pcb) \
                              if((pcb)->bt_working != NULL) \
                              ((pcb)->bt_working((pcb)->callback_arg))
#define HCI_EVENT_SCO_REQ(pcb,bdaddr) \
                              if((pcb)->sco_conn_req != NULL) \
                              ((pcb)->sco_conn_req((pcb)->callback_arg,(bdaddr)))
#define HCI_EVENT_SCO_CONN_COMPLETE(pcb,status,bdaddr) \
                              if((pcb)->sco_conn_complete != NULL) \
                              ((pcb)->sco_conn_complete((pcb)->callback_arg,status,(bdaddr)))
#define HCI_EVENT_SCO_DISCONN_COMPLETE(pcb,status,bdaddr) \
                              if((pcb)->sco_disconn_complete != NULL) \
                              ((pcb)->sco_disconn_complete((pcb)->callback_arg,status,(bdaddr)))
#define HCI_EVENT_LINK_REQ(pcb,bdaddr) \
                              if((pcb)->link_key_req != NULL) { \
                                ((pcb)->link_key_req((pcb)->callback_arg,(bdaddr))); \
                              	}
#define HCI_EVENT_LINK_KEY_NOT(pcb,bdaddr,key,key_type) \
                              if((pcb)->link_key_not != NULL) { \
                                ((pcb)->link_key_not((pcb)->callback_arg,(bdaddr),(key),(key_type))); \
                              }
#define HCI_EVENT_INQ_RESULT(pcb,result)\
					if((pcb)->inq_result != NULL) \
                              ((pcb)->inq_result((result)))
#define HCI_EVENT_INQ_COMPLETE(pcb,result) \
                              if((pcb)->inq_complete != NULL) \
                              ((pcb)->inq_complete((result)))
#define HCI_EVENT_LE_INQ_RESULT(pcb,result)\
							  if((pcb)->le_inq_result != NULL) \
							 ((pcb)->le_inq_result((result)))
#define HCI_EVENT_LE_INQ_COMPLETE(pcb,result) \
                              if((pcb)->le_inq_complete != NULL) \
                              ((pcb)->le_inq_complete((result)))
#define HCI_EVENT_REMOTE_NAME_REQ_COMPLETE(pcb,bdaddr,name) \
                              if((pcb)->name_req_complete != NULL) \
                              ((pcb)->name_req_complete((bdaddr),(name)))
#define HCI_EVENT_RBD_COMPLETE(pcb,bdaddr) \
                              if((pcb)->rbd_complete != NULL) \
                              ((pcb)->rbd_complete((pcb)->callback_arg,(bdaddr)));
#define HCI_EVENT_WLP_COMPLETE(pcb,bdaddr) \
                               if((pcb)->wlp_complete != NULL) \
                               ((pcb)->wlp_complete((pcb)->callback_arg,(bdaddr)));
#define HCI_EVENT_CONN_COMPLETE(pcb,bdaddr) \
                               if((pcb)->conn_complete != NULL) \
                               ((pcb)->conn_complete((pcb)->callback_arg,(bdaddr)));
#define HCI_EVENT_CMD_COMPLETE(pcb,opcode,result) \
                              if((pcb)->cmd_complete != NULL) \
                              ((pcb)->cmd_complete((pcb)->callback_arg,(opcode),(result)))
#define HCI_EVENT_HARDWARE_ERROR(pcb,reson) \
 							if((pcb)->hardware_error != NULL) \
 							((pcb)->hardware_error((reson)))
#define HCI_EVENT_LTK_REQ(pcb,bdaddr,random,ediv) \
 							if((pcb)->ltk_req != NULL) \
 							((pcb)->ltk_req((bdaddr),(random),(ediv)))
#define HCI_EVENT_ENC_CHANGE(pcb,bdaddr,enc) \
							if((pcb)->enc_change != NULL) \
							((pcb)->enc_change((bdaddr),(enc)))
#define HCI_EVENT_PUBLIC_KEY(pcb,key) \
							if((pcb)->public_key != NULL) \
							((pcb)->public_key((key)))
#define HCI_EVENT_DHKEY_COMPLETE(pcb,key) \
							if((pcb)->dhkey_complete != NULL) \
							((pcb)->dhkey_complete((key)))




#define HCI_REG(links, nlink) do { \
                            nlink->next = *links; \
                            *links = nlink; \
                            } while(0)
#define HCI_RMV(links, nlink) do { \
                            if(*links == nlink) { \
                               *links = (*links)->next; \
                            } else for(hci_tmp_link = *links; hci_tmp_link != NULL; hci_tmp_link = hci_tmp_link->next) { \
                               if(hci_tmp_link->next != NULL && hci_tmp_link->next == nlink) { \
                                  hci_tmp_link->next = nlink->next; \
                                  break; \
                               } \
                            } \
                            nlink->next = NULL; \
                            } while(0)

static hci_link_t *_hci_new_link(void);
static err_t _hci_delete_link(hci_link_t *link);
static hci_link_t *_hci_get_link_by_addr(struct bd_addr_t *bdaddr);
static hci_link_t *_hci_get_link_by_handle(uint16_t handle);
static uint8_t *_hci_get_error_code(uint8_t code);
static err_t _hci_reset_cmd_timeout(void *para);
static err_t _hci_pin_req_handle(void *arg, struct bd_addr_t *bdaddr);
static err_t _hci_set_chip_name(uint8_t *name);
static void _hci_vendor_init_done(uint8_t vendor_status);
static err_t _hci_vendor_init(uint8_t ogf,uint8_t ocf);
static err_t _hci_init_process(struct bt_pbuf_t *p);
static err_t _hci_init_cmd_compl_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_inq_comp_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_inq_result_evt_process(uint8_t evt_code,uint8_t *payload,uint16_t payload_len);
static err_t _hci_conn_comp_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_conn_req_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_sco_conn_comp_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_disconn_comp_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_remote_name_req_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_encryption_change_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_qos_setup_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_cmd_comp_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_cmd_status_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_hw_err_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_role_change_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_number_comp_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_mode_change_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_data_buf_overflow_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_max_slot_change_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_pincode_req_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_linkkey_req_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_linkkey_notify_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_io_cap_req_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_usr_confim_req_evt_process(uint8_t *payload,uint16_t payload_len);
static err_t _hci_vendor_evt_process(uint8_t *payload,uint16_t payload_len);
#if BT_BLE_ENABLE > 0
static err_t _hci_le_meta_evt_process(uint8_t *payload,uint16_t payload_len);
#endif



err_t hci_init(void)
{
    if((hci_pcb = bt_memp_malloc(MEMP_HCI_PCB)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    memset(hci_pcb, 0, sizeof(hci_pcb_t));

    hci_pcb->ssp_enable = BT_ENABLE_SSP;
    hci_pcb->ssp_io_cap = BT_SSP_IOCAP_CONF;
    hci_pcb->class_of_device = BT_CLASS_OF_DEVICE;
    hci_pcb->local_name = (const uint8_t *)BT_LOCAL_NAME;
    hci_pcb->pincode = (uint8_t *)BT_PIN_CODE;

    hci_register_pin_req(_hci_pin_req_handle);
    /* Clear globals */
    hci_active_links = NULL;
    hci_tmp_link = NULL;

#if BT_VENDOR_CSR8X11_SUPPORT > 0
    _hci_set_chip_name((uint8_t *)VENDOR_CSR8X11_NAME);
#endif

#if BT_VENDOR_BCM43430A1_SUPPORT > 0
    _hci_set_chip_name((uint8_t *)VENDOR_BCM43430A1_NAME);
#endif

#if BT_VENDOR_BCM4345C5_SUPPORT > 0
    _hci_set_chip_name((uint8_t *)VENDOR_BCM4345C5_NAME);
#endif

#if BT_VENDOR_CYW54591_SUPPORT > 0
	_hci_set_chip_name((uint8_t *)VENDOR_CYW54591_NAME);
#endif

#if BT_VENDOR_CYW43438_SUPPORT > 0
	_hci_set_chip_name((uint8_t *)VENDOR_CYW43438_NAME);
#endif

#if BT_VENDOR_COMMON_NO_FW_SUPPORT > 0
	_hci_set_chip_name((uint8_t *)VENDOR_H4_NO_FW_NAME);
#endif


    return BT_ERR_OK;
}


void hci_deinit(void)
{
    hci_link_t *link, *tlink;
    hci_inq_res_t *ires, *tires;

    hci_pcb->chip_mgr->vendor_deinit();

    for(link = hci_active_links; link != NULL;)
    {
        tlink = link->next;
        _hci_delete_link(link);
        link = tlink;
    }

    for(ires = hci_pcb->ires; ires != NULL;)
    {
        tires = ires->next;
        bt_memp_free(MEMP_HCI_INQ, ires);
        ires = tires;
    }
    bt_memp_free(MEMP_HCI_PCB, hci_pcb);

    hci_active_links = NULL;
    hci_tmp_link = NULL;


}


void hci_register_cmd_complete(cmd_complete_fun_cb cmd_complete)

{
    hci_pcb->cmd_complete = cmd_complete;
}

void hci_register_pin_req(pin_req_fun_cb pin_req)
{
    hci_pcb->pin_req = pin_req;
}


void hci_register_bt_working(bt_working_fun_cb bt_working)
{
    hci_pcb->bt_working = bt_working;
}

void hci_register_sco_req(sco_conn_req_fun_cb sco_conn_req)
{
    hci_pcb->sco_conn_req = sco_conn_req;
}

void hci_register_sco_conn_complete(sco_conn_complete_fun_cb sco_conn_complete)
{
    hci_pcb->sco_conn_complete = sco_conn_complete;
}

void hci_register_sco_disconn_complete(sco_disconn_complete_fun_cb sco_disconn_complete)
{
    hci_pcb->sco_disconn_complete = sco_disconn_complete;
}

void hci_register_link_key_req(link_key_req_fun_cb link_key_req)
{
    hci_pcb->link_key_req= link_key_req;
}


void hci_register_link_key_not(link_key_not_fun_cb link_key_not)
{
    hci_pcb->link_key_not = link_key_not;
}

void hci_register_connection_complete(conn_complete_fun_cb conn_complete)
{
    hci_pcb->conn_complete = conn_complete;
}

void hci_register_hardware_error(hardware_error_fun_cb hardware_error)
{
	hci_pcb->hardware_error = hardware_error;
}

void hci_register_ltk_req(ltk_request_fun_cb ltk_req)
{
	hci_pcb->ltk_req = ltk_req;
}

void hci_register_enc_change(enc_change_fun_cb enc_change)
{
	hci_pcb->enc_change = enc_change;
}

void hci_register_public_key(local_p256_public_key_fun_cb public_key)
{
	hci_pcb->public_key = public_key;
}

void hci_register_dhkey_complete(dhkey_complete_fun_cb dhkey_complete)
{
	hci_pcb->dhkey_complete = dhkey_complete;
}


void hci_register_write_policy_complete(wlp_complete_fun_cb wlp_complete)
{
    hci_pcb->wlp_complete = wlp_complete;
}


uint8_t hci_get_version(void)
{
	return hci_pcb->hci_version;
}

uint8_t *hci_get_local_addr(void)
{
	return hci_pcb->local_bd_addr.addr;
}



static void _hci_vendor_init_done(uint8_t vendor_status)
{
    BT_HCI_TRACE_DEBUG("_hci_vendor_init_done %d\n",vendor_status);

    if(vendor_status == VENDOR_STATUS_INITED)
    {
        hci_pcb->vendor_init_status = VENDOR_INITED;
#if BT_VENDOR_BCM4345C5_SUPPORT > 0
        hci_read_local_version_info();
#else
        hci_reset();
#endif
    }
}

static err_t _hci_vendor_init(uint8_t ogf,uint8_t ocf)
{
    hci_pcb->chip_mgr->vendor_init(_hci_vendor_init_done,ogf,ocf);

    return BT_ERR_OK;
}


err_t read_bdaddr_complete(void *arg, struct bd_addr_t *bdaddr)
{

    BT_HCI_TRACE_DEBUG("DEBUG:read_bdaddr_complete: %02x:%02x:%02x:%02x:%02x:%02x\n",
                       bdaddr->addr[5], bdaddr->addr[4], bdaddr->addr[3],
                       bdaddr->addr[2], bdaddr->addr[1], bdaddr->addr[0]);

    return BT_ERR_OK;
}

static err_t _hci_inq_comp_evt_process(uint8_t *payload,uint16_t payload_len)
{
    BT_HCI_TRACE_DEBUG("_hci_inq_comp_evt_process 0x%x %s\n",payload[0], _hci_get_error_code(payload[0]));
    HCI_EVENT_INQ_COMPLETE(hci_pcb,payload[0]);

    return BT_ERR_OK;
}

static err_t _hci_inq_result_evt_process(uint8_t evt_code,uint8_t *payload,uint16_t payload_len)
{
    uint8_t index = 0;
    hci_inq_res_t *inqres;
    struct bd_addr_t *bdaddr;
    uint8_t resp_offset;

    for(index=0; index<payload[0]; index++)
    {
        resp_offset = index*14;
        BT_HCI_TRACE_DEBUG("hci_event_input: Inquiry index %d\n",index);

		bt_addr_dump(payload+1+resp_offset+index);

        BT_HCI_TRACE_DEBUG("Page_Scan_Rep_Mode: 0x%x\n",payload[7+resp_offset]);

        BT_HCI_TRACE_DEBUG("Class_of_Dev: 0x%x 0x%x 0x%x\n",payload[10+resp_offset],
                           payload[11+resp_offset], payload[12+resp_offset]);
        BT_HCI_TRACE_DEBUG("Clock_Offset: 0x%x%x\n",payload[13+resp_offset],
                           payload[14+resp_offset]);
        bdaddr = (void *)(payload+(1+resp_offset));
        if((inqres = bt_memp_malloc(MEMP_HCI_INQ)) != NULL)
        {
            bd_addr_set(&(inqres->bdaddr), bdaddr);
            inqres->psrm = payload[7+resp_offset];
            inqres->psm = payload[9+resp_offset];

            if(evt_code == HCI_EXT_INQ_RESULT)
            {
                uint8_t *eir_data = payload + 15;
                uint8_t *temp_eir_data = eir_data;

                inqres->cod[2] = payload[9+resp_offset];
                inqres->cod[1] = payload[10+resp_offset];
                inqres->cod[0] = payload[11+resp_offset];
                inqres->co = *((uint16_t *)(payload+12+resp_offset));

				inqres->rssi = payload[14+resp_offset];

                while(temp_eir_data[0] != 0)
                {
                    uint8_t eir_element_len = temp_eir_data[0];
                    uint8_t eir_element_type = temp_eir_data[1];
                    if(eir_element_type == BT_DT_COMPLETE_LOCAL_NAME)
                    {
                        memset(inqres->remote_name,0,HCI_REMOTE_NAME_LEN);
                        memcpy(inqres->remote_name,temp_eir_data+2,eir_element_len-1);
                        break;
                    }
                    temp_eir_data += eir_element_len + 1;
                }
            }
            else
            {
                inqres->cod[2] = payload[10+resp_offset];
                inqres->cod[1] = payload[11+resp_offset];
                inqres->cod[0] = payload[12+resp_offset];
                inqres->co = *((uint16_t *)(payload+13+resp_offset));
            }
            HCI_REG(&(hci_pcb->ires), inqres);

            HCI_EVENT_INQ_RESULT(hci_pcb,inqres);
        }
        else
        {
            BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        }
    }

    return BT_ERR_OK;
}


static err_t _hci_conn_comp_evt_process(uint8_t *payload,uint16_t payload_len)
{
    struct bd_addr_t *bdaddr = (void *)(payload+3); /* Get the Bluetooth address */
    hci_link_t *link = _hci_get_link_by_addr(bdaddr);
    switch(payload[0])
    {
    case HCI_SUCCESS:
        BT_HCI_TRACE_DEBUG("hci_event_input: Conn successfully completed\n");
        if(link == NULL)
        {
            if((link = _hci_new_link()) == NULL)
            {
                /* Could not allocate memory for link. Disconnect */
                BT_HCI_TRACE_DEBUG("hci_event_input: Could not allocate memory for link. Disconnect\n");
                hci_disconnect_acl(bdaddr, HCI_OTHER_END_TERMINATED_CONN_LOW_RESOURCES);
                /* Notify L2CAP */
                lp_disconnect_ind(bdaddr);
                break;
            }
            bd_addr_set(&(link->bdaddr), bdaddr);
            link->conhdl = *((uint16_t *)(payload+1));
            HCI_REG(&(hci_active_links), link);
            HCI_EVENT_CONN_COMPLETE(hci_pcb,bdaddr); /* Allow applicaton to do optional configuration of link */
            BT_HCI_TRACE_DEBUG("hci_event_input: Calling l2cap cb 1\n");
            hci_get_remote_feature(bdaddr);
            if(link->state == SEND_CREATE_CONNECTION)
            {
                lp_connect_cfm(bdaddr, payload[10], BT_ERR_OK); /* Notify L2CAP */
            }
            else
                lp_connect_ind(&(link->bdaddr)); /* Notify L2CAP */
            link->state = OPEN;
        }
        else
        {
            link->conhdl = *((uint16_t *)(payload+1));
            HCI_EVENT_CONN_COMPLETE(hci_pcb,bdaddr); /* Allow applicaton to do optional configuration of link */
            BT_HCI_TRACE_DEBUG("hci_event_input: Calling l2cap cb 2\n");
            hci_get_remote_feature(bdaddr);
            if(link->state == SEND_CREATE_CONNECTION)
                lp_connect_cfm(bdaddr, payload[10], BT_ERR_OK); /* Notify L2CAP */
            else
                lp_connect_ind(&(link->bdaddr)); /* Notify L2CAP */
            link->state = OPEN;
        }
        //TODO: MASTER SLAVE SWITCH??
        break;
    default:
        BT_HCI_TRACE_DEBUG("hci_event_input: Conn failed to complete, 0x%x %s\n"
                           , payload[0], _hci_get_error_code(payload[0]));
        if(link != NULL)
        {
            BT_HCI_TRACE_DEBUG("hci_event_input: Link exists. Notify upper layer\n");
            if(link->state == SEND_CREATE_CONNECTION)
                lp_connect_cfm(bdaddr, payload[10], BT_ERR_CONN); /* Notify L2CAP */
            else
                lp_connect_ind(&(link->bdaddr)); /* Notify L2CAP */

            _hci_delete_link(link);
        }
        else
        {
            /* silently discard */
            BT_HCI_TRACE_DEBUG("hci_event_input: Silently discard. Link does not exist\n");
        }
        break;
    } /* switch */

    BT_HCI_TRACE_DEBUG("Conn_hdl: 0x%x 0x%x\n", payload[1], payload[2]);
    BT_HCI_TRACE_DEBUG("Address:");
    bt_hex_dump(payload+3,BD_ADDR_LEN);
    BT_HCI_TRACE_DEBUG("Link_type: 0x%x\n",payload[9]);
    BT_HCI_TRACE_DEBUG("Encryption_Mode: 0x%x\n",payload[10]);

    return BT_ERR_OK;
}


static err_t _hci_conn_req_evt_process(uint8_t *payload,uint16_t payload_len)
{
    struct bd_addr_t *bdaddr = (void *)payload; /* Get the Bluetooth address */
    hci_link_t *link = _hci_get_link_by_addr(bdaddr);
    uint8_t link_type = payload[9];

    BT_HCI_TRACE_DEBUG("hci_event_input: recv conn req type %d\n",link_type);

    if(link == NULL)
    {
        if((link = _hci_new_link()) == NULL)
        {
            /* Could not allocate memory for link. Disconnect */
            BT_HCI_TRACE_DEBUG("hci_event_input: Could not allocate memory for link. reject\n");
            hci_reject_connection_request(bdaddr,HCI_HOST_REJECTED_DUE_TO_LIMITED_RESOURCES);
        }
        else
        {

            bd_addr_set(&(link->bdaddr), bdaddr);
            memcpy(&link->cod,payload+6,3);
            link->role = HCI_ROLE_SLAVE;

            BT_HCI_TRACE_DEBUG("hci_event_input: con req cod 0x%x\n",link->cod);
            HCI_REG(&(hci_active_links), link);
            hci_accept_connection_request(bdaddr,HCI_ROLE_SLAVE);
            link->state = ACCEPTED_CONNECTION_REQUEST;
        }
    }
    else
    {
        if(link_type == HCI_LINK_TYPE_ACL)
        {
            /* Could not allocate memory for link. Disconnect */
            BT_HCI_TRACE_DEBUG("hci_event_input: con exit. reject\n");
            hci_reject_connection_request(bdaddr,HCI_ACL_CONNECTION_EXISTS);
            link->state = REJECTED_CONNECTION_REQUEST;
            _hci_delete_link(link);

        }
        else
        {
            HCI_EVENT_SCO_REQ(hci_pcb, bdaddr);
        }
    }

    return BT_ERR_OK;
}


static err_t _hci_sco_conn_comp_evt_process(uint8_t *payload,uint16_t payload_len)
{
    uint8_t status = payload[0];
    struct bd_addr_t *bdaddr = (void *)(payload+3);
    hci_link_t *link = _hci_get_link_by_addr(bdaddr);

    BT_HCI_TRACE_DEBUG("hci_event_input: sco connection complete\n");

    if(status == HCI_SUCCESS && link != NULL)
    {
        link->sco_connhdl = bt_le_read_16(payload,1);
        BT_HCI_TRACE_DEBUG("hci_event_input: status %d,handle 0%x\n",status,link->sco_connhdl);
        HCI_EVENT_SCO_CONN_COMPLETE(hci_pcb,status,bdaddr);
    }

    return BT_ERR_OK;
}

static err_t _hci_disconn_comp_evt_process(uint8_t *payload,uint16_t payload_len)
{
    uint8_t sco_disconnect;
    uint8_t status = payload[0];
    uint16_t con_handle = bt_le_read_16(payload,1);
    uint8_t reason = payload[3];
    hci_link_t *link = NULL;

    BT_HCI_TRACE_DEBUG("hci_event_input: Disconn has occurred\n");
    BT_HCI_TRACE_DEBUG("Reason: 0x%x (%s)\n", reason, _hci_get_error_code(reason));

    if(status == HCI_SUCCESS)
    {
        BT_HCI_TRACE_DEBUG("Conn_hdl: 0x%x\n", con_handle);

        for(link = hci_active_links; link != NULL; link = link->next)
        {
            if(link->conhdl == con_handle)
            {
                sco_disconnect = 0;
                break; /* Found */
            }

            if(link->sco_connhdl == con_handle)
            {
                sco_disconnect = 1;
                break;
            }
        }

        if(link != NULL && sco_disconnect == 0)
        {
            BT_HCI_TRACE_DEBUG("hci_event_input: acl disconnect handle 0x%x\n",link->conhdl);
            lp_disconnect_ind(&(link->bdaddr)); /* Notify upper layer */
            link->state = RECEIVED_DISCONNECTION_COMPLETE;
            _hci_delete_link(link);
            hci_pcb->controller_num_acl = hci_pcb->controler_max_acl;
        }
        if(link != NULL && sco_disconnect == 1)
        {
            BT_HCI_TRACE_DEBUG("hci_event_input: sco disconnect  handle 0x%x\n",link->sco_connhdl);

            link->sco_connhdl = 0;
            HCI_EVENT_SCO_DISCONN_COMPLETE(hci_pcb,status,&(link->bdaddr));

        }
    }
    else
    {
        BT_HCI_TRACE_DEBUG("hci_event_input: Disconn failed to complete, 0x%x %s\n", status, _hci_get_error_code(status));
    }


    return BT_ERR_OK;
}


static err_t _hci_remote_name_req_evt_process(uint8_t *payload,uint16_t payload_len)
{
    struct bd_addr_t *bdaddr = (void *)(payload+1);
    uint8_t *remote_name = (void *)(payload+7);
    BT_HCI_TRACE_DEBUG("hci_event_input: get remote name complete result = 0x%x\n", payload[0]);
    HCI_EVENT_REMOTE_NAME_REQ_COMPLETE(hci_pcb,bdaddr,remote_name);

    return BT_ERR_OK;
}

static err_t _hci_encryption_change_evt_process(uint8_t *payload,uint16_t payload_len)
{
	uint16_t con_handle = bt_le_read_16(payload,1);
	uint8_t enc = payload[3];
    BT_HCI_TRACE_DEBUG("hci_event_input: Encryption changed. Status = 0x%x, Encryption enable = 0x%x\n", payload[0], payload[3]);

	hci_link_t * link = _hci_get_link_by_handle(con_handle);

	HCI_EVENT_ENC_CHANGE(hci_pcb,&link->bdaddr,enc);

    return BT_ERR_OK;
}

static err_t _hci_qos_setup_evt_process(uint8_t *payload,uint16_t payload_len)
{
    BT_HCI_TRACE_DEBUG("hci_event_input: QOS setup complete result = 0x%x\n", payload[0]);

    return BT_ERR_OK;
}

static err_t _hci_cmd_comp_evt_process(uint8_t *payload,uint16_t payload_len)
{
    uint8_t num_complete_pack = payload[0];
    uint16_t opcode = bt_le_read_16(payload,1);
    uint8_t *param = payload+3;
    BT_HCI_TRACE_DEBUG("hci_event_input: Command Complete\n");
    BT_HCI_TRACE_DEBUG("Num_HCI_Command_Packets: 0x%x\n", num_complete_pack);

    /* Add number of completed command packets to the number of command packets that the BT module can buffer */
    hci_pcb->numcmd += num_complete_pack;

    switch(opcode)
    {
    case HCI_OP_INQUIRY_CANCEL:
    {
        BT_HCI_TRACE_DEBUG("HCI_INQUIRY_CANCEL command complete\n");
        HCI_EVENT_INQ_COMPLETE(hci_pcb,param[0]);
        break;
    }
    case HCI_OP_WRITE_POLICY_SETTINGS:
    {
        if(param[0] == HCI_SUCCESS)
        {
            hci_link_t *link = NULL;
            BT_HCI_TRACE_DEBUG("hci_event_input: Successful HCI_W_LINK_POLICY.\n");
            for(link = hci_active_links; link != NULL; link = link->next)
            {
                if(link->conhdl == bt_le_read_16(param,1))
                {
                    break;
                }
            }
            if(link == NULL)
            {
                BT_HCI_TRACE_DEBUG("hci_event_input: Connection does not exist\n");

                return BT_ERR_CONN; /* Connection does not exist */
            }
            HCI_EVENT_WLP_COMPLETE(hci_pcb, &link->bdaddr); /* Notify application.*/
        }
        else
        {
            BT_HCI_TRACE_DEBUG("Unsuccessful HCI_W_LINK_POLICY.\n");
        }
        break;
    }
    case HCI_OP_SET_HC_TO_HOST_FLOW_CTRL:
    {
        hci_pcb->flow = 1;
        break;
    }
    case HCI_OP_READ_BD_ADDR:
    {
        if(param[0] == HCI_SUCCESS)
        {
            struct bd_addr_t *bdaddr = (void *)(param + 1); /* Get the Bluetooth address */
            HCI_EVENT_RBD_COMPLETE(hci_pcb, bdaddr); /* Notify application.*/
        }
        break;
    }
    case HCI_OP_BLE_WRITE_SCAN_ENABLE:
    {
        BT_HCI_TRACE_DEBUG("HCI_LE_SET_SCAN le_inq_w2_stop(%d)\n",hci_pcb->le_inq_w2_stop);
        if(hci_pcb->le_inq_w2_stop == 1)
        {
            BT_HCI_TRACE_DEBUG("DEBUG:hci_event_input:le inquiry complete, 0x%x %s\n",param[0], _hci_get_error_code(param[0]));
            HCI_EVENT_LE_INQ_COMPLETE(hci_pcb,param[0]);
            hci_pcb->le_inq_w2_stop = 0;
        }
        break;
    }
    default:
        break;
    }

    HCI_EVENT_CMD_COMPLETE(hci_pcb,opcode,param[0]);

    return BT_ERR_OK;
}


static err_t _hci_cmd_status_evt_process(uint8_t *payload,uint16_t payload_len)
{
    uint8_t status = payload[0];

    hci_pcb->numcmd += payload[1];
    uint16_t opcode = bt_le_read_16(payload,2);

    BT_HCI_TRACE_DEBUG("hci_event_input: Command Status OGF(0x%x),OCF(0x%x) status(0x%x)\n",HCI_OGF(opcode),HCI_OCF(opcode),status);

    return BT_ERR_OK;
}

static err_t _hci_hw_err_evt_process(uint8_t *payload,uint16_t payload_len)
{
    BT_HCI_TRACE_DEBUG("hci_event_input: Hardware Error\n");
    BT_HCI_TRACE_DEBUG("Hardware_code: 0x%x\n\n", payload[0]);

	HCI_EVENT_HARDWARE_ERROR(hci_pcb,payload[0]);
    return BT_ERR_OK;
}

static err_t _hci_role_change_evt_process(uint8_t *payload,uint16_t payload_len)
{
    BT_HCI_TRACE_DEBUG("hci_event_input: Role change\n");
    BT_HCI_TRACE_DEBUG("Status: 0x%x\n", payload[0]);
    BT_HCI_TRACE_DEBUG("New Role: 0x%x\n", payload[7]);
    return BT_ERR_OK;
}

static err_t _hci_number_comp_evt_process(uint8_t *payload,uint16_t payload_len)
{
    uint8_t index = 0;
    uint8_t resp_offset;
    uint8_t num_handle = payload[0];
    BT_HCI_TRACE_DEBUG("hci_event_input: Number Of Completed Packets\n");
    BT_HCI_TRACE_DEBUG("Number_of_Handles: 0x%x\n", num_handle);

    for(index=0; index<num_handle; index++)
    {
        resp_offset = index*4;
        uint16_t handle = bt_le_read_16(payload,1+resp_offset);
        uint16_t num_comp_pack = bt_le_read_16(payload,3+resp_offset);
        BT_HCI_TRACE_DEBUG("Conn_hdl: 0x%x\n", handle);
        BT_HCI_TRACE_DEBUG("HC_Num_Of_Completed_Packets: 0x%x\n",num_comp_pack);
        /* Add number of completed ACL packets to the number of ACL packets that the BT module can buffer */
        hci_pcb->controller_num_acl += num_comp_pack;
#if HCI_FLOW_QUEUEING
        {
            hci_link_t *link = NULL;
            struct bt_pbuf_t *q;
            for(link = hci_active_links; link != NULL; link = link->next)
            {
                if(link->conhdl == handle)
                {
                    break;
                }
            }
            if(link != NULL)
            {
                q = link->p;
                /* Queued packet present? */
                if (q != NULL)
                {
                    /* NULL attached buffer immediately */
                    link->p = NULL;
                    BT_HCI_TRACE_DEBUG("Sending queued packet.\n");
                    /* Send the queued packet */
                    hci_acl_write(&link->bdaddr, q, link->len, link->pb);
                    /* Free the queued packet */
                    bt_pbuf_free(q);
                }
            }
        }
#endif /* HCI_FLOW_QUEUEING */
    }

    return BT_ERR_OK;
}

static err_t _hci_mode_change_evt_process(uint8_t *payload,uint16_t payload_len)
{
    BT_HCI_TRACE_DEBUG("hci_event_input: Mode change\n");
    BT_HCI_TRACE_DEBUG("Status: 0x%x\n", payload[0]);
    BT_HCI_TRACE_DEBUG("Conn_hdl: 0x%x\n", bt_le_read_16(payload,1));
    BT_HCI_TRACE_DEBUG("Mode: 0x%x\n", payload[3]);
    BT_HCI_TRACE_DEBUG("Interval: 0x%x\n", bt_le_read_16(payload,4));

    return BT_ERR_OK;
}

static err_t _hci_data_buf_overflow_evt_process(uint8_t *payload,uint16_t payload_len)
{
    BT_HCI_TRACE_DEBUG("hci_event_input: Data Buffer Overflow\n");
    BT_HCI_TRACE_DEBUG("Link_Type: 0x%x\n", payload[0]);

    return BT_ERR_OK;
}


static err_t _hci_max_slot_change_evt_process(uint8_t *payload,uint16_t payload_len)
{
    BT_HCI_TRACE_DEBUG("hci_event_input: Max slots changed\n");
    BT_HCI_TRACE_DEBUG("Conn_hdl: 0x%x\n", bt_le_read_16(payload,0));
    BT_HCI_TRACE_DEBUG("LMP max slots: 0x%x\n", payload[2]);

    return BT_ERR_OK;
}

static err_t _hci_pincode_req_evt_process(uint8_t *payload,uint16_t payload_len)
{
    struct bd_addr_t *bdaddr = (void *)payload;
    BT_HCI_TRACE_DEBUG("hci_event_input: Pin code request\n");
    HCI_EVENT_PIN_REQ(hci_pcb, bdaddr); /* Notify application. If event is not registered,send a negative reply */
    return BT_ERR_OK;
}

static err_t _hci_linkkey_req_evt_process(uint8_t *payload,uint16_t payload_len)
{
    struct bd_addr_t *bdaddr = (void *)payload;
    BT_HCI_TRACE_DEBUG("hci_event_input: link key request\n");
    HCI_EVENT_LINK_REQ(hci_pcb, bdaddr);

    return BT_ERR_OK;
}

static err_t _hci_linkkey_notify_evt_process(uint8_t *payload,uint16_t payload_len)
{
    struct bd_addr_t *bdaddr = (void *)payload;
    uint8_t *link_key = payload+6;
    uint8_t link_key_type = payload[6+16];
    BT_HCI_TRACE_DEBUG("hci_event_input: link key notification\n");
    HCI_EVENT_LINK_KEY_NOT(hci_pcb, bdaddr, link_key,link_key_type); /* Notify application.*/

    return BT_ERR_OK;
}

static err_t _hci_io_cap_req_evt_process(uint8_t *payload,uint16_t payload_len)
{
    struct bd_addr_t *bdaddr = (void *)payload;
    uint32_t num_value = bt_le_read_32(payload,6);
    BT_HCI_TRACE_DEBUG("hci_event_input: io cap request num value(%d)\n",num_value);
    hci_io_cap_req_replay(bdaddr,hci_pcb->ssp_io_cap,0x0,SSP_IO_AUTHREQ_MITM_PROTECTION_REQUIRED_DEDICATED_BONDING);

    return BT_ERR_OK;
}

static err_t _hci_usr_confim_req_evt_process(uint8_t *payload,uint16_t payload_len)
{
    struct bd_addr_t *bdaddr = (void *)payload;
    BT_HCI_TRACE_DEBUG("hci_event_input: user confirm request\n");
    hci_user_confirm_req_reply(bdaddr);

    return BT_ERR_OK;
}

#if BT_BLE_ENABLE > 0
static err_t _hci_le_meta_evt_process(uint8_t *payload,uint16_t payload_len)
{
    uint8_t sub_event = payload[0];
    BT_HCI_TRACE_DEBUG("hci_event_input: le meta event sub event 0x%x\n",sub_event);

    switch(sub_event)
    {
    case HCI_SUBEVENT_LE_CONN_COMPLETE:
    {
        uint16_t con_handle = bt_le_read_16(payload,2);
        hci_link_t * link = _hci_get_link_by_handle(con_handle);
        struct bd_addr_t *bdaddr = (struct bd_addr_t *)(payload+6);

        if(link == NULL)
        {
            if((link = _hci_new_link()) == NULL)
            {
                /* Could not allocate memory for link. Disconnect */
                BT_HCI_TRACE_DEBUG("hci_event_input: Could not allocate memory for link. Disconnect\n");
                break;
            }
            bd_addr_set(&(link->bdaddr), bdaddr);
            bt_hex_dump((uint8_t *)&(link->bdaddr),6);
			link->role = HCI_ROLE_SLAVE;
            link->conhdl = con_handle;
            HCI_REG(&(hci_active_links), link);
            link->state = OPEN;

			le_connect_handler(bdaddr,link->role);
        }
        break;
    }
    case HCI_SUBEVENT_LE_ADV_REPORT:
    {
        hci_le_inq_res_t hci_le_inq_res = {0};

        hci_le_inq_res.addr_type = payload[3];
        memcpy(hci_le_inq_res.bdaddr.addr,payload+4,BD_ADDR_LEN);
        hci_le_inq_res.adv_size = payload[10];
        hci_le_inq_res.adv_type = payload[2];
        memcpy(hci_le_inq_res.adv_data,payload+11,hci_le_inq_res.adv_size);
        hci_le_inq_res.rssi = payload[11+hci_le_inq_res.adv_size];

        HCI_EVENT_LE_INQ_RESULT(hci_pcb,&hci_le_inq_res);

        break;
    }
	case HCI_SUBEVENT_LE_LONG_TERM_KEY_REQUEST:
	{
		uint8_t random[8];
		uint16_t ediv;
		uint16_t con_handle = bt_le_read_16(payload,1);
		memcpy(random,payload+3,sizeof(random));
		ediv = bt_le_read_16(payload,11);

		hci_link_t * link = _hci_get_link_by_handle(con_handle);

		HCI_EVENT_LTK_REQ(hci_pcb,&link->bdaddr,random,ediv);
		break;
	}
	case HCI_SUBEVENT_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE:
	{
		uint8_t *public_key = payload+2;
		HCI_EVENT_PUBLIC_KEY(hci_pcb,public_key);
		break;
	}
	case HCI_SUBEVENT_LE_GENERATE_DHKEY_COMPLETE:
	{
		uint8_t *dhkey = payload+2;
		HCI_EVENT_DHKEY_COMPLETE(hci_pcb,dhkey);
		break;
	}
	case HCI_SUBEVENT_LE_ENHANCED_CONN_COMPLETE:
	{
		uint16_t con_handle = bt_le_read_16(payload,2);
        hci_link_t * link = _hci_get_link_by_handle(con_handle);
        struct bd_addr_t *bdaddr = (struct bd_addr_t *)(payload+6);

        if(link == NULL)
        {
            if((link = _hci_new_link()) == NULL)
            {
                /* Could not allocate memory for link. Disconnect */
                BT_HCI_TRACE_DEBUG("hci_event_input: Could not allocate memory for link. Disconnect\n");
                break;
            }
            bd_addr_set(&(link->bdaddr), bdaddr);
            bt_hex_dump((uint8_t *)&(link->bdaddr),6);
			link->role = HCI_ROLE_SLAVE;
            link->conhdl = con_handle;
            HCI_REG(&(hci_active_links), link);
            link->state = OPEN;

			le_connect_handler(bdaddr,link->role);
        }
        break;
	}
    default:
        break;
    }

    return BT_ERR_OK;
}
#endif

static err_t _hci_vendor_evt_process(uint8_t *payload,uint16_t payload_len)
{
    return BT_ERR_OK;
}


static err_t _hci_init_cmd_compl_process(uint8_t *payload,uint16_t payload_len)
{
    uint16_t opcode = bt_le_read_16(payload,1);


    BT_HCI_TRACE_DEBUG("_hci_init_cmd_compl_process:0x%x\n",opcode);

    if((opcode&HCI_GRP_VENDOR_SPECIFIC) == HCI_GRP_VENDOR_SPECIFIC)
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_GRP_VENDOR_SPECIFIC\n");
        _hci_vendor_init(HCI_OGF(opcode),HCI_OCF(opcode));
    }

    switch(opcode)
    {
    case HCI_OP_RESET:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_RESET\n");
        utimer_cancel(hci_pcb->timer);
        if (hci_pcb->vendor_init_status == VENDOR_NOT_INIT)
        {
            if(hci_pcb->chip_mgr != NULL)
                _hci_vendor_init(HCI_OGF(opcode),HCI_OCF(opcode));
        }
        else
        {
            hci_read_local_version_info();
        }

        break;
    }
    case HCI_OP_READ_LOCAL_VERSION_INFO:
    {
        uint8_t hci_version = payload[4];
        uint16_t hci_reversion = bt_le_read_16(payload,5);
        uint8_t lmp_version = payload[7];
        uint16_t manufacturer_name = bt_le_read_16(payload,8);
        uint16_t lmp_subversion = bt_le_read_16(payload,10);

        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_READ_LOCAL_VERSION_INFO\n");
        BT_HCI_TRACE_DEBUG("DEBUG:HCI version:0x%x\n",hci_version);
        BT_HCI_TRACE_DEBUG("DEBUG:HCI reversion:0x%x\n",hci_reversion);
        BT_HCI_TRACE_DEBUG("DEBUG:LMP version:0x%x\n",lmp_version);
        BT_HCI_TRACE_DEBUG("DEBUG:LMP reversion:0x%x\n",lmp_subversion);
        BT_HCI_TRACE_DEBUG("DEBUG:manufacturer_name:0x%x\n",manufacturer_name);
		hci_pcb->hci_version = hci_version;
		
        hci_read_buffer_size();
        break;
    }
    case HCI_OP_READ_BUFFER_SIZE:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_READ_BUFFER_SIZE\n");


        hci_pcb->acl_maxsize = bt_le_read_16(payload,4); /* Maximum size of an ACL packet that the BT module is able to accept */
        hci_pcb->controller_num_acl = bt_le_read_16(payload,7); /* Number of ACL packets that the BT module can buffer */
        hci_pcb->controler_max_acl = hci_pcb->controller_num_acl;
        BT_HCI_TRACE_DEBUG("Max ACL size(%d)\n",hci_pcb->acl_maxsize);
        BT_HCI_TRACE_DEBUG("Max ACL count(%d)\n",hci_pcb->controller_num_acl);
        hci_read_bd_addr(read_bdaddr_complete);
        break;
    }
    case HCI_OP_READ_BD_ADDR:
    {
        struct bd_addr_t *bdaddr = (void *)(payload + 4);
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_READ_BD_ADDR\n");
        memcpy(hci_pcb->local_bd_addr.addr,bdaddr,BD_ADDR_LEN);

        HCI_EVENT_RBD_COMPLETE(hci_pcb, bdaddr); /* Notify application.*/
        hci_write_cod((uint8_t *)&hci_pcb->class_of_device);
        break;
    }
    case HCI_OP_WRITE_CLASS_OF_DEVICE:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_WRITE_CLASS_OF_DEVICE\n");
        hci_write_local_name((uint8_t *)hci_pcb->local_name, (uint8_t)strlen((const char*)hci_pcb->local_name));
        break;
    }
    case HCI_OP_CHANGE_LOCAL_NAME:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_CHANGE_LOCAL_NAME\n");
        hci_write_page_timeout(BT_PAGE_TIMEOUT); /* value*0.625ms */
        break;
    }
    case HCI_OP_WRITE_PAGE_TOUT:
    {		
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_WRITE_PAGE_TOUT\n");
		hci_read_local_support_cmd();

        break;
    }
	case HCI_OP_READ_LOCAL_SUPPORTED_CMDS:
	{
		BT_HCI_TRACE_DEBUG("Init recv HCI_OP_READ_LOCAL_SUPPORTED_CMDS\n");
#if BT_BLE_ENABLE > 0
        hci_set_event_mask(0xffffffff,0x3FFFFFFF);
#else
        hci_set_event_mask(0xffffffff, 0x1FFFFFFF); /* base 0x1FFFFFFF:Add LE Meta event(bit 61) */
#endif
		break;
	}
    case HCI_OP_SET_EVENT_MASK:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_SET_EVENT_MASK\n");
        hci_write_ssp_mode(hci_pcb->ssp_enable);
        break;
    }
    case HCI_OP_WRITE_SIMPLE_PAIRING_MODE:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_WRITE_SIMPLE_PAIRING_MODE\n");
        hci_write_inquiry_mode(INQUIRY_MODE_EIR);
        break;
    }
    case HCI_OP_WRITE_INQUIRY_MODE:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_WRITE_INQUIRY_MODE\n");
#if BT_BLE_ENABLE > 0
        hci_write_le_enable(1,0);
#else
        hci_write_scan_enable(HCI_SCAN_EN_INQUIRY | HCI_SCAN_EN_PAGE);
#endif
        break;
    }
    case HCI_OP_WRITE_SCAN_ENABLE:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_WRITE_SCAN_ENABLE\n");
        if(hci_pcb->init_status == BLUETOOTH_INITING)
            HCI_BT_WORKING(hci_pcb);
        hci_pcb->init_status = BLUETOOTH_WORKING;
        break;
    }
#if BT_BLE_ENABLE > 0
    case HCI_OP_WRITE_LE_SUPPORT:
    {
		uint8_t event_mask[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0}; 
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_WRITE_LE_SUPPORT\n");
        hci_le_set_event_mask(event_mask); /* some bt modem back invalid commmand para,do not care,skip it */
        break;
    }
    case HCI_OP_BLE_SET_EVENT_MASK:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_BLE_SET_EVENT_MASK\n");
        hci_le_read_buffer_size();

        break;
    }
    case HCI_OP_BLE_READ_BUFFER_SIZE:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_BLE_READ_BUFFER_SIZE\n");
        hci_le_read_local_support_feature();

        break;
    }
    case HCI_OP_BLE_READ_LOCAL_SPT_FEAT:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_OP_BLE_READ_LOCAL_SPT_FEAT\n");
        hci_write_scan_enable(HCI_SCAN_EN_INQUIRY | HCI_SCAN_EN_PAGE);
        break;
    }
#endif
    default:
        break;
    }
    return BT_ERR_OK;
}

static hci_link_t *_hci_new_link(void)
{
    hci_link_t *link = bt_memp_malloc(MEMP_HCI_LINK);
    if(link != NULL)
    {
        memset(link, 0, sizeof(hci_link_t));
        return link;
    }

    BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

static err_t _hci_delete_link(hci_link_t *link)
{
#if HCI_FLOW_QUEUEING
    if(link->p != NULL)
    {
        bt_pbuf_free(link->p);
    }
#endif
    HCI_RMV(&(hci_active_links), link);
    bt_memp_free(MEMP_HCI_LINK, link);
    link = NULL;
    return BT_ERR_OK;
}



static hci_link_t *_hci_get_link_by_addr(struct bd_addr_t *bdaddr)
{
    hci_link_t *link = NULL;

    for(link = hci_active_links; link != NULL; link = link->next)
    {
        if(bd_addr_cmp(&(link->bdaddr), bdaddr))
        {
            break;
        }
    }
    return link;
}

static hci_link_t *_hci_get_link_by_handle(uint16_t handle)
{
    hci_link_t *link = NULL;

    for(link = hci_active_links; link != NULL; link = link->next)
    {
        if(link->conhdl == handle)
        {
            break;
        }
    }
    return link;
}


static uint8_t *_hci_get_error_code(uint8_t code)
{
    switch(code)
    {
    case HCI_SUCCESS:
        return (uint8_t *)("Success");
    case HCI_UNKNOWN_HCI_COMMAND:
        return (uint8_t *)("Unknown HCI Command");
    case HCI_NO_CONNECTION:
        return (uint8_t *)("No Connection");
    case HCI_HW_FAILURE:
        return (uint8_t *)("Hardware Failure");
    case HCI_PAGE_TIMEOUT:
        return (uint8_t *)("Page Timeout");
    case HCI_AUTHENTICATION_FAILURE:
        return (uint8_t *)("Authentication Failure");
    case HCI_KEY_MISSING:
        return (uint8_t *)("Key Missing");
    case HCI_MEMORY_FULL:
        return (uint8_t *)("Memory Full");
    case HCI_CONN_TIMEOUT:
        return (uint8_t *)("Connection Timeout");
    case HCI_MAX_NUMBER_OF_CONNECTIONS:
        return (uint8_t *)("Max Number Of Connections");
    case HCI_MAX_NUMBER_OF_SCO_CONNECTIONS_TO_DEVICE:
        return (uint8_t *)("Max Number Of SCO Connections To A Device");
    case HCI_ACL_CONNECTION_EXISTS:
        return (uint8_t *)("ACL connection already exists");
    case HCI_COMMAND_DISSALLOWED:
        return (uint8_t *)("Command Disallowed");
    case HCI_HOST_REJECTED_DUE_TO_LIMITED_RESOURCES:
        return (uint8_t *)("Host Rejected due to limited resources");
    case HCI_HOST_REJECTED_DUE_TO_SECURITY_REASONS:
        return (uint8_t *)("Host Rejected due to security reasons");
    case HCI_HOST_REJECTED_DUE_TO_REMOTE_DEVICE_ONLY_PERSONAL_SERVICE:
        return (uint8_t *)("Host Rejected due to remote device is only a personal device");
    case HCI_HOST_TIMEOUT:
        return (uint8_t *)("Host Timeout");
    case HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE:
        return (uint8_t *)("Unsupported Feature or Parameter Value");
    case HCI_INVALID_HCI_COMMAND_PARAMETERS:
        return (uint8_t *)("Invalid HCI Command Parameters");
    case HCI_OTHER_END_TERMINATED_CONN_USER_ENDED:
        return (uint8_t *)("Other End Terminated Connection: User Ended Connection");
    case HCI_OTHER_END_TERMINATED_CONN_LOW_RESOURCES:
        return (uint8_t *)("Other End Terminated Connection: Low Resources");
    case HCI_OTHER_END_TERMINATED_CONN_ABOUT_TO_POWER_OFF:
        return (uint8_t *)("Other End Terminated Connection: About to Power Off");
    case HCI_CONN_TERMINATED_BY_LOCAL_HOST:
        return (uint8_t *)("Connection Terminated by Local Host");
    case HCI_REPETED_ATTEMPTS:
        return (uint8_t *)("Repeated Attempts");
    case HCI_PAIRING_NOT_ALLOWED:
        return (uint8_t *)("Pairing Not Allowed");
    case HCI_UNKNOWN_LMP_PDU:
        return (uint8_t *)("Unknown LMP PDU");
    case HCI_UNSUPPORTED_REMOTE_FEATURE:
        return (uint8_t *)("Unsupported Remote Feature");
    case HCI_SCO_OFFSET_REJECTED:
        return (uint8_t *)("SCO Offset Rejected");
    case HCI_SCO_INTERVAL_REJECTED:
        return (uint8_t *)("SCO Interval Rejected");
    case HCI_SCO_AIR_MODE_REJECTED:
        return (uint8_t *)("SCO Air Mode Rejected");
    case HCI_INVALID_LMP_PARAMETERS:
        return (uint8_t *)("Invalid LMP Parameters");
    case HCI_UNSPECIFIED_ERROR:
        return (uint8_t *)("Unspecified Error");
    case HCI_UNSUPPORTED_LMP_PARAMETER_VALUE:
        return (uint8_t *)("Unsupported LMP Parameter Value");
    case HCI_ROLE_CHANGE_NOT_ALLOWED:
        return (uint8_t *)("Role Change Not Allowed");
    case HCI_LMP_RESPONSE_TIMEOUT:
        return (uint8_t *)("LMP Response Timeout");
    case HCI_LMP_ERROR_TRANSACTION_COLLISION:
        return (uint8_t *)("LMP Error Transaction Collision");
    case HCI_LMP_PDU_NOT_ALLOWED:
        return (uint8_t *)("LMP PDU Not Allowed");
    case HCI_ENCRYPTION_MODE_NOT_ACCEPTABLE:
        return (uint8_t *)("Encryption Mode Not Acceptable");
    case HCI_UNIT_KEY_USED:
        return (uint8_t *)("Unit Key Used");
    case HCI_QOS_NOT_SUPPORTED:
        return (uint8_t *)("QoS is Not Supported");
    case HCI_INSTANT_PASSED:
        return (uint8_t *)("Instant Passed");
    case HCI_PAIRING_UNIT_KEY_NOT_SUPPORTED:
        return (uint8_t *)("Pairing with Unit Key Not Supported");
    default:
        return (uint8_t *)("Error code unknown");
    }
}


static err_t _hci_reset_cmd_timeout(void *para)
{
    BT_HCI_TRACE_DEBUG("_hci_reset_cmd_timeout handle\n");
    hci_reset();

    return BT_ERR_OK;
}

static err_t _hci_pin_req_handle(void *arg, struct bd_addr_t *bdaddr)
{
    return hci_pin_code_request_reply(bdaddr, (uint8_t)strlen((const char*)hci_pcb->pincode), hci_pcb->pincode);
}

static err_t _hci_set_chip_name(uint8_t *name)
{
    if(hci_pcb != NULL)
    {
        hci_pcb->chip_mgr = bt_vendor_get_chip_mrg(name);

        if(hci_pcb->chip_mgr != NULL)
            return BT_ERR_ARG;
    }

    return BT_ERR_OK;
}


static err_t _hci_init_process(struct bt_pbuf_t *p)
{
    hci_evt_hdr_t *evt_hdr = p->payload;

    BT_HCI_TRACE_DEBUG("_hci_init_process:\n");
    //bt_hex_dump(p->payload,evt_hdr->len+HCI_EVT_HDR_LEN);


    switch(evt_hdr->code)
    {
    case HCI_COMMAND_COMPLETE:
    {
        bt_pbuf_header(p, -HCI_EVT_HDR_LEN); /* offset to para */
        _hci_init_cmd_compl_process(p->payload,evt_hdr->len);
        break;
    }
    case HCI_VENDOR_SPEC:
    {
        BT_HCI_TRACE_DEBUG("Init recv HCI_VENDOR_SPEC\n");
        _hci_vendor_init(HCI_OGF(HCI_OP_NONE),HCI_OCF(HCI_OP_NONE));
        break;
    }
    default:
        break;
    }

    return BT_ERR_OK;
}



void hci_event_input(struct bt_pbuf_t *p)
{
    hci_evt_hdr_t *evhdr = p->payload;;

#if 0
	BT_HCI_TRACE_DEBUG("-------------------------\n");
	BT_HCI_TRACE_DEBUG("DEBUG:BT RX EVENT LEN:%d\n",evhdr->len);
    bt_hex_dump(p->payload,evhdr->len + HCI_ACL_HDR_LEN);
	BT_HCI_TRACE_DEBUG("-------------------------\n\n");
#endif

    if(hci_pcb->init_status == BLUETOOTH_INITING)
    {
        _hci_init_process(p);
        return;
    }

    bt_pbuf_header(p, -HCI_EVT_HDR_LEN);

    switch(evhdr->code)
    {
    case HCI_INQUIRY_COMPLETE:
        _hci_inq_comp_evt_process(p->payload,evhdr->len);
        break;
    case HCI_INQUIRY_RESULT:
    case HCI_EXT_INQ_RESULT:
        _hci_inq_result_evt_process(evhdr->code,p->payload,evhdr->len);
        break;
    case HCI_CONNECTION_COMPLETE:
        _hci_conn_comp_evt_process(p->payload,evhdr->len);
        break;
    case HCI_CONNECTION_REQUEST:
        _hci_conn_req_evt_process(p->payload,evhdr->len);
        break;
    case HCI_SCO_CONNECTION_COMPLETE:
        _hci_sco_conn_comp_evt_process(p->payload,evhdr->len);
        break;
    case HCI_DISCONNECTION_COMPLETE:
        _hci_disconn_comp_evt_process(p->payload,evhdr->len);
        break;
    case HCI_GET_REMOTE_NAME_COMPLETE:
        _hci_remote_name_req_evt_process(p->payload,evhdr->len);
        break;
    case HCI_ENCRYPTION_CHANGE:
        _hci_encryption_change_evt_process(p->payload,evhdr->len);
        break;
    case HCI_QOS_SETUP_COMPLETE:
        _hci_qos_setup_evt_process(p->payload,evhdr->len);
        break;
    case HCI_COMMAND_COMPLETE:
        _hci_cmd_comp_evt_process(p->payload,evhdr->len);
        break;
    case HCI_COMMAND_STATUS:
        _hci_cmd_status_evt_process(p->payload,evhdr->len);
        break;
    case HCI_HARDWARE_ERROR:
        _hci_hw_err_evt_process(p->payload,evhdr->len);
        break;
    case HCI_ROLE_CHANGE:
        _hci_role_change_evt_process(p->payload,evhdr->len);
        break;
    case HCI_NBR_OF_COMPLETED_PACKETS:
        _hci_number_comp_evt_process(p->payload,evhdr->len);
        break;
    case HCI_MODE_CHANGE:
        _hci_mode_change_evt_process(p->payload,evhdr->len);
        break;
    case HCI_DATA_BUFFER_OVERFLOW:
        _hci_data_buf_overflow_evt_process(p->payload,evhdr->len);
        break;
    case HCI_MAX_SLOTS_CHANGE:
        _hci_max_slot_change_evt_process(p->payload,evhdr->len);
        break;
    case HCI_PIN_CODE_REQUEST:
        _hci_pincode_req_evt_process(p->payload,evhdr->len);
        break;
    case HCI_LINK_KEY_REQUEST:
        _hci_linkkey_req_evt_process(p->payload,evhdr->len);
        break;
    case HCI_LINK_KEY_NOTIFICATION:
        _hci_linkkey_notify_evt_process(p->payload,evhdr->len);
        break;
    case HCI_IO_CAP_REQ:
        _hci_io_cap_req_evt_process(p->payload,evhdr->len);
        break;
    case HCI_USER_CONF_REQ:
        _hci_usr_confim_req_evt_process(p->payload,evhdr->len);
        break;
    case HCI_VENDOR_SPEC:
        _hci_vendor_evt_process(p->payload,evhdr->len);
        break;
#if BT_BLE_ENABLE > 0
    case HCI_LE_META:
        _hci_le_meta_evt_process(p->payload,evhdr->len);
        break;
#endif
    default:
        BT_HCI_TRACE_DEBUG("hci_event_input: Undefined event code 0x%x\n", evhdr->code);
        break;
    }/* switch */
}


err_t hci_acl_write(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p, uint16_t len, uint8_t pb)
{
    hci_link_t *link;
    static hci_acl_hdr_t *aclhdr;
    struct bt_pbuf_t *q;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not exist\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN;
    }
    BT_HCI_TRACE_DEBUG("hci_acl_write: HC num ACL %d\n", hci_pcb->controller_num_acl);

    if(hci_pcb->controller_num_acl == 0)
    {
        BT_HCI_TRACE_DEBUG("hci_acl_write: HC out of buffer space\n");

#if HCI_FLOW_QUEUEING
        if(p != NULL)
        {
            /* Packet can be queued? */
            if(link->p != NULL)
            {
                BT_HCI_TRACE_DEBUG("hci_acl_write: Host buffer full. Dropped packet\n");
                return BT_ERR_OK; /* Drop packet */
            }
            else
            {
                p = bt_pbuf_take(p);
                /* Remember bt_pbuf_t to queue, if any */
                link->p = p;
                link->len = len;
                link->pb = pb;
                /* Pbufs are queued, increase the reference count */
                bt_pbuf_ref(p);
                BT_HCI_TRACE_DEBUG("hci_acl_write: Host queued packet %p\n", (void *)p);

            }
        }
#else
        BT_HCI_TRACE_DEBUG("hci_acl_write: Dropped packet\n");
#endif
        return BT_ERR_OK;
    }

    if((q = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_ACL_HDR_LEN, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    bt_pbuf_chain(q, p);

    aclhdr = (void *)(((uint8_t*)q->payload));
    aclhdr->conhdl_pb_bc = link->conhdl; /* Received from connection complete event */
    aclhdr->conhdl_pb_bc |= pb << 12; /* Packet boundary flag */
    aclhdr->conhdl_pb_bc &= 0x3FFF; /* Point-to-point */
    aclhdr->len = len;

    BT_HCI_TRACE_DEBUG("hci_acl_write: q->tot_len = %d aclhdr->len + q->len = %d\n", q->tot_len, aclhdr->len + q->len);

    phybusif_output(q, aclhdr->len + q->len,PHYBUSIF_PACKET_TYPE_ACL_DATA);

    --hci_pcb->controller_num_acl;

    /* Free ACL header. Upper layers will handle rest of packet */
    p = bt_pbuf_dechain(q);
    bt_pbuf_free(q);
    return BT_ERR_OK;
}

uint8_t hci_is_connected(struct bd_addr_t *bdaddr)
{
    hci_link_t *link;

    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        return 0;
    }
    return 1;
}


uint16_t hci_pdu_maxsize(void)
{
    return hci_pcb->acl_maxsize;
}

void hci_acl_input(struct bt_pbuf_t *p)
{
    uint16_t conhdl;
    hci_link_t *link = NULL;  
	hci_acl_hdr_t *aclhdr = p->payload;

#if 0
	BT_HCI_TRACE_DEBUG("-------------------------\n");
    BT_HCI_TRACE_DEBUG("DEBUG:BT RX ACL LEN:%d\n",aclhdr->len);
    bt_hex_dump(p->payload,aclhdr->len + HCI_ACL_HDR_LEN);
	BT_HCI_TRACE_DEBUG("-------------------------\n\n");
#endif

    //bt_pbuf_header(p, -HCI_ACL_HDR_LEN);

    conhdl = aclhdr->conhdl_pb_bc & 0x0FFF; /* Get the connection handle from the first 12 bits */
    if(hci_pcb->flow)
    {
        //TODO: XXX??? DO WE SAVE NUMACL PACKETS COMPLETED IN LINKS LIST?? SHOULD WE CALL
        //hci_host_num_comp_packets from the main loop when no data has been received from the
        //serial port???
        --hci_pcb->host_num_acl;
        if(hci_pcb->host_num_acl == 0)
        {
            hci_host_num_comp_packets(conhdl, HCI_HOST_MAX_NUM_ACL);
            hci_pcb->host_num_acl = HCI_HOST_MAX_NUM_ACL;
        }
    }

    for(link = hci_active_links; link != NULL; link = link->next)
    {
        if(link->conhdl == conhdl)
        {
            break;
        }
    }

    if(link != NULL)
    {
        if(aclhdr->len)
        {
            BT_HCI_TRACE_DEBUG("DEBUG:Forward ACL packet to higher layer p->tot_len = %d\n",p->tot_len);
            l2cap_acl_input(p, &(link->bdaddr));
        }
    }
}



struct bt_pbuf_t *hci_cmd_ass(struct bt_pbuf_t *p, uint8_t ocf, uint8_t ogf, uint8_t len)
{
    ((uint8_t *)p->payload)[0] = (ocf & 0xff); /* OCF & OGF */
    ((uint8_t *)p->payload)[1] = (ocf >> 8)|(ogf << 2);
    ((uint8_t *)p->payload)[2] = len-HCI_CMD_HDR_LEN; /* Param len = plen - cmd hdr  */
    if(hci_pcb->numcmd != 0)
    {
        --hci_pcb->numcmd; /* Reduce number of cmd packets that the host controller can buffer */
    }
    return p;
}


err_t hci_inquiry(uint32_t lap, uint8_t inq_len, uint8_t num_resp,
                  inq_result_fun_cb inq_result,
                  inq_complete_fun_cb inq_complete)
{
    struct bt_pbuf_t *p;
    hci_inq_res_t *tmpres;

    /* Free any previous inquiry result list */
    while(hci_pcb->ires != NULL)
    {
        tmpres = hci_pcb->ires;
        hci_pcb->ires = hci_pcb->ires->next;
        bt_memp_free(MEMP_HCI_INQ, tmpres);
    }

    hci_pcb->inq_complete = inq_complete;
    hci_pcb->inq_result = inq_result;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_INQUIRY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_INQUIRY, HCI_LINK_CONTROL, HCI_INQUIRY_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_24((uint8_t *)p->payload,3,lap);
    ((uint8_t *)p->payload)[6] = inq_len;
    ((uint8_t *)p->payload)[7] = num_resp;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_cancel_inquiry(void)
{
    struct bt_pbuf_t *p;
    hci_inq_res_t *tmpres;

    /* Free any previous inquiry result list */
    while(hci_pcb->ires != NULL)
    {
        tmpres = hci_pcb->ires;
        hci_pcb->ires = hci_pcb->ires->next;
        bt_memp_free(MEMP_HCI_INQ, tmpres);
    }


    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_CANCEL_INQUIRY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_INQUIRY_CANCEL, HCI_LINK_CONTROL, HCI_CANCEL_INQUIRY_PLEN);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_periodic_inquiry(uint16_t min_periodic,uint16_t max_periodic,uint32_t lap, uint8_t inq_len, uint8_t num_resp,
                           inq_result_fun_cb inq_result,
                           inq_complete_fun_cb inq_complete)
{
    struct bt_pbuf_t *p;
    hci_inq_res_t *tmpres;

    /* Free any previous inquiry result list */
    while(hci_pcb->ires != NULL)
    {
        tmpres = hci_pcb->ires;
        hci_pcb->ires = hci_pcb->ires->next;
        bt_memp_free(MEMP_HCI_INQ, tmpres);
    }

    hci_pcb->inq_complete = inq_complete;
    hci_pcb->inq_result = inq_result;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_PERIDIC_INQUIRY, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_PERIODIC_INQUIRY, HCI_LINK_CONTROL, HCI_PERIDIC_INQUIRY);
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,max_periodic);
    bt_le_store_16((uint8_t *)p->payload,5,min_periodic);
    bt_le_store_24((uint8_t *)p->payload,7,lap);
    ((uint8_t *)p->payload)[10] = inq_len;
    ((uint8_t *)p->payload)[11] = num_resp;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_cancel_periodic_inquiry(void)
{
    struct bt_pbuf_t *p;
    hci_inq_res_t *tmpres;

    /* Free any previous inquiry result list */
    while(hci_pcb->ires != NULL)
    {
        tmpres = hci_pcb->ires;
        hci_pcb->ires = hci_pcb->ires->next;
        bt_memp_free(MEMP_HCI_INQ, tmpres);
    }


    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_CANCEL_PERIDIC_INQYIRY, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_EXIT_PERIODIC_INQUIRY, HCI_LINK_CONTROL, HCI_CANCEL_PERIDIC_INQYIRY);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_connect_req(struct bd_addr_t *bdaddr, uint8_t allow_role_switch)
{
    uint8_t page_scan_repetition_mode, page_scan_mode;
    uint16_t clock_offset;
    struct bt_pbuf_t *p;
    hci_link_t *link = _hci_new_link();
    hci_inq_res_t *inqres;

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d]  fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM; /* Could not allocate memory for link */
    }

    link->state = SEND_CREATE_CONNECTION;
    bd_addr_set(&(link->bdaddr), bdaddr);
    HCI_REG(&(hci_active_links), link);


    /* Check if module has been discovered in a recent inquiry */
    for(inqres = hci_pcb->ires; inqres != NULL; inqres = inqres->next)
    {
        if(bd_addr_cmp(&inqres->bdaddr, bdaddr))
        {
            page_scan_repetition_mode = inqres->psrm;
            page_scan_mode = inqres->psm;
            clock_offset = inqres->co;
            break;
        }
    }
    if(inqres == NULL)
    {
        /* No information on parameters from an inquiry. Using default values */
        page_scan_repetition_mode = 0x01; /* Assuming worst case: time between
					 successive page scans starting
					 <= 2.56s */
        page_scan_mode = 0x00; /* Assumes the device uses mandatory scanning, most
			      devices use this. If no conn is established, try
			      again w this parm set to optional page scanning */
        clock_offset = 0x00; /* If the device was not found in a recent inquiry
			    this  information is irrelevant */
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_CREATE_CONN_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_CREATE_CONNECTION, HCI_LINK_CONTROL, HCI_CREATE_CONN_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, 6);
    bt_le_store_16((uint8_t *)p->payload,9,HCI_PACKET_TYPE);
    ((uint8_t *)p->payload)[11] = page_scan_repetition_mode;
    ((uint8_t *)p->payload)[12] = page_scan_mode;
    bt_le_store_16((uint8_t *)p->payload,13,clock_offset);
    ((uint8_t *)p->payload)[15] = allow_role_switch;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}



err_t hci_disconnect_acl(struct bd_addr_t *bdaddr, uint8_t reason)
{
    struct bt_pbuf_t *p;
    hci_link_t *link;

    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] link not exist\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN; /* Connection does not exist */
    }
    link->state = SEND_DISCONNECT;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_DISCONN_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_DISCONNECT, HCI_LINK_CONTROL, HCI_DISCONN_PLEN);

    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->conhdl);
    ((uint8_t *)p->payload)[5] = reason;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_disconnect_sco(struct bd_addr_t *bdaddr, uint8_t reason)
{
    struct bt_pbuf_t *p;
    hci_link_t *link;

    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN; /* Connection does not exist */
    }
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_DISCONN_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_DISCONNECT, HCI_LINK_CONTROL, HCI_DISCONN_PLEN);

    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->sco_connhdl);
    ((uint8_t *)p->payload)[5] = reason;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_accept_connection_request(struct bd_addr_t *bdaddr, uint8_t role)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_ACCEPT_CON_REQ_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_ACCEPT_CONNECTION_REQUEST, HCI_LINK_CONTROL, HCI_ACCEPT_CON_REQ_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload) + 3, bdaddr->addr, 6);
    ((uint8_t *)p->payload)[9] = role;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_reject_connection_request(struct bd_addr_t *bdaddr, uint8_t reason)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_REJECT_CONN_REQ_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_REJECT_CONNECTION_REQUEST, HCI_LINK_CONTROL, HCI_REJECT_CONN_REQ_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload) + 3, bdaddr->addr, 6);
    ((uint8_t *)p->payload)[9] = reason;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_link_key_request_reply(struct bd_addr_t *bdaddr, uint8_t *link_key)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_LINK_KEY_REQ_REPLAY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LINKKEY_REQ_REPLY, HCI_LINK_CONTROL, HCI_LINK_KEY_REQ_REPLAY_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload) + 3, bdaddr->addr, 6);
    memcpy(((uint8_t *)p->payload) + 3+6, link_key, 16);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_link_key_request_negative_reply(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_LINK_KEY_REQ_NG_REPLAY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LINKKEY_REQ_NEG_REPLY, HCI_LINK_CONTROL, HCI_LINK_KEY_REQ_NG_REPLAY_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload) + 3, bdaddr->addr, 6);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_pin_code_request_reply(struct bd_addr_t *bdaddr, uint8_t pinlen, uint8_t *pincode)
{
    struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_PIN_CODE_REQ_REP_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Reset buffer content just to make sure */
    memset((uint8_t *)p->payload, 0, HCI_PIN_CODE_REQ_REP_PLEN);

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_PIN_CODE_REQ_REP, HCI_LINK_CONTROL, HCI_PIN_CODE_REQ_REP_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload) + 3, bdaddr->addr, 6);
    ((uint8_t *)p->payload)[9] = pinlen;
    memcpy(((uint8_t *)p->payload) + 10, pincode, pinlen);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_pin_code_request_neg_reply(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_PIN_CODE_REQ_NEG_REP_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_PIN_CODE_REQ_NEG_REP, HCI_LINK_CONTROL, HCI_PIN_CODE_REQ_NEG_REP_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, 6);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_get_remote_name(struct bd_addr_t *bdaddr,err_t (*name_req_complete)(struct bd_addr_t *bdaddr,uint8_t * name))
{
    uint8_t page_scan_repetition_mode;
    uint16_t clock_offset;
    struct bt_pbuf_t *p;
    hci_inq_res_t *inqres;

    hci_pcb->name_req_complete = name_req_complete;
    /* Check if module has been discovered in a recent inquiry */
    for(inqres = hci_pcb->ires; inqres != NULL; inqres = inqres->next)
    {
        if(bd_addr_cmp(&inqres->bdaddr, bdaddr))
        {
            page_scan_repetition_mode = inqres->psrm;
            clock_offset = inqres->co;
            break;
        }
    }
    if(inqres == NULL)
    {
        /* No information on parameters from an inquiry. Using default values */
        page_scan_repetition_mode = 0x01; /* Assuming worst case: time between
					 successive page scans starting
					 <= 2.56s */
        clock_offset = 0x00; /* If the device was not found in a recent inquiry
			    this  information is irrelevant */
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_GET_REMOTE_NAME_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_REMOTE_NAME_REQ, HCI_LINK_CONTROL, HCI_GET_REMOTE_NAME_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, 6);
    ((uint8_t *)p->payload)[9] = page_scan_repetition_mode;
    ((uint8_t *)p->payload)[10] = 0;
    bt_le_store_16((uint8_t *)p->payload,11,clock_offset);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}


err_t hci_cancel_get_remote_name(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_CANCEL_GET_REMOTE_NAME_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_REMOTE_NAME_REQ_CANCEL, HCI_LINK_CONTROL, HCI_CANCEL_GET_REMOTE_NAME_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, 6);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t hci_get_remote_feature(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    hci_link_t *link;

    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN; /* Connection does not exist */
    }
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_GET_REMOTE_FEATURE_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_READ_REMOTE_SUPPORT_FEATURE, HCI_LINK_CONTROL, HCI_GET_REMOTE_FEATURE_PLEN);

    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->conhdl);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);


    return BT_ERR_OK;
}


err_t hci_connect_sco(struct bd_addr_t *bdaddr,uint32_t transmit_bandwidth,uint32_t receive_bandwidth,
                      uint16_t max_latency,uint16_t voice_setting,uint8_t retransmission_effort,uint16_t packet_type)
{
    struct bt_pbuf_t *p;
    uint8_t offset = 0;
    hci_link_t *link = _hci_get_link_by_addr(bdaddr);
    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN;
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_CONNECT_SCO_REQ_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_CONNECT_SCO_REQ, HCI_LINK_CONTROL, HCI_CONNECT_SCO_REQ_PLEN);
    offset += 3;
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,offset,link->conhdl);
    offset += 2;
    bt_le_store_32((uint8_t *)p->payload,offset,transmit_bandwidth);
    offset += 4;
    bt_le_store_32((uint8_t *)p->payload,offset,receive_bandwidth);
    offset += 4;
    bt_le_store_16((uint8_t *)p->payload,offset,max_latency);
    offset += 2;
    bt_le_store_16((uint8_t *)p->payload,offset,voice_setting);
    offset += 2;
    ((uint8_t *)p->payload)[offset] = retransmission_effort;
    offset += 1;
    bt_le_store_16((uint8_t *)p->payload,offset,packet_type);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_accept_sco_req(struct bd_addr_t *bdaddr,uint32_t transmit_bandwidth,uint32_t receive_bandwidth,
                         uint16_t max_latency,uint16_t voice_setting,uint8_t retransmission_effort,uint16_t packet_type)
{
    struct bt_pbuf_t *p;
    uint8_t offset = 0;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_ACCEPT_SCO_REQ_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_ACCEP_SCO_REQ, HCI_LINK_CONTROL, HCI_ACCEPT_SCO_REQ_PLEN);
    offset += 3;
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+offset, bdaddr->addr, BD_ADDR_LEN);
    offset += BD_ADDR_LEN;
    bt_le_store_32((uint8_t *)p->payload,offset,transmit_bandwidth);
    offset += 4;
    bt_le_store_32((uint8_t *)p->payload,offset,receive_bandwidth);
    offset += 4;
    bt_le_store_16((uint8_t *)p->payload,offset,max_latency);
    offset += 2;
    bt_le_store_16((uint8_t *)p->payload,offset,voice_setting);
    offset += 2;
    ((uint8_t *)p->payload)[offset] = retransmission_effort;
    offset += 1;
    bt_le_store_16((uint8_t *)p->payload,offset,packet_type);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_reject_sco_req(struct bd_addr_t *bdaddr,uint8_t reason)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_REJECT_SCO_REQ_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_REJECT_SCO_REQ, HCI_LINK_CONTROL, HCI_REJECT_SCO_REQ_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, BD_ADDR_LEN);
    ((uint8_t *)p->payload)[9] = reason;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_io_cap_req_replay(struct bd_addr_t *bdaddr,uint8_t io_cap,uint8_t oob,uint8_t auth_req)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_IOCAP_REQ_REPLY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_IOCAP_REQ_REPLY, HCI_LINK_CONTROL, HCI_IOCAP_REQ_REPLY_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, BD_ADDR_LEN);
    ((uint8_t *)p->payload)[9] = io_cap;
    ((uint8_t *)p->payload)[10] = oob;
    ((uint8_t *)p->payload)[11] = auth_req;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_user_confirm_req_reply(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_USER_CONFIRM_REQ_REPLY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_USER_CONFIRM_REQ_REPLY, HCI_LINK_CONTROL, HCI_USER_CONFIRM_REQ_REPLY_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, BD_ADDR_LEN);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}
err_t hci_user_confirm_req_neg_reply(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_USER_CONFIRM_REQ_NEG_REPLY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_USER_CONFIRM_REQ_NEG_REPLY, HCI_LINK_CONTROL, HCI_USER_CONFIRM_REQ_NEG_REPLY_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, BD_ADDR_LEN);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_user_passkey_req_reply(struct bd_addr_t *bdaddr,uint32_t num_value)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_USER_PASSKEY_REQ_REPLY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_USER_PASSKEY_REQ_REPLY, HCI_LINK_CONTROL, HCI_USER_PASSKEY_REQ_REPLY_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, BD_ADDR_LEN);
    bt_le_store_32((uint8_t *)p->payload,9,num_value);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_user_passkey_req_neg_reply(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_USER_PASSKEY_REQ_NEG_REPLY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_USER_PASSKEY_REQ_NEG_REPLY, HCI_LINK_CONTROL, HCI_USER_PASSKEY_REQ_NEG_REPLY_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, BD_ADDR_LEN);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_io_cap_req_neg_reply(struct bd_addr_t *bdaddr,uint8_t reason)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_IOCAP_REQ_NEG_REPLY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_IOCAP_REQ_NEG_REPLY, HCI_LINK_CONTROL, HCI_IOCAP_REQ_NEG_REPLY_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, BD_ADDR_LEN);
    ((uint8_t *)p->payload)[9] = reason;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_hold_mode(struct bd_addr_t *bdaddr, uint16_t max_interval, uint16_t min_interval)
{
    struct bt_pbuf_t *p;
    hci_link_t *link;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN;
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_HOLD_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_HOLD_MODE, HCI_LINK_POLICY, HCI_HOLD_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->conhdl);
    bt_le_store_16((uint8_t *)p->payload,5,max_interval);
    bt_le_store_16((uint8_t *)p->payload,7,min_interval);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}


err_t hci_sniff_mode(struct bd_addr_t *bdaddr, uint16_t max_interval, uint16_t min_interval,
                     uint16_t attempt, uint16_t timeout)
{
    struct bt_pbuf_t *p;
    hci_link_t *link;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN;
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SNIFF_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_SNIFF_MODE, HCI_LINK_POLICY, HCI_SNIFF_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->conhdl);
    bt_le_store_16((uint8_t *)p->payload,5,max_interval);
    bt_le_store_16((uint8_t *)p->payload,7,min_interval);
    bt_le_store_16((uint8_t *)p->payload,9,attempt);
    bt_le_store_16((uint8_t *)p->payload,11,timeout);


    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t hci_exit_sniff_mode(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    hci_link_t *link;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN;
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_EXIT_SNIFF_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_EXIT_SNIFF_MODE, HCI_LINK_POLICY, HCI_EXIT_SNIFF_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->conhdl);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t hci_role_discovery(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    hci_link_t *link;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN;
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_ROLE_DISCOVERY_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_ROLE_DISCOVERY, HCI_LINK_POLICY, HCI_ROLE_DISCOVERY_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->conhdl);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t hci_switch_role(struct bd_addr_t *bdaddr,uint8_t role)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SWITCH_ROLE_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_SWITCH_ROLE, HCI_LINK_POLICY, HCI_SWITCH_ROLE_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, bdaddr->addr, BD_ADDR_LEN);
    ((uint8_t *)p->payload)[9] = role;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_read_link_policy(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    hci_link_t *link;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN;
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_R_LINK_POLICY_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_R_LINK_POLICY, HCI_LINK_POLICY, HCI_R_LINK_POLICY_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->conhdl);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}


err_t hci_write_link_policy(struct bd_addr_t *bdaddr, uint16_t link_policy)
{
    struct bt_pbuf_t *p;
    hci_link_t *link;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }

    if( (p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_W_LINK_POLICY_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_W_LINK_POLICY, HCI_LINK_POLICY, HCI_W_LINK_POLICY_PLEN);

    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->conhdl);
    bt_le_store_16((uint8_t *)p->payload,5,link_policy);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t hci_read_default_link_policy(void)
{
    struct bt_pbuf_t *p;

    if( (p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_R_DEFAULT_LINK_POLICY_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_R_DEFAULT_LINK_POLICY, HCI_LINK_POLICY, HCI_R_DEFAULT_LINK_POLICY_PLEN);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t hci_write_default_link_policy(uint16_t link_policy)
{
    struct bt_pbuf_t *p;

    if( (p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_W_DEFAULT_LINK_POLICY_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_W_DEFAULT_LINK_POLICY, HCI_LINK_POLICY, HCI_W_DEFAULT_LINK_POLICY_PLEN);
    bt_le_store_16((uint8_t *)p->payload,3,link_policy);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t hci_set_event_mask(uint32_t mask_lo,uint32_t mask_hi)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_EVENT_MASK_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_SET_EVENT_MASK, HCI_HOST_C_N_BB, HCI_SET_EVENT_MASK_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_32((uint8_t *)p->payload,3,mask_lo);
    bt_le_store_32((uint8_t *)p->payload,7,mask_hi);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_reset(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_RESET_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_RESET, HCI_HOST_C_N_BB, HCI_RESET_PLEN);
    hci_pcb->timer = utimer_create(HCI_RESET_TIMEOUT, _hci_reset_cmd_timeout, 0);
    /* Assembling cmd prameters */
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_set_event_filter(uint8_t filter_type, uint8_t filter_cond_type, uint8_t* cond)
{
    uint8_t cond_len = 0x00;
    struct bt_pbuf_t *p;
    switch(filter_type)
    {
    case HCI_SET_EV_FILTER_CLEAR:
        BT_HCI_TRACE_DEBUG("hci_set_event_filter: Clearing all filters\n");

        cond_len = 0x00;
        break;
    case HCI_SET_EV_FILTER_INQUIRY:
        switch(filter_cond_type)
        {
        case HCI_SET_EV_FILTER_ALLDEV:
            cond_len = 0x00;
            break;
        case HCI_SET_EV_FILTER_COD:
            cond_len = 0x06;
            break;
        case HCI_SET_EV_FILTER_BDADDR:
            cond_len = 0x06;
            break;
        default:
            BT_HCI_TRACE_DEBUG("hci_set_event_filter: Entered an unspecified filter condition type!\n");

            break;
        }
        break;
    case HCI_SET_EV_FILTER_CONNECTION:
        switch(filter_cond_type)
        {
        case HCI_SET_EV_FILTER_AUTOACC_OFF:
            cond_len = 0x01;
            break;
        case HCI_SET_EV_FILTER_AUTOACC_NOROLESW:
            cond_len = 0x07;
            break;
        case HCI_SET_EV_FILTER_AUTOACC_ROLESW:
            cond_len = 0x07;
            break;
        default:
            BT_HCI_TRACE_DEBUG("hci_set_event_filter: Entered an unspecified filter condition type!\n");

            break;
        }
        break;
    default:
        BT_HCI_TRACE_DEBUG("hci_set_event_filter: Entered an unspecified filter type!\n");

        break;
    }
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_EV_FILTER_PLEN+cond_len, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_SET_EVENT_FILTER, HCI_HOST_C_N_BB, HCI_SET_EV_FILTER_PLEN+cond_len);
    ((uint8_t *)p->payload)[3] = filter_type;
    ((uint8_t *)p->payload)[4] = filter_cond_type;
    /* Assembling cmd prameters */
    if(cond_len)
    {
        memcpy(((uint8_t *)p->payload)+5, cond, cond_len);
    }
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}


err_t hci_write_stored_link_key(struct bd_addr_t *bdaddr, uint8_t *link)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_WRITE_STORED_LINK_KEY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_STORED_LINK_KEY, HCI_HOST_C_N_BB, HCI_WRITE_STORED_LINK_KEY_PLEN);
    /* Assembling cmd prameters */
    ((uint8_t *)p->payload)[3] = 0x01;
    memcpy(((uint8_t *)p->payload) + 4, bdaddr->addr, 6);
    memcpy(((uint8_t *)p->payload) + 10, link, 16);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_write_local_name(uint8_t *name, uint8_t len)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_CHANGE_LOCAL_NAME_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_LOCAL_NAME, HCI_HOST_C_N_BB, HCI_CHANGE_LOCAL_NAME_PLEN);
    /* Assembling cmd prameters */
    memset(((uint8_t *)p->payload) + 3,0,HCI_CHANGE_LOCAL_NAME_PLEN-3);
    memcpy(((uint8_t *)p->payload) + 3, name, len);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_read_local_name(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_READ_LOCAL_NAME_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_READ_LOCAL_NAME, HCI_HOST_C_N_BB, HCI_READ_LOCAL_NAME_PLEN);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_write_page_timeout(uint16_t page_timeout)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_W_PAGE_TIMEOUT_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_PAGE_TIMEOUT, HCI_HOST_C_N_BB, HCI_W_PAGE_TIMEOUT_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,page_timeout);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_write_scan_enable(uint8_t scan_enable)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_W_SCAN_EN_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_SCAN_ENABLE, HCI_HOST_C_N_BB, HCI_W_SCAN_EN_PLEN);
    /* Assembling cmd prameters */
    ((uint8_t *)p->payload)[3] = scan_enable;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_write_cod(uint8_t *cod)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_W_COD_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_COD, HCI_HOST_C_N_BB, HCI_W_COD_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload)+3, cod, 3);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_read_voice_setting(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_R_VOICE_SETTING_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_READ_VOICE_SETTING, HCI_HOST_C_N_BB, HCI_R_VOICE_SETTING_PLEN);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}
err_t hci_write_voice_setting(uint16_t voice_setting)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_W_VOICE_SETTING_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_VOICE_SETTING, HCI_HOST_C_N_BB, HCI_W_VOICE_SETTING_PLEN);

    bt_le_store_16((uint8_t *)p->payload,3,voice_setting);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_write_flush_timeout(struct bd_addr_t *bdaddr, uint16_t flushto)
{
    hci_link_t *link;
    struct bt_pbuf_t *p;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_W_FLUSHTO_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_FLUSHTO, HCI_HOST_C_N_BB, HCI_W_FLUSHTO_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,4,link->conhdl);
    bt_le_store_16((uint8_t *)p->payload,6,flushto);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t hci_set_hc_to_h_fc(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_HC_TO_H_FC_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_SET_HC_TO_H_FC, HCI_HOST_C_N_BB, HCI_SET_HC_TO_H_FC_PLEN);
    /* Assembling cmd prameters */
    ((uint8_t *)p->payload)[3] = 0x01; /* Flow control on for HCI ACL Data Packets and off for HCI
				     SCO Data Packets in direction from Host Controller to
				     Host */
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_host_buffer_size(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_H_BUF_SIZE_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_HOST_BUFF_SIZE, HCI_HOST_C_N_BB, HCI_H_BUF_SIZE_PLEN);
    bt_le_store_16((uint8_t *)p->payload,4,HCI_HOST_ACL_MAX_LEN); /* Host ACL data packet maximum length */
    ((uint8_t *)p->payload)[6] = 255; /* Host SCO Data Packet Length */
    *((uint16_t *)(((uint8_t *)p->payload)+7)) = HCI_HOST_MAX_NUM_ACL; /* Host max total num ACL data packets */
    bt_le_store_16((uint8_t *)p->payload,8,1); /* Host Total Num SCO Data Packets */
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    hci_pcb->host_num_acl = HCI_HOST_MAX_NUM_ACL;

    return BT_ERR_OK;
}

err_t hci_host_num_comp_packets(uint16_t conhdl, uint16_t num_complete)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_H_NUM_COMPL_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_HOST_NUM_COMPL, HCI_HOST_C_N_BB, HCI_H_NUM_COMPL_PLEN);
    ((uint8_t *)p->payload)[3] = 1;
    bt_le_store_16((uint8_t *)p->payload,4,conhdl);
    bt_le_store_16((uint8_t *)p->payload,6,num_complete); /* Number of completed acl packets */

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    hci_pcb->host_num_acl += num_complete;

    return BT_ERR_OK;
}


err_t hci_write_current_iac_lap(uint8_t iac_num,uint32_t *iac)
{
	uint8_t index = 0;
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_WRITE_IAC_LAP_HDR_PLEN+1+3*iac_num, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_CURRENT_IAC, HCI_HOST_C_N_BB, HCI_WRITE_IAC_LAP_HDR_PLEN+1+3*iac_num);
	/* Assembling cmd prameters */
	((uint8_t *)p->payload)[3] = iac_num;
    
	for(index = 0; index < iac_num; index++)
    	bt_le_store_24((uint8_t *)p->payload,4+index*3,*(iac+index));

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_write_inquiry_mode(uint8_t inquiry_mode)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_W_INQUIRY_MODE_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_INQUIRY_MODE, HCI_HOST_C_N_BB, HCI_W_INQUIRY_MODE_LEN);
    ((uint8_t *)p->payload)[3] = inquiry_mode;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);


    return BT_ERR_OK;
}

err_t hci_write_eir(uint8_t *eir_data)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_WRITE_EIR_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_EIR, HCI_HOST_C_N_BB, HCI_WRITE_EIR_PLEN);
    /* Assembling cmd prameters */
    ((uint8_t *)p->payload)[3] = 0x01; /* FEC is required */
    memset(((uint8_t *)p->payload)+4,0,240);
    memcpy(((uint8_t *)p->payload)+4,eir_data,240);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_write_ssp_mode(uint8_t ssp_mode)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_WRITE_SSP_MODE_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_SSP_MODE, HCI_HOST_C_N_BB, HCI_WRITE_SSP_MODE_PLEN);
    /* Assembling cmd prameters */
    ((uint8_t *)p->payload)[3] = ssp_mode; /* FEC is required */

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_write_le_enable(uint8_t le_support,uint8_t simultaneous)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_WRITE_LE_SUPPORT_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_WRITE_LE_SUPPORT, HCI_HOST_C_N_BB, HCI_WRITE_LE_SUPPORT_PLEN);
    /* Assembling cmd prameters */
    ((uint8_t *)p->payload)[3] = le_support;
    ((uint8_t *)p->payload)[4] = simultaneous;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;

}

err_t hci_read_local_version_info(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_R_LOCOL_VER_INFO_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_READ_LOCAL_VER_INFO, HCI_INFO_PARAM, HCI_R_LOCOL_VER_INFO_PLEN);
    /* Assembling cmd prameters */
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_read_local_support_cmd(void)
{
	struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_R_LOCAL_SUPPORT_CMD_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_READ_LOCAL_SUPPORT_CMD, HCI_INFO_PARAM, HCI_R_LOCAL_SUPPORT_CMD_PLEN);
    /* Assembling cmd prameters */
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_read_buffer_size(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_R_BUF_SIZE_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_READ_BUFFER_SIZE, HCI_INFO_PARAM, HCI_R_BUF_SIZE_PLEN);
    /* Assembling cmd prameters */
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_read_bd_addr(err_t (* rbd_complete)(void *arg, struct bd_addr_t *bdaddr))
{
    struct bt_pbuf_t *p;

    hci_pcb->rbd_complete = rbd_complete;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_R_BD_ADDR_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_READ_BD_ADDR, HCI_INFO_PARAM, HCI_R_BD_ADDR_PLEN);
    /* Assembling cmd prameters */
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_read_rssi(struct bd_addr_t *bdaddr)
{
    hci_link_t *link;
    struct bt_pbuf_t *p;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_READ_RSSI_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_READ_RSSI, HCI_STATUS_PARAM, HCI_READ_RSSI_PLEN);
    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,4,link->conhdl);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t hci_enable_dut_mode(void)
{
    struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_ENABLE_DUT_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_ENABLE_DUT_MODE, HCI_TESTING, HCI_ENABLE_DUT_PLEN);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

#if BT_BLE_ENABLE > 0

err_t hci_le_set_event_mask(uint8_t mask[8])
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_LE_EVENT_MASK_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_SET_EVT_MASK, HCI_LE, HCI_SET_LE_EVENT_MASK_PLEN);
    /* Assembling cmd prameters */
    memcpy((uint8_t *)p->payload+3,mask,8);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_le_read_buffer_size(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_LE_R_BUF_SIZE_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_READ_BUF_SIZE, HCI_LE, HCI_LE_R_BUF_SIZE_PLEN);
    /* Assembling cmd prameters */
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;

}

err_t hci_le_read_local_support_feature(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_LE_R_LOCAL_SUPPORT_FEATURES_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_READ_LOCAL_SUPPORT_FEATURES, HCI_LE, HCI_LE_R_LOCAL_SUPPORT_FEATURES_PLEN);
    /* Assembling cmd prameters */
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_set_random_address(struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_LE_SET_RANDOM_ADDR_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);


        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_SET_RANDOM_ADDRESS, HCI_LE, HCI_LE_SET_RANDOM_ADDR_PLEN);
    /* Assembling cmd prameters */
    memcpy(((uint8_t *)p->payload) + 3, bdaddr->addr, BD_ADDR_LEN);
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}




err_t hci_le_set_scan_param(uint8_t scan_type,uint16_t scan_interval,uint16_t scan_window,uint8_t own_type,uint8_t scan_filter)
{
    struct bt_pbuf_t *p;
    uint8_t offset = 0;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_LE_SCAN_PARAM_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_SET_SCAN_PARAM, HCI_LE, HCI_SET_LE_SCAN_PARAM_PLEN);
    offset += 3;
    ((uint8_t *)p->payload)[offset] = scan_type;
    offset += 1;
    bt_le_store_16((uint8_t *)p->payload,offset,scan_interval);
    offset += 2;
    bt_le_store_16((uint8_t *)p->payload,offset,scan_window);
    offset += 2;
    ((uint8_t *)p->payload)[offset] = own_type;
    offset += 1;
    ((uint8_t *)p->payload)[offset] = scan_filter;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_inquiry(uint8_t filter_duplicates,
                     le_inq_result_fun_cb le_inq_result,
                     le_inq_complete_fun_cb le_inq_complete)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_LE_SCAN_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    hci_pcb->le_inq_result = le_inq_result;
    hci_pcb->le_inq_complete = le_inq_complete;

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_SET_SCAN, HCI_LE, HCI_SET_LE_SCAN_PLEN);
    /* Assembling cmd prameters */
    ((uint8_t *)p->payload)[3] = 1;
    ((uint8_t *)p->payload)[4] = filter_duplicates;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_le_cancel_inquiry(void)
{
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_LE_SCAN_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    hci_pcb->le_inq_w2_stop  = 1;
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_SET_SCAN, HCI_LE, HCI_SET_LE_SCAN_PLEN);
    /* Assembling cmd prameters */
    ((uint8_t *)p->payload)[3] = 0;
    ((uint8_t *)p->payload)[4] = 0;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_set_adv_param(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type,
                           uint8_t own_address_typ, uint8_t peer_address_type,struct bd_addr_t *peer_address, uint8_t channel_map, uint8_t filter_policy)
{
    struct bt_pbuf_t *p;
    uint8_t offset = 0;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_LE_ADV_PARAM_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_SET_ADV_PARAM, HCI_LE, HCI_SET_LE_ADV_PARAM_PLEN);
    offset += 3;
    bt_le_store_16((uint8_t *)p->payload,offset,adv_int_min);
    offset += 2;
    bt_le_store_16((uint8_t *)p->payload,offset,adv_int_max);
    offset += 2;
    ((uint8_t *)p->payload)[offset] = adv_type;
    offset += 1;
    ((uint8_t *)p->payload)[offset] = own_address_typ;
    offset += 1;
    ((uint8_t *)p->payload)[offset] = peer_address_type;
    offset += 1;
    memcpy(((uint8_t *)p->payload)+offset, peer_address->addr, BD_ADDR_LEN);
    offset += BD_ADDR_LEN;
    ((uint8_t *)p->payload)[offset] = channel_map;
    offset += 1;
    ((uint8_t *)p->payload)[offset] = filter_policy;
    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_set_adv_data(uint8_t adv_len,uint8_t *adv_data)
{
    struct bt_pbuf_t *p;
    uint8_t offset = 0;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_LE_ADV_DATA_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_SET_ADV_DATA, HCI_LE, HCI_SET_LE_ADV_DATA_PLEN);
    offset += 3;
    ((uint8_t *)p->payload)[offset] = adv_len;
    offset += 1;

    memset(((uint8_t *)p->payload) + offset,0,HCI_SET_LE_ADV_DATA_PLEN-offset);
    memcpy(((uint8_t *)p->payload)+offset, adv_data, adv_len);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_set_adv_enable(uint8_t enable)
{
    struct bt_pbuf_t *p;
    uint8_t offset = 0;
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_LE_ADV_ENABLE_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_SET_ADV_ENABLE, HCI_LE, HCI_SET_LE_ADV_ENABLE_PLEN);
    offset += 3;
    ((uint8_t *)p->payload)[offset] = enable;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


err_t hci_le_ltk_req_reply(struct bd_addr_t *bdaddr,uint8_t *ltk)
{
	struct bt_pbuf_t *p;
    uint8_t offset = 0;
	hci_link_t *link = _hci_get_link_by_addr(bdaddr);
    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_LTK_REQ_REPLY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

	if(!link)
	{
		BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] link is not exit\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
	}
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_LTK_REQ_REPLY, HCI_LE, HCI_LTK_REQ_REPLY_PLEN);
    offset += 3;
    bt_le_store_16((uint8_t *)p->payload,offset,link->conhdl);
	offset += 2;

	memcpy(((uint8_t *)p->payload)+offset, ltk, 16);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_ltk_req_neg_replay(struct bd_addr_t *bdaddr)
{
	struct bt_pbuf_t *p;
    hci_link_t *link;

    /* Check if an ACL connection exists */
    link = _hci_get_link_by_addr(bdaddr);

    if(link == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Connection does not existl\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }

    if( (p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_LTK_REQ_NEG_REPLAY_PLEN, BT_PBUF_RAM)) == NULL)   /* Alloc len of packet */
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_LTK_REQ_NEG_REPLAY, HCI_LE, HCI_LTK_REQ_NEG_REPLAY_PLEN);

    /* Assembling cmd prameters */
    bt_le_store_16((uint8_t *)p->payload,3,link->conhdl);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}


err_t hci_le_read_p256_public_key(void)
{
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_READ_LOCAL_P256_PUBLIC_KEY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_READ_LOCAL_P256_PUBLIC_KEY, HCI_LE, HCI_READ_LOCAL_P256_PUBLIC_KEY_PLEN);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_generate_dhkey(uint8_t *remote_public_key)
{
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_GENERATE_DHKEY_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_GENERATE_DHKEY, HCI_LE, HCI_GENERATE_DHKEY_PLEN);
	memcpy(((uint8_t *)p->payload)+3, remote_public_key, 64);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_add_dev_resolv_list(uint8_t peer_iat,uint8_t peer_ia[6],uint8_t peer_irk[16],uint8_t local_irk[16])
{
	uint8_t offset = 0;
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_ADD_DEV_RESOLV_LIST_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_ADD_DEV_RESOLVING_LIST, HCI_LE, HCI_ADD_DEV_RESOLV_LIST_PLEN);
	offset += 3;
    ((uint8_t *)p->payload)[offset] = peer_iat;
	offset += 1;
	memcpy(((uint8_t *)p->payload)+offset, peer_ia, 6);
	offset += 6;
	memcpy(((uint8_t *)p->payload)+offset, peer_irk, 16);
	offset += 16;
	memcpy(((uint8_t *)p->payload)+offset, local_irk, 16);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_rm_dev_resolv_list(uint8_t peer_iat,uint8_t peer_ia[6])
{
	uint8_t offset = 0;
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_RM_DEV_RESOLV_LIST_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_RM_DEV_RESOLVING_LIST, HCI_LE, HCI_RM_DEV_RESOLV_LIST_PLEN);
	offset += 3;
    ((uint8_t *)p->payload)[offset] = peer_iat;
	offset += 1;
	memcpy(((uint8_t *)p->payload)+offset, peer_ia, 6);
	offset += 6;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_clear_resolv_list(void)
{
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_CLEAR_RESOLV_LIST_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_CLEAR_RESOLVING_LIST, HCI_LE, HCI_CLEAR_RESOLV_LIST_PLEN);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_read_resolv_list_size(void)
{
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_READ_RESOLV_LIST_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_READ_RESOLV_SIZE, HCI_LE, HCI_READ_RESOLV_LIST_PLEN);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_read_peer_resolv_addr(uint8_t peer_iat,uint8_t peer_ia[6])
{
	uint8_t offset = 0;
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_READ_PEER_RESOLV_ADDR_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_READ_PEER_RESOLV_ADDR, HCI_LE, HCI_READ_PEER_RESOLV_ADDR_PLEN);
	offset += 3;
    ((uint8_t *)p->payload)[offset] = peer_iat;
	offset += 1;
	memcpy(((uint8_t *)p->payload)+offset, peer_ia, 6);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_read_local_resolv_addr(uint8_t peer_iat,uint8_t peer_ia[6])
{
	uint8_t offset = 0;
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_READ_LOCAL_RESOLV_ADDR_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_READ_LOCAL_RESOLV_ADDR, HCI_LE, HCI_READ_LOCAL_RESOLV_ADDR_PLEN);
	offset += 3;
    ((uint8_t *)p->payload)[offset] = peer_iat;
	offset += 1;
	memcpy(((uint8_t *)p->payload)+offset, peer_ia, 6);

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t hci_le_set_addr_resolution_enable(uint8_t enable)
{
	struct bt_pbuf_t *p;

    if((p = bt_pbuf_alloc(BT_TRANSPORT_TYPE, HCI_SET_ADDR_RESOLUTION_ENABLE_PLEN, BT_PBUF_RAM)) == NULL)
    {
        BT_HCI_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Assembling command packet */
    p = hci_cmd_ass(p, HCI_LE_SET_ADDR_RESOLUTION_ENABLE, HCI_LE, HCI_SET_ADDR_RESOLUTION_ENABLE_PLEN);
    ((uint8_t *)p->payload)[3] = enable;

    phybusif_output(p, p->tot_len,PHYBUSIF_PACKET_TYPE_CMD);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


#endif


