

int8 misc_dial8(int8 &value, int8 direction, int8 gearing)
{
   static int8 temp_count = 0;
   res1 = read_counter();
   if(res1 == res2) return 0;
   else
   {
      temp_count +=1;
      res2 = res1;
      if(temp_count > gearing)
      {
         
         if(!direction)
         {
            if(dial_dir) value+=1; else value-=1;
         }  
         else
         {
            if(dial_dir) value-=1; else value+=1;
         }
         temp_count = 0;
         return 1;
      }
      return 0;
   }
}



int8 misc_dial(int16 &value, int8 direction)
{
   res1 = read_counter();
   if(res1 == res2) return 0;
   else
   {
      if(!direction)
      {
         if(dial_dir) value+=1; else value-=1;
      }  
      else
      {
         if(dial_dir) value-=1; else value+=1;
      }
      return 1;
   } 
   return 0;
}

int8 dial_moved()
{
   res1 = read_counter();
   if(res1 == res2) return 0;
   else 
   {
      res2 = res1; 
      if(dial_dir) return 2; else return 1;
      }
}



#ifdef include_dial_support

int8 freq_dial_basic(int32 &value, int8 dial_increment)
{
   if(!dl)
   {
      res1 = read_counter();
      if(res1 == res2) return 0;
      else
      {
            if(dial_dir) value+=dial_increment;
            else value-=dial_increment;
            return 1;
      }
   }
}

#endif

#ifdef include_dial_accel



int8 freq_dial_accel(int32 &value, int8 start_increment)
{
   
   
   int1 res = 0;
   int8 report_back = 0;
   static int8 count = 0;
   static int8 stopcount = 0;  
   static int1 startcount = 0;
   dial_increment = start_increment;
   
   res1 = read_counter();
   if(res1 != res2)
   {
      res = 1;
      startcount = 1;
      stopcount = 0;
   }
   
   if(startcount)
   {
      if(count < interval_count) ++count; else count = 0; 
   }
   
   
   if(res)
   {

   if((dial_timer > 0) && (dial_timer < speed1)) {dial_increment = start_increment;}
   else if((dial_timer >= speed1) && (dial_timer < speed2)) {dial_increment = increment_speed1;}
   else if((dial_timer >= speed2) && (dial_timer < speed3)) {dial_increment = increment_speed2;}
   else if((dial_timer >= speed3) && (dial_timer < speed4)) {dial_increment = increment_speed3;}
   else if((dial_timer >= speed4) && (dial_timer < speed5)) {dial_increment = increment_speed4;}
   else if((dial_timer >= speed5) && (dial_timer < speed6)) {dial_increment = increment_speed5;}
   else if(dial_timer >= speed6) {dial_increment = increment_speed6;}
   if(dial_dir) value +=dial_increment; else value -=dial_increment;
   if(dial_increment <= 2) report_back = 1; else report_back = 2;
   }
   
  
   if(res == 0)
   {  
      if((count == interval_sample1) || (count == interval_sample2))
      {
      if(dial_timer > dial_timer_pullback) dial_timer -= dial_timer_pullback;
      if(stopcount < 5) ++stopcount;
      
      }
   }
   if(res == 1)
   {
      if(dial_timer < (dial_timer_max - dial_timer_increment)) dial_timer += dial_timer_increment;
   }
  
  
  
  if(stopcount >= 5)
  {
      dial_increment = start_increment;
      dial_timer = 0;
      count = 0;
      report_back = 0;
      startcount = 0;
  
  }
  
  //if(report_back) VFD_data (state,dcs, dial_increment, mem_channel, 1);  
return report_back;




}     

#endif


