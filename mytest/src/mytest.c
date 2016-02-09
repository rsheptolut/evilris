#define reg_joypad1 *(char*)0x4016
#define reg_joypad2 *(char*)0x4017

#define reg_v_ctl1 *(char*)0x2000
#define reg_v_ctl2 *(char*)0x2001
#define reg_v_stat *(char*)0x2002
#define reg_v_spra *(char*)0x2003
#define reg_v_sprv *(char*)0x2004
#define reg_v_scrl *(char*)0x2005
#define reg_v_vida *(char*)0x2006
#define reg_v_vidv *(char*)0x2007

#define p_v_palette 0x3F00
#define p_v_bkg_palette p_v_palette
#define p_v_spr_palette p_v_palette + 0x10
         
#define VideoWriteSet(addr) reg_v_vida = addr >> 8; \
			    reg_v_vida = addr & 0xFF; 
    
#define VideoWriteNext(val) reg_v_vidv = val


#define VideoWrite(addr, val) VideoWriteSet(addr); \
    VideoWriteNext(val);

#include "bin_palette.h"

void SetupPPU()
{
  reg_v_ctl1 = 0x88; // 11111111b;
  reg_v_ctl2 = 0x08; // 00001000b;
}

void LoadPalette()
{
  unsigned char i;
  const unsigned char * pal;
  reg_v_vida = 0x3F;
  reg_v_vida = 0x00; 
  
  pal = bin_palette;

  for (i = BIN_PALETTE_LENGTH; i; i--)
  {
    reg_v_vidv = *pal;
    pal++;
  }
}

void LoopForever()
{
  while (1) {};
}

void main(void)
{
  SetupPPU();
  LoadPalette();
  
  LoopForever();
}