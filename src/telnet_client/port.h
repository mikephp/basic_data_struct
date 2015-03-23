#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STR_LEN 128
typedef struct port{
char port_name[MAX_STR_LEN];
char port_desc[MAX_STR_LEN];//descripe
char port_type[MAX_STR_LEN];
char port_stat[MAX_STR_LEN];
char port_band[MAX_STR_LEN];
long int port_in;
long int port_out;
long int port_in_sin;//total single packet
long int  port_in_mul;//total multi packet
long int port_out_sin;//total single packet
long int port_out_mul;//total multi packet
struct port  *next;
}PT;

