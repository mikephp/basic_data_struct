#include <string.h>

int allisdigit(char*s)
{
    int ret=1;
	if(s){
	    while(*s){
	        if(!isdigit(*s)){
	            printf("%c is not an digit character\n",*s);
	            //return 0;
	            //break;
	            ret = 0;
	        }
	        s++;
	    }
	}
	else{
		printf("char pointer is null\n");
		ret = -1;
	}
    return ret;
}