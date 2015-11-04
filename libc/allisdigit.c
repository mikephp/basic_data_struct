int allisdigit(char*s)
{
    int ret=1;
    while(*s){
        if(!isdigit(*s)){
            printf("error:%c is not an digit character\n",*s);
            ret = 0;
        }
        s++;
    }
    return ret;
}