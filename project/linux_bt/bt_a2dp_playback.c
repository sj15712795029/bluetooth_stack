#include "bt_a2dp_playback.h"

#include <alsa/asoundlib.h>
#include "bt_config.h"

#define A2DP_PLAYBACK_BIT_WIDTH 2

typedef struct
{
    snd_pcm_t *playback_handle;
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
	int rc;
	snd_pcm_hw_params_t *hw_params;
    
    /* Open PCM device for playback. */
    rc = snd_pcm_open(&a2dp_playback_dev_mgr.playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc != 0)
    {
        printf("unable to open pcm device: %s\n", snd_strerror(rc));
        exit(1);
    }


    /* Allocate a hardware parameters object. */
    rc = snd_pcm_hw_params_malloc(&hw_params);
    if (rc != 0)
    {
        printf("unable to snd_pcm_hw_params_alloca: %s\n", snd_strerror(rc));
        exit(1);
    }


    /* Fill it in with default values. */
    rc = snd_pcm_hw_params_any(a2dp_playback_dev_mgr.playback_handle, hw_params);
    if (rc != 0)
    {
        printf("unable to snd_pcm_hw_params_any: %s\n", snd_strerror(rc));
        exit(1);
    }

    /* Interleaved mode */
    rc = snd_pcm_hw_params_set_access(a2dp_playback_dev_mgr.playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (rc != 0)
    {
        printf("unable to snd_pcm_hw_params_set_access: %s\n", snd_strerror(rc));
        exit(1);
    }
    /* Signed 16-bit format */
    rc = snd_pcm_hw_params_set_format(a2dp_playback_dev_mgr.playback_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    if (rc != 0)
    {
        printf("unable to snd_pcm_hw_params_set_format: %s\n", snd_strerror(rc));
        exit(1);
    }

    rc = snd_pcm_hw_params_set_rate (a2dp_playback_dev_mgr.playback_handle, hw_params, a2dp_playback_dev_mgr.sample_rate, 0);
    if (rc != 0)
    {
        printf("unable to snd_pcm_hw_params_set_rate: %s\n", snd_strerror(rc));
        exit(1);
    }

    /* Two channels (stereo) */
    rc = snd_pcm_hw_params_set_channels(a2dp_playback_dev_mgr.playback_handle, hw_params, 2);
    if (rc != 0)
    {
        printf("unable to snd_pcm_hw_params_set_channels: %s\n", snd_strerror(rc));
        exit(1);
    }


    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(a2dp_playback_dev_mgr.playback_handle, hw_params);
    if (rc != 0)
    {
        printf("unable to set hw parameters: %s\n", snd_strerror(rc));
        exit(1);
    }


    a2dp_playback_dev_mgr.playback_open = 1;
}
void bt_a2dp_playback_write(uint8_t *data,uint16_t size)
{
    if(a2dp_playback_dev_mgr.playback_open == 1)
    {
        int rc = snd_pcm_writei(a2dp_playback_dev_mgr.playback_handle, data, size/A2DP_PLAYBACK_BIT_WIDTH/a2dp_playback_dev_mgr.channel);
        if (rc == -EPIPE)
        {
            /* EPIPE means underrun */
            printf("underrun occurred\n");
            snd_pcm_prepare(a2dp_playback_dev_mgr.playback_handle);
        }
        else if (rc < 0)
        {
            printf("error from writei: %s\n", snd_strerror(rc));
        }
    }
}
void bt_a2dp_playback_close()
{
    a2dp_playback_dev_mgr.playback_open = 0;

    snd_pcm_drop(a2dp_playback_dev_mgr.playback_handle);
    snd_pcm_drain(a2dp_playback_dev_mgr.playback_handle);
    snd_pcm_close(a2dp_playback_dev_mgr.playback_handle);
}

