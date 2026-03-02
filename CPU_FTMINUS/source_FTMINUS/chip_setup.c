# bit dial_clk=PORTA.0  //input  // from q66 counter input (normally high, when counter = 0, goes low FOR uS). unused. Using up/down counter = 0 which is same result. Use for TX
# bit disp_INT=PORTA.4  //output // display interupt
# bit k1=PORTA.5  //output // display bit 0
# bit pina6=PORTA.6
# bit pina7=PORTA.7
# bit BUS_DATA1=PORTB.0  //output // bus data bit 0 pll d0
# bit BUS_DATA2=PORTB.1  //output //          bit 1     d1
# bit BUS_DATA4=PORTB.2  //output //          bit 2     d2
# bit BUS_DATA8=PORTB.3  //output //          bit 3     d3
# bit BUS_DATA16=PORTB.4  //output // pll a0
# bit BUS_DATA32=PORTB.5  //output //     a1
# bit BUS_DATA64=PORTB.6  //output //     a2
# bit BUS_DATA128=PORTB.7  //output // strobe FOR pll2  q42
# bit pb0=PORTC.0  //input  // keypad pb0
# bit pb1=PORTC.1  //input  // keypad pb1
# bit pb2=PORTC.2  //input  // keypad pb2   // also tells us when not to write to the display
# bit sw_500k=PORTC.3  //input  // 500khz step SWITCH
# bit dial_dir=PORTC.4  //input  // Dial counting direction
# bit mic_up=PORTC.5  //input  // mic up button
# bit mic_dn=PORTC.6  //input  // mic down button
# bit pinc7=PORTC.7  //output // remote (CAT) wire (may use this FOR some sort of debugging output)
# bit sw_pms=PORTD.0  //input 
# bit mic_fast=PORTD.1  //input  // microphone fst (fast) button
# bit squelch_open=PORTD.2  //input  // Squelch open when high (FOR scanning)
# bit tx_mode=PORTD.3  //input  // PTT/On The Air (even high when txi set)
# bit pind4=PORTD.4 //BCD Bit 3
# bit pind5=PORTD.5 //BCD Bit 2
# bit pind6=PORTD.6 //BCD Bit 1
# bit pind7=PORTD.7 //BCD Bit 0
# bit k2=PORTE.0  //output // display bit 1   also these bits are FOR scanning the keypad
# bit k4=PORTE.1  //output // display bit 2
# bit k8=PORTE.2  //output // display bit 3

#define clarifier_button 1
#define down_button 2
#define up_button 3
#define mvfo_button 4
#define vfoab_button 5
#define dial_lock_button 6
#define vfom_button 7
#define mrvfo_button 8
#define split_button 9
#define vfom_swap_button 10

unsigned int32 buffer[5];
unsigned int32 temp_freq;
unsigned int32 aux_buffer[2];

unsigned int8 BITSA, BITSB, BITSC, BITSD, BITSE, BITSF;
unsigned int8 dmhz1, dmhz2, d100k, d10k, d1k, d100h, d10h;
unsigned int8 band1;
unsigned int8 band2;
unsigned int8 band3;
unsigned int8 state1;
unsigned int8 state2;
unsigned int8 vfo;
unsigned int8 mem_channel;
unsigned int8 PLLband;
unsigned int8 autosave;

unsigned int8 dcs;
unsigned int8 update;
unsigned int8 fl_cnt;
unsigned int8 fl_cnt_gearing;
unsigned int8 old_state;
unsigned int8 temp_display_type;
unsigned int8 dial_inc;
unsigned int8 beep_int;
unsigned int8 res1, res2;
unsigned int8 btnscan; 
unsigned int8 btnres; 
unsigned int8 btn_count; 
unsigned int8 rtnres;
unsigned int8 timer;
unsigned int8 btntimer;
unsigned int8 next_in;
unsigned int8 dummy_mode;
unsigned int8 opcode;
unsigned int8 hgsel;
unsigned int8 baud;
unsigned char temp_byte;
unsigned char cat_buffer[cat_buffer_size];
unsigned char cat_command_timer;


# bit display_on = BITSA.0
# bit fine_tune = BITSA.1
# bit dl = BITSA.2
# bit cl = BITSA.3
# bit sl = BITSA.4
# bit gen_tx = BITSA.5
# bit hard_500k = BITSA.6
# bit soft_500k = BITSA.7




# bit offset_loaded = BITSB.0
# bit setup_offset = BITSB.1
# bit tx_offset_only = BITSB.2
# bit rx_offset_only = BITSB.3
# bit otx = BITSB.4
# bit jump = BITSB.5
# bit transmitting = BITSB.6
# bit tx_override = BITSB.7



# bit timerstart = BITSC.0
# bit timerticked = BITSC.1
# bit btntimerstart = BITSC.2
# bit btntimerticked = BITSC.3
# bit long_press = BITSC.4
# bit mic_down = BITSC.5
# bit btn_down = BITSC.6
# bit flash = BITSC.7


# bit valid = BITSD.0
# bit yaesu_command_received = BITSD.1
# bit stopped = BITSD.2
# bit pms = BITSD.3
# bit soft_pms = BITSD.4
# bit flash_ticked = BITSD.5
# bit fine_tune_display = BITSD.6
# bit sw_pms_pressed = BITSD.7

# bit cat_com_tmr_start = BITSE.0
# bit cat_com_tmr_enable = BITSE.1
# bit cat_com_tmr_ticked = BITSE.2
# bit saved = BITSE.3
# bit cat_tx_request = BITSE.4


# bit   fdl=BITSF.0
# bit   fsl=BITSF.1
# bit   fcl=BITSF.2
# bit   fvf=BITSF.3
# bit   fmo=BITSF.4
# bit   ftx=BITSF.5
# bit   fgh=BITSF.6
# bit   fsp=BITSF.7


