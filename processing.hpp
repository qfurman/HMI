#ifndef _IS_INCLUDED_PROCESSING_H
#define _IS_INCLUDED_PROCESSING_H

extern bool coordinates;   // координати натискання



extern char keys[32*40];
extern char input_string[40];
extern char input_string_[40];
extern char cursor_position;



extern char*(*enter_func)();
extern char*(*for_enter_func)();

extern void init_crsr_position();
extern bool tolerance(char x_,char y_,int width,int hight);
extern void create_fill(char width,char hight);
extern void create_key(char width,char hight,bool inv,const char *charact);


#endif
