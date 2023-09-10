/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __EXYNOS_AUDIOHAL_H__
#define __EXYNOS_AUDIOHAL_H__

/* Definition of AudioHAL */
#include <system/audio.h>
#include <hardware/hardware.h>
#include <hardware/audio.h>

#include <cutils/list.h>

#include "audio_streams.h"
#include "audio_usages.h"
#include "audio_devices.h"
#include "audio_offload.h"
#include "audio_definition.h"

/* Voice call - RIL interface */
#include "voice_manager.h"

/* Factory Mode Test */
#include "factory_manager.h"


/**
 ** Stream Status
 **/
typedef enum {
    STATUS_STANDBY   = 0,   // Stream is opened, but Device(PCM or Compress) is not opened yet.
    STATUS_READY,           // Stream is opened, but Device(PCM or Compress) is not opened yet. But, started something
    STATUS_IDLE,            // Stream is opened & Device(PCM or Compress) is opened.
    STATUS_PLAYING,         // Stream is opened & Device(PCM or Compress) is opened & Device is working.
    STATUS_PAUSED,          // Stream is opened & Device(Compress) is opened & Device is pausing.(only available for Compress Offload Stream)
} stream_status;

/**
 ** Call Mode
 **/
typedef enum {
    CALL_OFF   = 0,         // No Call Mode
    VOICE_CALL,             // CP Centric Voice Call Mode
    VOLTE_CALL,             // CP Centric VOLTE Call Mode
    VOWIFI_CALL,            // AP Centric VoWiFi Call Mode

    CALL_MODE_CNT,
    CALL_MODE_MAX   = CALL_MODE_CNT - 1,
} call_mode;

/**
 ** FM State
 **/
typedef enum {
    FM_OFF   = 0,
    FM_ON,
    FM_RECORDING,
} fm_state;

/* Macro for Routing */
#define ROUTE               true
#define UNROUTE             false

typedef enum {
    NON_FORCE_ROUTE     = 0,
    FORCE_ROUTE,
    CALL_DRIVE
} force_route;


/**
 ** Structure for Audio Output Stream
 ** Implements audio_stream_out structure
 **/
struct stream_common {
    pthread_mutex_t         lock;

    // Audio Proxy to provide HW Services
    void *proxy_stream;

    /* These variables are needed to save Android Request
       becuase requested PCM Config and Real PCM Config can be different */
    audio_io_handle_t       handle;
    audio_devices_t         requested_devices;
    uint32_t                requested_sample_rate;
    audio_channel_mask_t    requested_channel_mask;
    audio_format_t          requested_format;
    uint32_t                requested_frame_count;
    audio_format_t          offload_audio_format;

    /* These variables show the purpose of stream */
    audio_stream_type       stream_type;
    audio_usage             stream_usage;
    stream_status           stream_status;
};

/* Compress Offload Specific Variables */
struct offload_msg {
    struct listnode node;
    offload_msg_type msg;
};

struct stream_offload {
    int nonblock_flag;

    stream_callback_t callback;
    void *cookie;

    pthread_t callback_thread;

    pthread_cond_t msg_cond;
    struct listnode msg_list;

    pthread_cond_t sync_cond;
    bool callback_thread_blocked;
};

struct stream_out {
    struct audio_stream_out stream;
    struct stream_common common;

    audio_output_flags_t    requested_flags;

    struct audio_device *   adev;

    struct stream_offload offload;
    float  vol_left, vol_right;

    /* Force Routing */
    force_route force;
    audio_devices_t rollback_devices;
};

struct playback_stream {
    struct listnode node;
    struct stream_out *out;
};

/**
 ** Structure for Audio Input Stream
 ** Implements audio_stream_in structure
 **/
struct stream_in {
    struct audio_stream_in stream;
    struct stream_common common;

    audio_input_flags_t    requested_flags;
    audio_source_t          requested_source;

    bool pcm_reconfig;

    struct audio_device *   adev;
};

struct capture_stream {
    struct listnode node;
    struct stream_in *in;
};

/**
 ** Structure for Audio Primary HW Module
 ** Implements audio_hw_device structure
 **/
struct audio_device {
    struct audio_hw_device hw_device;
    pthread_mutex_t lock;

    // Audio Proxy to provide HW Services
    void *proxy;

    // Stream Information
    struct listnode playback_list;
    struct listnode capture_list;

    // Routing Information
    audio_devices_t actual_playback_device;
    audio_devices_t actual_capture_device;
    audio_devices_t previous_playback_device;
    audio_devices_t previous_capture_device;

    bool is_route_created;

    bool is_playback_path_routed;
    audio_usage active_playback_ausage;
    device_type active_playback_device;
    modifier_type active_playback_modifier;

    bool is_capture_path_routed;
    audio_usage active_capture_ausage;
    device_type active_capture_device;
    modifier_type active_capture_modifier;

    audio_devices_t current_devices;

    // Voice
    struct voice_manager *voice;
    float voice_volume;
    bool mic_mute;

    // Factory
    struct factory_manager *factory;

    // Important Streams
    struct stream_out *primary_output;
    struct stream_in  *active_input;
    struct stream_out *compress_output;

    // Audio/Call Modes
    audio_mode_t amode;
    audio_mode_t previous_amode;
    call_mode    call_mode;

    //special incall-music stream
    bool incallmusic_on;

    // VoIP SE
    bool voipse_on;

    bool bluetooth_nrec;
    bool screen_on;

    fm_state fm_state;
    bool fm_need_route;

    bool seamless_enabled;

    // Customer Specific varibales
    bool support_reciever;
    bool support_backmic;
    bool support_thirdmic;
    bool fm_via_a2dp;
    bool fm_radio_mute;
    int  pcmread_latency;
    bool update_offload_volume;
};


/* Functions for External Usage */
bool adev_set_route(void *stream, audio_usage_type usage_type, bool set, force_route force);
void set_call_forwarding(struct audio_device *adev, bool mode);

//void update_uhqa_stream(struct stream_out *out);
void update_call_stream(struct stream_out *out, audio_devices_t current_devices, audio_devices_t new_devices);
void update_capture_stream(struct stream_in *in, audio_devices_t current_devices, audio_devices_t new_devices);

#endif  // __EXYNOS_AUDIOHAL_H__
