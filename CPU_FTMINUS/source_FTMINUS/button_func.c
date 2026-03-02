int8 dcs_res[32] = 
{
   //dcs dl cl sl
   15, 0, 0, 0,
   14, 0, 1, 1,
   12, 0, 0, 1,
   4, 1, 1, 1,
   3, 1, 1, 0,
   2, 0, 1, 0,
   1, 1, 0, 0,
   0, 1, 0, 1
};

int8 get_dcs()
{
   INT8 res = 15;
   for(INT8 i  = 0; i < 8; i++)
   {
      IF((dl == dcs_res[(i * 4) + 1])&&(cl == dcs_res[(i * 4) + 2])&&(sl == dcs_res[(i * 4) + 3])){res = dcs_res[i * 4]; break; }
   }
   RETURN res;
}

void set_dcs(INT8 res)
{
   for (INT8 i = 0; i < 8; i++)
   {
      IF(res == dcs_res[(i * 4)]){dl = dcs_res[(i * 4) + 1]; cl = dcs_res[(i * 4) + 2]; sl = dcs_res[(i * 4) + 3]; break;}
   }
}

void inc_mem_channel()
{
   if (mem_channel < 14) ++mem_channel; else mem_channel = 0; buffer[3] = load_mem_ch_f (mem_channel);
}

void dec_mem_channel()
{
   if (mem_channel > 0) --mem_channel; else mem_channel = 14; buffer[3] = load_mem_ch_f (mem_channel);
}

void change_band(int8 state, int1 direction)
{
   int8 temp_band = 0;
   switch(state)
   {
      case 1: temp_band = band1; break;
      case 2: temp_band = band2; break;
   }
   save_band_state_f (state, temp_band);
   if(!direction) {if (temp_band > 0) --temp_band; else temp_band = 9;}
   else {if (temp_band < 9) ++temp_band; else temp_band = 0;}
   buffer[state] = load_band_state_f (state, temp_band);
   switch(state)
   {
      case 1: band1 = temp_band; break;
      case 2: band2 = temp_band; break;
   }
}

void clarifier_button_handler()
{
   if ( ! cl)
   {
      sl = 0; cl = 1;
      aux_buffer[0] = buffer[state1];
   }

   else
   {
      cl = 0;
      aux_buffer[0] = 0;
   }
}

void btn_dn_handler(int8 res)
{
   if (!res)
   {
      res = state1;
      if((state1 == 1) || (state1 == 2)) {if ((hard_500k) || (soft_500k)) res += 4;}
   }
   
   if((res) && (res < 3)) change_band(state1, 0);
   if((res) && (res == 3)) dec_mem_channel();
   if((res) && (res > 4)) buffer[state1] -= 50000;

   
}

void btn_up_handler(int8 res)
{
   if (!res)
   {
      res = state1;
      if((state1 == 1) || (state1 == 2)){if ((hard_500k) || (soft_500k)) res += 4;}
   }
   
   if((res) && (res < 3)) change_band(state1, 1);
   if((res) && (res == 3)) inc_mem_channel();
   if((res) && (res > 4)) buffer[state1] += 50000;

}

void mvfo_handler()
{
   if(state1 == 4) buffer[(int8) vfo + 1] = buffer[4];
   else buffer[(int8) vfo + 1] = load_mem_ch_f (mem_channel);
   quick_disp_flash(2, (int8) vfo + 1);
}

void vfoab_handler()
{
   save_all();
   if (state1 == 1) state1 = 2;
   else
   if (state1 == 2) state1 = 1;
   write8 (state_n, state1);
}

void dial_lock_button_handler()
{
   if ( ! dl) dl = 1; else dl =  0;
}

void vfom_handler()
{
   save_mem_ch_f (mem_channel, buffer[state1]);
   quick_disp_flash(2, 3);
}

void mrvfo_handler()
{
   if (state1 == 3) state1 = vfo + 1; else state1 = 3;
   buffer[3] = load_mem_ch_f (mem_channel);
   write8 (state_n, state1);
}

void split_button_handler()
{
   if(state1 != 4)
   {
   if ( ! sl) {cl = 0; sl = 1; } else sl =  0;
   }
}

void vfom_swap_handler(int1 opt)
{
   int8 a, b;
   if(!opt){a = vfo + 1; b = 3;} else {a = 1; b = 2;}
   buffer[0] = buffer[a];
   buffer[a] = buffer[b];
   buffer[b] = buffer[0];
   buffer[0] = 0;
   if(!opt) save_mem_ch_f(mem_channel, buffer[3]);
   quick_disp_flash(1, (int8) vfo + 1);
   quick_disp_flash(1, 3);
}

