
#include "bt_smp_key.h"
#include "cmac_mode.h"
#include "constants.h"

#define __bswap_16(x) ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
#define __bswap_32(x) ((uint32_t) ((((x) >> 24) & 0xff) | \
				   (((x) >> 8) & 0xff00) | \
				   (((x) & 0xff00) << 8) | \
				   (((x) & 0xff) << 24)))

#define sys_be32_to_cpu(val) __bswap_32(val)
#define sys_cpu_to_be32(val) __bswap_32(val)



int bt_encrypt_le(const uint8_t key[16], const uint8_t plaintext[16],
                  uint8_t enc_data[16])
{
    struct tc_aes_key_sched_struct s;
    uint8_t tmp[16];

    BT_SMP_TRACE_DEBUG("key %s plaintext %s\n", bt_hex_string(key, 16), bt_hex_string(plaintext, 16));

    bt_memcpy_swap(tmp, key, 16);

    if (tc_aes128_set_encrypt_key(&s, tmp) == TC_CRYPTO_FAIL)
    {
        return -1;
    }

    bt_memcpy_swap(tmp, plaintext, 16);

    if (tc_aes_encrypt(enc_data, tmp, &s) == TC_CRYPTO_FAIL)
    {
        return -1;
    }

    bt_mem_swap(enc_data, 16);

    BT_SMP_TRACE_DEBUG("enc_data %s\n", bt_hex_string(enc_data, 16));

    return 0;
}


static void xor_128(const uint8_t p[16], const uint8_t q[16], uint8_t r[16])
{
    uint16_t len = 16;

    while (len--)
    {
        *r++ = *p++ ^ *q++;
    }
}

static int bt_smp_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len,uint8_t *out) {
  struct tc_aes_key_sched_struct sched;
  struct tc_cmac_struct state;

  if (tc_cmac_setup(&state, key, &sched) == TC_CRYPTO_FAIL) {
    return -1;
  }

  if (tc_cmac_update(&state, in, len) == TC_CRYPTO_FAIL) {
    return -1;
  }

  if (tc_cmac_final(out, &state) == TC_CRYPTO_FAIL) {
    return -1;
  }

  return 0;
}


int smp_c1(const uint8_t k[16], const uint8_t r[16], const uint8_t preq[7],const uint8_t pres[7],
           const uint8_t ia[6],uint8_t iat,const uint8_t ra[6],uint8_t rat,uint8_t enc_data[16])
{
    uint8_t p1[16], p2[16];
    int err;

    BT_SMP_TRACE_DEBUG("k %s\n", bt_hex_string(k, 16));
    BT_SMP_TRACE_DEBUG("r %s\n", bt_hex_string(r, 16));
    BT_SMP_TRACE_DEBUG("ia->type %d\n", iat);
    BT_SMP_TRACE_DEBUG("ia %s\n", bt_hex_string(ia, 6));
    BT_SMP_TRACE_DEBUG("(ra)->type %d\n", rat);
    BT_SMP_TRACE_DEBUG("(ra) %s\n", bt_hex_string(ra, 6));

    BT_SMP_TRACE_DEBUG("preq %s\n", bt_hex_string(preq, 7));
    BT_SMP_TRACE_DEBUG("pres %s\n", bt_hex_string(pres, 7));

    /* pres, preq, rat and iat are concatenated to generate p1 */
    p1[0] = iat;
    p1[1] = rat;
    memcpy(p1 + 2, preq, 7);
    memcpy(p1 + 9, pres, 7);

    BT_SMP_TRACE_DEBUG("p1 %s\n", bt_hex_string(p1, 16));

    /* c1 = e(k, e(k, r XOR p1) XOR p2) */

    /* Using enc_data as temporary output buffer */
    xor_128(r, p1, enc_data);

    err = bt_encrypt_le(k, enc_data, enc_data);
    if (err)
    {
        return err;
    }

    /* ra is concatenated with ia and padding to generate p2 */
    memcpy(p2, ra, 6);
    memcpy(p2 + 6, ia, 6);
    (void)memset(p2 + 12, 0, 4);

    BT_SMP_TRACE_DEBUG("p2 %s\n", bt_hex_string(p2, 16));

    xor_128(enc_data, p2, enc_data);

    return bt_encrypt_le(k, enc_data, enc_data);
}


int32_t smp_s1(const uint8_t k[16], const uint8_t r1[16],const uint8_t r2[16], uint8_t out[16])
{
	/* The most significant 64-bits of r1 are discarded to generate
	 * r1' and the most significant 64-bits of r2 are discarded to
	 * generate r2'.
	 * r1' is concatenated with r2' to generate r' which is used as
	 * the 128-bit input parameter plaintextData to security function e:
	 *
	 *    r' = r1' || r2'
	 */
	memcpy(out, r2, 8);
	memcpy(out + 8, r1, 8);

	/* s1(k, r1 , r2) = e(k, r') */
	return bt_encrypt_le(k, out, out);
}


int32_t smp_f4(const uint8_t *u, const uint8_t *v, const uint8_t *x,uint8_t z, uint8_t res[16])
{
	uint8_t xs[16];
	uint8_t m[65];
	int err;

	BT_SMP_TRACE_DEBUG("u %s\n", bt_hex_string(u, 32));
	BT_SMP_TRACE_DEBUG("v %s\n", bt_hex_string(v, 32));
	BT_SMP_TRACE_DEBUG("x %s z 0x%x\n", bt_hex_string(x, 16), z);

	/*
	 * U, V and Z are concatenated and used as input m to the function
	 * AES-CMAC and X is used as the key k.
	 *
	 * Core Spec 4.2 Vol 3 Part H 2.2.5
	 *
	 * note:
	 * bt_smp_aes_cmac uses BE data and smp_f4 accept LE so we swap
	 */
	bt_memcpy_swap(m, u, 32);
	bt_memcpy_swap(m + 32, v, 32);
	m[64] = z;

	bt_memcpy_swap(xs, x, 16);

	err = bt_smp_aes_cmac(xs, m, sizeof(m), res);
	if (err) {
		return err;
	}

	bt_mem_swap(res, 16);

	BT_SMP_TRACE_DEBUG("res %s\n", bt_hex_string(res, 16));

	return err;
}


