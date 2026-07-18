//v1.7.0b - Bugfix release
//Zero wasn't being hidden when frequency was under 10mhz - FIXED
//Frequency was counting up by 2 instead of 1 in fine tune mode with accelerated dial enabled = FIXED
//Redone CAT tx as tx inhibit wasn't working correctly via standard PTT - FIXED
//Frequency updates via CAT was triggering the fine tune display, regardless of frequency jump, leading to confusion - ALTERATION
//Timings sync'd to 50hz for CPU control and 2.5khz for complete VFD refresh, to maintain compatibility across all TMS chip implementations - ALTERATION
//Removed CAT command timeout. Was disabled in release versions anyway, but was an option in source. Removed completely now - ALTERATION

//v1.7.0 FTPlus
//Y-Library added to CAT
//All button / panel functions are now accessible via CAT, with query / set facilities (including 500K / PMS switches)
//CAT driver improvements(legacy FT757GX mode in Commander shouldn't get random functions activated now
//when moving mouse wheel too fast)
//CAT tx timeout
//split/clar fixed when using cat tx

//v0.7 Configurator
//Removed heartbeat keep-alive - was causing issues in Linux (for me) and it needed
//certain long-press button functions locking out when in Configurator. Now all functions can be used.

//#define 18f4520
#define 18f452
//#define 16f877a //FTMinus is automatically selected
//#define 16f877 //FTMinus is automatically selected

#define bootloader_upgrade //FTPlus only. If you do not own a Pickit, this MUST BE UNCOMMENTED!!!
//define ft_minus // Very basic mode, echoing standard CPU. Meant for use with smaller PICs

#include "chip_set.c"

#ifdef ft_minus
      
      //#define use_ram_eeprom //Not enough space to use this on small PICs
      #define include_display //VFD control
      #define include_dial_support
      #define include_standard_btn //Button controls / standard memory management features
      //#define adjust_baud_rate
      #define include_cat_yaesu // Yaesu basic CAT mode
      //#define include_cat_kenwood //on small chips, this works on its own, with VFO wheel, maybe with display enabled too. Thats ALL! Baud needs changing in source. Headless CAT server?
      #define include_pms_basic //A basic frequency scanner
      #define include_oob_checking // Out of band checking, based on front panel switch position
      //#define include_fine_tuning // Fine tuning - Hidden digit scrolls into view. Toggle on/off with D-LOCK. Needs display and standard buttons enabled

#else

      //#define use_ram_eeprom
      #define include_display
      #define include_dial_support
      #define include_standard_btn
      #define include_enhanced_btn
      #define adjust_baud_rate
      //#define include_cat_basic // Yaesu basic CAT mode
      //#define include_pms_basic //A basic frequency scanner
      #define include_cat_kenwood
      #define include_cat_yaesu
      #define include_pms_advanced
      #define include_oob_checking
      #define include_offset_programming
      #define include_manual_tuning
      #define include_dial_accel
      #define include_cb
      #define include_fine_tuning
      #define include_custom_PLL_REF
      #define include_custom_filter_selection
      #define include_app_selectable_minmax

#endif

#define min_freq 10000
#define max_freq 3200000
#define offset_band_mode 0
#define offset_txrx 3
#define offset_max 9999
//#define eeprom_save_debug8
//#define eeprom_save_debug32
#define default_PLL1REF 1500
#define default_PLL2REF 30

#define mem_ch_block 0
#define offset_block 15
#define vfoa_block 26
#define vfob_block 37


#define fine_tune_display_time 4
#define save_all_time 14
#define timer_timeout 16

//#define cat_command_timer_enabled //prevents lockups. Bleeds off receive buffer if kbhit
#define cat_command_timeout 10 //in mS approx
#define cat_command_timeout_prescale 8 //change this to 1,2,4 or 8. 8 being slowest (safest)
#define cat_tx_safety_timeout 0 //seconds (approx - maybe slightly long second - instead of 65535(int16), we round to 70000 (7) in long timer, as we use 2xint8)
#define cat_buffer_size 25                                //disabled by default(0)

//#define debug_timer
//#define debug_increment
#define dial_timer_max 65535 //max spin down-timer. All percentages are based of this. Default 40000
#define dial_timer_increment 1
#define dial_timer_pullback 5
#define interval_count 128 //represents one complete timing cycle. The faster the cycle, the harder to increase values when spinning
#define interval_sample1 0 //first sample point
#define interval_sample2 64 //second

