
void beep();
void overrides()
{
//!   if(temp_freq > max_freq) temp_freq = min_freq;
//!   if(temp_freq < min_freq) temp_freq = max_freq;
   
   if((fl_cnt) & 8)
   {
      if(!flash_ticked) {flash_ticked = 1; fl_cnt_gearing +=1;}
   }
   else
   {
      if(flash_ticked) {flash_ticked = 0;}
      if(fl_cnt_gearing >= 5)
      {
         if(flash) flash = 0; else flash = 1;
         fl_cnt_gearing = 0;
      }
   }
   
   
   if((fl_cnt) & 4)
   {
      if((!hard_500k) && (sw_500k)) {hard_500k = 1; soft_500k = 1;}
      if((hard_500k) && (!sw_500k)) {hard_500k = 0; soft_500k = 0;}
      
   }
   

if ((fl_cnt) & 3)
   {
      if(tx_mode) transmitting = 1;
      if(cat_tx_request) transmitting = 1;
      
      if(transmitting)
      {
         if(valid)
         {
            if(cat_tx_request) default_Q64(2);
            else
            {
               switch(dl)
               {
                  case 0: default_Q64(0); break;
                  case 1: default_Q64(1); break;
               }
            }
         }
         
         if(!valid) 
         {
            default_Q64(3);
         }
         if((!cat_tx_request) && (!tx_mode)) transmitting = 0;
      }

   }     
   
   if(btntimerstart)
   {
      if(((fl_cnt) & 8) && (!btntimerticked)) 
      {
         if(btntimer < 255) btntimer +=1; else btntimerstart = 0;
         btntimerticked = 1;}
      else btntimerticked = 0;
   }
   
   if(timerstart)
   {
      if(((fl_cnt) & 8) && (!timerticked)) 
      {
         if(timer < 255) timer +=1; else timerstart = 0;
         timerticked = 1;}
      else timerticked = 0;
   }

   
      
      IF((sw_pms) && (!sw_pms_pressed))
      {
         sw_pms_pressed = 1;
         if(pms == 1) {soft_pms = 0; pms = 0;} else {soft_pms = 1; pms = 1;}
      }
      if((!sw_pms) && (sw_pms_pressed)) sw_pms_pressed = 0;


}

#INT_TIMER1
void t1_isr(void)
{
if(fl_cnt < 255) ++fl_cnt; else fl_cnt = 0;
//reset_counter();
overrides();
set_timer1(tmr1_preload);
clear_interrupt(INT_TIMER1);
}

