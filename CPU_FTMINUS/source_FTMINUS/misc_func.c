#define cycles_delay 2

void strobe_Q64(INT8 val)
{
   switch(val)
   {
      case 0: PORTA = 0; delay_cycles(1); PORTA = 0; delay_cycles(cycles_delay); break;
      case 1: PORTA = 0; delay_cycles(1); PORTA = 8; delay_cycles(cycles_delay); break; //PLL1
      case 2: PORTA = 0; delay_cycles(1); PORTA = 4; delay_cycles(cycles_delay); break; //counter preset
      case 3: PORTA = 0; delay_cycles(1); PORTA = 12; delay_cycles(cycles_delay); break; //bpf data Q69/Q70
      case 4: PORTA = 0; delay_cycles(1); PORTA = 2; delay_us(40); break; //beep
      case 5: PORTA = 0; delay_cycles(1); PORTA = 10; delay_cycles(cycles_delay); break; //DL
      case 6: PORTA = 0; delay_cycles(1); PORTA = 6; delay_cycles(cycles_delay); break; //inh
      case 7: PORTA = 0; delay_cycles(1); PORTA = 14; delay_cycles(cycles_delay); break; //CAT TX
      case 8: BUS_DATA128 = 1; delay_cycles(cycles_delay); BUS_DATA128 = 0; break;
   } 
   PORTA = 0;
}

int8 old_PORTA = 0;
void default_Q64(INT8 val)
{      
   switch(val)
   {
      default: PORTA = 0; break;
      case 1: PORTA = 10; break;
      case 2: PORTA = 14; break;
      case 3: PORTA = 6; break;
   }
         
}

void PLL1(int8 data){PORTB = data; strobe_Q64(1);}
void PLL2(int8 data){PORTB = data; strobe_Q64(8);}
void counter_preset_enable(){strobe_Q64(2);}
void banddata(){strobe_Q64(3);}


void load_10hz(INT8 val)
{
   INT8 loc100 = 112;
   PORTB = loc100 + val;
   counter_preset_enable();
}
void load_100hz(INT8 val)
{
   //save_port_b();
   INT8 loc100 = 112;
   PORTB = loc100 + val;
   //restore_port_b();
}

void beep(){strobe_Q64(4);}
void errorbeep(INT8 beeps)
{
   for (INT8 i = 0; i < beeps; ++i)
   {beep(); delay_ms(200);}
}



# separate
void split_value(int32 value, INT8 &d3, int8 &d4, int8 &d5, int8 &d6, int8 &d7, int8 &d8, int8 &d9, int1 zero)
{
   if(!zero)
   {
   d3  = 0; IF (value < 1000000) d3 = 15;
   d4  = 0; IF (value < 100000) d4 = 15;
   d5  = 0; IF (value < 10000) d5 = 15;
   d6  = 0; IF (value < 1000) d6 = 15;
   d7  = 0; if (value < 100) d7 = 15;
   d8  = 0; if (value < 10) d8 = 15;
   d9  = 0; if (value < 1) d9 = 0;
   }
    WHILE (value >= 1000000){value -= 1000000; d3 += 1; }
    WHILE (value >= 100000){value -= 100000; d4 += 1; }
    WHILE (value >= 10000){value -= 10000; d5 += 1; }
    WHILE (value >= 1000){value -= 1000; d6 += 1; }
    WHILE (value >= 100){value -= 100; d7 += 1; }
    WHILE (value >= 10) {value -= 10; d8 += 1; }
    WHILE (value >= 1) {value -= 1; d9 += 1; }
   
   if(!zero)
   {
   
   
   }
   
}

unsigned int32 join_value(int8 d3, int8 d4, int8 d5, int8 d6, int8 d7, int8 d8, int8 d9)
{
   unsigned int32 v = 0;
   while(d9) {v += 1; d9 -= 1;}
   while(d8) {v += 10; d8 -= 1;}
   while(d7) {v += 100; d7 -= 1;}
   while(d6) {v += 1000; d6 -= 1;}
   while(d5) {v += 10000; d5 -= 1;}
   while(d4) {v += 100000; d4 -= 1;}
   while(d3) {v += 1000000; d3 -= 1;}
   return v;
}