#define speed1 100
#define speed2 500
#define speed3 1000
#define speed4 3000
#define speed5 6000
#define speed6 9000

#define increment_speed1 10
#define increment_speed2 20
#define increment_speed3 30
#define increment_speed4 50
#define increment_speed5 80
#define increment_speed6 130

#define T0_CLK
#define T0_DIV
#define T0_PRELOAD

#define T1_CLK T1_INTERNAL
#define T1_DIV T1_DIV_BY_8
#define T1_PRELOAD 40536

#define T2_DIV T2_DIV_BY_16
#define T2_PR2 125
#define T2_POSTSCALE 1

unsigned int32 storage_buffer[5];
unsigned int32 aux_buffer[2];
unsigned int32 temp_freq;

int8 temp_display_type = 0;
int8 general_timer = 0;
char temp_byte;
char buffer[cat_buffer_size];
int8 next_in = 0;

#ifdef include_dial_accel
unsigned int16 dial_timer = 0;
unsigned int8 dial_increment;
#endif
volatile int8 res1, res2, dcs, state, old_state, band1, band2, band3, mem_channel, PLLband, state1, state2, vfo;
int8 lp_dial_lock_ndl, lp_vfoab;
int8 baud_rate = 0; 
volatile int8 Q64_val = 0;
#ifdef include_offset_programming
int8 txrx_offset_only;
#endif
int8 update;

#ifdef include_cat_kenwood
//unsigned int32 cat_main_frequency;
//unsigned int32 cat_sub_frequency;
int8 dummy_mode = 49;
#endif

#ifdef include_enhanced_btn
int16 offset, temp_offset, temp_offset2;
int8  lp_dial_lock_dl,
      lp_clarifier,
      lp_btn_dn,
      lp_btn_up,
      lp_mvfo,
      lp_vfom,
      lp_mrvfo,
      lp_split,
      lp_swap,
      lp_fast;
#ifdef include_cb
int8 cb_channel;
int8 cb_region;
#endif

#endif

#ifdef include_custom_PLL_REF
unsigned int16 PLL1REF;
unsigned int16 PLL2REF;
#endif
#ifdef include_app_selectable_minmax
unsigned int32 minimum_freq = 0;
unsigned int32 maximum_freq = 0;
#endif


  
#include "eeprom_state.c"
#include "display_ctrl.c"
#include "PLL_ctrl.c"
#include "timers.c"
#include "encoder.c"
#include "enh_button.c"
#include "std_button.c"
#include "buttons.c"

#include "cat.c"
#include "cat_com.c"

#include "pms.c"
#include "cb.c"



void set_defaults()
{
   for(int i = 0; i < 255; ++i)                          {write8(i, 0);}
   for (i = mem_ch_block; i <= (offset_block - 1); i++)  {write32 (i, 700000); }
   for (i = offset_block; i <= (vfoa_block - 1); i++)    {write32 (i, 0); }
   for (i = vfoa_block; i <= (vfob_block - 1); i++)      {write32 (i, band_bank[i - vfoa_block]); }
   for (i = vfob_block; i <= (vfob_block + 10); i++)     {write32 (i, band_bank[i - vfob_block]); }
#ifdef include_custom_PLL_REF
                                                   write32((vfob_block + 11), default_PLL1REF);
                                                   write32((vfob_block + 12), default_PLL2REF);
#endif   
#ifdef include_app_selectable_minmax
                                                   write32((vfob_block + 13), min_freq);
                                                   write32((vfob_block + 14), max_freq);

#endif
#ifdef include_custom_filter_selection
                                                   write8(fil0, 1);
                                                   write8(fil1, 1);
                                                   write8(fil2, 2);
                                                   write8(fil3, 3);
                                                   write8(fil4, 4);
                                                   write8(fil5, 5);
                                                   write8(fil6, 6);
                                                   write8(fil7, 7);
                                                   write8(fil8, 8);
                                                   write8(fil9, 9);
#endif
                                                   write8(band1_n, 3);
                                                   write8(band2_n, 3);
                                                   write8(band3_n, 3);
                                                   write8(state_n, 1); //Active VFO A / B 
                                                   write8(baud_n, 3);
                                                   
                                                   write8(fine_tune_n, 0);
                                                   write8(dial_n, 0);
                                                   write8(savetimer_n, 1);
                                                   write8(id_enable_n, 0);
                                                   write8(band_offset_n, offset_band_mode);
                                                   write8(display_on_n, 1);
                                                   write8(txrx_offset_only_n, offset_txrx);
                                                   write8(lp_dial_lock_ndl_n, 1);
                                                   write8(lp_vfoab_n, 2);

#ifdef include_enhanced_btn
                                                   write8(lp_dial_lock_dl_n, 3);
                                                   write8(lp_clarifier_n, 4);
                                                   write8(lp_btn_dn_n, 5);
                                                   write8(lp_btn_up_n, 6);
                                                   write8(lp_mvfo_n, 7);
                                                   write8(lp_vfom_n, 8);
                                                   write8(lp_mrvfo_n, 9);
                                                   write8(lp_split_n, 10);
                                                   write8(lp_swap_n, 11);
                                                   write8(lp_fast_n, 0);

#endif
#ifdef include_cb
                                                   write8(cb_ch_n, 19);
                                                   write8(cb_reg_n, 3);
#endif
                                                   write8(checkbyte_n, 1); //Check byte
}


