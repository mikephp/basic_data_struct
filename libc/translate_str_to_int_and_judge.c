//translate serviceid_str to serviceid,which is int data type
//you'll deal with str,such as "-","-123","123-","12-123",
//"abc-a","a1-2a","a-2","a2","2a","abc",
extern int allisdigit(char*s);

int trans_str2int_judge(char* serviceid_str,i){
	int serviceid;
	char* s1,*s2,ch[5];
	int v1,v2;
	strcpy(ch,serviceid_str);
	if(strchr(ch,'-')){
		s1 = strtok(ch,"-");
		if(s1){//judge if string s1 is not an digit character
			if(allisdigit(s1)){
				v1 = atoi(s1);
				if(v1<16 && v1>=0){
					s2 = strtok(NULL,"-");
					if(s2){
						if(allisdigit(s2)){
							v2 = atoi(s2);
							if(v2<16 && v2>=0){
								if(v2>v1){
									for(serviceid=v1;serviceid<=v2 ;serviceid++){
										//lib_service_hashmode(QUEUE_ID_CLID,serviceid,hashmode);
									}
								}else{
									printf("%s is not bigger than %s\n",s2,s1);
								}
							}else{
								printf("%s is not between 0 and 15\n",s2);
								return -1;
							}
						}
					}else{
						printf("please input two integer,such as 8-13\n");
					}
				}else{
					printf("%s is not between 0 and 15\n");
					return -1;
				}
			}
		}
		else{
			printf("error:char pointer is null,please input right string\n");
			return -1;
		}
	}
	else{
		if(allisdigit(ch)){
			v1 = atoi(ch);
			if(v1<16 && v1>=0){
				serviceid = v1;
				//lib_service_hashmode(QUEUE_ID_CLID,serviceid,hashmode);
			}else{
				printf("error:%s is not between 0 and 15\n",ch);
				return -1;
			}
		}else{
			return -1;
		}
	}
	return 0;
}