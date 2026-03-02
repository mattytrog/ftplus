#INT_RDA
void  RDA_isr(VOID)
{  
      //cat_com_tmr_start = 1;
      temp_byte = getc();
      cat_buffer[next_in] = temp_byte;
      //while(kbhit()){}
      ++next_in;
     
      if(next_in >= cat_buffer_size) next_in = 0;
      if(next_in == 5) {next_in = 0; yaesu_command_received = 1;}
   
   clear_interrupt(INT_RDA);
}




//BCD converters and decoders
//converts BCD (least significant digit first) to unsigned int32
//eg decoded_freq = convert_from_BCD(67, 45, 23, 01);
//   decoded_freq = 0123456

unsigned int32 convert_from_BCD( unsigned int8 c1, unsigned int8 c2, unsigned int8 c3, unsigned int8 c4)
{
   return join_value((c4 & 0xF), ((c3 >> 4) & 0xF), (c3 & 0xF), ((c2 >> 4) & 0xF), (c2 & 0xF), ((c1 >> 4) & 0xF), (c1 & 0xF));
}

//converts unsigned int32 to BCD(least significant digit first). Unsigned int8 are outputted
// eg convert_to_BCD(01234567);
// LSD = digit0(67), digit1(45), digit2(23), digit3(01)
# separate
void convert_to_BCD(unsigned int32 value, unsigned int8 &c1, unsigned int8 &c2, unsigned int8 &c3, unsigned int8 &c4)
{
   int8 b0 = 0, b1 =0,b2 = 0,b3 = 0,b4 = 0,b5 = 0,b6 = 0,b7 = 0;
   split_value(value, b1,b2,b3,b4,b5,b6,b7,1); //split 32 bit to 7x8bit
   c1 = ((b6 << 4) + b7); //(x10 << 4) + x1
   c2 = ((b4 << 4) + b5); //(x1000 << 4) + x100
   c3 = ((b2 << 4) + b3); //(x100000 << 4) + x10000
   c4 = ((b0 << 4) + b1); //(x1000000) - Never over 9, so can leave unaltered. Never over 3 actually
}

#ifdef include_cat_yaesu_GX2

unsigned int8 cat_tx_buf[75] = 
{0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0};

unsigned int8 stat_flags()
{

   if(dl) fdl = 1; else fdl = 0;             //d lock
   if(sl) fsl = 1; else fsl = 0;             //split
   if(cl) fcl = 1; else fcl = 0;             //clar
   switch(state1)
   {
      case 1: fvf = 0; fmo = 0; break;       //vfo mode, vfo A
      case 2: fvf = 1; fmo = 0; break;       //vfo mode, vfo b
      case 3: fvf = vfo; fmo = 1; break;     //mr mode (current vfo is still reported)
   }

   if(transmitting) ftx = 1; else ftx = 0;   //we in transmit?
   if(!gen_tx) fgh = 1; else fgh = 0;        //we widebanded?
   fsp = 0;                                  //blank / spare
return BITSF;
}