void load_values()
{
   if(read8(checkbyte_n) != 1) set_defaults();
   
   dcs = 15;
   state = read8(state_n);
   band1 = read8(band1_n);
   band2 = read8(band2_n);
   band3 = read8(band3_n);
   mem_channel = read8(mem_ch_n);
   lp_dial_lock_ndl = read8(lp_dial_lock_ndl_n);
   lp_vfoab = read8(lp_vfoab_n);
   autosave = read8(savetimer_n);
   id_enable = read8(id_enable_n);
   display_on = read8(display_on_n);
#ifdef include_offset_programming
   txrx_offset_only = read8(txrx_offset_only_n);
   if(txrx_offset_only == 1) {tx_offset_only = 1; rx_offset_only = 0;}
   if(txrx_offset_only == 2) {tx_offset_only = 0; rx_offset_only = 1;}
   if(txrx_offset_only == 3) {tx_offset_only = 1; rx_offset_only = 1;}
   per_band_offset = read8(band_offset_n);
#endif
#ifdef include_custom_PLL_REF
   PLL1REF = read32(vfob_block + 11);
   PLL2REF = read32(vfob_block + 12);
#endif
#ifdef include_app_selectable_minmax
   minimum_freq = read32(vfob_block + 13);
   maximum_freq = read32(vfob_block + 14);
#endif
#ifdef include_fine_tuning
   fine_tune_display = read8(fine_tune_n);
#endif

#ifdef include_dial_accel
   speed_dial = read8(dial_n);
#endif


   
   


#ifdef include_cb
   check_cb();
   cb_channel = read8(cb_ch_n);
   cb_region = read8(cb_reg_n);
#endif

   
#ifdef include_enhanced_btn
   lp_dial_lock_dl = read8(lp_dial_lock_dl_n);
   lp_clarifier = read8(lp_clarifier_n);
   lp_btn_dn = read8(lp_btn_dn_n);
   lp_btn_up = read8(lp_btn_up_n);
   lp_mvfo = read8(lp_mvfo_n);
   
   lp_vfom = read8(lp_vfom_n);
   lp_mrvfo = read8(lp_mrvfo_n);
   lp_split = read8(lp_split_n);
   lp_swap = read8(lp_swap_n);
   lp_fast = read8(lp_fast_n);
#endif
   load_all_buffers();
   
   dl = 0; cl = 0; sl = 0;
   dcs = 15;
#ifdef include_dial_accel
   dial_timer = 0;
#endif
   old_state = 0;
   cat_mode = 0;
   
   k1 = 1; delay_us (1);
   if (pb0) gen_tx = 0; else gen_tx = 1;
   k1 = 0; 
}



