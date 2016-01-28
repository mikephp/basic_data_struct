#ifndef pdebug
#define pdebug printf
#endif
/*
*	width: 	the byte number of an element.
*	length:	the total byte number of all element.
*/
int host2network_byte_order(void *data,int width,int length)
{
	int i = 0;
	unsigned int *ch4;
	unsigned short int *ch2;
	//unsigned char *ch = (unsigned char*)data;
	if(!data)return -1;
	if(width<2)return -2;
	if(length<2)return -3;
	switch(width){
		case 2:
			ch2 = (unsigned short int*)data;
			for(i = 0; i< length/width;i++){
				*ch2 = htons(*ch2);
				ch2++;
			}
			break;
		case 4:
			ch4 = (unsigned int*)data;
			for(i = 0; i< length/width;i++){
				*ch4 = htonl(*ch4);
				ch4++;
			}
			break;
		default:
			return -4;
			break;
	}

	return 0;
}

/*
*	data:	data pointer.
*	length:	the length of the data.
*	step:	the step of the data.
*/
int network_byte_order_increase_by_step(void* data,int length,unsigned short int step)
{
	unsigned short  int i = 0,over_flow = 0,*start,*last,*current,loop = 0,result;
	current = (unsigned short int *)data;
	loop =  length/sizeof(unsigned short int);

	/*!!!attention !!!*/
	/*from the first ,so i begin from 1 not 0 !!!*/
	for( i = 1; i< loop; i++ ){
		current++;
	}

	last = current;
	pdebug("data %p,current %p\n",data,current);
	pdebug("step %d,loop %d,last 0x%04x,current 0x%04x\n",step,loop,*last,*current);
	*last += step;
	if( *last < step ){
		over_flow = 1;
	}else{
		over_flow = 0;
	}
	current--;

	for(i = 1; i < loop; i++){
		*current += over_flow;
		if( *current < over_flow){
			over_flow = 1;
		}else{
			over_flow = 0;
		}
		current--;
	}
	return 0;
}

int host_byte_order_increase_by_step(void* data,int length,unsigned short step)
{
	unsigned short  int i = 0,over_flow = 0,*start,*current,loop = 0,result;
	start = (unsigned short *)data;
	current = start;
	loop =  length/sizeof(unsigned short int);

	*start+=step;
	if(*current < step){
		over_flow = 1;
	}else{
		over_flow = 0;
	}
	current++;
	for(i = 1; i < loop; i++){
		*current += over_flow;
		if( *current < over_flow){
			over_flow = 1;
		}else{
			over_flow = 0;
		}
		current++;
	}
	return 0;
}