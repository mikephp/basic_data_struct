#include <stdio.h>
#include <string.h>
int		log_level_mask=1;
char	root_info[1024];
int		root_info_len=0;
#define log_info_serialize(priority,format,args...) \
{ \
	if(priority & log_level_mask){\
		sprintf(root_info + root_info_len,format,##args);\
		root_info_len = root_info_len + strlen(root_info+root_info_len); \
	}\
}
#define log_info_print(priority,format,args...) \
{ \
	if(priority & log_level_mask){\
		printf(format,##args);\
		root_info_len = root_info_len + strlen(root_info+root_info_len); \
	}\
}
#define log_print(priority,format,args...) \
{ \
	if(priority & log_level_mask){\
		printf("[%s %s %d]"format,__FILE__,__func__,__LINE__,##args);\
		root_info_len = root_info_len + strlen(root_info+root_info_len); \
	}\
}

int main()
{	
	log_info_print(1,"%d %d %d\n",1,2,3);
	log_print(1,"%s = %d\n","rv",123);
	return 0;
}
