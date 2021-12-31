#include "bt_a2dp_playback.h"


#include "bt_config.h"

#define A2DP_PLAYBACK_BIT_WIDTH 2

typedef struct
{
    uint32_t channel;
    uint32_t sample_rate;
    uint32_t playback_open;
} a2dp_playback_dev_mgr_t;

static a2dp_playback_dev_mgr_t a2dp_playback_dev_mgr = {0};


void bt_a2dp_playback_init(uint32_t sample_rate,uint32_t channel)
{
    a2dp_playback_dev_mgr.sample_rate = sample_rate;
    a2dp_playback_dev_mgr.channel = channel;
}
void bt_a2dp_playback_open()
{
	
    a2dp_playback_dev_mgr.playback_open = 1;
}
void bt_a2dp_playback_write(uint8_t *data,uint16_t size)
{
    
}
void bt_a2dp_playback_close()
{
    a2dp_playback_dev_mgr.playback_open = 0;

}

