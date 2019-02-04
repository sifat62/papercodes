//Detailed Publication: https://doi.org/10.1109/ICIEV.2016.7759972 

//=====================   Declaring Variables =================

int flag1;
int flag2;
int flag3;
int stop=1;
int load1,load2,load3,load4,load5,load6;
void check_phase12(void);
void check_phase23(void);
void check_phase31(void);
void delay_timer1(void);
void delay_timer2(void);
unsigned int store(void);
void LED(void);

sbit LCD_RS at RB4_bit;
sbit LCD_EN at RB5_bit;
sbit LCD_RW at RB6_bit;
sbit LCD_D4 at RB0_bit;
sbit LCD_D5 at RB1_bit;
sbit LCD_D6 at RB2_bit;
sbit LCD_D7 at RB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_RW_Direction at TRISB6_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;
// End LCD module connections

char txt1[] = "Positive";
char txt2[] = "Negative";
char txt3[] = "Sequence";
//char txt3[] = "Lcd4bit";
//char txt4[] = "example";



//=========================================================

void main()
     
{
     CCP1CON =0x07;
     TRISA=0b01101111;          // initializing RA0,RA1, RA3, RA4, RA5 as input
     TRISD.RD7=0;                     // RD7 port as output
     TRISD.RD6=0;               // RD6 port as output
     TRISC = 0x00;
     //PORTD = 0x00;              // PORTD as output
     TRISB = 0x00;

//=====initialize timer 0=========
     T0CON=0b00000000;     // 16bit timer, 1:2 prescaler, internal clock source
     
     TMR0H=0xFB;                              // timer value for delay 5ms
     TMR0L=0x1E;
     
     
//=====initialize timer 1=========

     T1CON=0b10001000; // 16bit timer; timer 1 oscilator enable. internal clock source
     
     TMR1H=0xEC;                                             // timer value for 0.01ms delay
     TMR1L=0x78;


     INTCON.GIE=1;                                             // Global interupt enable
     INTCON.PEIE=1;                                            // Periferal interupt enable
     PIR1.TMR1IF=0;                                            // Timer1 interupt flag enable
     INTCON.TMR0IF=0;                                          // Timer0 interupt flag enable
   



 //=================function checking sequnce=================
 
   check_phase12();
   delay_timer2(); //wait for 1/4 th of a cycle i.e. 5ms
   check_phase23();
   delay_timer2(); //wait for 1/4 th of a cycle i.e. 5ms
   check_phase31();
   
   
 //================logic to detect sequence==================
 
     Lcd_Init();                        // Initialize LCD
     Lcd_Cmd(_LCD_CLEAR);               // Clear display
     Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
     Delay_ms(150);
     LED();

   while(1);
}


//====================initializig ADC for AN0, AN1, AN4    ports=============================


void check_phase12(void)
{
        stop =1;
        while(stop)
        {
        ADCON0=0b11000001;                              // power up adc,AN0 channel select, RCclock
        ADCON1=0b10001111;                              // +Vref & -Vref, RC clock, right shifted
        ADCON0.GO=1;                                    // initialize adc
        while(ADCON0.DONE==1);                          // when complete
        load1=store();                                  // store the data into load 1

        ADCON0=0b11001001;                              // power up adc,AN1 channel select, RC clock
        ADCON1=0b10001101;                              // +Vref & -Vref,AN1, AN2, RC clock, right shifted
        ADCON0.GO=1;                                    // initialize adc
        while(ADCON0.DONE==1);                          // when complete
        load2=store();                                  // store the data into load 2


            if(load1==1023)
                        {
            flag1=1;
            stop=0;
            /* Phase1=A,Phase2=B ,sequence ABC
               Phase1=B,Phase2=C ,sequence BCA
               Phase1=A,Phase2=C ,sequence ACB */
            }

            else if(load2==1023)
                        {
            flag1=0;
            stop=0;
            /* Phase1=B,Phase2=A ,sequence BAC
               Phase1=C,Phase2=B ,sequence CBA
               Phase1=C,Phase2=A ,sequence CAB */
            }

                delay_timer1(); //delay 0.01 sec for 100HZ

        }
}

