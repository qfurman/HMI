//#include <inarm.h>
//#include "lpc2138.h"
#include "iolpc2138.h"

double*time_num_fild[10];

void RTC_isr()
{
    char i=0;
    char*time_ptr;
    char time;

    if(CIIR_bit.IMSEC == 1)
    {
        while(time_num_fild[i])
        {
            time_ptr = (char*)time_num_fild[i];

            time = *time_ptr;
            if(++time>59)
            {
                *time_ptr++ = 0;
                time = *time_ptr;
                if(++time>59)
                {
                    *time_ptr++ = 0;
                    time = *time_ptr;
                    if(++time>23)*time_ptr = 0;
                    else*time_ptr = time;
                }
                else *time_ptr = time;
            }
            else*time_ptr = time;
            i++;
        }
    }
}
