#ifdef include_pms_basic

#define VFO_dwell_time 1                  // Delay after tuning to next frequency. Lower numbers = faster scanning speed, though may overshoot if too fast

void pms_scan_basic()
{
   temp_freq = buffer[state1];
   //int32 end_freq = band_bank[PLLband + 1];
   int1 upd_count = 0;
   int8 count = 0;
   int8 res = 0;
   int8 scr = 0;
   int8 old_mem_channel = mem_channel;
   beep();
   while(true)
   {
      if(flash)
      {
         if(count < 255)
         {
            if(!upd_count) {++count; upd_count = 1;}
         }
         else count = 0;
         scr = state1;
      }
         else 
      {
         if(state1 < 3) scr = 0; else scr = state1;
         upd_count = 0;
      }
      VFD_data (scr, temp_freq, 0);
      set_PLL(0, 0xFF);
      if(state1 < 3) delay_us(1); else delay_ms(100);
      while(pb2){}
      if(squelch_open)
         {
            if((!stopped) && (!scan_buttons())) {stopped = 1; count = 0;}
         }
         
      if(!stopped)
      {
         
         if(state1 == 3)
         {
            if(mem_channel < 14) mem_channel +=1; else mem_channel = 0;
            temp_freq = load_mem_ch_f(mem_channel);
         }
         else temp_freq +=2;
         count = 0;
      }
      else
      {
         if(squelch_open) count = 0;
         if(scan_buttons()) {count = 3; stopped = 0;}
         
         
         if(count >= 3)
         {
            if(!squelch_open) stopped = 0;
         }
      }
#ifdef include_cat_yaesu_GX2      
      if(yaesu_command_received)
         {
            res = action_yaesu_cat(); 
            if(res) {temp_freq = buffer[state1];}
         }
#endif
      //if(temp_freq >= max_freq) temp_freq = min_freq;
      IF(!pms)
      {
         beep(); delay_ms(200); break;
      }
      
      
   }

   if(squelch_open) 
   {
      buffer[state1] = temp_freq;
   } else mem_channel = old_mem_channel;

}

#endif
