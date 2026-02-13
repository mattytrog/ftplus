#if defined(include_cat_yaesu) && defined (include_cat_kenwood)
#INT_RDA
void  RDA_isr(VOID)
{  

   if(kbhit())
   {
      //cat_com_tmr_start = 1;
      temp_byte = getc();
      buffer[next_in] = temp_byte;
      //if(kbhit()) while(kbhit()){ if(!kbhit()) break;}
      if(next_in < cat_buffer_size) ++next_in; else next_in = 0;

   }
      if (!yaesu_cat_lock) 
      {
         if(temp_byte == 0x3B) {next_in = 0; command_received = 1; cat_mode = 1;}
      }
      
      if ((yaesu_cat_lock) || (cat_mode == 0))
      {
         if(next_in == 5) {next_in = 0; command_received = 1;}
       
      }
   clear_interrupt(INT_RDA);
}

#elif defined (include_cat_yaesu) && !defined (include_cat_kenwood)
#INT_RDA
void  RDA_isr(VOID)
{  
      cat_com_tmr_start = 1;
      cat_mode = 0;
      temp_byte = getc();
      buffer[next_in] = temp_byte;
      while(kbhit()){}
      ++next_in;
     
      if(next_in >= cat_buffer_size) next_in = 0;
      if(next_in == 5) {next_in = 0; command_received = 1;}
   
   clear_interrupt(INT_RDA);
}

#elif !defined (include_cat_yaesu) && defined (include_cat_kenwood)

#INT_RDA
void  RDA_isr(VOID)
{  
   cat_com_tmr_start = 1;
   cat_mode = 1;
   temp_byte = getc();
   buffer[next_in] = temp_byte;
   while(kbhit()){}
   ++next_in;
   
   if(next_in >= cat_buffer_size) next_in = 0;
   if(temp_byte == 0x3B) {next_in = 0; command_received = 1;}
   
   clear_interrupt(INT_RDA);
}
#endif





