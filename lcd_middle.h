#ifndef _IS_INCLUDED_LCD_MIDDLE_H
#define _IS_INCLUDED_LCD_MIDDLE_H


#define DefaultFont fixedsys

extern char LCD_str[];
extern char LCD_input_str[];
extern char LCD_input_str_[];
extern char *LCD_crsr_position;

extern char keys[];

void LCD_put_str(char row, char column,const char *string);
void LCD_put_str(char row, char column,const char *string, bool inv);
void LCD_put_str(char row, char column,const char *string,const char *font);
void LCD_put_str(char row, char column,const char *string,const char *font, bool inv);
char LCD_init_crsr_position_right(void);
void LCD_highlight(char row, char column);
void LCD_highlight(char row, char column, char *ptr);
void LCD_highlight_crsr(char row, char column, char*str);
void place_crsr(char row, char pos_flash);
void create_fill(char width,char hight);

#endif    //_IS_INCLUDED_LCD_MIDDLE_H