void micup()
{
   if(state1 < 3)
   {
      int8 temp_inc;
#ifdef include_fine_tuning
      if(fine_tune_display) temp_inc = 1; else temp_inc = 10;
#else
      temp_inc = 10;
#endif
      buffer[state1] += temp_inc;
   }
   if(state1 == 3)
   {
      inc_mem_channel();
   }

}

void micdn()
{

   if(state1 < 3)
   {
      int8 temp_inc;
#ifdef include_fine_tuning
      if(fine_tune_display) temp_inc = 1; else temp_inc = 10;
#else
      temp_inc = 10;
#endif
      buffer[state1] -= temp_inc;
   }
   if(state1 == 3)
   {
      dec_mem_channel();
   }

}



void micfst()
{
   switch (state1)
   {
      case 1: btn_up_handler (1); break;
      case 2: btn_up_handler (2); break;

   }
}

#ifdef include_fine_tuning
void micup_fst(){buffer[state1] += 10;}
void micdn_fst(){buffer[state1] -= 10;}
void micup_hold(){buffer[state1] += 2;}
void micdn_hold(){buffer[state1] -= 2;}
void micup_fst_hold(){buffer[state1] += 10;}
void micdn_fst_hold(){buffer[state1] -= 10;}

#else
void micup_fst(){buffer[state1] += 10;}
void micdn_fst(){buffer[state1] -= 10;}
void micup_hold(){buffer[state1] += 2;}
void micdn_hold(){buffer[state1] -= 2;}
void micup_fst_hold(){buffer[state1] += 10;}
void micdn_fst_hold(){buffer[state1] -= 10;}
#endif

void toggle_fine_tune()
{
#ifdef include_fine_tuning
   if (fine_tune_display) {dial_inc = 2; fine_tune_display = 0; beep_int = 1;} else {dial_inc = 1; fine_tune_display = 1; beep_int = 2;}
   //write8 (fine_tune_n, fine_tune_display);
   errorbeep (beep_int) ;
#endif
}

void reset_eeprom()
{
   write8(checkbyte_n, 0xFF);
   while(true)
   {
   errorbeep(3); 
   delay_ms(1000);
   }
}


void long_press_vfoab(){ reset_eeprom();}
void long_press_dial_lock_ndl(){toggle_fine_tune();}

int8 buttonaction(int8 opt)
{
   int8 report_back = 0;

   SWITCH(opt)
   {
      CASE 1: beep();         clarifier_button_handler();         report_back = 3;  break;
      CASE 2: beep();         btn_dn_handler(0);                  report_back = 2;  break;
      CASE 3: beep();         btn_up_handler(0);                  report_back = 2;  break;
      CASE 4:                 mvfo_handler();                     report_back = 2;  break; //MVFO
      CASE 5: beep();         vfoab_handler();                    report_back = 2;  break; //VFOAB
      CASE 6: beep();         dial_lock_button_handler();         report_back = 3;  break;
      CASE 7:                 vfom_handler();                     report_back = 2;  break; //VFOM
      CASE 8: beep();         mrvfo_handler();                    report_back = 2;  break; //MRVFO
      CASE 9: beep();         split_button_handler();             report_back = 3;  break;
      CASE 10:                vfom_swap_handler(0);                report_back = 2;  break; //VFOM SWAP
      CASE 11:                micup();                            report_back = 1;  break;
      CASE 12:                micdn();                            report_back = 1;  break;
      CASE 13: beep();        micfst();                           report_back = 2;  break;
      CASE 14:                micup_fst();                        report_back = 2;  break;
      CASE 15:                micdn_fst();                        report_back = 2;  break;
      case 16:                micup_hold();                       report_back = 2;  break;
      case 17:                micdn_hold();                       report_back = 2;  break;
//!      case 18:                long_press_fast();                  report_back = 2;  break;
      case 19:                micup_fst_hold();                   report_back = 2;  break;
      case 20:                micdn_fst_hold();                   report_back = 2;  break;
      case 35:                long_press_vfoab();                 report_back = 2;  break;
      case 36:                long_press_dial_lock_ndl();         report_back = 2;  break;
   }

   if(report_back == 3) {dcs = get_dcs(); report_back = 2;}
   
   return report_back;
}

int8 scan_mic_buttons()
{
   int8 res = 0;
   if (!mic_fast&& ! mic_up) res = 11;
   if (!mic_fast&& ! mic_dn) res = 12;
   if (mic_fast&&mic_up&&mic_dn)  res = 13;
   if (mic_fast&& ! mic_up) res = 14;
   if (mic_fast&& ! mic_dn) res = 15;
   return res;
}