#ifdef include_cat_kenwood
const char cat_comm[284] =
{

   //1st char, 2nd char, location of terminator, res. eg A is 0, I is 1, 0 or 1 would be 2, terminator at 3, result is function 1.
   'I', 'D', 2, 1, // ID = READ. ANSWER.  Answer (ID006;)
   'A', 'I', 3, 2,  //AI0 = SET. NO ANSWER. IF off. AI1 = SET. ANSWER. IF on. Answer IF
   'D', 'N', 2, 3,  // DN = SET. NO ANSWER. DOWN. No reply. Action up button
   'U', 'P', 2, 4,  // UP = SET. NO ANSWER. UP. No reply. Action down button
   'F', 'A', 2, 5,  // FA = READ. ANSWER. VFOA Answer (FA000) (Freq) (0) (;)
   'F', 'B', 2, 6,  // FB = READ. ANSWER. VFOB Answer (FB000) (Freq) (0) (;)
   'F', 'A', 13, 7, // FA = SET. ANSWER. VFOA Set & Answer (FA000) (freq) (0) (;)
   'F', 'B', 13, 8, // FB = SET. ANSWER. VFOB Set & Answer (FB000) (freq) (0) (;)
   'F', 'N', 3, 9,  // FN = SET. NO ANSWER. (0;) = VFOA, (1;) = VFOB, (2;) = MR,
   'F', 'R', 3, 10,  // FR = SET. ANSWER. (FR) (0;) = VFOA, (1;) = VFOB, Answer FR (VFO)(;)
   'F', 'R', 2, 11,  // FR = READ. ANSWER. (FR) (0;) = VFOA, (1;) = VFOB
   'F', 'T', 3, 12,  // FT = SET. ANSWER. (FT) (0;) = VFOA, (1;) = VFOB, Answer FT (VFO)(;)
   'F', 'T', 2, 13,  // FT = READ. ANSWER. (FT) (0;) = VFOA, (1;) = VFOB
   'I', 'F', 2, 14, // IF = READ. ANSWER. Answer IF
   'I', 'E', 3, 15, // IE = *CUSTOM SET Rig ID
   'L', 'K', 2, 16, // LK = READ. ANSWER. Answer LK0; or LK1; UNLOCK OR LOCK
   'L', 'K', 3, 17, // LK = SET. ANSWER. LK0; Lock off. LK1; Lock on....Answer LK0; / LK1; UNLOCK/LOCK
   'M', 'C', 5, 18, // MC = SET. NO ANSWER. MC Memory channel. (MC)(0)(CH). eg MC002; = mem 2
   'M', 'D', 3, 19, // MD = SET. NO ANSWER. MD; MODE - Fake mode 1 = LSB, 2 = USB, 3 = CW, 4 = FM, 5 = AM, 7 = CWN
   'M', 'R', 6, 20, // MR = READ. ANSWER. (MR) (0) (0) (memch) (;). ANSWER (MR) (0) (0) (mem ch. 2 digits) (000) (Frequency. + 0) (dummy mode) (0) (0) (00) (0) ;
   'M', 'W', 23, 21,// MW = SET. NO ANSWER (MW) (0) (0) (mem ch. 2 digits) (000) (Frequency. + 0) (dummy mode) (0) (0) (00) (0) ;

   'R', 'C', 2, 22, // RC = SET. NO ANSWER. Clarifier offset = 0.
   'R', 'D', 2, 23, // RD = SET. NO ANSWER. Clarifier freq decrease.
   'R', 'U', 2, 24, // RU = SET. NO ANSWER.  Clarifier freq increase.
   'R', 'T', 3, 25, // RT = SET. NO ANSWER. RT0; = Clar off. RT1 = Clar on.
   'R', 'X', 2, 26, // RX = SET. NO ANSWER. RX; mode.
   'T', 'X', 2, 27, // TX = SET. NO ANSWER. TX; mode.
   'S', 'C', 3, 28, // SC = SET. NO ANSWER. SC0; PMS off...SC1; PMS on.
   'S', 'P', 3, 29, // SP = SET. NO ANSWER. SP0; split off...SP1 split on.
   'Y', 'A', 3, 30, // YA = SET. NO ANSWER. Yaesu mode.
   'Y', 'A', 2, 31, // YA = SET. NO ANSWER. Yaesu mode.
   //configurator set section
   'Y', 'B', 3, 32, // Dial normal - accelerated
   'Y', 'B', 2, 33, // Dial type query
   'Y', 'C', 3, 34, // Auto save on/off
   'Y', 'C', 2, 35, // Auto save query
   'Y', 'D', 3, 36, // Fine tune on/off
   'Y', 'D', 2, 37, // Fine tune query
   'Y', 'E', 3, 38, // Offset set
   'Y', 'E', 2, 39, // Offset query
   'Y', 'F', 7, 40, // alignment
   'Y', 'G', 3, 41, // CB Mode/state
   'Y', 'G', 2, 42, // CB Mode/state query
   'Y', 'H', 4, 43, // CB channel
   'Y', 'H', 2, 44, // CB channel query
   'Y', 'I', 3, 45, // +/- band
   'Y', 'J', 3, 46, // CB Band
   'Y', 'J', 2, 47, // CB Band query
   'Y', 'K', 3, 48, // Baud
   'Y', 'K', 2, 49, // Baud query
   'Y', 'L', 3, 50, // display on/off
   'Y', 'L', 2, 51, // display query
   'Y', 'M', 3, 52, // soft PMS set
   'Y', 'M', 2, 53, // soft PMS query
   'Y', 'N', 3, 54, // soft 500k set
   'Y', 'N', 2, 55, // soft 500k query
   'Y', 'O', 3, 56, // offset tx rx
   'Y', 'O', 2, 57, // offset tx rx query
   //buttons
   'Y', 'P', 2, 58, // split
   'Y', 'Q', 2, 59, // mrvfo
   'Y', 'R', 2, 60, // vfom
   'Y', 'S', 2, 61, // dl //similar func above
   'Y', 'T', 2, 62, // vfoab //similar func above
   'Y', 'U', 2, 63, // hidden wideband switch
   'Y', 'V', 2, 64, // mvfo
   'Y', 'W', 2, 65, // btnup
   'Y', 'X', 2, 66, // btndn
   'Y', 'Y', 2, 67, // clar
   'Y', 'Z', 3, 68, // swap
   

          
   
   '1', 'F', 2, 69 // Set button operations
   '3', '0', 2, 70 // Set button operations
   '3', '1', 2, 71 // Set button operations
   
   //34,36,31,37,40,39,35,38,32,33
   //!
};

