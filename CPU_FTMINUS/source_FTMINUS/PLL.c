const int32 band_bank[11] = 
{
   50000, 180000, 350000, 700000, 1010000,
   1400000, 1800000, 2100000, 2400000, 2800000, 3000000
};






//for future app support
void PLL_REF(int16 ref1, ref2)
{
   //PORTB data looks like this, sent to the MC145145 PLLs:
   //0          16          32          48          64          80          96
   //LATCH0     LATCH1      LATCH2      LATCH3      LATCH4      LATCH5      LATCH6
   //NCODEBIT0  NCODEBIT1   NCODEBIT2   NCODEBIT3   REF_BIT0    REF_BIT1    REF_BIT2
   //Each bit is strobed to the PLLs every PLL update, this never changes though, so we only need to send this once, unless it gets overwritten.
   //Should probably add something to ensure it doesn't... Or should we send it every PLL update? I don't see the need...
   //Latch6 address = 96 (or 6<<4)
   //Latch5 address = 80 (or 5<<4)
   //Latch4 Address = 64 (or 4<<4)
   //These are written to latches 6,5 and 4.
   PLL1((6<<4) + (ref1 >> 8));    //Latch6 = Bit2 = address + (5 in hex = 5). Could write this as 96 + 5
   PLL1((5<<4) + ((ref1 >> 4) & 0xF));    //Latch5 = Bit1 = address + (13 in hex = D)or 80 + 13
   PLL1((4<<4) + (ref1 & 0xF));    //Latch4 = Bit0 = address + (12 in hex = C)or 64 + 12
   //5DC in hex = 1500
   PLL2((6<<4) + (ref2 >> 8));     //Latch6 = Bit2 address + (0 in hex = 0)or 96 + 0
   PLL2((5<<4) + ((ref2 >> 4) & 0xF));    //Latch5 = Bit1 address + (1 in hex = 1)or 80 + 1
   PLL2((4<<4) + (ref2 & 0xF));    //Latch4 = Bit0 address + (14 in hex = E)or 64 + 14
   //01E in hex = 30
}


#ifdef include_oob_checking
const int32 blacklist[20]= 
{
   150000, 200000,
   350000, 400000,
   700000, 750000,
   1000000, 1050000,
   1400000, 1450000,
   1800000, 1850000,
   2100000, 2150000,
   2400000, 2550000,
   2800000, 3000000
};
#endif


