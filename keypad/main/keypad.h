#ifndef KEYPAD_H
#define KEYPAD_H

#include "inttypes.h"
#include <stdbool.h>

void keypad_main();
void change_hmac_ctx(int64_t new_counter);
int set_briefcase_state(bool new_state);

#endif // !KEYPAD_H
