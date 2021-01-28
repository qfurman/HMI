
#ifndef _IS_INCLUDED_LCD_H
#define _IS_INCLUDED_LCD_H

extern const char *fnt_ptr;     //активний шрифт
#define set_font(font_name) fnt_ptr = &font_name[0]

extern char layer;              //активний прошарок для виводу тексту 0,1,2
#define set_layer(layer_name) layer = layer_name

/*****************************************************************************/
#define lcd_set_display()   lcd_put_command(0x59);lcd_put_data(display);
/*
typedef struct
{
    unsigned  char fc   :2; //cursor
    unsigned  char fp0  :2; //layer0
    unsigned  char fp1  :2; //layer1
    unsigned  char fp2  :2; //layer2
}disp;

typedef struct
{
    unsigned  char mx   :2; //composition
    unsigned  char dm1  :1; //mode
    unsigned  char dm2  :1; //mode
    unsigned  char ov   :1; //overlay
    unsigned  char un   :3; //unused
}ovlay;

extern union
{
    char display;
    disp disp_bit;
};

extern union
{
    char overlay;
    ovlay ovlay_bit;
};
 */
/*****************************************************************************/
extern char lcd_buf[9600];
/*****************************************************************************/
void lcd_put_command(char command);
void lcd_put_data(char data);
void lcd_cls(void);
void lcd_init(void);
void lcd_ready(void);
void lcd_busy(void);
void lcd_draw_pix(int x,int y,char layer,char color);
void lcd_write_byte(char byte,int x,int y,char layer);
void lcd_line(int x0,int y0,int x1,int y1,char layer,char color);
void lcd_write_memory(int x,int y,char layer,const char *data,int row,int column);
void lcd_all_screen(char layer,const char *data);
void lcd_put_text(int row,int column,char const *str);
void lcd_write_array(int column,int row,char layer,const char *data,int hight,int width);

void lcd_buf_put_text(int row,int column,char const *str,char overlay,bool inv);
void lcd_buf_write_array(int column,int row, const char *data,int hight,int width, char overlay,bool inv);
void lcd_buf_draw_pix(int x, int y, bool color);
void lcd_buf_write_byte(char byte, int x, int y, char overlay);
void lcd_buf_line(int x0,int y0,int x1,int y1,bool color);
void lcd_buf_DrawLine(int x, int y, int x1, int y1, char color);
void lcd_buf_fill_window(int row, int column, int hight, int width, char data, char overlay,bool inv);
void lcd_buf_window(int row, int column, int hight, int width);

#endif //_IS_INCLUDED_LCD_H
