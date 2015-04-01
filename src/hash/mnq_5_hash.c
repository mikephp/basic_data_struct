#include "hash5.h"
#ifndef u32
#define u32 unsigned int
#endif

#ifndef u16
#define u16 unsigned short 
#endif

#ifndef u8
#define u8 unsigned char 
#endif

#ifndef xor
#define xor ^
#endif

u32 hash_sip(u32 sip)
{
	u32 ht = sip xor 0;
	u16 ht_h16 = (ht& 0xFFFF0000)>>16;//high 16 bit
	u16 ht_l16 = (ht& 0x0000FFFF);//low
	u16 ht16 = ht_l16 xor ht_h16;
	u16 ht16_h4 = (ht16& 0xF000);
	u16 ht16_m4 = (ht16& 0x0F00)<<4;
	u16 ht4 = ht16_h4 xor ht16_m4;
	u16 ht_l8 = ht16 & 0xFF;
	u16 ht12 = ht_l8 | ht4;
	return (ht12>> 2) & 0x03FF;
}

u16 hash_sip_dip(u32 sip,u32 dip)
{
	u16 ht = dip xor sip;
	u16 ht_l16 = (ht& 0xFFFF0000 >> 16);
	u16 ht_h16 = (ht& 0xFFFF);
	u16 ht16 = ht_l16 xor ht_h16;
	u16 ht16_h4 = (ht16& 0xF000);
	u16 ht16_m4 = (ht16& 0x0F00);
	u16 ht4 = ht16_h4 xor ht16_m4;
	u8 ht_l8 = ht16& 0xFF;
	u16 ht12 = ht_l8 | (ht4<< 8);
	   return ht12>> 2;
}