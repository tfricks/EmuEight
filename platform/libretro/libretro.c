#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <stdio.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#endif
#include "libretro.h"
#include "cpu.h"


static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   //vfprintf(stderr, fmt, va);
   va_end(va);
}


static retro_environment_t environ_cb;

static chip8_t *p_cpu;

void retro_init(void)
{
   p_cpu = cpu_init();
   p_cpu->ipf = 7;
}

void retro_deinit(void)
{
   free(p_cpu);
   p_cpu = NULL;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   (void)port;
   (void)device;
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "EmuEight";
   info->library_version  = "0.1";
   info->need_fullpath    = false;
   info->valid_extensions = "ch8";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{


   info->geometry.base_width   = CHIP8_DISPLAY_W;
   info->geometry.base_height  = CHIP8_DISPLAY_H;
   info->geometry.max_width    = CHIP8_DISPLAY_W;
   info->geometry.max_height   = CHIP8_DISPLAY_H;
   info->geometry.aspect_ratio = (float) CHIP8_DISPLAY_W / (float) CHIP8_DISPLAY_H;
   info->timing.fps            = 60.0;
}

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
      log_cb = logging.log;
   else
      log_cb = fallback_log;

   static const struct retro_controller_description controllers[] = {
      { "Keypad", RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_KEYBOARD, 0) },
   };

   static struct retro_controller_info ports[] = {
      { controllers, 1 },
      { NULL, 0 },
   };

   cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

//static unsigned phase;

void retro_reset(void)
{

}

void retro_run(void)
{
   cpu_cycle(p_cpu);
   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
   {
      
   }
   video_cb(p_cpu->vram, CHIP8_DISPLAY_W, CHIP8_DISPLAY_H, (uint32_t)CHIP8_DISPLAY_W * sizeof(uint32_t));
   p_cpu->display_wait = false;
}

bool retro_load_game(const struct retro_game_info *info)
{
   if (info == NULL || info->data == NULL || info->size == 0)
   {
      return false;
   }

   if (!cpu_load_program_from_buffer(p_cpu, (const uint8_t *)info->data, info->size))
   {
      return false;
   }

   struct retro_input_descriptor desc[] = {
      { 0, RETRO_DEVICE_KEYBOARD, 0, RETROK_1,  "1" },
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_2, "2"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_3, "3"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_4, "C"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_q, "4"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_w, "5"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_e, "6"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_r, "D"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_a, "7"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_s, "8"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_d, "9"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_f, "E"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_z, "A"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_x, "0"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_c, "B"},
      {0, RETRO_DEVICE_KEYBOARD, 0, RETROK_v, "F"},
      { 0 },
   };

   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;

   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      log_cb(RETRO_LOG_ERROR, "XRGB8888 is not supported.\n");
      return false;
   }
   
   return true;
}

void retro_unload_game(void)
{
   cpu_reset(p_cpu);
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   (void)type;
   (void)info;
   (void)num;
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   (void)data_;
   (void)size;
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   (void)data_;
   (void)size;
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void)
{

}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}