#define dummy_s_meter_val 3 //0 - 15 //0 = FSD
void calc_status()
{
//must use dummy mode as no way of changing modulation modes - its a physical switch
//we make dummy mode a saved integer. Makes bugger-all difference really
//also use dummy s meter value
convert_to_BCD(buffer[1], cat_tx_buf[10], cat_tx_buf[11], cat_tx_buf[12], cat_tx_buf[13]);
convert_to_BCD(buffer[2], cat_tx_buf[15], cat_tx_buf[16], cat_tx_buf[17], cat_tx_buf[18]);

//optional. No program asks for this information. If you need it, you might need bigger PIC!
//convert_to_BCD(aux_buffer[0], cat_tx_buf[20], cat_tx_buf[21], cat_tx_buf[22], cat_tx_buf[23]);      //clarifier
//!convert_to_BCD(load_mem_ch_f(0), cat_tx_buf[25], cat_tx_buf[26], cat_tx_buf[27], cat_tx_buf[28]);  //ch0
//!convert_to_BCD(load_mem_ch_f(1), cat_tx_buf[30], cat_tx_buf[31], cat_tx_buf[32], cat_tx_buf[33]);  //ch1
//!convert_to_BCD(load_mem_ch_f(2), cat_tx_buf[35], cat_tx_buf[36], cat_tx_buf[37], cat_tx_buf[38]);  //ch2
//!convert_to_BCD(load_mem_ch_f(3), cat_tx_buf[40], cat_tx_buf[41], cat_tx_buf[42], cat_tx_buf[43]);  //ch3
//!convert_to_BCD(load_mem_ch_f(4), cat_tx_buf[45], cat_tx_buf[46], cat_tx_buf[47], cat_tx_buf[48]);  //ch4
//!convert_to_BCD(load_mem_ch_f(5), cat_tx_buf[50], cat_tx_buf[51], cat_tx_buf[52], cat_tx_buf[53]);  //ch5
//!convert_to_BCD(load_mem_ch_f(6), cat_tx_buf[55], cat_tx_buf[56], cat_tx_buf[57], cat_tx_buf[58]);  //ch6
//!convert_to_BCD(load_mem_ch_f(7), cat_tx_buf[60], cat_tx_buf[61], cat_tx_buf[62], cat_tx_buf[63]);  //ch7
//!convert_to_BCD(load_mem_ch_f(8), cat_tx_buf[65], cat_tx_buf[66], cat_tx_buf[67], cat_tx_buf[68]);  //ch8
//!convert_to_BCD(load_mem_ch_f(9), cat_tx_buf[70], cat_tx_buf[71], cat_tx_buf[72], cat_tx_buf[73]);  //ch9
cat_tx_buf[0] = stat_flags();
cat_tx_buf[1] = pms;
cat_tx_buf[3] = PLLband;
cat_tx_buf[4] = mem_channel;
cat_tx_buf[5] = ((d100h << 4) + d10h);
cat_tx_buf[6] = ((d10k << 4) + d1k);
cat_tx_buf[7] = ((dmhz2 << 4) + d100k);
cat_tx_buf[8] = (dmhz1);
cat_tx_buf[9] = dummy_mode;
cat_tx_buf[14] = dummy_mode;
cat_tx_buf[19] = dummy_mode;
//cat_tx_buf[24] = dummy_mode;
//cat_tx_buf[29] = dummy_mode;
}

unsigned int8 cat_delay = 0;
void send_cat(int8 len)
{

delay_ms(cat_delay);
for (int i = 0; i < len; ++i)
putchar (cat_tx_buf[i]);
}