void setup()
{
   setup_adc(ADC_OFF);
   set_tris_a(0b00001);
   set_tris_b(0b00000000);
   set_tris_c(0b11111111);
   set_tris_d(0b11111111);
   set_tris_e(0b000);
   //detune PLL incase MCLR is high
   //this disables TX in most(all?) cases by wiping reference, so PLL cannot lock.
   //failsafe if CAT PTT activates and 
   //update via Pickit without antenna connected etc
#ifdef include_custom_PLL_REF
   PLL1REF = 0; PLL2REF = 0;
   PLL_REF();
#endif
   BITSA = 0; BITSB = 0; BITSC = 0, BITSD = 0, BITSE = 0, BITSF = 0, BITSG = 0;
   PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0;
   k1 = 0; k2 = 0; k4 = 0; k8 = 0;
   //
   //setup_timer_0(T0_INTERNAL|T0_DIV_128);  
   setup_timer_1(T1_CLK | T1_DIV);
   set_timer1(T1_PRELOAD);
#ifdef include_display
   setup_timer_2(T2_DIV, T2_PR2, T2_POSTSCALE);
#endif
   
   //clear_interrupt(INT_TIMER0);              
   
   //enable_interrupts(INT_TIMER0);            
   enable_interrupts(INT_TIMER1);               
   enable_interrupts(INT_TIMER2);
   enable_interrupts(INT_RDA);
   clear_interrupt(INT_TIMER1);                 
   clear_interrupt(INT_TIMER2);
   
   enable_interrupts (global);
   
   
   
   
}

