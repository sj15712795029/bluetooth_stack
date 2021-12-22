#ifndef BT_SMP_KEY_H_H_H
#define BT_SMP_KEY_H_H_H

#include "bt_common.h"
#include "bt_l2cap.h"



int32_t smp_c1(const uint8_t k[16], const uint8_t r[16], const uint8_t preq[7],const uint8_t pres[7], 
           const uint8_t ia[6],uint8_t iat,const uint8_t ra[6],uint8_t rat,uint8_t enc_data[16]);
int32_t smp_s1(const uint8_t k[16], const uint8_t r1[16],const uint8_t r2[16], uint8_t out[16]);
int32_t smp_f4(const uint8_t *u, const uint8_t *v, const uint8_t *x,uint8_t z, uint8_t res[16]);
int32_t smp_g2(const uint8_t u[32], const uint8_t v[32],const uint8_t x[16], const uint8_t y[16], uint32_t *passkey);
int32_t smp_f5(const uint8_t *w, const uint8_t *n1, const uint8_t *n2,uint8_t *a1,uint8_t a1t, uint8_t *a2,uint8_t a2t,uint8_t *mackey,uint8_t *ltk);
int32_t smp_f6(const uint8_t *w, const uint8_t *n1, const uint8_t *n2,const uint8_t *r, const uint8_t *iocap,uint8_t *a1,  uint8_t a1t,uint8_t *a2,uint8_t a2t,uint8_t *check);


#endif

