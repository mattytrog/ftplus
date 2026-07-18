



void overrides()
{

//display flash interrupt
   if(!flash) flash = 1; else flash = 0;
  
   //{if(flash == 1) flash = 0; else flash = 1;}
//CAT virtual 500k interrupts, need to work in any function. PMS is handled in buttons

      if((!hard_500k) && (sw_500k)) {hard_500k = 1; soft_500k = 1;}
      if((hard_500k) && (!sw_500k)) {hard_500k = 0; soft_500k = 0;}



//various timers
   if(general_timer_start) //main loop timer
   {
      switch(gen_tmr_ticked)
      {
         case 0: general_timer+=1; gen_tmr_ticked = 1; break;
         case 1: gen_tmr_ticked = 0; break;
      }
      if(general_timer >=255) general_timer_start = 0;
   }



#ifdef include_cat_kenwood

   
   
#else
if(!dl) default_Q64(0);
if(dl) default_Q64(1);
#endif

}
int1 flag = 0;
#INT_TIMER1
void t1_isr(void)
{
set_timer1(T1_PRELOAD);

if(flag) flag = 0; else flag = 1;
if(flag) overrides();
switch(state)
{
   case 1: vfo = 0; break;
   case 2: vfo = 1; break;
}

clear_interrupt(INT_TIMER1);
}