void main()
{
   setup();
   load_values();
   delay_ms(500);
   k1 = 1; delay_us (1);
   if (pb0) gen_tx = 0; else gen_tx = 1;
   
   k1 = 0;
   int8 btnres = 0;
   //while(kbhit()) {getc(); delay_us(40);}
app_detected = 0;
#ifdef adjust_baud_rate
   btnres = scan_buttons();
   if((btnres == mvfo_button) || (btnres == mrvfo_button))
   {
      while(scan_buttons()){}
      set_baud_rate(0);
   } 
   else 
   {
#ifdef include_cat_kenwood
   app_detected = send_magic_packet();
   if(!app_detected) set_baud_rate(1);
   else errorbeep(3);
#else
   set_baud_rate(1);
#endif
   }

#endif

   
   if(!update) update = 2;
   
   btnres = 0;
   res1 = read_counter();
   res2 = res1;
   temp_freq = storage_buffer[state];
   command_received = 0;
#ifdef cat_command_timer_enabled
   cat_com_tmr_enable = 1;
#else
   cat_com_tmr_enable = 0;
#endif
   //if(!kbhit()) enable_interrupts(INT_RDA); else pause_cat = 1;
   command_received = 0;
   state1 = 0; state2 = 0;
   PLL_REF();
   while(true)
   {
#ifdef include_offset_programming   
      if(!update)
      {
         if(tx_offset_only)
         {
            if((!otx) && (tx_mode)) { otx = 1; update = 2;}
            if((otx) && (!tx_mode)) { otx = 0; update = 2;}
         }
         if(rx_offset_only)
         {
            if((!otx) && (tx_mode)) { otx = 1; update = 2;}
            if((otx) && (!tx_mode)) { otx = 0; update = 2;}
         }
      }
#endif   
      
#ifdef include_pms_basic
      if(pms) {
               pms_scan_basic(); 
               update = 2;
               temp_freq = storage_buffer[state];
               }
#endif

#ifdef include_pms_advanced
      if(pms) {
               pms_scan_advanced(); 
               temp_freq = storage_buffer[state];
               jump = 1;
               }
#endif


#if defined (include_cat_kenwood) || defined (include_cat_yaesu)
      //if(app_detected) {app_detected = 0; sectimer = 1; update = 0;}
      if((!pause_cat) && (command_received))
      {
         if(((general_timer > 1) && (baud_rate <= 3)) || (baud_rate > 3))
         {
            update = perform_cat_action();
            if(update) 
            {
#ifdef include_cb
               if((state == 4) && (!cl)) load_cb_state(0);
#endif
               temp_freq = storage_buffer[state];
               
               
               
               
               if(update == 1) general_timer = 1;
               
            }
         }
   
      }     
     
     
#endif

      
#ifdef include_standard_btn
      if(!update)
      {
         if(general_timer >= 1)
         {
            btnres = buttons(0);
            if(btnres) 
            {
            
               update = buttonaction(btnres);
#ifdef include_cb
               if((state == 4) && (!cl)) load_cb_state(0);
#endif
               temp_freq = storage_buffer[state];
               if(update == 1) general_timer = 1;
            }
         }
      }
#endif

#ifdef include_dial_accel
if(!update)
      {

         if((state == 4) && (!cl))
         {
            update = misc_dial8 (cb_channel, 0, 9);
            if(update) 
            {
            load_cb_state(0);
            temp_freq = storage_buffer[state];
            }
         }
         else
         if((state != 4) || ((state == 4) && (cl)))
         {
           if(!speed_dial)
           {
            switch(fine_tune_display)
            {
               default: update = freq_dial_basic (temp_freq, 2); break;
               case 1: update = freq_dial_basic (temp_freq, 1); break;
            }
            
            
           }

           else
           {
            switch(fine_tune_display)
            {
               default: update = freq_dial_accel (temp_freq, 2); break;
               case 1: update = freq_dial_accel (temp_freq, 1); break;
            }
           
           }
         }

        if(update) general_timer = 0;
        //if(update == 2) general_timer = fine_tune_display_time;
         
      } 
#else
if(!update)
      {
#ifdef include_cb
         if(state == 4)
         {
            update = misc_dial8 (cb_channel, 0, 9);
            if(update) 
            {
            load_cb_state(0);
            temp_freq = storage_buffer[state];
            }
         }
         else
#endif
         if(state != 4)
         {
#ifdef include_fine_tuning
           switch(fine_tune_display)
           {
              default: update = freq_dial_basic (temp_freq, 2); break;
              case 1: update = freq_dial_basic (temp_freq, 1); break;
           }
#else
   #ifdef include_dial_support
            update = freq_dial_basic (temp_freq, 2);
   #endif
#endif
         }
         if(update) general_timer = 0;
      }
#endif




if(update)
   {
      if(state == 4) {temp_cl = cl;}
#ifdef include_fine_tuning
      if(update == 1)
      {
         if(fine_tune_display) temp_display_type = 1;
      }
      if(update != 1)
      {
         temp_display_type = 0;
      }
#endif
   }
 
      
jump:   
      if((update) || (jump))
      {         
         
         
         switch(state)
         {
            default: VFD_data (state,dcs, temp_freq, mem_channel, temp_display_type); break;
            //default: VFD_data (state,dcs, dial_increment, mem_channel, 1); break;
#ifdef include_cb
            case 4: if(!temp_cl) VFD_data (state,dcs, cb_channel, mem_channel, 2); else VFD_data (state,dcs, temp_freq, mem_channel, temp_display_type); break;
#endif
         }
         update_PLL(temp_freq, 0xFF);
         storage_buffer[state] = temp_freq;
         update = 0; 
         jump = 0;
         general_timer_start = 1;

      }
      
      if(state < 3)
      {
         if(cl)
         {
            if((transmitting) && (!tx_override)){tx_override = 1; aux_buffer[1] = temp_freq; temp_freq = aux_buffer[0]; jump = 1; goto jump;}
            if((!transmitting) && (tx_override)){tx_override = 0; temp_freq = aux_buffer[1]; update = 1; goto jump;}
         }
      }
      if(sl)
      {
         if(!state1) state1 = state;
         if(state1 == 1) {state2 = 2;}
         if(state1 == 2) {state2 = 1;}
         //if((!cat_tx_request) && (tx_mode)) while(tx_mode){}
         if((transmitting) && (!tx_override)){tx_override = 1; state = state2; temp_freq = storage_buffer[state]; jump = 1; goto jump;}
         if((!transmitting) && (tx_override)){tx_override = 0; state = state1; temp_freq = storage_buffer[state]; jump = 1; goto jump;}
         if((!transmitting) && (!tx_override)) state1 = 0;
      }
      
      if(general_timer >= fine_tune_display_time)
      {
#ifdef include_fine_tuning
         if(fine_tune_display) 
         {
            if(temp_display_type == 1) {temp_display_type = 0; jump = 1;}
         }
#endif
         
         
      }
      
      if(general_timer == save_all_time) 
      {
         if(state == 4) 
         {
            if(temp_cl) {temp_cl = 0; jump = 1;}
         }
         if(autosave) {save_all();}
      }
      
      if(general_timer == timer_timeout)
      {
         general_timer_start = 0;
         
      }
      
      
      
   }

}
