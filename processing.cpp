#include <type.h>

bool coordinates;   // координати натискання

CURSOR cursor = {159,119};

char keys[32*40];
char input_string[40]={""};
char input_string_[40]={""};
char cursor_position;


/*****************************************************************************/
//особлива функція - викликається при натисканні клавіші "enter"
char*(*enter_func)();
//підготовка числа зля зміни
char*(*for_enter_func)();
/*****************************************************************************/

void init_crsr_position()
{
    char i;
    i = 0;
    do input_string_[i] = ' ';
    while(i++ < sizeof(input_string)-2);

    i = 0;
    while(input_string[i] <= ' ')i++;
    cursor_position = i;
    input_string_[i] = '';
}
/*****************************************************************************/

bool tolerance(char x_,char y_,int width,int hight)
{
//  int x,y,xe,ye;
//  float coord;
/*
    //coord = (int)(inp>>16);
    //coord *= 1.1;
    coord = ((int)(coordinates>>16));
    //if(coord > 0x300)coord *= 1.1;
    coord = (coord - 0x20)* 1.119256;
    x = coord;
    //x = (int)(coordinates>>16);
    coord = ((int)(0xffff & coordinates));
    //if(coord > 0x300)coord *= 1.1;
    coord = (coord - 0x20)* 1.119256;
    y = coord;//((int)(0xffff & inp));

    xe = 0x3ff*x_/40;
    ye = 0x3ff*y_/240;
    width = 0x3ff*width/40;
    hight = 0x3ff*hight/240;
*/
    if(!coordinates)return(false);
    if(cursor.x  > x_*8)
        if(cursor.x  < (x_ + width)*8)
            if(cursor.y > y_)
                if(cursor.y < (y_ + hight))return(true);
    return(false);
}
/*****************************************************************************/
/*****************************************************************************/

void create_fill(char width,char hight)
{
    char k,l,fill;

    for(l=0;l<width;l++)
    {
        for(k=0;k<hight;k++)
        {
            if(k==0 || k==hight-1)fill = 0xff;
            else
                if(l>0 && l<width-1)fill = 0x00;
                else
                {
                    if(l)fill = 0x01;
                    else fill = 0x80;
                }
            if((l==0 || l==width-1) && (k==0 || k==hight-1))fill = 0x00;
            if((k==0 || k==hight-1) && l==0)fill = 0x3f;
            if((k==1 || k==hight-2) && l==0)fill = 0x40;
            if((k==0 || k==hight-1) && l==width-1)fill = 0xfc;
            if((k==1 || k==hight-2) && l==width-1)fill = 0x02;

            keys[k+l*hight] = fill;

        }
    }
}
/*****************************************************************************/
/*****************************************************************************/

void create_key(char width,char hight,bool inv,const char *charact)
{
    char k,l,fill;
    char x,y;   //ширина і висота внутрішнього зображення в х - байтах
    char corr_x,corr_y;
    char hor_l,hor_r,ver_u,ver_d;

    if(charact)
    {
        x = *charact++;
        y = *charact++;

        corr_x = (width - x)/2;
        corr_y = (hight - y)/2;

        hor_l = width - x - corr_x;
        hor_r = width - corr_x;
        ver_u = hight - y - corr_y;
        ver_d = hight - corr_y;
    }

    for(l=0;l<width;l++)
    {
        for(k=0;k<hight;k++)
        {
            if(k==1 || k==hight-2)fill = 0xff;
            else
                if(l>0 && l<width-1)fill = 0x00;
                else
                {
                    if(l)fill = 0x02;
                    else fill = 0x40;
                }
            if((l==0 || l==width-1) && (k==0 || k==hight-1))fill = 0x00;
            if((k==1 || k==hight-2) && l==0)fill = 0x7f;
            if((k==1 || k==hight-2) && l==width-1)fill = 0xfe;

            if((l>=hor_l)&&(l<hor_r)&&(k>=ver_u)&&(k<ver_d))
            {
                if((width - x)%2)
                {
                    if(l<hor_r-1)
                    {
                        keys[k+(l+1)*hight] = 0xf0 & *charact<<4;
                        keys[k+l*hight] |= fill ^ (*charact++ >> 4);
                    }
                    else keys[k+l*hight] |= fill;
                }
                else keys[k+l*hight] = fill ^ *charact++;
            }
            else
            {
                if((charact)&&(((width - x)%2)&&(l>=hor_l-1)&&(l<hor_r-1)&&(k>=ver_u)&&(k<ver_d)))
                {
                    keys[k+(l+1)*hight] = 0xf0 & *charact<<4;
                    keys[k+l*hight] = fill ^ *charact++ >> 4;
                }
                else keys[k+l*hight] = fill;
            }
            if(inv) keys[k+l*hight] = ~keys[k+l*hight];
        }
    }
}
/*****************************************************************************/
