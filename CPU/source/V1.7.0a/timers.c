void overrides()
{

//display flash interrupt
   if((fl_cnt) & 8) flash = 1; else flash = 0;
   
   //{if(flash == 1) flash = 0; else flash = 1;}
//CAT virtual 500k interrupts, need to work in any function. PMS is handled in buttons
   if((fl_cnt) & 4)
   {
      if((!hard_500k) && (sw_500k)) {hard_500k = 1; soft_500k = 1;}
      if((hard_500k) && (!sw_500k)) {hard_500k = 0; soft_500k = 0;}
   }


//various timers
   if(general_timer_start) //main loop timer
   {
      switch(gen_tmr_ticked)
      {
         case 0: if ((fl_cnt) & 8) {general_timer+=1; gen_tmr_ticked = 1;} break;
         case 1: if ((fl_cnt) & 8) {gen_tmr_ticked = 0;} break;
      }
      if(general_timer >=255) general_timer_start = 0;
   }
#ifdef include_cat_kenwood
   if(tx_timer_start)
   {  
      switch(tx_ticked)
      {
         case 0: if ((fl_cnt) & 8) {cat_tx_timer+=1; tx_ticked = 1;} break;
         case 1: if ((fl_cnt) & 8) {tx_ticked = 0;} break;
      }
      if(cat_tx_timer >=255) cat_tx_timer = 0;
      if(cat_tx_long_timer >=255) cat_tx_long_timer = 0;
      
      if(cat_tx_timer >= 7) {++cat_tx_long_timer; cat_tx_timer = 0;}
      
      if(cat_tx_timeout)
      {
         if(cat_tx_long_timer >= cat_tx_timeout) {tx_timer_start = 0; cat_tx_timer = 0; cat_tx_long_timer = 0; cat_tx_request = 0;}
      }
   }
#endif
   
   if((cat_com_tmr_start) && (cat_com_tmr_enable))
   {
      switch(cat_com_tmr_ticked)
      {
         case 0: if ((fl_cnt) & cat_command_timeout_prescale) {cat_command_timer+=1; cat_com_tmr_ticked = 1;} break;
         case 1: if ((fl_cnt) & cat_command_timeout_prescale) {cat_command_timer+=1; cat_com_tmr_ticked = 0;} break;
      }
      if(cat_command_timer >=255) cat_com_tmr_start = 0;
      
      if(cat_command_timer > cat_command_timeout) 
      {
         if(kbhit()) {getc();while(kbhit()){}} else {cat_command_timer = 0; cat_com_tmr_start = 0;}
         next_in = 0; command_received = 0;
      }
   } else {cat_com_tmr_start = 0; cat_command_timer = 0;}

#ifdef include_cat_kenwood
if ((fl_cnt) & 3)
   {
      //if(tx_mode) mic_tx_request = 1;
      
      if(valid)
      {
         
         if(cat_tx_request)
         {
            default_Q64(2);
            if(!tx_mode) while(!tx_mode){}
            transmitting = 1;  
         }
         if(!cat_tx_request)
         {
            
            switch(dl)
            {
               case 0: default_Q64(0); transmitting = 0; break;
               case 1: default_Q64(1); transmitting = 0; break;
            }
            if(tx_mode) transmitting = 1;
         }
      }
      
      if(!valid)
      {
         if((tx_mode) && !cat_tx_request) {default_Q64(3); transmitting = 1;}
         if(cat_tx_request)
         {
            default_Q64(3);
            transmitting = 1;  
         }
         if(!cat_tx_request)
         {
            switch(dl)
            {
               case 0: default_Q64(0); transmitting = 0; break;
               case 1: default_Q64(1); transmitting = 0; break;
            }
            if(tx_mode) transmitting = 1;
         }
      }
     
      
     
   }     
//!
#else
if(!dl) default_Q64(0);
if(dl) default_Q64(1);
#endif

}
