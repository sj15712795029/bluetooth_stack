#ifndef BT_SMP_KEY_H_H_H
#define BT_SMP_KEY_H_H_H

#include "bt_common.h"
#include "bt_l2cap.h"



int32_t smp_c1(const uint8_t k[16], const uint8_t r[16], const uint8_t preq[7],const uint8_t pres[7], 
           const uint8_t ia[6],uint8_t iat,const uint8_t ra[6],uint8_t rat,uint8_t enc_data[16]);
int32_t smp_s1(const uint8_t k[16], const uint8_t r1[16],const uint8_t r2[16], uint8_t out[16]);




#endif

