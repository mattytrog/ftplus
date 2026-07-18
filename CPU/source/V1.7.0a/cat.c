

#ifdef include_cat_kenwood

char cat_ans[24] = 
{
   '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'
};
//char* cat_ptr = &cat_ans[];

//38 char IF buffer. For Kenwood information request. Elements are swapped as needed
char ifbuf[38] = 
{
   'I', 'F', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
   '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', ';'
};

char idbuf[5] = 
{
   'I', 'D', '0', '0', '6'
};



void calc_int32_buf(unsigned int32 val, int8 loc, int8 buffer)
{
   int8 c1 = 0,c2 = 0,c3 = 0,c4 = 0,c5 = 0, c6 = 0, c7 = 0;
   
   split_value(val, c1,c2,c3,c4,c5,c6,c7);
   
   if(!buffer)
   {
      cat_ans[loc] = 48 + c1;
      cat_ans[loc+1] = 48 + c2;
      cat_ans[loc+2] = 48 + c3;
      cat_ans[loc+3] = 48 + c4;
      cat_ans[loc+4] = 48 + c5;
      cat_ans[loc+5] = 48 + c6;
      cat_ans[loc+6] = 48 + c7;
   }
   if(buffer == 1) 
   {
      ifbuf[loc] = 48 + c1;
      ifbuf[loc+1] = 48 + c2;
      ifbuf[loc+2] = 48 + c3;
      ifbuf[loc+3] = 48 + c4;
      ifbuf[loc+4] = 48 + c5;
      ifbuf[loc+5] = 48 + c6;
      ifbuf[loc+6] = 48 + c7;
   }
   if(buffer == 2)
   {
      ifbuf[loc] = 48 + c4; ifbuf[loc+1] = 48 + c5; ifbuf[loc+2] = 48 + c6; ifbuf[loc+3] = 48 + c7;
   }
}


void calc_fx_freq(unsigned int32 tmp_val) {calc_int32_buf(tmp_val, 5, 0);}
void calc_if_freq(unsigned int32 tmp_val) {calc_int32_buf(tmp_val, 5, 1); tuned = 1;}
   
int32 cat_set_freq(INT8 base)
{
   cat_ans[base] = buffer[5];
   cat_ans[base + 1] = buffer[6];
   cat_ans[base + 2] = buffer[7];
   cat_ans[base + 3] = buffer[8];
   cat_ans[base + 4] = buffer[9];
   cat_ans[base + 5] = buffer[10];
   cat_ans[base + 6] = buffer[11];
   
   temp_freq = join_value(cat_ans[base] - 48,cat_ans[base + 1] - 48,cat_ans[base + 2] - 48,
   cat_ans[base + 3] - 48,cat_ans[base + 4] - 48,cat_ans[base + 5] - 48,cat_ans[base + 6] - 48);
   
   if((temp_freq < min_freq) || (temp_freq > max_freq)) {temp_freq = storage_buffer[state];}
   RETURN temp_freq;
}

VOID send_cat(int8 buf)
{
   //display_stop;
   int8 tmp_char;
   

      for (INT i = 0; i < 255; i++)
      {
         if(!buf) {tmp_char = ifbuf[i];}// ifbuf[i] = '0';}
         if(buf == 1) {tmp_char = cat_ans[i];}// cat_ans[i] = '0';}
         if(buf == 2) {tmp_char = read_ram_eeprom(i);}//
         putchar(tmp_char);
         //if(!buf) ifbuf[i] = '0';
         if(buf == 1) cat_ans[i] = '0';
         IF(tmp_char == ';') break;
      }
      if(buf == 2) putchar(';');
      
   //display_start;
   
}


void calc_ans(char F, char AB, unsigned int32 val)
{
   
   calc_int32_buf(val, 2, 0);
   cat_ans[0] = F;
   cat_ans[1] = AB;
   cat_ans[9] = ';';
   send_cat(1);

}


VOID ID_read()
{
   IF(id_enable)
   {
      cat_ans[0] = idbuf[0];
      cat_ans[1] = idbuf[1];
      cat_ans[2] = idbuf[2];
      cat_ans[3] = idbuf[3];
      cat_ans[4] = idbuf[4];
      cat_ans[5] = ';';
      send_cat(1);
   }
}

void FA_read()
{
   switch(state)
   {
      default: calc_fx_freq(storage_buffer[state]); break;
      case 2: calc_fx_freq(storage_buffer[1]); break;
   }
   
   cat_ans[0] = 'F';  cat_ans[1] = 'A'; cat_ans[13] = ';'; 
   send_cat(1);
}

