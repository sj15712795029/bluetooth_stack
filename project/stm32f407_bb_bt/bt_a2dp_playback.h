#ifndef BT_A2DP_PLAYBACK_H_H_H
#define BT_A2DP_PLAYBACK_H_H_H

#include <stdint.h>

void bt_a2dp_playback_init(uint32_t sample_rate,uint32_t channel);
void bt_a2dp_playback_open(void);
void bt_a2dp_playback_write(uint8_t *data,uint16_t size);
void bt_a2dp_playback_close(void);


#endif
