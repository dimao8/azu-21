#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum key_tag
{
  kNone,
  kStart,
  kUp,
  kDown,
  kMode
} key_t;

typedef void (*key_callback_proc_t)(key_t);

void InitKeyboard(key_callback_proc_t callback);

void ProcessKeyboard();

#endif // KEYBOARD_H