void FB_read()
{
   switch(state)
   {
      default: calc_fx_freq(storage_buffer[state]); break;
      case 1: calc_fx_freq(storage_buffer[2]); break;
   }
   cat_ans[0] = 'F';  cat_ans[1] = 'B'; cat_ans[13] = ';';
   send_cat(1);

}
void FA_set()
{
   switch(state)
   {
      default: storage_buffer[state] = cat_set_freq(5); break;
      case 2: storage_buffer[1] = cat_set_freq(5); break;
   }

   FA_read();
}
void FB_set()
{
   switch(state)
   {
      default: storage_buffer[state] = cat_set_freq(5); break;
      case 1: storage_buffer[2] = cat_set_freq(5); break;
   }

   FB_read();
}

void mode_SWITCH_kenwood()
{
   switch(buffer[2])
      {
         default: state = (int8)vfo + 1; break;
         case '0': state = 1; break;
         case '1': state = 2; break;
         case '2': state = 3; break;
         case '3': state = 4; break;
         case '4': if(state == 4) {state = old_state; old_state = 0;} else state = 4; break;
      }
   
   write8(state_n, state);
}

void FR_read()
{

   switch(state)
   {
      case 1: cat_ans[2] = '0'; break;
      case 2: cat_ans[2] = '1'; break;
   }
   calc_ans('F','R', cat_ans[2]);
}


void FT_read()
{
   switch(state)
   {
      case 1: cat_ans[2] = '0'; break;
      case 2: cat_ans[2] = '1'; break;
   }
   calc_ans('F','T', cat_ans[2]);
}

void FR_set()
{

   switch(buffer[2])
   {
      case '0': state = 1; break;
      case '1': state = 2; break;
   }
   write8(state_n, state);
   FR_read();
}

void FT_set()
{
   

   switch(buffer[2])
   {
      case '0': state = 1; break;
      case '1': state = 2; break;
   }
   write8(state_n, state);
   FT_read();
}

VOID calc_if()
{
   
   ifbuf[0] = 'I';
   ifbuf[1] = 'F';
   

         switch(state)
         {
            default: calc_if_freq(storage_buffer[state]); break;
            case 2: calc_if_freq(storage_buffer[1]); break;
         }

      temp_offset2 = offset;
      if(!temp_offset2) ifbuf[18] = '0';
      else
      {
      if(temp_offset2 > 10000) {temp_offset2 -= 10000; ifbuf[18] = '+';} else ifbuf[18] = '-';
      }
      calc_int32_buf(temp_offset2, 19, 2);
   IF(mem_channel > 9)
   {
      ifbuf[26] = 48 + 1; //Mem CH first digit(eg 0)in ASCII not HEX
      ifbuf[27] = 48 + (mem_channel - 10); //Mem CH second digit(eg 8)in ASCII not HEX
   }

   ELSE
   {
      ifbuf[26] = 48; //Mem CH first digit(eg 0)in ASCII not HEX
      ifbuf[27] = 48 + (mem_channel); //Mem CH second digit(eg 8)in ASCII not HEX
   }

      switch(transmitting)
      {
         case 0: ifbuf[28] = ('0'); break;
         case 1: ifbuf[28] = ('1'); break;
      }

   
   ifbuf[29] = dummy_mode; //Mode 1 = LSB, 2 = USB, 3 = CW, 4 = FM, 5 = AM, 6 = FSK, 7 = CWN. All dummy values. Should reflect mode change in application
   switch(state)
   {
      case 1: ifbuf[30] = ('0'); ifbuf[32] = ('0'); break;
      case 2: ifbuf[30] = ('1'); ifbuf[32] = ('1'); break;
      case 3: ifbuf[30] = ('2'); ifbuf[32] = ('2'); break;
      //case 4: ifbuf[30] = ('3'); ifbuf[32] = ('3'); break;
   }
   switch(pms)
      {
         case 0: ifbuf[31] = ('0'); break;
         case 1: ifbuf[31] = ('1'); break;
      }
   switch(sl)
   {
      case 0: ifbuf[32] = ('0'); break;
      case 1: ifbuf[32] = ('1'); break;
   }
}

VOID IE_set()
{
   switch(buffer[2])
   {
      case '0': id_enable = 0; break;
      case '1': id_enable = 1;break;
   }
   calc_ans('I','E',id_enable);
   write8(id_enable_n,id_enable);
   beep();
}

