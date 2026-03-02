
int8 read_counter()
{
   INT8 res = 0;
   if(pind4) res +=8;
   if(pind5) res +=4;
   if(pind6) res +=2;
   if(pind7) res +=1;
   RETURN res;
}

void reset_counter()
{
   res1 = read_counter();
   res2 = res1;
}

int8 freq_dial_basic(int32 &value, int8 dial_increment)
{
#ifdef include_dial_support
      res1 = read_counter();
      if(dl) return 0;
      if(res1 == res2) return 0;
      else
      {
            if(dial_dir) value+=dial_increment;
            else value-=dial_increment;
            return 1;
      }
#endif
}
