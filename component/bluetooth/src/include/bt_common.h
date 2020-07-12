/******************************************************************************
  * @file           bt_common.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-14
  * @brief          bt common header file
******************************************************************************/
 
#ifndef COMMON_H_H_H
#define COMMON_H_H_H
#include "bt_config.h"
#include "bt_debug.h"
#include "bt_mem.h"
#include "bt_memp.h"

#ifndef err_t
typedef int8_t err_t;
#endif

/** Definitions for error constants. */
typedef enum {
/** No error, everything OK. */
  BT_ERR_OK         = 0,
/** Out of memory error.     */
  BT_ERR_MEM        = -1,
/** Buffer error.            */
  BT_ERR_BUF        = -2,
/** Timeout.                 */
  BT_SERR_TIMEOUT    = -3,
/** Routing problem.         */
  BT_ERR_RTE        = -4,
/** Operation in progress    */
  BT_ERR_INPROGRESS = -5,
/** Illegal value.           */
  BT_ERR_VAL        = -6,
/** Operation would block.   */
  BT_ERR_WOULDBLOCK = -7,
/** Address in use.          */
  BT_ERR_USE        = -8,
/** Already connecting.      */
  BT_ERR_ALREADY    = -9,
/** Conn already established.*/
  BT_ERR_ISCONN     = -10,
/** Not connected.           */
  BT_ERR_CONN       = -11,
/** Low-level netif error    */
  BT_ERR_IF         = -12,

/** Connection aborted.      */
  BT_ERR_ABRT       = -13,
/** Connection reset.        */
  BT_ERR_RST        = -14,
/** Connection closed.       */
  BT_ERR_CLSD       = -15,
/** Illegal argument.        */
  BT_ERR_ARG        = -16
} bt_err_enum_t;


#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#define MEM_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_ALIGNMENT - 1) & ~(mem_ptr_t)(MEM_ALIGNMENT-1)))
#define BT_CONST_CAST(target_type, val) ((target_type)((ptrdiff_t)val))
#define BT_UNUSED_ARG(x) (void)(x=x)

#define BT_MAX(x , y)  (((x) > (y)) ? (x) : (y))
#define BT_MIN(x , y)  (((x) < (y)) ? (x) : (y))

#define bt_htons(x) ((((x) & 0x00ffUL) << 8) | (((x) & 0xff00UL) >> 8))
#define bt_ntohs(x) ((((x) & 0x00ffUL) << 8) | (((x) & 0xff00UL) >> 8))
#define bt_htonl(x) ((((x) & 0x000000ffUL) << 24) | \
                     (((x) & 0x0000ff00UL) <<  8) | \
                     (((x) & 0x00ff0000UL) >>  8) | \
                     (((x) & 0xff000000UL) >> 24))
#define bt_ntohl(x) ((((x) & 0x000000ffUL) << 24) | \
                     (((x) & 0x0000ff00UL) <<  8) | \
                     (((x) & 0x00ff0000UL) >>  8) | \
                     (((x) & 0xff000000UL) >> 24))

#define bt_htonll(x) ((((uint64_t)bt_htonl(x)) << 32) + bt_htonl((x) >> 32))

#define SYS_ARCH_INC(var, val) do { \
                                var += val; \
                              } while(0)

#define SYS_ARCH_DEC(var, val) do { \
                                var -= val; \
                              } while(0)

#define SYS_ARCH_GET(var, ret) do { \
                                ret = var; \
                              } while(0)

#define SYS_ARCH_SET(var, val) do { \
                                var = val; \
                              } while(0)


struct bd_addr_t {
  uint8_t addr[6];
};

#define BD_ADDR_LEN 6

#define BD_ADDR(bdaddr, a, b, c, d, e, f) do{ \
                                        bdaddr->addr[0] = a; \
				        bdaddr ->addr[1] = b; \
				        bdaddr->addr[2] = c; \
				        bdaddr->addr[3] = d; \
				        bdaddr->addr[4] = e; \
				       bdaddr->addr[5] = f; }while(0)

#define bd_addr_cmp(addr1, addr2) (((addr1)->addr[0] == (addr2)->addr[0]) && \
				   ((addr1)->addr[1] == (addr2)->addr[1]) && \
				   ((addr1)->addr[2] == (addr2)->addr[2]) && \
				   ((addr1)->addr[3] == (addr2)->addr[3]) && \
				   ((addr1)->addr[4] == (addr2)->addr[4]) && \
				   ((addr1)->addr[5] == (addr2)->addr[5]))

