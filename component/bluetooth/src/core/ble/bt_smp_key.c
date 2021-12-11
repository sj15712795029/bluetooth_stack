
#include "bt_smp_key.h"
#include "cmac_mode.h"
#include "constants.h"


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



