/******************************************************************************
  * @file           bt_snoop.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-21
  * @brief          bt snoop source file
******************************************************************************/

#include "bt_snoop.h"
#if BT_ENABLE_SNOOP > 0

#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

#define BT_SNOOP_PATH "./"

int bt_snoop_file = -1;


#pragma pack (1) 

/* Android btsnoop packet format */
typedef struct
{
    uint32_t length_original;
    uint32_t length_captured;
    uint32_t flags;
    uint32_t dropped_packets;
    uint64_t timestamp;
    uint8_t type;
} btsnoop_header_t;
#pragma pack () 

static const uint64_t BTSNOOP_EPOCH_DELTA = 0x00dcddb30f2f8000ULL;


/******************************************************************************
 * func name   : bt_snoop_init
 * para        : VOID
 * return      : 返回bt_snoop_init结果
 * description : btsnoop的初始化
******************************************************************************/
err_t bt_snoop_init(void)
{
    time_t t;
    struct tm *p;

    time(&t);
    p = gmtime(&t);
    uint8_t bt_snoop_name[64] = {0};

    sprintf((char *)bt_snoop_name,"%sbtsnoop_%04d_%02d_%02d_%02d_%02d_%02d.log",BT_SNOOP_PATH,1900 + p->tm_year,1 + p->tm_mon, \
            p->tm_mday,8 + p->tm_hour,p->tm_min,p->tm_sec);/* time zone hour+8 */


    bt_snoop_file = open((char *)bt_snoop_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(bt_snoop_file <0)
    {
        BT_SNOOP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_snoop_init open fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_VAL;
    }
    BT_SNOOP_TRACE_DEBUG("bt snoop fd %d\n",bt_snoop_file);

    write(bt_snoop_file, "btsnoop\0\0\0\0\1\0\0\x3\xea", 16);

    return BT_ERR_OK;
}


/******************************************************************************
 * func name   : bt_snoop_deinit
 * para        : VOID
 * return      : 返回bt_snoop_deinit结果
 * description : btsnoop的deinit
******************************************************************************/
err_t bt_snoop_deinit(void)
{
    if(bt_snoop_file != -1)
        close(bt_snoop_file);

	return BT_ERR_OK;
}


/******************************************************************************
 * func name   : bt_snoop_write
 * para        : packet_type(IN)	--> packet type
                 in(IN)	--> 标示是否是蓝牙芯片到协议栈的数据
                 packet(IN)		--> 要写的的HCI数据
                 len(IN)		--> 要写入的长度
 * return      : 返回bt_snoop_write的结果
 * description : 把HCI数据写入到btsnoop file中
******************************************************************************/
err_t bt_snoop_write(uint8_t  packet_type, uint8_t  in, uint8_t *packet, uint16_t len)
{
    struct timeval curr_time;
    /* struct tm* ptm; */
    uint32_t length_he = 0;
    uint32_t flags = 0;
    uint64_t time_stamp = 0;

    btsnoop_header_t header;

    if (bt_snoop_file < 0)
    {
        BT_SNOOP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_snoop_write fd invalid\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_VAL;
    }

    //BT_SNOOP_TRACE_DEBUG("bt_snoop_write type(%d) in(%d) len(%d)\n",packet_type,in,len);
    gettimeofday(&curr_time, NULL);
    time_stamp = (curr_time.tv_sec+8*60*60)*1000000ULL+curr_time.tv_usec+\
		BTSNOOP_EPOCH_DELTA;/* time zone hour+8 */

    switch (packet_type)
    {
    case BT_SNOOP_PACKET_TYPE_CMD:
        length_he = packet[2] + 4;
        flags = 2;
        break;
    case BT_SNOOP_PACKET_TYPE_ACL_DATA:
        length_he = (packet[3] << 8) + packet[2] + 5;
        flags = in;
        break;
    case BT_SNOOP_PACKET_TYPE_SCO_DATA:
        length_he = packet[2] + 4;
        flags = in;
        break;
    case BT_SNOOP_PACKET_TYPE_EVT:
        length_he = packet[1] + 3;
        flags = 3;
        break;
    default:
        return BT_ERR_ARG;
    }
    header.length_original = bt_htonl(length_he);
    header.length_captured = header.length_original;
    header.flags = bt_htonl(flags);
    header.dropped_packets = 0;
    header.timestamp = bt_htonll(time_stamp);
    header.type = packet_type;

    write(bt_snoop_file,&header, sizeof(btsnoop_header_t));
    write(bt_snoop_file,packet, length_he - 1);


    return BT_ERR_OK;
}

#endif