#define bd_addr_set(addr1, addr2) do { \
                                   (addr1)->addr[0] = (addr2)->addr[0]; \
				   (addr1)->addr[1] = (addr2)->addr[1]; \
				   (addr1)->addr[2] = (addr2)->addr[2]; \
				   (addr1)->addr[3] = (addr2)->addr[3]; \
				   (addr1)->addr[4] = (addr2)->addr[4]; \
				   (addr1)->addr[5] = (addr2)->addr[5]; }while(0)

#define bd_addr_debug_print(bdaddr) printf("0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n", \
		    (bdaddr)->addr[0], \
		    (bdaddr)->addr[1], \
		    (bdaddr)->addr[2], \
		    (bdaddr)->addr[3], \
		    (bdaddr)->addr[4], \
		    (bdaddr)->addr[5])


#define BT_DT_FLAGS	0x01
#define BT_DT_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS      0x02
#define BT_DT_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS        0x03
#define BT_DT_INCOMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS      0x04
#define BT_DT_COMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS        0x05
#define BT_DT_INCOMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS     0x06 
#define BT_DT_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS       0x07
#define BT_DT_SHORTENED_LOCAL_NAME                               0x08
#define BT_DT_COMPLETE_LOCAL_NAME                                0x09
#define BT_DT_TX_POWER_LEVEL                                     0x0A
#define BT_DT_CLASS_OF_DEVICE                                    0x0D
#define BT_DT_SIMPLE_PAIRING_HASH_C                              0x0E
#define BT_DT_SIMPLE_PAIRING_HASH_C_192                          0x0E
#define BT_DT_SIMPLE_PAIRING_RANDOMIZER_R                        0x0F
#define BT_DT_SIMPLE_PAIRING_RANDOMIZER_R_192                    0x0F
#define BT_DT_DEVICE_ID                                          0x10
#define BT_DT_SECURITY_MANAGER_TK_VALUE                          0x10
#define BT_DT_SECURITY_MANAGER_OUT_OF_BAND_FLAGS                 0x11
#define BT_DT_SLAVE_CONNECTION_INTERVAL_RANGE                    0x12
#define BT_DT_LIST_OF_16_BIT_SERVICE_SOLICITATION_UUIDS          0x14
#define BT_DT_LIST_OF_32_BIT_SERVICE_SOLICITATION_UUIDS          0x1F
#define BT_DT_LIST_OF_128_BIT_SERVICE_SOLICITATION_UUIDS         0x15
#define BT_DT_SERVICE_DATA                                       0x16
#define BT_DT_SERVICE_DATA_16_BIT_UUID                           0x16
#define BT_DT_SERVICE_DATA_32_BIT_UUID                           0x20
#define BT_DT_SERVICE_DATA_128_BIT_UUID                          0x21
#define BT_DT_LE_SECURE_CONNECTIONS_CONFIRMATION_VALUE           0x22
#define BT_DT_LE_SECURE_CONNECTIONS_RANDOM_VALUE                 0x23
#define BT_DT_URI                                                0x24
#define BT_DT_INDOOR_POSITIONING                                 0x25
#define BT_DT_TRANSPORT_DISCOVERY_DATA                           0x26
#define BT_DT_PUBLIC_TARGET_ADDRESS                              0x17
#define BT_DT_RANDOM_TARGET_ADDRESS                              0x18
#define BT_DT_APPEARANCE                                         0x19
#define BT_DT_ADVERTISING_INTERVAL                               0x1A
#define BT_DT_LE_BLUETOOTH_DEVICE_ADDRESS                        0x1B
#define BT_DT_LE_ROLE                                            0x1C
#define BT_DT_SIMPLE_PAIRING_HASH_C_256                          0x1D
#define BT_DT_SIMPLE_PAIRING_RANDOMIZER_R_256                    0x1E
#define BT_DT_3D_INFORMATION_DATA                                0x3D
#define BT_DT_MANUFACTURER_SPECIFIC_DATA                         0xFF


uint16_t bt_le_read_16(const uint8_t * buffer, int pos);
uint32_t bt_le_read_24(const uint8_t * buffer, int pos);
uint32_t bt_le_read_32(const uint8_t * buffer, int pos);
void bt_le_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
void bt_le_store_24(uint8_t *buffer, uint16_t pos, uint32_t value);
void bt_le_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);
uint32_t bt_be_read_16( const uint8_t * buffer, int pos);
uint32_t bt_be_read_24( const uint8_t * buffer, int pos);
uint32_t bt_be_read_32( const uint8_t * buffer, int pos);
void bt_be_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
void bt_be_store_24(uint8_t *buffer, uint16_t pos, uint32_t value);
void bt_be_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);
void bt_be_store_64(uint8_t *buffer,uint16_t pos,uint64_t value);
uint32_t bt_atoi_spec_size(const uint8_t *str,uint8_t size);
uint32_t bt_atoi(const uint8_t *str);
void bt_hex_dump(uint8_t *data,uint32_t len);

#endif

