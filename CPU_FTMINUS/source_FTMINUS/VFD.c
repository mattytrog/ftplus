char disp_buf[13] = {10,0,15,15,15,15,15,15,15,15,15,15,15};







void send_disp()
{
   static int8 cycles_to_go = disp_latches;
   static int1 not_added = 0;
   static int8 i = 0;
   if(!pb2) not_added = 1;
   if((pb2) && (cycles_to_go > 0) && (not_added))
   {
      not_added = 0;
      --cycles_to_go;
   }
   if(!cycles_to_go)
      {
         k8 = (disp_buf[i])&0xF; k4 = (disp_buf[i] >> 1)&0xF; k2 = (disp_buf[i] >> 2)&0xF; k1 = (disp_buf[i] >> 3)&0xF;
         if(pb2) disp_INT = 1;
         int8 j = 0;
         while(j < display_interrupt_hold)
         {
            ++j; delay_us(1); if(!pb2) break;
         }
         k8 = 0; k4 = 0; k2 = 0; k1 = 0;
         j = 0; disp_INT = 0;
         if(i < 13) i+=1;
         if(!pb2) {i = 0; cycles_to_go = disp_latches;}
      }
      else {k8 = 0; k4 = 0; k2 = 0; k1 = 0;}
}



void VFD_info_segments(int8 state)
{
   int8 d,v,c;
   switch(state)
      {
         case 1: d = dcs; v = 1; c = 15; break;
         case 2: d = dcs; v = 12; c= 15; break;
         case 3: d = dcs; v = 2, c = mem_channel; break;
         default: d = 15; v = 15; c = 15; break;
      }
   
   disp_buf[4] = v;
   disp_buf[5] = d;
   disp_buf[12] = c;

}

void VFD_freq(unsigned int32 value, int8 disp_type)
{
//!      if(!disp_type)
//!      {
//!      if(!old_value) old_value = value;
//!      else if((value > (old_value + 9)) || (value < (old_value - 9))) old_value = value; else return;
//!      }
   
   
   int8 d3,d4,d5,d6,d7,d8,d9;
   
   split_value (value, d3, d4, d5, d6, d7, d8, d9,0);
   

   switch(disp_type)
   {
     default: disp_buf[6] = d3; disp_buf[7] = d4; disp_buf[8] = d5; disp_buf[9] = d6; disp_buf[10] = d7; disp_buf[11] = d8; break;
      case 1: disp_buf[6] = d4; disp_buf[7] = d5; disp_buf[8] = d6; disp_buf[9] = d7; disp_buf[10] = d8; disp_buf[11] = d9; break;
   }
}

void VFD_data(INT8 disp_state, unsigned int32 value, int8 disp_type)
{

      VFD_info_segments(disp_state);
      VFD_freq(value, disp_type);
}

void quick_disp_flash(int8 beeps, int8 st)
{


   
   for (INT8 i = 0; i < beeps; ++i)
   {
      strobe_Q64(4);
      switch(st)
      {
         case 1: disp_buf[4] = 1; disp_buf[12] = 15; break;
         case 2: disp_buf[4] = 12; disp_buf[12] = 15; break;
         case 3: disp_buf[4] = 2; disp_buf[12] = mem_channel; break;
      }
      delay_ms(100); 
      disp_buf[12] = 15; disp_buf[4] = 15; delay_ms(100);
   }


}

#INT_TIMER2
void t2_isr(void)
{
   if(!display_on)
   {
      disp_buf[4] = 15;
      disp_buf[5] = 15;
      disp_buf[6] = 15;
      disp_buf[7] = 15;
      disp_buf[8] = 15;
      disp_buf[9] = 15;
      disp_buf[10] = 15;
      disp_buf[11] = 15;
      disp_buf[12] = 15;
   }
send_disp();
clear_interrupt(INT_TIMER2);
}