int8 action_kenwood_cat()
{
   int8 res = 0;
   int8 report_back = 0;
   FOR(int16 i  = 0; i < 100; i++)
      {
         IF((buffer[0]  == cat_comm[(i * 4)])&&(buffer[1] == cat_comm[(i * 4) + 1])&&(buffer[cat_comm[(i * 4) + 2]] == ';'))
         {res = cat_comm[(i * 4) + 3]; BREAK; }
      }
   //if(res== 0) {cat_ans[0] = '?'; cat_ans[1] = ';'; send_cat();}
   
   SWITCH(res)
      {
         //all no answer
      CASE 1:              ID_read();                                   report_back = 2; break; //ID
      //case 2: AI_SWITCH(); break;
      CASE 3:              btn_dn_handler(0);                           report_back = 2; break;
      CASE 4:              btn_up_handler(0);                           report_back = 2; break;
      CASE 5:              FA_read();                                   report_back = 0; break;
      CASE 6:              FB_read();                                   report_back = 0; break;
      CASE 7:              FA_set();                                    report_back = 1; break;
      CASE 8:              FB_set();                                    report_back = 1; break;
      case 9:              mode_SWITCH_kenwood();                       report_back = 2; break; //FN
      CASE 10:             FR_set();                                    report_back = 1; break;
      CASE 11:             FR_read();                                   report_back = 0; break;
      CASE 12:             FT_set();                                    report_back = 1; break;
      CASE 13:             FT_read();                                   report_back = 0; break;
      case 14:             calc_IF(); send_cat(0);                      report_back = 0; break;
      CASE 15:             IE_set();                                    report_back = 2; break;
      CASE 16:             LK_set(1);                                   report_back = 3; break; //LK;
      CASE 17:             LK_set(0);                                   report_back = 3; break; //LK + 0 or 1;
      CASE 18:             MC_set();                                    report_back = 2; break;//MC
      CASE 19:             dummy_mode = (buffer[2]);                    report_back = 0; break; //save8(dummy_mode_n,dummy_mode); break;
      //CASE 20: temp_value = storage_buffer[active_vfo]; break;
      //CASE 21: break; //MW
      //CASE 22: break; //Clear clar freq
      //CASE 23: break; //Clar freq - 1 or 10
      //CASE 24: break; //Clar freq + 1 or 10
      //CASE 25: break; //toggle clar on off
      CASE 26:             cat_tx_request = 0;                              report_back = 0; break; //set rx mode
      CASE 27:             cat_tx_request = 1;                              report_back = 0; break; //set tx mode
      //CASE 28: break; //PMS on / off
      //CASE 29: break; //split on / off
      case 30:             catmode_change(0);                           report_back = 0; break; //set
      case 31:             catmode_change(1);                           report_back = 0; break; //query
      case 32:             dialYB(0);                                   report_back = 0; break; //dial set
      case 33:             dialYB(1);                                   report_back = 0; break; //dial query
      case 34:             autosaveYC(0);                               report_back = 0; break; //autosave
      case 35:             autosaveYC(1);                               report_back = 0; break; //autosave query
      case 36:             finetuneYD(0);                               report_back = 0; break; //finetune
      case 37:             finetuneYD(1);                               report_back = 0; break; //finetune query
      case 38:             offsettypeYE(0);                             report_back = 0; break; //finetune
      case 39:             offsettypeYE(1);                             report_back = 0; break; //finetune query
#ifdef include_enhanced_btn
      case 40:             alignmentYF();                               report_back = 2; break; //alignment       
         //case 36: mantune1F(); report_back = 3; break; //man_tune
#endif

#ifdef include_cb
      case 41:             cbmodeYG(0);                                 report_back = 2; break; //cb mode
      case 42:             cbmodeYG(1);                                 report_back = 2; break; //cb mode
      case 43:             cbchYH(0);                                   report_back = 2; break; //cb ch
      case 44:             cbchYH(1);                                   report_back = 2; break; //cb ch
      case 45:             cbbnd_inc_decYI();                           report_back = 2; break; //cb band
      case 46:             cbbndYJ(0);                                  report_back = 2; break; //cb band
      case 47:             cbbndYJ(0);                                  report_back = 2; break;
#endif
      case 48:             baudYK(0);                                   report_back = 0; break; //baud
      case 49:             baudYK(1);                                   report_back = 0; break; //baud query
      case 50:             dispYL(0);                                   report_back = 2; break; //disp on/off
      case 51:             dispYL(1);                                   report_back = 0; break; //disp query
      case 52:                                                   report_back = pmsYM(0); break; //disp on/off
      case 53:             pmsYM(1);                                    report_back = 0; break; //disp query
      case 54:                                                report_back = sw500kYN(0); break; //disp on/off
      case 55:             sw500kYN(1);                                 report_back = 0; break; //disp query6
      case 56:             offset_swYO(0);                              report_back = 2; break;
      case 57:             offset_swYO(1);                              report_back = 0; break;
      CASE 58: beep();     split_button_handler();                      report_back = 3; break;
      CASE 59: beep();     mrvfo_handler();                             report_back = 2; break;
      CASE 60:             vfom_handler();                              report_back = 2; break;
      CASE 61: beep();     dial_lock_button_handler();                  report_back = 3; break;
      CASE 62: beep();     vfoab_handler();                             report_back = 2; break;
      case 63: beep();     IF( ! gen_tx)gen_tx = 1; else gen_tx = 0;    report_back = 0; break;
      CASE 64:             mvfo_handler();                              report_back = 2; break;
      CASE 65: beep();     btn_up_handler(0);                           report_back = 2; break;
      CASE 66: beep();     btn_dn_handler(0);                           report_back = 2; break;
      CASE 67: beep();     clarifier_button_handler();                  report_back = 3; break;
      CASE 68: beep();     swapYZ();                                    report_back = 2; break;
      //case 69:             heartbeat();                                 report_back = 4; break;
      case 70:             dumpset30();                                 report_back = 4; break;
      case 71:             receiveset31();                              report_back = 4; break;

      }
   return report_back;
}