VOID LK_set(int8 opt)
{

   if(!opt)
   {
   switch(buffer[2])
      {
         case '0': dl = 0; break;
         case '1': dl = 1;break;
         case '2': if(dl) {beep(); dl = 0;} else {errorbeep(2); dl = 1;} 
      }
   }
   calc_ans('L','K', dl);
   
}

void MC_set()
{
//!   temp_val = (buffer[3] - 48) * 10;
//!   temp_val += (buffer[4] - 48);
//!   
//!   if((temp_val >= 0) && (temp_val <= 14))
//!   {
//!   mem_channel = temp_val;
//!   temp_val = 0;
//!   state = 3;
//!   storage_buffer[3] = load_mem_ch_f (mem_channel);
//!   write8 (state_n, state);   
//!   write8(mem_ch_n,mem_channel);
//!   }
}  

void catmode_change(int1 opt)
{
   if(!opt)
   {
      switch(buffer[2])
      {
         case '0': yaesu_cat_lock = 1; cat_mode = 0; break;
         case '1': yaesu_cat_lock = 0; cat_mode = 1; break;
      }
   }
   calc_ans('Y','A', cat_mode);

}

void dialYB(int8 opt) 
{
   if(!opt)
   {
      switch(buffer[2])
      {
         case '0': speed_dial = 0; break;
         case '1': speed_dial = 1;break;
         case '2': if(speed_dial) {beep(); speed_dial = 0;} else {errorbeep(2); speed_dial = 1;} break;
      }
      write8(dial_n, speed_dial);
   }
   calc_ans('Y','B', speed_dial);
   
}

void autosaveYC(int8 opt) 
{
   if(!opt)
   {
      switch(buffer[2])
      {
         case '0': autosave = 0;break;
         case '1': autosave = 1;break;
         case '2': if(autosave) {beep(); autosave = 0;} else {errorbeep(2); autosave = 1;} break;
      }
      write8(savetimer_n, autosave);
   }
   calc_ans('Y','C', autosave);
   
}

void finetuneYD(int8 opt)
{
   if(!opt)
   {
      switch(buffer[2])
      {
         case '0': fine_tune_display = 0;break;
         case '1': fine_tune_display = 1;break;
         case '2': if(fine_tune_display) {beep(); fine_tune_display = 0;} else {errorbeep(2); fine_tune_display = 1;} break;
      }
      write8(fine_tune_n, fine_tune_display);
   }
   calc_ans('Y','D', fine_tune_display);
   
}

void offsettypeYE(int8 opt)
{
   if(!opt)
   {
      switch(buffer[2])
      {
         case '0': per_band_offset = 0;break;
         case '1': per_band_offset = 1;break;
         case '2': if(per_band_offset) {beep(); per_band_offset = 0;} else {errorbeep(2); per_band_offset = 1;} break;
      }
      write8(band_offset_n, per_band_offset);
   }   
   calc_ans('Y','E', per_band_offset);
   
}



#ifdef include_enhanced_btn   
void alignmentYF() 
{
   
   buffer[0] = 48; buffer[1] = 48;
   int16 temp_val = join_value(buffer[0] - 48,buffer[1] - 48,buffer[2] - 48,
   buffer[3] - 48,buffer[4] - 48,buffer[5] - 48,buffer[6] - 48);
   program_offset(temp_val);
   calc_ans('Y','F', temp_val);
}
#endif
   
#ifdef include_cb
void cbmodeYG(int1 opt)
{
   if(!opt)
   {
      if(!old_state) 
      {
         old_state = state;
         if(old_state == 4) old_state = (int8)vfo + 1;
      }
      switch(buffer[2])
      {
         default: state = (int8)vfo + 1; break;
         case '1': state = 1; break;
         case '2': state = 2; break;
         case '3': state = 3; break;
         case '4': state = 4; break;
         case '5': if(state == 4) {state = old_state; old_state = 0;} else state = 4; break;
      }
   }
   cl = 0; sl = 0;
   dcs = get_dcs();
   calc_ans('Y','G', state);
}

void cbchYH(int1 opt) 
{
   int8 temp_val;
   if(!opt)
   {
      if(buffer[3] == ';') {errorbeep(3); return;}
      temp_val = (buffer[2] - 48) * 10;
      temp_val += (buffer[3] - 48);
      
      if((temp_val > 0) && (temp_val < 41))
      {
      cb_channel = temp_val;
      temp_val = 0;
      state = 4;
      write8(cb_ch_n,cb_channel);
      load_cb_state(0);
      }
   }
   calc_ans('Y', 'H', cb_channel);
   
}

