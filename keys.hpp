#ifndef _IS_INCLUDED_KEYS_HPP
#define _IS_INCLUDED_KEYS_HPP

extern char input_string[];
extern char input_string_[];
extern char cursor_position;
extern char keys[32*40];
extern bool coordinates;   // координати натискання

//class num_fild;
extern class num_fild num[];


void create_key(char width,char hight,bool inv,const char *charact);
void create_fill(char width,char hight);
void init_crsr_position();
void sheet_captions();
void sheet_view();

void sys_time_init();

#endif



