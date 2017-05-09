/* mel690.c  Play a melody */

/*    Low pin count demo board               J1      ----------
         ___________  ___________           1 RA5 --| earphone |
        |           \/           |          2 RA4 --| earphone |
  +5V---|Vdd     16F690       Vss|---GND    3 RA3    ----------
     ---|RA5        RA0/AN0/(PGD)|-<-RP1    4 RC5
     ---|RA4            RA1/(PGC)|---       5 RC4
  SW1---|RA3/!MCLR/(Vpp)  RA2/INT|---       6 RC3
     ---|RC5/CCP              RC0|->-DS1    7 RA0
     ---|RC4                  RC1|->-DS2    8 RA1
  DS4-<-|RC3                  RC2|->-DS3    9 RA2
        |RC6                  RB4|         10 RC0
        |RC7               RB5/Rx|         11 RC1
        |RB7/Tx               RB6|         12 RC2
        |________________________|         13 +5V
                                           14 GND
*/

/* Connect high impedance earphone to J1:1 and J1:2 */

/* B Knudsen Cc5x C-compiler - not ANSI-C */
#include "16F690.h"
#pragma config |= 0x00D4

#include "lookup_ph.c"
#include "delays.c"
#define EIGHT_NOTE 250
char LookUpNote(char);  /* function prototype */
void delay(char);

void  main(void)
{
  char note, time;
  bit out, DS1_led, button = 1;
  /* AD setup */ 
  ANSEL.0 = 1;
  ADCON1 = 0b0.001.0000; /* AD conversion clock 'fosc/8' */
  /* 
     0.x.xxxx.x.x  ADRESH:ADRESL is 10 bit left justified
     x.0.xxxx.x.x  Vref is Vdd
     x.x.0000.x.x  Channel 00 (AN0)
     x.x.xxxx.0.x  Go/!Done start later
     x.x.xxxx.x.1  Enable AD-converter
  */
  ADCON0 = 0b0.0.0000.0.1; 
  PORTA = 0;
  TRISA.5 = 0; /* RB5 will act as "ground-pin" for earphone */
  TRISA.3 = 1; /* SW1 input                                 */
  delay(100);  /* 100 ms for demo board initialization      */
  OPTION = 0b111; /* Timer0 Prescaler divide by 256         */

  while(1)
   {
     GO=1;          /* start AD                               */
     char i;   
     for(i=0;i<5;i++)
     {
	   
       note = ADRESH;
       if( note == 0 ) break;
       if( note == 1 ) TRISA.4 = 1;  /* pause note is silent */
       else TRISA.4 =  0;            /* RA4 is output        */
          TMR0 = 0;                  /* Reset timer0         */
          while (TMR0 < EIGHT_NOTE)  /* "1/8"-note duration  */
      	    {
              char j;
              for(j = note; j > 0; j--) 
                { /* Delay. Loop + 4 nop()'s totals 10 us  */
                  nop(); nop(); nop(); nop();
                }
              /* Toggle Output bit RA4 On/Off */
              out = !out; 
              PORTA.4 = out;
            }
     }
    while(PORTA.3 == 1){ /* wait */ } /* SW1 to play again */ 
   }
}