void cbbnd_inc_decYI()
{
   switch(buffer[2])
   {
      case '0': if(cb_region > 0) --cb_region; else cb_region = 7; break;
      case '1': if(cb_region < 7) ++cb_region; else cb_region = 0; break;
   }
   state = 4;
   write8(cb_reg_n,cb_region);
   load_cb_state(0);
}

void cbbndYJ(int1 opt) 
{
   if(!opt)
   {
      if(((buffer[2] - 48) > 0) && ((buffer[2] - 48) < 9))
      {
      cb_region = buffer[2] - 49;
      state = 4;
      write8(cb_reg_n,cb_region);
      load_cb_state(0);
      }
   }
   calc_ans('Y', 'J', cb_region+1);
   
}
#endif
void baudYK(int1 opt)
{
   if(!opt)
   {
      
      if(((buffer[2] - 48) > 0) && ((buffer[2] - 48) < 9))
      {
      baud_rate = buffer[2] - 48;
      write8(baud_n, baud_rate);
      }
   }
   calc_ans('Y', 'K', baud_rate);
}

void dispYL(int8 opt)
{
   if(!opt)
   {
      switch(buffer[2])
      {
         case '0': display_on = 0;break;
         case '1': display_on = 1;break;
         case '2': if(display_on) {beep(); display_on = 0;} else {errorbeep(2); display_on = 1;} break;
      }
      write8(display_on_n, display_on);
   }   
   calc_ans('Y','L', display_on);
}



int8 pmsYM(int8 opt)
{
   int8 res = 0;
   if(!opt)
   {
      switch(buffer[2])
      {
         case '0': soft_pms = 0; pms = 0; res = 4; break;
         case '1': soft_pms = 1; pms = 1; res = 5; break;
         case '2': if(pms)
                     {
                     beep();
                     soft_pms = 0; pms = 0; res = 4;
                     } 
                     else 
                     {
                     errorbeep(2); 
                     soft_pms = 1; pms = 1; res = 5;
                     }
                     break;
      }
      //write8(display_on_n, display_on);
   }   
   calc_ans('Y','M', soft_pms);
   return res;
}

int8 sw500kYN(int8 opt)
{
   int8 res = 0;
   if(!opt)
   {
      switch(buffer[2])
      {
         case '0': soft_500k = 0; res = 6; break;
         case '1': soft_500k = 1; res = 7; break;
         case '2': if(soft_500k) {beep(); soft_500k = 0;res = 6;} else {errorbeep(2); soft_500k = 1;res = 7;} break;
      }
      //write8(display_on_n, display_on);
   }   
   calc_ans('Y','N', soft_500k);
   return res;
}

void offset_swYO(int1 opt)
{
#ifdef include_offset_programming
      if(!opt)
      {
         switch(buffer[2])
         {
            case '0': txrx_offset_only = 0;break;
            case '1': txrx_offset_only = 1;break;
            case '2': txrx_offset_only = 2;break;
            case '3': txrx_offset_only = 3;break;
         }
         if(txrx_offset_only == 0) {tx_offset_only = 0; rx_offset_only = 0;}
         if(txrx_offset_only == 1) {tx_offset_only = 1; rx_offset_only = 0;}
         if(txrx_offset_only == 2) {tx_offset_only = 0; rx_offset_only = 1;}
         if(txrx_offset_only == 3) {tx_offset_only = 1; rx_offset_only = 1;}
         write8(txrx_offset_only_n, txrx_offset_only);
      }
      calc_ans('Y','O', txrx_offset_only);
#endif  
}

void swapYZ()
{
   switch(buffer[2])
      {
         case '0': vfom_swap_handler(0); break;
         case '1': vfom_swap_handler(1); break;
      }

}

void dumpset30()
{
   send_cat(2);
}

void load_values();

void receiveset31()
{
   disable_interrupts(INT_RDA);
   int i = 0;
   while(true)
   {
      if(kbhit()) 
      {
      write_ram_eeprom(i, getc());
      if(read_ram_eeprom(i) == 0x3B) {break;} 
      ++i;
      }
   }
   int j = 0;
   while(j<i)
   {
     write8(j, read_ram_eeprom(j));
     delay_us(20);
     if(read8(j) == read_ram_eeprom(j)) ++j; else {break;}
   }
   enable_interrupts(INT_RDA);
   beep();
   load_values();
}


#else
void dumpset30(){}
void receiveset31(){}
#endif
