//!GX2 emulation opcodes implemented & variables/helpers:
//!SPLIT       01h      sl
//!MR          02h      load_mem_ch_f
//!M           03h      save_mem_ch_f
//!D LOCK      04h      dl
//!VFO A/B     05h      state1
//!M>VFO       06h      load_mem_ch_f
//!UP          07h      hgsel / btn up hdlr
//!DOWN        08h      hgsel / dtn dn hdlr
//!CLAR        09h      cl
//!FRQ SET     0Ah      convert_from_BCD
//!SWAP        0Bh      swap_hdlr
//!MODESEL     0Ch      dummy_mode
//!HGSEL       0Dh      hgsel
//!RTN_DELAY   0Eh      cat_delay
//!CAT_TX 0/1  0Fh      cat_tx_request
//!STATUS      10h      calc_status
//!CAT_TX TGL  11h      cat_tx_request
//!BAUD        FDh      baud_n
//!EEPROM_RST  FEh      checkbyte_n
//!REBOOT      FFh      reset_cpu


//#define 18f4520
//#define 18f452
//#define 16f877a //FTMinus is automatically selected
#define 16f877 //FTMinus is automatically selected

//#define bootloader_upgrade //FTPlus only. If you do not own a Pickit, this MUST BE UNCOMMENTED!!!

#define include_dial_support
//#define adjust_baud_rate
//#define include_cat_yaesu_GX // Yaesu basic CAT mode
#define include_cat_yaesu_GX2 // GX2 emulation
#define include_pms_basic //A basic frequency scanner
#define include_oob_checking // Out of band checking, based on front panel switch position
#define include_fine_tuning // Fine tuning - Hidden digit scrolls into view. Toggle on/off with D-LOCK. Needs display and standard buttons enabled


#ifdef 18f452
#include <18F452.h>
#endif

#ifdef 18f4520
#include <18F4520.h>
#endif

#ifdef 16f877a
#include <16F877A.h>
#endif

#ifdef 16f877
#include <16F877.h>
#endif

#use delay(clock=20000000)
#use rs232(baud=4800, xmit=PIN_C6, rcv=PIN_C7, parity=N, stop=2, ERRORS)

#if defined(16f877) || defined (16f877a)
#fuses HS,PUT, NOWDT,NOPROTECT,NOLVP, BROWNOUT
# byte PORTA = 0x05
# byte PORTB = 0x06
# byte PORTC = 0x07
# byte PORTD = 0x08
# byte PORTE = 0x09
#endif

#if defined(18f452) || defined (18f4520)
#fuses HS,PUT, NOWDT,NOPROTECT,NOLVP, BORV27
# byte PORTA = 0x0f80
# byte PORTB = 0x0f81
# byte PORTC = 0x0f82
# byte PORTD = 0x0f83
# byte PORTE = 0x0f84
#ifdef bootloader_upgrade
   #include <bootloader.h>
#endif
#endif

#define tmr1_preload 61536 //40536 //10hz tmr1, 1:8 prescale
#define disp_int_tmr 119 //115 2 PR2 reg
#define display_interrupt_hold 40
#define postscale 1 //1:1 postscale
#define disp_latches 1
#define min_freq 10000
#define max_freq 3200000
#define cat_buffer_size 25
#define cat_command_timer_enabled //prevents lockups. Bleeds off receive buffer if kbhit
#define cat_command_timeout 1 //in mS approx
#define cat_command_timeout_prescale 8 //change this to 1,2,4 or 8. 8 being slowest (safest)


//#define eeprom_save_debug8
//#define eeprom_save_debug32


#include "chip_setup.c"
#include "EEPROM.c"
#include "misc_func.c"
#include "VFD.c"
#include "dial.c"
#include "timers.c"
#include "button_func.c"
#include "PLL.c"
#include "cat.c"
#include "pms.c"



void setup()
{
   setup_adc(ADC_OFF);
   set_tris_a(0b00001);
   set_tris_b(0b00000000);
   set_tris_c(0b11111111);
   set_tris_d(0b11111111);
   set_tris_e(0b000);
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
   setup_timer_2(T2_DIV_BY_16, disp_int_tmr, postscale);
   set_timer1(tmr1_preload);
   enable_interrupts(INT_TIMER1);               
   enable_interrupts(INT_TIMER2);
   enable_interrupts(INT_RDA);
   clear_interrupt(INT_TIMER1);                 
   clear_interrupt(INT_TIMER2);
   enable_interrupts (global);
   
   BITSA = 0; BITSB = 0; BITSC = 0, BITSD = 0, BITSE = 0, BITSF = 0;
   PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0;
   k1 = 0; k2 = 0; k4 = 0; k8 = 0;

   PLL_REF(0, 0);
   delay_ms(200);
}



void set_defaults()
{
   for(int i = 0; i < 255; ++i)                          {write8(i, 0);}
   for (i = mem_ch_block; i <= (offset_block - 1); i++)  {write32 (i, 700000); }
   for (i = offset_block; i <= (vfoa_block - 1); i++)    {write32 (i, 0); }
   for (i = vfoa_block; i <= (vfob_block - 1); i++)      {write32 (i, band_bank[i - vfoa_block]); }
   for (i = vfob_block; i <= (vfob_block + 10); i++)     {write32 (i, band_bank[i - vfob_block]); }

                                                   write8(band1_n, 3);
                                                   write8(band2_n, 3);
                                                   write8(band3_n, 3);
                                                   write8(state_n, 1); //Active VFO A / B 
                                                   write8(vfo_n, 0);
                                                   write8(mem_ch_n, 0);
                                                   write8(savetimer_n, 1);                                                   
                                                   write8(display_on_n, 1);
                                                   write8(dummy_mode_n, 0);                                        
                                                   write8(checkbyte_n, 1); //Check byte
                                                   write8(baud_n, 1);
}


