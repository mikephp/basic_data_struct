#include <stdio.h>
#include <time.h>
 
int main(int argc, char *argv[])
{
   if(argc < 2){
		  fprintf(stderr, "Error: usging command file_path");
		  exit(1);
   }

   utime(argv[1], NULL);

   return(0);
}