//24 BIT EEPROM STORAGE. Uses less EEPROM as the unused extra byte in int32 is nulled and all values moved over 1 byte
//each read int32 must be to a zeroed buffer in 24 bit mode or the "extra" byte will be already in the buffer, giving stupid values
//32 bit leaves the unused zero intact. More human-readable as we can see where each block starts/ends

#define bytecount 4
//#define bytecount 3

   void eeprom_write(unsigned int8 base, unsigned int8 data){write_eeprom(base, data);}
   unsigned int8 eeprom_read(unsigned int8 base){return read_eeprom(base);}


#define fil0                     0xD0
#define fil1                     0xD1
#define fil2                     0xD2
#define fil3                     0xD3
#define fil4                     0xD4
#define fil5                     0xD5
#define fil6                     0xD6
#define fil7                     0xD7
#define fil8                     0xD8
#define fil9                     0xD9

#define state_n                  0xE0
#define band1_n                  0xE1
#define band2_n                  0xE2
#define band3_n                  0xE3
#define band_offset_n            0xE4
#define mem_ch_n                 0xE5
#define cb_ch_n                  0xE6
#define cb_reg_n                 0xE7
//#define fine_tune_n              0xE8
#define dial_n                   0xE9
#define savetimer_n              0xEA
#define cat_mode_n               0xEB
#define baud_n                   0xEC
#define lp_dial_lock_ndl_n       0xED
#define lp_vfoab_n               0xEE
#define lp_dial_lock_dl_n        0xEF
#define lp_clarifier_n           0xF0
#define lp_btn_dn_n              0xF1
#define lp_btn_up_n              0xF2
#define lp_mvfo_n                0xF3
#define lp_vfom_n                0xF4
#define lp_mrvfo_n               0xF5
#define lp_split_n               0xF6
#define lp_swap_n                0xF7
#define lp_fast_n                0xF8
#define id_enable_n              0xF9
#define display_on_n             0xFA
#define txrx_offset_only_n       0xFB 
#define dummy_mode_n             0xFC
#define old_state_n              0xFD
#define vfo_n                    0xFE
#define checkbyte_n              0xFF

#define mem_ch_block 0
#define offset_block 15
#define vfoa_block 26
#define vfob_block 37

void write32(int8 base, unsigned int32 data)
{
   int32 temp_data = 0;
   int8 address = (base * bytecount);
   int8 i;
   for (i = 0; i < bytecount; i++)
      * ( (int8 * ) (&temp_data) + i) = eeprom_read (address +  i);

   
   if (data == temp_data) {return;}
   
   for (i = 0; i < bytecount; i++)
   {
      
      eeprom_write (address + i, * ( (int8 *) (&data) + i));
   }
#ifdef eeprom_save_debug32
   beep () ;
#endif
   //load_10hz (0) ;
}

unsigned int32 read32(int8 base)
{
   int8 address = (base * bytecount);
   int8 i;
   int32 data = 0;
   for (i = 0; i < bytecount; i++)
      * ( (int8 * ) (&data) + i) = eeprom_read (address +  i);

   return (data) ;
}

void write8(int8 base, int8 value)
{

int8 temp_data = read_eeprom(base);
if(temp_data == value) {return;}

eeprom_write(base, value);

#ifdef eeprom_save_debug8
      beep();
#endif
}

int8 read8(int8 base)
{
   return eeprom_read(base);
}



void save_band_state_f(int8 state, int8 band)
{
   if (state == 1)                                {write32 ((vfoa_block + band), buffer[1]);}
   if (state == 2)                                {write32 ((vfob_block + band), buffer[2]);}
}


int32 load_band_state_f(int8 state, int8 band) 
{
   if (state == 1)                                return (read32 (vfoa_block + band));
   if (state == 2)                                return (read32 (vfob_block + band));
}

void save_mem_ch_f(int8 channel, int32 data)   {write32 (channel+mem_ch_block, data);}
int32 load_mem_ch_f(int8 channel)               {return (read32(channel+mem_ch_block));}

void load_all_buffers()
{
   buffer[1] = load_band_state_f(1, band1);
   buffer[2] = load_band_state_f(2, band2);
   buffer[3] = load_mem_ch_f(mem_channel);
}

void save_all()
{
   write8(band1_n, band1);
   write8(band2_n, band2);
   write8(mem_ch_n, mem_channel);
   save_band_state_f(1, band1);
   save_band_state_f(2, band2); 
   saved = 1;
}
