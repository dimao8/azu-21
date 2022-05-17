#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum key_tag
{
  k_none,
  k_start,
  k_up,
  k_down,
  k_mode
} key_t;

typedef void (*key_callback_proc_t)(key_t);

void init_keyboard(key_callback_proc_t callback);

void process_keyboard();

#endif // KEYBOARD_H