int8 action_yaesu_cat()
{
   
   int8 res = 0;

   opcode = cat_buffer[4];
   
   SWITCH(opcode)
   {
      CASE 0x01:
         switch(cat_buffer[3])
         {
            default: sl = 0; break;
            case 1: sl = 1; break;
         }
         res = 3; break;
         
      CASE 0x02: state1 = 3; buffer[state1] = load_mem_ch_f(cat_buffer[3]); res = 2;break;
      CASE 0x03: state1 = (vfo + 1); save_mem_ch_f(cat_buffer[3], buffer[state1]); res = 2;break;
      CASE 0x04: 
         switch(cat_buffer[3])
         {
            default: dl = 0; break;
            case 1: dl = 1; break;
         }
         res = 3; 
         break;

      CASE 0x05: 
         switch(cat_buffer[3])
         {
            default: state1 = 1; break;
            case 1: state1 = 2; break;
         } 
         res = 2;
         break;
         
      CASE 0x06: buffer[vfo + 1] = load_mem_ch_f(cat_buffer[3]); 
      res = 2;
      break;
      
      CASE 0x07: btn_up_handler(hgsel);
      res = 2;
      break;
      
      CASE 0x08: btn_dn_handler(hgsel);
      res = 2;
      break;
      
      CASE 0x09: 
         clarifier_button_handler();
         switch(cat_buffer[3])
         {
            default: cl = 0; break;
            case 1: cl = 1; break;
         }
         res = 3; 
         break;
      
      CASE 0x0A:
         temp_freq = convert_from_BCD(cat_buffer[0], cat_buffer[1], cat_buffer[2], cat_buffer[3]); 
                     buffer[state1] = temp_freq; 
                     temp_freq = 0; 
                     res = 1; 
      break;
      
      CASE 0x0B: 
      vfom_swap_handler(0);
      res = 2; 
      break;
      
      case 0x0D: 
         switch(cat_buffer[3])
         {
            default: hgsel = 5; break;
            case 1: hgsel = 0; break;
         } 
         break;


      CASE 0x0C: dummy_mode = cat_buffer[3]; write8(dummy_mode_n, dummy_mode); break;
      
      CASE 0x0E: buffer[3] = cat_delay; send_cat(75); break;
      //CASE 0x0F: temp_freq = join_value(byte1_upper,byte1_lower,byte2_upper,byte2_lower,byte3_upper,byte3_lower,byte4_upper); buffer[state1] = temp_freq; temp_freq = 0; res = 1; break;
      case 0x0F: 
         switch(cat_buffer[3])
         {
            default: cat_tx_request = 0; break;
            case 1: cat_tx_request = 1; break;
         } 
         break;
      
      case 0x11:
         if(!cat_tx_request) 
         {
            cat_tx_request = 1;
         }
         else 
         {
            cat_tx_request = 0;
         }
         break;
      
      CASE 0x10:
         switch(cat_buffer[3])
         {
            default: send_cat(75); break;
            case 1: 
            if(squelch_open) cat_tx_buf[0] = (dummy_s_meter_val); 
            else cat_tx_buf[0] = 15;
            
            send_cat(1);
            
            break;
         }
         break;

      case 0xFD:
         switch(cat_buffer[3])
         {
            default: baud = 1; break;
            case 2: baud = 2; break;
            case 3: baud = 3; break;
            case 4: baud = 4; break;
         }
         write8(baud_n, baud);
         beep();
         reset_cpu();
         break;
      
      CASE 0xFE: write8 (checkbyte_n, 0xFF); reset_cpu(); break;
      CASE 0xFF: reset_cpu(); break;
      
    }
    if(res == 3) {dcs = get_dcs(); res = 2;}
    yaesu_command_received = 0;
    return res;
  
}

#endif

#ifdef include_cat_yaesu_GX

int8 action_yaesu_cat()
{

int8 res = 0;
   INT8 opcode = buffer[4];
   INT8 byte4_upper = ((buffer[3] >> 4) & 0xF);
   INT8 byte4_lower = buffer[3] & 0xF;
   INT8 byte3_upper = ((buffer[2] >> 4) & 0xF);
   INT8 byte3_lower = buffer[2] & 0xF;
   INT8 byte2_upper = ((buffer[1] >> 4) & 0xF);
   INT8 byte2_lower = buffer[1] & 0xF;
   INT8 byte1_upper = ((buffer[0] >> 4) & 0xF);
   INT8 byte1_lower = buffer[0] & 0xF;
   
   SWITCH(opcode)
   {
      CASE 0x01: split_button_handler(); res = 3; break;
      CASE 0x02: mrvfo_handler(); res = 2;break;
      CASE 0x03: vfom_handler(); res = 2;break;
      CASE 0x04: dial_lock_button_handler(); res = 3; break;
      CASE 0x05: vfoab_handler(); res = 2;break;
      CASE 0x06: mvfo_handler(); res = 2;break;
      CASE 0x07: btn_up_handler(0); res = 2;break;
      CASE 0x08: btn_dn_handler(0); res = 2;break;
      CASE 0x09: clarifier_button_handler(); res = 3; break;
      CASE 0x0A: temp_freq = convert_from_BCD(cat_buffer[0], cat_buffer[1], cat_buffer[2], cat_buffer[3]); buffer[state1] = temp_freq; temp_freq = 0; res = 1; break;
      CASE 0x0B: vfom_swap_handler(0); res = 2; break;
      
      case 0xFC: res = 0; IF( ! gen_tx)gen_tx = 1; else gen_tx = 0; break;

      CASE 0xFE: write8 (checkbyte_n, 0xFF); reset_cpu(); break;
      CASE 0xFF: reset_cpu(); break;
    }
    //if((byte5 == 0xFB) || (byte1 == 0xFB)) {beep(); cat_mode = 1; res = 1;}
       return res;
}

#endif
