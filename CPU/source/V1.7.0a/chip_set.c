#ifdef 18f452
#include <18F452.h>
   #ifdef bootloader_upgrade
   #include <bootloader.h>
   #endif
#endif

#ifdef 18f4520
#include <18F4520.h>
   #ifdef bootloader_upgrade
   #include <bootloader.h>
   #endif
#endif

#ifdef 16f877a
#include <16F877A.h>
#define ft_minus
#endif

#ifdef 16f877
#include <16F877.h>
#define ft_minus
#endif

#ifdef 18f452
#fuses HS,PUT, NOWDT,NOPROTECT,NOLVP, BORV27
#endif

#ifdef 18f4520
#fuses HS,PUT, NOWDT,NOPROTECT,NOLVP, BORV27
#endif

#ifdef 16f877a
#fuses HS,PUT, NOWDT,NOPROTECT,NOLVP, BROWNOUT
#endif

#ifdef 16f877
#fuses HS,PUT, NOWDT,NOPROTECT,NOLVP, BROWNOUT
#endif

#use delay(clock=20000000)
#use rs232(baud=4800, xmit=PIN_C6, rcv=PIN_C7, parity=N, stop=2, ERRORS)

#ifdef 16f877
# byte PORTA = 0x05
# byte PORTB = 0x06
# byte PORTC = 0x07
# byte PORTD = 0x08
# byte PORTE = 0x09
#endif

#ifdef 16f877a
# byte PORTA = 0x05
# byte PORTB = 0x06
# byte PORTC = 0x07
# byte PORTD = 0x08
# byte PORTE = 0x09
#endif

#ifdef 18f452
# byte PORTA = 0x0f80
# byte PORTB = 0x0f81
# byte PORTC = 0x0f82
# byte PORTD = 0x0f83
# byte PORTE = 0x0f84
#endif

#ifdef 18f4520
# byte PORTA = 0x0f80
# byte PORTB = 0x0f81
# byte PORTC = 0x0f82
# byte PORTD = 0x0f83
# byte PORTE = 0x0f84
#endif

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

int8 BITSA, BITSB, BITSC, BITSD, BITSE, BITSF, BITSG;
//# bit fine_tune_active =      BITSA.0
# bit speed_dial =            BITSA.1
# bit autosave =              BITSA.2
# bit display_on =            BITSA.3
# bit per_band_offset =       BITSA.4
# bit dl =                    BITSA.5
# bit cl =                    BITSA.6
# bit sl =                    BITSA.7
# bit id_enable =             BITSB.0
# bit cat_mode =              BITSB.1
# bit cb_disabled =           BITSB.2
# bit gen_tx =                BITSB.3
# bit soft_500k =             BITSB.4
//# bit vfo =                   BITSB.5
# bit pause_cat =             BITSB.6
# bit tx_offset_only =        BITSB.7
# bit rx_offset_only =        BITSC.0
# bit soft_pms =              BITSC.1
# bit btn_down =              BITSC.2
# bit general_timer_start =   BITSC.3
# bit long_press =            BITSC.4
# bit mic_down =              BITSC.5
# bit pms =                   BITSC.6
# bit command_received =      BITSC.7
# bit transmitting =          BITSD.0
# bit flash =                 BITSD.1
# bit fine_tune_display =     BITSD.2
# bit jump =                  BITSD.3
# bit tuned =                 BITSD.4
# bit cat_tx_request =        BITSD.5
# bit setup_offset =          BITSD.6
# bit dir =                   BITSD.7
# bit temp_cl =               BITSE.0
# bit stopped =               BITSE.1
# bit app_detected =          BITSE.2
# bit yaesu_cat_lock =        BITSE.3
# bit hard_500k =             BITSE.4
# bit long_tmr_ticked =       BITSE.5
# bit offset_loaded =         BITSE.6
# bit otx =                   BITSE.7
# bit cat_com_tmr_enable =    BITSF.0
# bit cat_com_tmr_ticked =    BITSF.1
# bit cat_com_tmr_start =     BITSF.2
# bit tx_override =           BITSF.3
# bit tx_timer_start =        BITSF.4
# bit tx_ticked =             BITSF.5
# bit valid =                 BITSF.6
# bit gen_tmr_ticked =        BITSF.7
# bit rit_pn =                BITSG.0
