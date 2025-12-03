#ifndef DISPLAY_HANDLER
#define DISPLAY_HANDLER

#include <stdbool.h>
void init_display_device(void);

void update_display(int state);

extern char keypad_input[];
extern int keypad_idx;

extern bool is_unlocked;

#endif // !DISPLAY_HANDLER