int1 send_magic_packet()
{
   int8 res = 0;
   set_uart_speed (4800);
   delay_ms(100);
      
      
   cat_ans[0] = 'F';
   cat_ans[1] = 'T';
   cat_ans[2] = 'C';
   cat_ans[3] = 'F';
   cat_ans[4] = 'G';
   cat_ans[5] = ';';
   send_cat(1);

   int8 count = 0;
   while (count < 100)
   {
         if(command_received) res = perform_cat_action();
         delay_ms(2);
         if( res == 4) {break;}
         ++count;
         
   }
   if(res == 4) return 1;
   return 0;
}

#endif

int8 action_yaesu_cat()
{
   
#ifdef include_cat_yaesu
   int8 res = 0;
   INT8 byte5 = buffer[4];
   INT8 byte4_upper = ((buffer[3] >> 4) & 0xF);
   INT8 byte4_lower = buffer[3] & 0xF;
   INT8 byte3_upper = ((buffer[2] >> 4) & 0xF);
   INT8 byte3_lower = buffer[2] & 0xF;
   INT8 byte2_upper = ((buffer[1] >> 4) & 0xF);
   INT8 byte2_lower = buffer[1] & 0xF;
   INT8 byte1_upper = ((buffer[0] >> 4) & 0xF);
   INT8 byte1_lower = buffer[0] & 0xF;
   
   SWITCH(byte5)
   {
      CASE 0x01: split_button_handler(); beep(); res = 3; break;
      CASE 0x02: mrvfo_handler(); res = 2;break;
      CASE 0x03: vfom_handler(); res = 2;break;
      CASE 0x04: dial_lock_button_handler(); beep(); res = 3; break;
      CASE 0x05: vfoab_handler(); res = 2;break;
      CASE 0x06: mvfo_handler(); res = 2;break;
      CASE 0x07: btn_up_handler(0); beep(); res = 2;break;
      CASE 0x08: btn_dn_handler(0); beep(); res = 2;break;
      CASE 0x09: clarifier_button_handler(); beep(); res = 3; break;
      CASE 0x0A: temp_freq = join_value(byte4_lower,byte3_upper,byte3_lower,byte2_upper,byte2_lower,byte1_upper,byte1_lower); storage_buffer[state] = temp_freq; temp_freq = 0; res = 1; break;
      CASE 0x0B: vfom_swap_handler(0); res = 2; break;
      CASE 0x0F: temp_freq = join_value(byte1_upper,byte1_lower,byte2_upper,byte2_lower,byte3_upper,byte3_lower,byte4_upper); storage_buffer[state] = temp_freq; temp_freq = 0; res = 1; break;
      
      case 0xFC: beep(); res = 0; IF( ! gen_tx)gen_tx = 1; else gen_tx = 0; break;
#ifdef include_cb
      case 0xFD: res = 2; IF(gen_tx)toggle_cb_mode(); break;
#endif
      CASE 0xFE: write8 (checkbyte_n, 0xFF); reset_cpu(); break;
      CASE 0xFF: reset_cpu(); break;
    }
    //if((byte5 == 0xFB) || (byte1 == 0xFB)) {beep(); cat_mode = 1; res = 1;}
       return res;
#endif
  
}

