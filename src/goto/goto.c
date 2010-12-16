#include <stdio.h>
 
int main()
{
    int c = 1;
    void *label_array[] = 
    {   
        &&op_hello,
        &&op_world
    };  
 
    scanf("%d", &c);
 
    if (c < 0 || c > 1)
    {   
        goto op_end;
    }   
    else
		goto *label_array[c];
     
op_hello:
    printf("hello\n");
    goto op_end;
op_world:
    printf("world\n");
    goto op_end;
op_end:
    return 0;
}