void set_PLL(int1 force, int8 force_BPF)
{
// We only need to update, if there is a new NCODE to send, ie when new frequency is requested. We don't touch ref latches(6,5,4), so no need to resend
//if PLL update is requested for the SAME frequency, you are sent on your way and bounced back whence you came lol
       //if(calc_frequency > max_freq) calc_frequency = max_freq;
      //oob_check_freq = offset_frequency;
      
      if(temp_freq > max_freq) temp_freq = min_freq;
      if(temp_freq < min_freq) temp_freq = max_freq;
      INT32 offset_frequency = temp_freq;
      STATIC int16 old_khz_freq;
      STATIC int16 old_band_freq;
      STATIC int8 old_PLLband;
      STATIC int8 old_d10h;
      
      int16 tmp_band_freq = 0;
      int16 tmp_khz_freq = 0;
      

      
      if(force)
      {
      old_khz_freq = 0;
      old_band_freq = 0;
      old_PLLband = 0;

      old_d10h = 0;
      }
      
      //The correct filtering is selected by having a value on PORTB (1 - 9), strobed to Q64, which in turn strobes to Q69, which then decodes the value and sends to Q70
      //This is left as an option to be changed in Update_PLL. Default value is 0xFF which is automatic. Or can be forced by replacing with a value (1-9), <actually 0-9>
      //Pointless going higher than 9 because all inputs of Q70 will be high if value >9, leading to no filter being selected and loss of tx/rx
      if(force_BPF == 0xFF)
      {
      
         if(offset_frequency < 9999999) PLLband = 9;
         if(offset_frequency < 2500000) PLLband = 8;
         if(offset_frequency < 2150000) PLLband = 7;
         if(offset_frequency < 1850000) PLLband = 6;
         if(offset_frequency < 1450000) PLLband = 5;
         if(offset_frequency < 1050000) PLLband = 4;
         if(offset_frequency < 750000) PLLband = 3;
         if(offset_frequency < 400000) PLLband = 2;
         if(offset_frequency < 250000) PLLband = 1;
         if(offset_frequency < 150000) PLLband = 1;
         
         if(PLLband != old_PLLband)
         {
         offset_loaded = 0;
         old_d10h = 0;
         PORTB = PLLband; banddata(); 
         old_PLLband = PLLband;
         }
      }
      else
      {
         PLLband = force_BPF;
         if(PLLband != old_PLLband)
         {
         offset_loaded = 0;
         old_d10h = 0;
         PORTB = PLLband; banddata(); 
         old_PLLband = PLLband;
         }
      }

      dmhz1 = 0; dmhz2 = 0; d100k = 0; d10k = 0; d1k = 0; d100h = 0; d10h = 0;
      while(offset_frequency > 999999)    {offset_frequency -= 1000000;  tmp_band_freq += 100;   dmhz1+=1;}
      while(offset_frequency > 99999)     {offset_frequency -= 100000;  tmp_band_freq += 10;    dmhz2+=1;}
      while(offset_frequency > 9999)      {offset_frequency -= 10000;   tmp_band_freq +=1;      tmp_khz_freq += 100; d100k+=1;}
      while(offset_frequency > 999)       {offset_frequency -= 1000;    tmp_khz_freq += 10;     d10k+=1;}
      while(offset_frequency > 99)        {offset_frequency -= 100;     tmp_khz_freq += 1;      d1k+=1;}
      while(offset_frequency > 9)         {offset_frequency -= 10;      d100h+=1;}
      while(offset_frequency > 0)         {offset_frequency -= 1;       d10h+=1;}

      if(tmp_khz_freq >= 500) tmp_khz_freq -=500; 
      tmp_khz_freq +=560;
      
      int8 PLL1_NCODE_L3 = 0; //empty latch 48
      int8 PLL1_NCODE_L2 = 0; //            32
      int8 PLL1_NCODE_L1 = 0; //            16
      int8 PLL1_NCODE_L0 = 0;//              0
      int8 PLL2_NCODE_L3 = 0; //empty latch 48
      int8 PLL2_NCODE_L2 = 0; //empty latch 32
      int8 PLL2_NCODE_L1 = 0; //            16
      int8 PLL2_NCODE_L0 = 0; //             0
      
      if(tmp_khz_freq != old_khz_freq)
      {
      //841 in hex is 349. So latch2 = 3, latch 1 = 4, latch 0 = 9;
      PLL1_NCODE_L2 = (tmp_khz_freq >> 8); //first digit
      PLL1_NCODE_L1 = ((tmp_khz_freq >> 4) & 0xF);//middle digit
      PLL1_NCODE_L0 = (tmp_khz_freq & 0xF);//final digit
      //PLL1 end
      PLL1((3<<4) + PLL1_NCODE_L3);
      PLL1((2<<4) + PLL1_NCODE_L2);
      PLL1((1<<4) + PLL1_NCODE_L1);
      PLL1(0 + PLL1_NCODE_L0);
      old_khz_freq = tmp_khz_freq;
      }

      if(tmp_band_freq != old_band_freq)
      {
         if(PLLband < 6)
         {
         PLL2_NCODE_L1 = (((tmp_band_freq / 5) + 12) >> 4); 
         PLL2_NCODE_L0 = (((tmp_band_freq / 5) + 12) & 0xF);
         }
         else
         {
         PLL2_NCODE_L1 = (((tmp_band_freq / 5) -18) >> 4);
         PLL2_NCODE_L0 = (((tmp_band_freq / 5) -18) & 0xF);
         }
   
          
          
          PLL2((3<<4) + PLL2_NCODE_L3);
          PLL2((2<<4) + PLL2_NCODE_L2); // Prepare latch
          PLL2((1<<4) + PLL2_NCODE_L1);
          PLL2(0 + PLL2_NCODE_L0);
          
          old_band_freq = tmp_band_freq;
       }
       //res2 = d10;
       
       

       if(old_d10h != d10h)
       {
       load_10hz(d10h+1);
       old_d10h = d10h;
       }

       load_100hz(d100h+1);

       
       
       
}

//!int8 calc_band(INT32 frequency)
//!{
//!   for(INT i  = 0; i < 10; i++)
//!   {
//!      IF((frequency >= band_bank[i])&&(frequency < band_bank[i + 1]))break;
//!   }
//!
//!   RETURN i;
//!}


