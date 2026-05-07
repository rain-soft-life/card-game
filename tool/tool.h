#ifndef TOOL_H
#define TOOL_H
#include "baseheadfiles/basehead.h"
#include "../config/config.h"


static int hash_char[257];


void initialize(void);
void set_priority(char arr[], int change_numbers);
int random(int random_number_area);
void flush_input(void);
void flush_output(void);
int init_windows_ANSI(void);
void get_screen_size(int* width, int* height);
void cursor_to_xy(int x, int y);
void reset_cursor(int height_screen);

#endif