void check_phase23(void)
{
        stop=1;
        while(stop)
        {
        ADCON0=0b11001001;                                 // power up adc,AN1 channel select, RCclock
        ADCON1=0b10001101;                                 // +Vref & -Vref,AN1, AN2, RC clock, right shifted
        ADCON0.GO=1;                                       // initialize adc
        while(ADCON0.DONE==1);                            // when complete
       load3=store();                                    // store the data into load 3

        ADCON0=0b11100001;                                // power up adc,AN4 channel select, RCclock
        ADCON1=0b10001100;                                // +Vref & -Vref,AN1, AN2, AN4, RC clock, right shifted
        ADCON0.GO=1;                                      // initialize adc
        while(ADCON0.DONE==1);                            // when complete
        load4=store();                                    // store the data into load 4


             if(load3==1023)
                        {

            flag2=1;
            stop=0;
            /* Phase2=B,Phase3=C ,sequence ABC
               Phase2=A,Phase3=B ,sequence CAB*/
            }

              else if(load4==1023)
                        {
            flag2=0;
            stop=0;
            /* Phase2=C,Phase3=A ,sequence BCA
               Phase2=C,Phase3=B ,sequence ACB
               Phase2=B,Phase3=A ,sequence CBA
               Phase2=A,Phase3=C ,sequence BAC */ // because of two half cycle distance in peaks of A to C it is different for our 1/4th cycle delay
            }

               delay_timer1();

        }

}

void check_phase31(void)
{
        stop=1;
        while(stop)
        {
        ADCON0=0b11100001;                        // channel AN4, RCclock
        ADCON1=0b10001100;                        // +Vref & -Vref,AN1, AN2, AN4, RC clock, right shifted
        ADCON0.GO=1;                              // initialize adc
        while(ADCON0.DONE==1);                    //while done
       load5=store();                           //store the data into load 5

        ADCON0=0b11000001;                        // channel AN1, RC clock
        ADCON1=0b10001111;                        // +Vref & -Vref,AN1, AN2, AN4, RC clock, right shifted
        ADCON0.GO=1;                              // initialize adc
        while(ADCON0.DONE==1);                    //while done
        load6=store();                           //store the data into load 5


                if(load5==1023)
                        {
            flag3=1;
            stop=0;
            /*enters 'if' for all sequence */
            }

                else if(load6==1023)
                        {
            flag3=0;
            stop=0; //never enters 'else if'!
            }

           //Delay_ms(10);
                delay_timer1();

        }
}

void delay_timer1(void)                //delay 0.02 sec for 50HZ
{
      T1CON.TMR1ON=1;                  // start timer 0
      while(PIR1.TMR1IF==0);           // when timer 1 interupt flag 0 then
      PIR1.TMR1IF=0;                   // timer flag interupt flag assigned to zero
      T1CON.TMR1ON=0;                  // stop timer
      TMR1H=0xFF;                      // time to take int flag to be zero
      TMR1L=0xAE;

}



unsigned int store(void)
{
        int load=0;
        int save=0;
        load=ADRESL;                 // 10bit adc low bit store to "load"
        save=ADRESH;                 // 10bit adc high bit store to "save"
        save=save<<8;                // allow 2 bit to "save"
        load=load|save;              // stor low 8 + high 2 = 10bit adc result to "load"
        return load;                 // return the result to "store" function

}

void delay_timer2(void)                //delay 5ms
{
      T0CON.TMR0ON=1;               // start timer 0
      while(INTCON.TMR0IF==0);      // when timer 0 interupt flag 0 then
      T0CON.TMR0ON=0;               // stop timer
      
      TMR0H=0xFF;                   // time to take int flag to be zero
      TMR0L=0xEB;

}

 void LED(void){
                if(flag1==1&&flag2==1&&flag3==1){

                   PORTD.RD7=1; //phase ABC
                   PORTD.RD6=0;
                   Lcd_Out(1,1,txt1);
                   Delay_ms(150);
                   Lcd_Out(2,1,txt3);
                   }
          else if(flag1==0&&flag2==1&&flag3==1){
                    PORTD.RD7=1; //phase CAB
                    PORTD.RD6=0;
                    Lcd_Out(1,1,txt1);
                   Delay_ms(150);
                   Lcd_Out(2,1,txt3);
                    }
           else if(flag1==1&&flag2==0&&flag3==1){
                  PORTD.RD7=1; //phase BCA
                  PORTD.RD6=0;
                 Lcd_Out(1,1,txt1);
                   Delay_ms(150);
                   Lcd_Out(2,1,txt3);
                  }
           else{
                PORTD.RD7=0;
                PORTD.RD6=1; // PHASE ACB
                Lcd_Out(1,1,txt2);
                Delay_ms(150);
                Lcd_Out(2,1,txt3);
        }
        }
