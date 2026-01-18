//FTPlus Dial Unit firmware
//(c)2026 Matthew Bostock M0WCA

//#define 16f684
#define 16f737

#ifdef 16f684
#include <16F684.h>
#endif

#ifdef 16f737
#include <16F737.h>
#endif


//movement pulses
//uses timer0
//T0_INTERNAL | T0_DIV_4 + t0_preload(of 246) = pulse frequency of 50khz per pulse
//50khz is about the maximum Yaesu CPU can handle. FTPlus can go faster, but we wont here
//each movement is quadrature (CHA up, CHA down, CHB up, CHB down)
//so encoder resolution eg 256, one revolution per phase/rise/fall = 256 x 4.
// = 10.240khz per revolution. Stock Yaesu encoder is 250 pulse encoder? would be 10khz as per user manual

//#define reverse_direction //disabled by default. Some cheap encoders need this. Reverses CW/CCW values
 //how many time to repeat clock. Default 1. Higher values make bigger jumps per pulse

#define encoder_resolution 256 //resolution of fitted encoder. Compensates for low value encoders by still making 1 revolution = 10khz(ish)
#define t0_preload 246 //default 246 = 50khz clock
int8 pulsechain;
int16 pulsechainmax;

//only applies to speaker. Doesn't apply to DC buzzer
#define beepvol 2
#define beeppitch 29
#define beeplen 1000
unsigned int16 beeptimer;


#ifdef 16f684
#fuses INTRC_IO, NOWDT, BROWNOUT, PUT
#use delay(internal=8M)
# byte PORTA = 0x05
# byte PORTC = 0x07

//outputs
# bit dir_out=PORTC.0
# bit clk_out=PORTC.1
# bit beep=PORTC.5
# bit abeep=PORTC.4

//inputs
# bit CHA=PORTA.5
# bit CHB=PORTA.4
# bit dl=PORTA.1
# bit buzz=PORTA.2
#endif

#ifdef 16f737
#fuses INTRC_IO, NOWDT, BROWNOUT, BORV42, PUT
#use delay(internal=8M)
# byte PORTA = 0x05
# byte PORTB = 0x06
# byte PORTC = 0x07
# byte PORTE = 0x09

//outputs
# bit dir_out=PORTC.0
# bit clk_out=PORTC.1
# bit beep=PORTC.2
# bit abeep=PORTC.3

//inputs
# bit CHA=PORTA.0
# bit CHB=PORTA.1
# bit dl=PORTA.2
# bit buzz=PORTB.0
#endif

int8 BITSA, BITSB;
# bit currentStateCHA=BITSA.0
# bit previousStateCHA=BITSA.1
# bit currentStateCHB=BITSA.2
# bit previousStateCHB=BITSA.3
# bit currentStateBEEP=BITSA.4
# bit previoustStateBEEP=BITSA.5
# bit detected=BITSA.6
# bit beeptimerstart=BITSA.7
# bit beeping=BITSB.0
# bit clk0 = BITSB.1
# bit clk1 = BITSB.2

#define max_enc_resolution_supported 256 //dont change this unless you have a stupid high resolution encoder

#int_EXT
void ext_isr(void)
{
   if(buzz) {beeptimerstart = 1;} //time to sound beeper
   CLEAR_INTERRUPT(int_EXT);//clear and exit back to main loop
}

#INT_TIMER0
void t0_isr(void)
{ 
  if(detected)
  {
      //quadrature. 4clks per pulse, (high, low, high, low)
           if((clk0 == 0) && (clk1 == 0)) {clk_out = 1; clk0 = 1; clk1 = 0;}
      else if((clk0 == 1) && (clk1 == 0)) {clk_out = 0; clk0 = 1; clk1 = 1;}
      else if((clk0 == 1) && (clk1 == 1)) {clk_out = 1; clk0 = 0; clk1 = 1;}
      else if((clk0 == 0) && (clk1 == 1)) {clk_out = 0; clk0 = 0; clk1 = 0; ++pulsechain;}
  }
  if(pulsechain == pulsechainmax) {detected = 0; pulsechain = 0;}  
  CLEAR_INTERRUPT(int_timer0);
  set_timer0(t0_preload);   
}

void setup()
{
   setup_ccp1(CCP_PWM);// Configure CCP1 as a PWM
   setup_timer_2(T2_DIV_BY_16,beeppitch,1);
   setup_timer_0(T0_INTERNAL|T0_DIV_4);
   set_timer0(t0_preload);
   setup_adc (ADC_OFF);
   
   #ifdef 16f737
   set_tris_a(0b00111);
   set_tris_b(0b00001111);
   set_tris_c(0b00000000);
   set_tris_e(0b000);
   #endif
   
   #ifdef 16f684
   set_tris_a (0b111110);
   set_tris_c (0b000000);
   port_a_pullups (true);
   #endif
   ext_INT_edge (L_TO_H);
   disable_interrupts(int_EXT);
   enable_interrupts(int_EXT);
   disable_interrupts(int_timer0);
   enable_interrupts(int_timer0);
   enable_interrupts(GLOBAL);
   
   BITSA = 0; BITSB = 0;
   delay_ms(10);
   pulsechainmax = (max_enc_resolution_supported / encoder_resolution);
   beeptimer = 0;
   beeptimerstart = 1;
   //get base state of channels
   currentStateCHA = CHA;
   currentStateCHB = CHB;
   previousStateCHA = currentStateCHA;
   previousStateCHB = currentStateCHB;
   dir_out = 1;
}

void main()
{
   setup();
   while(true)
   {
      
      
         if(beeptimerstart)
         {
            ++beeptimer;
            if(beeptimer < beeplen)
            {
               set_pwm1_duty(beepvol);
               abeep = 1;
            }
            if(beeptimer >= beeplen)
            {
               set_pwm1_duty(0);
               abeep = 0;
               beeptimer = 0;
               beeptimerstart = 0;
               
            }
            delay_us(1);
         }
         //beep(500);
         //work out direction
         //CHA is 180 deg out of phase with CHB acc to datasheet
         
         //if CHA is high and CHB is low,we wait for CHA pulse to finish. then check if CHB is under it.
         //if so,then B is at least 90 degrees out of phase with A,thus we are spinning clockwise.
         //Send signal to radio high,look for pulses
         if((CHA)&&( ! CHB))
         {
            while(CHA)
            {
               #ifdef reverse_direction
               if(CHB){ dir_out  = 0;break;}
               #else
               if(CHB){ dir_out  = 1;break;}
               #endif
               if(beeptimerstart) break;
             
            }
         }
         
         //if CHA is low and CHB is high,we wait for CHB pulse to finish. then check if CHA is under it.
         //if so,then A is at least 90 degrees out of phase with B,thus we are spinning anti  - clockwise.
         //Send signal to radio low,look for pulses
         if(( ! CHA)&&(CHB))
         {
            while(CHB)
            {
               #ifdef reverse_direction
               if(CHA){ dir_out  = 1;break;}
               #else
               if(CHA){ dir_out  = 0;break;}
               #endif
               if(beeptimerstart) break;
            }
         }
         
            //keep checking A
            currentStateCHA = CHA;
            currentStateCHB = CHB;
            
            //if A has changed,send a pulse,update previous reading with now current reading
            
            if(previousStateCHA  != currentStateCHA)
            {
               //if( ! dl) detected = 1;
               previousStateCHA = currentStateCHA;
            }
            if(previousStateCHB  != currentStateCHB)
            {
               if( ! dl) detected = 1;
               previousStateCHB = currentStateCHB;
            }
         
         
      }
}