int8 perform_cat_action()
{   
   int8 res = 0;
#ifdef include_cat_yaesu
      if(cat_mode == 0) 
      { 
         res = action_yaesu_cat();
         if(res) yaesu_cat_lock = 0;
         
      }
#endif
#ifdef include_cat_kenwood
      if(cat_mode == 1) {res = action_kenwood_cat();}
#endif
      command_received = 0;
      cat_com_tmr_start = 0;
      //while(kbhit()){getc(); delay_us(40);}
      if(res == 3) dcs = get_dcs();  
   return res;
}

int32 bauds[8]=
{1200,2400,4800,9600,19200,38400,57600,115200};

void set_baud_rate(int1 skip)
{
#ifdef adjust_baud_rate
   INT1 upd = 1;
   int1 start_baud = 1;
   INT8 res = 0;
   k4 = 0;
   baud_rate = read8(baud_n);
//!//display_start;
   if(!skip)
   {
      //if(k4 == 1) k4 = 0;
      res1 = read_counter();
      while (true)
      {
         
         //delay_ms (60) ;
         
         
         
         if(upd)
         {
            if((res == mvfo_button) || (res == mrvfo_button)) {errorbeep(3); break;}
         }
         
         if (upd)
         {
            VFD_data (0xFF, 0xFF, bauds[baud_rate - 1], 1,1);
            if(start_baud) {delay_ms(1000); start_baud = 0;}
            upd = 0;
            
         }
         
         if(!upd) upd = misc_dial8(baud_rate,0, 9);
         if(!upd) res = buttons(0);
         if(res)
         {
            if(res == 3) ++baud_rate;
            if(res == 2) --baud_rate;
            upd = 1;
         }
         if(baud_rate < 1) baud_rate = 1;
         else
         if(baud_rate > 8) baud_rate = 8;
   
         
      }
   }
//!   
//!
      switch (baud_rate)
      {
         
         case 1: set_uart_speed (1200); break;
         case 2: set_uart_speed (2400); break;
         case 3: set_uart_speed (4800); break;
         case 4: set_uart_speed (9600); break;
         case 5: set_uart_speed (19200); break;
         case 6: set_uart_speed (38400); break;
         case 7: set_uart_speed (57600); break;
         case 8: set_uart_speed (115200); break;
      }
//!
//!      
      if(!skip)
      {
      write8(baud_n, baud_rate) ;
      }
#endif
}