void load_values()
{
   if(read8(checkbyte_n) != 1) set_defaults();
   
                                                   band1 = read8(band1_n);
                                                   band2 = read8(band2_n);
                                                   band3 = read8(band3_n);
                                                   state1 = read8(state_n);
                                                   vfo = read8(vfo_n);
                                                   mem_channel = read8(mem_ch_n);
                                                   autosave = read8(savetimer_n);
                                                   display_on = read8(display_on_n);
                                                   dummy_mode = read8(dummy_mode_n);
                                                   baud = read8(baud_n);
                                                   switch(baud)
                                                   {
                                                      case 1: set_uart_speed (4800); break;
                                                      case 2: set_uart_speed (19200); break;
                                                      case 3: set_uart_speed (57600); break;
                                                      case 4: set_uart_speed (115200); break;
                                                   }

   k1 = 1; delay_us (1);
   if (pb0) gen_tx = 0; else gen_tx = 1;
   k1 = 0;

#ifdef cat_command_timer_enabled
   cat_com_tmr_enable = 1;
#endif
   load_all_buffers();   
   dcs = get_dcs();
   update = 1;
   fl_cnt = 0;
   fl_cnt_gearing = 0;
   old_state = 0;
   temp_display_type = 0;
   dial_inc = 2;
   beep_int = 0;
   btnscan = 1;
   btnres = 0;
   rtnres = 0;
   btn_down = 0;
   timer = 0;
   btntimer = 0;
   next_in = 0;
   temp_byte = 0;
   cat_command_timer = 0;
   fine_tune_display = 0;
   opcode = 0;
   hgsel = 0;
}


void main()
{

   setup();
   load_values();
   PLL_REF(1500, 30);
   k1 = 1; delay_us (1);
   k1 = 0;
   reset_counter();
   temp_freq = buffer[state1];

   while(true)
   {
      if(!update) { temp_freq = buffer[state1];}

#ifdef include_pms_basic
      if(!update)
      {
         if(pms) 
         {

            pms_scan_basic();
            temp_freq = buffer[state1]; timer = 5;
            update = 2;
         }
      }
#endif

#if defined(include_cat_yaesu_GX) || defined(include_cat_yaesu_GX2)
      if(!update)
      {
#ifdef include_cat_yaesu_GX2
         calc_status();
#endif
         if((!btntimer) && (timer >= 5) && (yaesu_command_received))
         {
            update = action_yaesu_cat(); 
            if(update) {temp_freq = buffer[state1]; timer = 5;}
         }
      
      }
#endif
      
  
      if(!update)
    
      {
         if(timer > 0)
         {
            btnres = buttons(0);
            if(btnres) 
            {
               update = buttonaction(btnres);
            }
            if(update) {temp_freq = buffer[state1]; timer = 1;}
         }
      }
      
#ifdef include_dial_support
      if(!update)
      {
         update = freq_dial_basic(temp_freq, dial_inc);
         if(update) {timer = 0;}
      }
#endif
      if(update == 1) 
      {
         
         if(fine_tune_display) temp_display_type = 1; else temp_display_type = 0;
      }
jump:      
      if((update) || (jump))
      {

#ifdef include_oob_checking
         IF(!gen_tx)
         {
            old_PORTA = PORTA;
            for (INT i = 0; i < 10; i++)
               {
                  valid = 0; PORTA = 6;
                  IF ( (temp_freq >= blacklist[i * 2]) && (temp_freq <= blacklist[ (i * 2) + 1])) {valid = 1; break;}
               }
            PORTA = old_PORTA;
            
         } else valid = 1;
#else
         
         valid = 1;
#endif

         VFD_data(state1, temp_freq, temp_display_type);
         set_PLL(0, 0xFF);
         buffer[state1] = temp_freq;
         update = 0; jump = 0;
         reset_counter();
         timerstart = 1;
         saved = 0;
      }
   
      if(state1 < 3)
      {
         if(cl)
         {
            if((transmitting) && (!tx_override)){tx_override = 1; aux_buffer[1] = temp_freq; temp_freq = aux_buffer[0]; jump = 1;}
            if((!transmitting) && (tx_override)){tx_override = 0; temp_freq = aux_buffer[1]; jump = 1;}
         }
      
         if(sl)
         {
            if(!old_state)
            {
            old_state = state1;
            if(state1 == 1) {state2 = 2;}
            if(state1 == 2) {state2 = 1;}
            }
            //if((!cat_tx_request) && (tx_mode)) while(tx_mode){}
            if((transmitting) && (!tx_override)){tx_override = 1; state1 = state2; temp_freq = buffer[state1]; jump = 1;}
            if((!transmitting) && (tx_override)){tx_override = 0; state1 = old_state; temp_freq = buffer[state1]; jump = 1;}
            if((!transmitting) && (!tx_override)) old_state = 0;
         }
      }
      if((timerstart) && (timer > 20)) 
      {
         
         if(temp_display_type == 1) 
            {
            temp_display_type = 0;
            jump = 1;
            }
      }
      if((timerstart) && (timer > 50)) 
      {
         if(!saved) save_all();
      }
      if(jump) goto jump;
   }
}