#define ondelay 1
int8 scan_buttons()
{
int8 res = 0;
   if((!k8) && (!k4) && (!k2) && (!k1))
   {
         k4 = 0; k8 = 0; k1 = 0; k2 = 1; delay_us(ondelay);
         IF(pb2) res = clarifier_button;//(RESULT: 1)Clarifier
         IF(pb1) res = down_button;//(RESULT: 2)Down
         IF(pb0) res = up_button;//(RESULT: 3)Up
         
         k2 = 0; k4 = 1; delay_us(ondelay);
         IF(pb2) res = mvfo_button;//(RESULT: 4)M > VFO
         IF(pb1) res = vfoab_button;//(RESULT: 5)VFO A / B
         IF(pb0) res = dial_lock_button;//(RESULT: 6)Dial lock
         
         k4 = 0; k8 = 1; delay_us(ondelay);
         IF(pb2) res = vfom_button;//(RESULT: 7)VFO > M
         IF(pb1) res = mrvfo_button;//(RESULT: 8)MR / VFO
         IF(pb0) res = split_button;//(RESULT: 9)SPLIT
         
         k8 = 0; k1 = 1; delay_us(ondelay);
         IF(pb1) res = vfom_swap_button;//(RESULT: 11)VFO < > M
         k8 = 0; k4 = 0; k2 = 0; k1 = 0;
   }
return res;
}

#define countdelay 100
#define holdcount 50
#define micholdcount 30
#define micholdcountfast 100
int8 micdelay = 20;

int8 buttons(INT8 option)
{
   
   STATIC INT8 btnres = 0;
   STATIC INT8 micres = 0;
   STATIC INT8 mic_count = 0;
   INT8 rtnres = 0;
   int8 res = 0;
   int8 debounce = 0;
   
//!   if(option == 2) debounce = 0;
//!   else debounce = 1;
   IF(pb2)
   {
      while(pb2){}
      if(!btn_down)
      {
         if(scan_buttons()) btn_down = 1;
         btn_count= 0;
      }
      
      if(btn_down)
      {
         if(scan_buttons())
         {
            btnres = scan_buttons();
            if(btn_count < 255) ++btn_count;
            if(option != 2) delay_us(countdelay);
         }

         if(btn_count > debounce)
         {
            if(!option)
            {
               if(btn_count < holdcount) res = 1;
               if(btn_count >= holdcount) res = 2;
            }
            
         }
         
         if(res == 1)
         {
            if(!scan_buttons())
            {
               //add some protection for staticy buttons
               for(int i = 0; i < 10; ++i)
               {
                  if(scan_buttons()) break;
               }
               if(i >= 10)
               {
                  if(long_press) {rtnres = 0; long_press = 0;}
                  else rtnres = btnres;
                  btnres = 0;
                  btn_down = 0;
                  return rtnres;
               }
            }
         }
         
         if(res == 2)
         {
               if(btnres) rtnres = btnres + 30;
               btnres = 0;
               btn_count = 0;
               long_press = 1;
               btn_down = 0;
               return rtnres; 
         }
      
      }
      
      if(!mic_down)
      {
         if(scan_mic_buttons()) mic_down = 1;
         mic_count= 0;
      
      }
      
      if(mic_down)
      {
         if(scan_mic_buttons())
         {
            micres = scan_mic_buttons();
            if(mic_count < 255) ++mic_count;
            delay_us(micdelay);
         }
         if(mic_count > debounce)
         {
               switch(micres)
               {
                  default:
                  if(mic_count < micholdcount) res = 1;
                  if(mic_count >= micholdcount) res = 2;
                  break;
                  
                  case 13:
                  if(mic_count < micholdcount) res = 1;
                  if(mic_count >= micholdcountfast) res = 2;
                  break;
               
               }
         }
         
         if(res == 1)
         {
            if(!scan_mic_buttons())
            {
               if(long_press) {long_press = 0; rtnres = 0;}
               else rtnres = micres;
               micres = 0;
               mic_down = 0;
               mic_count = 0;
               return rtnres;
            }
         }
         
         if(res == 2)
         {
               //micres = scan_mic_buttons();
               rtnres = micres + 5;
               if((!scan_mic_buttons()) || (rtnres == 18))
               {
                  mic_down = 0;
                  if(rtnres != 18) rtnres = 0;
                  
               }
               long_press = 1;
               return rtnres; 
         }
         
         
      
      
      }
   }
   
   return 0;
}
