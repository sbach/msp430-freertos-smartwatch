/*******************************************************************************
 *
 * Light implementation of sprintf for MSP430x familiy
 *
 *
 * Note: The original code was written by oPossum and posted
 * on the 43oh.com forums. For more information on this code,
 * please see the link below.
 *
 * http://www.43oh.com/forum/viewtopic.php?f=10&t=1732
 *
 * A big thanks to oPossum for sharing such great code!
 *
 ******************************************************************************/

#include <stdarg.h>

#include "vuprintf.h"


static const unsigned long dv[] = {
//  4294967296      // 32 bit unsigned max
    1000000000,     // +0
     100000000,     // +1
      10000000,     // +2
       1000000,     // +3
        100000,     // +4
//       65535      // 16 bit unsigned max
         10000,     // +5
          1000,     // +6
           100,     // +7
            10,     // +8
             1,     // +9
};


static void sputchar(char **str, unsigned c) {
    *(*str)++ = c;
}

static void sputstr(char **str, char *s) {
    while(*s) sputchar(str, *s++);
}

static void sputhex(char **str, unsigned n)
{
    static const char hex[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    sputchar(str, hex[n & 15]);
}


static void sxtoa(char **str, unsigned long x, const unsigned long *dp)
{
    char c;
    unsigned long d;
    if(x) {
        while(x < *dp) ++dp;
        do {
            d = *dp++;
            c = '0';
            while(x >= d) ++c, x -= d;
            sputchar(str, c);
        } while(!(d & 1));
    } else
        sputchar(str, '0');
}


void vuprintf(char *str, const char *format, va_list a)
{
    char c;
    int i;
    long n;

    while((c = *format++)) {
        if(c == '%') {
            switch(c = *format++) {
                case 's':                       // String
                    sputstr(&str, va_arg(a, char*));
                    break;
                case 'c':                       // Char
                    sputchar(&str, va_arg(a, unsigned));
                    break;
                case 'i':                       // 16 bit Integer
                case 'u':                       // 16 bit Unsigned
                    i = va_arg(a, int);
                    if(c == 'i' && i < 0) i = -i, sputchar(&str, '-');
                    sxtoa(&str, (unsigned)i, dv + 5);
                    break;
                case 'l':                       // 32 bit Long
                case 'n':                       // 32 bit uNsigned loNg
                    n = va_arg(a, long);
                    if(c == 'l' &&  n < 0) n = -n, sputchar(&str, '-');
                    sxtoa(&str, (unsigned long)n, dv);
                    break;
                case 'x':                       // 16 bit heXadecimal
                    i = va_arg(a, int);
                    sputhex(&str, i >> 12);
                    sputhex(&str, i >> 8);
                    sputhex(&str, i >> 4);
                    sputhex(&str, i);
                    break;
                case 0: return;
                default: goto bad_fmt;
            }
        } else
bad_fmt:    sputchar(&str, c);
    }
    sputchar(&str, 0);
}
