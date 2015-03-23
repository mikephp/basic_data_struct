/*
*author: 	lijianying
*datetime:	2015-02-27 19:24:00
*/

#include "port.h"
#include <stdio.h>
#define gnrow(c) for(i=0;i<c;i++)fgets(buf,128,fp)
int do_if(FILE *fp,PT* tmp){
	char buf[128];
	char t1[128],t2[128];
	long int i;
	gnrow(2);

	//strncpy(tmp->port_name,buf,25);
	//strncpy(tmp->port_stat,buf+41,2);
	sscanf(buf,"%s %s %s %s",tmp->port_name,t1,t2,tmp->port_stat);
	gnrow(2);
	sscanf(buf,"%s %s %s",t1,tmp->port_desc,t2);
	gnrow(2);
	strcpy(tmp->port_type,buf+1);
	i=strlen(tmp->port_type);
	tmp->port_type[i-1]=0;
	tmp->port_type[i-2]=0;
	gnrow(1);
	//strncpy(tmp->port_band,buf+1,7);
	sscanf(buf,"%s %s %s %s",tmp->port_band,t1,t1,t1);
	gnrow(19);
	sscanf(buf,"%s %s %ld %s %ld",t1,t1,&i,t1,&(tmp->port_in));
	gnrow(3);
	sscanf(buf,"%ld %s %ld %s %ld %s",&tmp->port_in_sin,t1,&i,t1,&tmp->port_in_mul,t1);
	gnrow(4);
	sscanf(buf,"%s %s %ld %s %ld %s",t1,t1,&i,t1,&tmp->port_out,t2);
	gnrow(3);
	sscanf(buf,"%ld %s %ld %s %ld %s",&tmp->port_out_sin,t1,&i,t1,&tmp->port_out_mul,t1);
    gnrow(4);
}

int sw_ifstat(FILE *fp,PT *head){
    fseek(fp,0L,SEEK_SET);
    PT *cur;
	cur = head;
	int count = 0;int i=0;
	char buf[128];
	gnrow(13);
	for(i=0;i<6;i++){
		PT *tmp = (PT *)malloc(sizeof(PT));
        do_if(fp,tmp);
		cur->next=tmp;
		cur=tmp;
		count++;
		//gnrow(39);
    }
	return count;
}

int port_data(char *pdata,int cc,PT *result){
	//char *ch;int i;ch=pdata;
    FILE *fp=fopen("portdata.txt","w+");
	fwrite(pdata,1,cc,fp);
	fseek(fp,0L,SEEK_SET);
	do_if(fp,result);
	fclose(fp);
	return 0;
}