int32_t smp_g2(const uint8_t u[32], const uint8_t v[32],const uint8_t x[16], const uint8_t y[16], uint32_t *passkey)
{
	uint8_t m[80], xs[16];
	int err;

	BT_SMP_TRACE_DEBUG("u %s\n", bt_hex_string(u, 32));
	BT_SMP_TRACE_DEBUG("v %s\n", bt_hex_string(v, 32));
	BT_SMP_TRACE_DEBUG("x %s y %s\n", bt_hex_string(x, 16), bt_hex_string(y, 16));

	bt_memcpy_swap(m, u, 32);
	bt_memcpy_swap(m + 32, v, 32);
	bt_memcpy_swap(m + 64, y, 16);

	bt_memcpy_swap(xs, x, 16);

	/* reuse xs (key) as buffer for result */
	err = bt_smp_aes_cmac(xs, m, sizeof(m), xs);
	if (err) {
		return err;
	}
	BT_SMP_TRACE_DEBUG("res %s\n", bt_hex_string(xs, 16));

	memcpy(passkey, xs + 12, 4);

	*passkey = sys_be32_to_cpu(*passkey) % 1000000;

	BT_SMP_TRACE_DEBUG("passkey %u\n", *passkey);

	return 0;
}

int32_t smp_f5(const uint8_t *w, const uint8_t *n1, const uint8_t *n2,uint8_t *a1,uint8_t a1t, uint8_t *a2,uint8_t a2t,uint8_t *mackey,uint8_t *ltk)
{
	static const uint8_t salt[16] = { 0x6c, 0x88, 0x83, 0x91, 0xaa, 0xf5,
					  0xa5, 0x38, 0x60, 0x37, 0x0b, 0xdb,
					  0x5a, 0x60, 0x83, 0xbe };
	uint8_t m[53] = { 0x00, /* counter */
			  0x62, 0x74, 0x6c, 0x65, /* keyID */
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*n1*/
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*2*/
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* a1 */
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* a2 */
			  0x01, 0x00 /* length */ };
	uint8_t t[16], ws[32];
	int err;

	BT_SMP_TRACE_DEBUG("w %s\n", bt_hex_string(w, 32));
	BT_SMP_TRACE_DEBUG("n1 %s n2 %s\n", bt_hex_string(n1, 16), bt_hex_string(n2, 16));

	bt_memcpy_swap(ws, w, 32);

	err = bt_smp_aes_cmac(salt, ws, 32, t);
	if (err) {
		return err;
	}

	BT_SMP_TRACE_DEBUG("t %s\n", bt_hex_string(t, 16));

	bt_memcpy_swap(m + 5, n1, 16);
	bt_memcpy_swap(m + 21, n2, 16);
	m[37] = a1t;
	bt_memcpy_swap(m + 38, a1, 6);
	m[44] = a2t;
	bt_memcpy_swap(m + 45, a2, 6);

	err = bt_smp_aes_cmac(t, m, sizeof(m), mackey);
	if (err) {
		return err;
	}

	BT_SMP_TRACE_DEBUG("mackey %1s\n", bt_hex_string(mackey, 16));

	bt_mem_swap(mackey, 16);

	/* counter for ltk is 1 */
	m[0] = 0x01;

	err = bt_smp_aes_cmac(t, m, sizeof(m), ltk);
	if (err) {
		return err;
	}

	BT_SMP_TRACE_DEBUG("ltk %s\n", bt_hex_string(ltk, 16));

	bt_mem_swap(ltk, 16);

	return 0;
}

int32_t smp_f6(const uint8_t *w, const uint8_t *n1, const uint8_t *n2,const uint8_t *r, const uint8_t *iocap,uint8_t *a1,  uint8_t a1t,uint8_t *a2,uint8_t a2t,uint8_t *check)
{
	uint8_t ws[16];
	uint8_t m[65];
	int err;

	BT_SMP_TRACE_DEBUG("w %s\n", bt_hex_string(w, 16));
	BT_SMP_TRACE_DEBUG("n1 %s n2 %s\n", bt_hex_string(n1, 16), bt_hex_string(n2, 16));
	BT_SMP_TRACE_DEBUG("r %s io_cap %s\n", bt_hex_string(r, 16), bt_hex_string(iocap, 3));
	BT_SMP_TRACE_DEBUG("a1 %s a2 %s\n", bt_hex_string(a1, 6), bt_hex_string(a2, 6));

	bt_memcpy_swap(m, n1, 16);
	bt_memcpy_swap(m + 16, n2, 16);
	bt_memcpy_swap(m + 32, r, 16);
	bt_memcpy_swap(m + 48, iocap, 3);

	m[51] = a1t;
	memcpy(m + 52, a1, 6);
	bt_memcpy_swap(m + 52, a1, 6);

	m[58] = a2t;
	memcpy(m + 59, a2, 6);
	bt_memcpy_swap(m + 59, a2, 6);

	bt_memcpy_swap(ws, w, 16);

	err = bt_smp_aes_cmac(ws, m, sizeof(m), check);
	if (err) {
		return err;
	}

	BT_SMP_TRACE_DEBUG("res %s\n", bt_hex_string(check, 16));

	bt_mem_swap(check, 16);

	return 0;
}



