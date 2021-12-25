/******************************************************************************
  * @file    bt_debug.h
  * @author  Yu-ZhongJun(124756828@qq.com)
  * @version V0.0.1
  * @date    2020-3-31
  * @brief   bt debug header file
******************************************************************************/
#ifndef BT_DEBUG_H_H_H
#define BT_DEBUG_H_H_H
#include "bt_config.h"

#if BT_HEX_TRACE_COLOR > 0
#define LOG_COLOR_YELLOW "\033[40;33m"
#define LOG_COLOR_RED "\033[40;31m"
#define LOG_COLOR_BLUE "\033[40;36m"
#define LOG_COLOR_PURPLE "\033[40;35m"
#define LOG_COLOR_RESET "\033[0m"
#else
#define LOG_COLOR_YELLOW 
#define LOG_COLOR_RED 
#define LOG_COLOR_BLUE 
#define LOG_COLOR_PURPLE 
#define LOG_COLOR_RESET 
#endif


/* Define trace levels */
#define BT_TRACE_LEVEL_NONE 0    /* No trace messages to be generated    */
#define BT_TRACE_LEVEL_ERROR 1   /* Error condition trace messages       */
#define BT_TRACE_LEVEL_WARNING 2 /* Warning condition trace messages     */
#define BT_TRACE_LEVEL_DEBUG 3   /* Full debug messages                  */
#define BT_TRACE_LEVEL_INFO 4 /* info messages                  */

#define BT_HEX_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_HEX_TRACE_LEVEL > BT_TRACE_LEVEL_NONE)               \
      BT_DEBUG(fmt,##__VA_ARGS__); \
  }


#define BT_PBUF_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_PBUF_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ PBUF ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_PBUF_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_PBUF_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ PBUF ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_PBUF_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_PBUF_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ PBUF ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_MEMORY_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_MEMORY_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ MEM ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_MEMORY_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_MEMORY_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ MEM ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_MEMORY_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_MEMORY_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ MEM ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_SNOOP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_SNOOP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ BTSNOOP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_SNOOP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_SNOOP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ BTSNOOP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_SNOOP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_SNOOP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ BTSNOOP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_TRANSPORT_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_TRANSPORT_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ TRANSPORT ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_TRANSPORT_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_TRANSPORT_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ TRANSPORT ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_TRANSPORT_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_TRANSPORT_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ TRANSPORT ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_VENDOR_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_VENDOR_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ TRANSPORT ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_VENDOR_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_VENDOR_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ VENDOR ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_VENDOR_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_VENDOR_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ VENDOR ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_HCI_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_HCI_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG( LOG_COLOR_BLUE"[ HCI ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_HCI_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_HCI_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HCI ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_HCI_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_HCI_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HCI ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_L2CAP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_L2CAP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ L2CAP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_L2CAP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_L2CAP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ L2CAP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_L2CAP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_L2CAP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ L2CAP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_SDP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_SDP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ SDP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_SDP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_SDP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ SDP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_SDP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_SDP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ SDP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_RFCOMM_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_RFCOMM_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ RFCOMM ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_RFCOMM_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_RFCOMM_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ RFCOMM ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_RFCOMM_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_RFCOMM_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ RFCOMM ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_HFP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_HFP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HFP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_HFP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_HFP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HFP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_HFP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_HFP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HFP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_HSP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_HSP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HSP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_HSP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_HSP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HSP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_HSP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_HSP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HSP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_OBEX_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_OBEX_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ OBEX ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_OBEX_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_OBEX_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ OBEX ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_OBEX_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_OBEX_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ OBEX ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_PBAP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_PBAP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ PBAP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_PBAP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_PBAP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ PBAP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_PBAP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_PBAP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ PBAP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_MAP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_MAP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ MAP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_MAP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_MAP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ MAP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_MAP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_MAP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ MAP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_DID_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_DID_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ DID ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_DID_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_DID_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ DID ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_DID_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_DID_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ DID ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_SPP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_SPP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ SPP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_SPP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_SPP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ SPP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_SPP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_SPP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ SPP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_AVDTP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_AVDTP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVDTP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_AVDTP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_AVDTP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVDTP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_AVDTP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_AVDTP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVDTP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_AVDTP_INFO_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_AVDTP_TRACE_LEVEL >= BT_TRACE_LEVEL_INFO)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVDTP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_AVCTP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_AVCTP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVCTP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_AVCTP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_AVCTP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVCTP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_AVCTP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_AVCTP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVCTP ]"LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_A2DP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_A2DP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ A2DP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_A2DP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_A2DP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ A2DP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_A2DP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_A2DP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ A2DP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_A2DP_INFO_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_A2DP_TRACE_LEVEL >= BT_TRACE_LEVEL_INFO)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ A2DP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_AVRCP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_AVRCP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVRCP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_AVRCP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_AVRCP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVRCP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_AVRCP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_AVRCP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ AVRCP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_HID_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_HID_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HID ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_HID_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_HID_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HID ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_HID_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_HID_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ HID ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_SMP_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_SMP_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ SMP ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_SMP_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_SMP_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ SMP ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_SMP_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_SMP_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ SMP ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }



#define BT_ATT_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_ATT_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ ATT ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_ATT_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_ATT_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ ATT ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }


#define BT_ATT_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_ATT_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ ATT ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }


#define BT_GATT_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_GATT_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ GATT ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_GATT_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_GATT_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ GATT ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_GATT_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_GATT_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ GATT ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }

#define BT_BAS_TRACE_ERROR(fmt,...)                                      \
  {                                                                \
    if (BT_BAS_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ BAS ] "LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_BAS_TRACE_WARNING(fmt,...)                                      \
  {                                                                \
    if (BT_BAS_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ BAS ] "LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET,##__VA_ARGS__); \
  }

#define BT_BAS_TRACE_DEBUG(fmt,...)                                      \
  {                                                                \
    if (BT_BAS_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG)               \
      BT_DEBUG(LOG_COLOR_BLUE"[ BAS ] "LOG_COLOR_RESET fmt,##__VA_ARGS__); \
  }



#define BT_ERROR(message, expression, handler) do { if (!(expression)) { \
  BT_DEBUG(message); handler;}} while(0)

#define BT_ASSERT(message, assertion) do { if (!(assertion)) { \
  BT_DEBUG(message); }} while(0)

#endif
