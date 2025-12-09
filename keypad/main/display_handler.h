#ifndef DISPLAY_HANDLER
#define DISPLAY_HANDLER

typedef enum {
  WELCOME_MSG = 0, /* !< Default message */
  KEYPAD_UNLOCK = 1,
  BLE_UNLOCK = 3,
  LACKING_TICKS = -1,
  DEBUG = 2,
} screen_state_t;

#include <stdbool.h>
void init_display_device(void);

void update_display(screen_state_t state);

extern char keypad_input[];
extern int keypad_idx;

extern bool is_unlocked;

#endif // !DISPLAY_HANDLER
