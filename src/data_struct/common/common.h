#ifndef _COMMON_H
#define _COMMON_H
extern int p_debug_mask;
extern int p_debug;

#define pdg(priority,format,args...) \
	{ \
		if(priority & p_debug_mask){\
			printf("[%s %s %d]" format,__FILE__,__func__,__LINE__,##args);\
		}\
	}

#define DEBUG_FORMAT __FILE__,__func__,__LINE__

#define ps(s) 		if( p_debug  & p_debug_mask	)printf("[%s %s %d]%s\n",DEBUG_FORMAT,s)
#define psi(s,i) 	if(	p_debug  & p_debug_mask )printf("[%s %s %d]%s %d\n",DEBUG_FORMAT,s,i)
#define psx(s,i) 	if( p_debug  & p_debug_mask )printf("[%s %s %d]%s 0x%x\n",DEBUG_FORMAT,s,i)
#define pss(s,i) 	if( p_debug  & p_debug_mask )printf("[%s %s %d]%s %s\n",DEBUG_FORMAT,s,i)
#define psz(s,i) 	if( p_debug  & p_debug_mask )printf("[%s %s %d]%s %zu\n",DEBUG_FORMAT,s,i)

extern int printc(char* ch,int rn, void *buf);
#endif