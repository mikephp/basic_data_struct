#include <stdio.h>  

#include <stdlib.h>  

#include <unistd.h>  

#include <string.h>  

#include <pthread.h>  

#include <sys/types.h>  

#include <sys/socket.h>  

#include <netinet/in.h>  

#include <arpa/inet.h>  

#include <netdb.h>  

#define MAX_THREAD 100  

  

typedef struct URLInfo  

{  

    char schema[8];  

    char host[256];  

    char host_name[256];  

    unsigned int port;  

    char file[256];  

}URLInfo;  

  

typedef struct Connection  

{  

    int sock;  

    URLInfo url_info;  

    int avaliable;  

}Connection;  

  

typedef struct Resource  

{  

    char file_url[256];  

    int file_size;  

    char file_name[256];  

}Resource;  

  

typedef struct ThreadArg  

{  

    Resource* res;  

    int start_pos;  

    int limit;  

    int no;  

}ThreadArg;  

  

typedef struct BreakPoint  

{  

    int downloaded;  

    int thread_amount;  

    int tasks[MAX_THREAD][2];  

      

}BreakPoint;  

  

pthread_mutex_t g_mut;  

int g_total=0;  

int g_downloaded=0;  

BreakPoint g_breakpoint;  

  

URLInfo parse_url(const char *url);  

Connection open_url(const char * url);  

Resource get_resource(const char *url);  

void join_url(const char* old_url,const char* redirect,char * new_url);  

void download(const char* url,int thread_amount,const char* file_name);  

void* download_part(void* args);  

void* monitor(void *args);  

void store_breakpoint(char * cfgName);  

  

void store_breakpoint(char * cfgName)  

{  

    int z;  

    FILE* f;  

    f=fopen(cfgName,"w");  

    fprintf(f,"%d\n",g_breakpoint.downloaded);  

    fprintf(f,"%d\n",g_breakpoint.thread_amount);  

    for(z=0;z<g_breakpoint.thread_amount;z++){  

       fprintf(f,"%d-%d\n",g_breakpoint.tasks[z][0],g_breakpoint.tasks[z][1]);  

    }  

    fclose(f);  

}  

  

void join_url(const char* old_url,const char* redirect,char * new_url)  

{  

    char stack1[256][256]={0},stack2[256][256]={0};  

    int i=0,j=0,p1=0,p2=0;  

    char seg[256]={0};  

    URLInfo temp_urlinfo;  

      

    memset(new_url,0,sizeof(new_url));  

    if(strstr(redirect,"://")!=NULL){  

        strcpy(new_url,redirect);  

    }  

    else{  

        while(1){  

            while(redirect[i]!='/' && redirect[i]!=0){  

                seg[j++]=redirect[i++];  

            }      

            strcpy(stack1[p1++],seg);  

            memset(seg,0,sizeof(seg));  

            j=0;  

            if(redirect[i]==0)  

                break;  

            i++;  

        }  

        for(i=0;i<p1;i++){  

            if(!strcmp(stack1[i],"..") && p2>-1)  

                p2--;  

            else if(strcmp(stack1[i],".")){  

                strcpy(stack2[p2++],stack1[i]);  

            }  

        }  

        //printf("##%s\n",stack2[0]);  

     

        if(!strcmp(stack2[0],"")){  

            temp_urlinfo=parse_url(old_url);  

            sprintf(new_url,"%s://%s:%d/",temp_urlinfo.schema,temp_urlinfo.host,temp_urlinfo.port);            

        }  

        else{  

            i=strlen(old_url)-1;  

            while(old_url[i]!='/')  

                i--;  

            //printf("##%c\n",old_url[i]);  

            strncpy(new_url,old_url,i+1);  

            new_url[i+1]=0;  

        }  

        //printf("##%s\n",new_url);  

        for(j=0;j<p2-1;j++){  

            strcat(new_url,stack2[j]);  

            strcat(new_url,"/");  

        }  

        strcat(new_url,stack2[p2-1]);  

    }  

}  

  

URLInfo parse_url(const char* url){  

    int i=0,j=0;  

    char schema[8]={0};  

    char host[256]={0};  

    char port[8]={0};  

    char file[256]={0};  

    char IP[32]={0};  

    URLInfo url_info;  

    struct hostent* hptr;  

      

    while(url[i]!=':'){  

        schema[j++]=url[i++];  

    }  

  

    for(i+=3,j=0;url[i]!=':' && url[i]!='/' && url[i]!=0;){  

        host[j++]=url[i++];  

    }  

      

    if(url[i]==':'){  

        for(i+=1,j=0;url[i]!='/';){  

            port[j++]=url[i++];  

        }  

        sscanf(port,"%d",&url_info.port);  

    }  

    else{  

        url_info.port=80;  

    }  

      

    if(url[i]!=0){  

        for(j=0;url[i]!=0;){  

            file[j++]=url[i++];  

        }  

    }  

    else{  

        file[0]='/';  

    }  

      

    strcpy(url_info.schema,schema);  

    strcpy(url_info.file,file);  

    strcpy(url_info.host_name,host);  

    hptr=gethostbyname(host);  

  

     

    if(hptr!=NULL){  

        strcpy(url_info.host,  

            inet_ntop(hptr->h_addrtype,*(hptr->h_addr_list),IP,sizeof(IP))  

        );  

    }  

    //printf("%s\n",url_info.host);  

    return url_info;  

}  

Connection open_url(const char* url){  

    Connection conn;  

    struct sockaddr_in remote_addr,local_addr;  

  

    conn.avaliable=0;  

    conn.url_info=parse_url(url);  

      

    local_addr.sin_family=AF_INET;  

    local_addr.sin_addr.s_addr=htonl(INADDR_ANY);  

    local_addr.sin_port=htons(0);  

    remote_addr.sin_family=AF_INET;  

    remote_addr.sin_addr.s_addr=inet_addr(conn.url_info.host);  

    remote_addr.sin_port=htons(conn.url_info.port);  

      

    conn.sock=socket(AF_INET,SOCK_STREAM,0);  

    if(bind(conn.sock,  

        (struct sockaddr*)&local_addr,  

        sizeof(local_addr))<0){  

            printf("bind error\n");  

    }  

      

      

      

    if(conn.sock){  

        if(  

            connect(conn.sock,(struct sockaddr*)&remote_addr,sizeof(remote_addr))!=-1  

        ){  

            conn.avaliable=1;  

        }  

    }  

      

    return conn;  

}  

  

Resource get_resource(const char* url){  

    char pack[1024]={0};  

    char buf[1024]={0};  

    char redirect[256]={0},new_url[256]={0},old_url[256]={0};  

    static int redirect_count=0;  

    char* i;  

    char* j;  

    char* z;  

    Resource res;  

      

    Connection conn=open_url(url);  

    if(!conn.avaliable){  

        return res;  

    }  

    sprintf(pack,"GET %s HTTP/1.1\nHost: %s\nAccept: */*\nReferer: http://%s\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)\nPragma: no-cache\nCache-Control: no-cache\nConnection: close\n\n",conn.url_info.file,conn.url_info.host_name,conn.url_info.host_name);  

    send(conn.sock,pack,strlen(pack),0);  

    recv(conn.sock,buf,sizeof(buf),0);  

    //printf("%s\n",buf);  

    if(strstr(buf,"HTTP/1.1 404")!=NULL || strstr(buf,"HTTP/1.0 404")!=NULL){  

       return res;  

    }  

    i=(char *)strstr(buf,"Location:");  

    if(i!=NULL && redirect_count<5){  

        sscanf(i,"Location: %s",redirect);  

        sprintf(old_url,"%s://%s:%d%s",conn.url_info.schema,conn.url_info.host_name,conn.url_info.port,conn.url_info.file);  

        join_url(old_url,redirect,new_url);  

        //printf("@#%s\n",new_url);  

        redirect_count++;  

        return get_resource(new_url);  

    }  

    i=(char *)strstr(buf,"Content-Length:");  

    if(i!=NULL){  

        sscanf(i,"Content-Length: %d",&res.file_size);  

    }  

    strcpy(res.file_url,url);  

    //printf("#%d\n",res.file_size);  

    for(z=(char*)url;(j=strstr(z,"/"))!=NULL;){  

        z=j+sizeof(char);  

    }  

    strcpy(res.file_name,z);  

    close(conn.sock);  

    return res;  

}  

  

void* download_part(void * args)  

{  

    ThreadArg* targ=(ThreadArg*)args;  

    Connection conn;  

    FILE* f=NULL;  

    char pack[1024]={0};  

    char buf[1024]={0};  

    int i=0,ct=0;  

    char* body=NULL;  

    //printf("%s,%d-%d\n",targ->res->file_url, targ->start_pos,targ->limit);  

    conn=open_url(targ->res->file_url);  

    while(!conn.avaliable){  

        sleep(1);  

        conn=open_url(targ->res->file_url);  

    }  

    if(conn.avaliable){  

  

        f=fopen(targ->res->file_name,"rb+");  

        fseek(f,targ->start_pos,0);  

        sprintf(pack,"GET %s HTTP/1.1\nHost: %s\nAccept: */*\nReferer: http://%s\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)\nRange: bytes=%d-%d\nPragma: no-cache\nCache-Control: no-cache\n\n",conn.url_info.file,conn.url_info.host_name,conn.url_info.host_name,targ->start_pos,targ->start_pos+targ->limit-1);  

        //printf("%s",pack);  

begin_down:  

        send(conn.sock,pack,strlen(pack),0);  

        i=recv(conn.sock,buf,sizeof(buf),0);  

          

        if(strstr(buf,"HTTP/1.1 206")==NULL && strstr(buf,"HTTP/1.0 206")==NULL && strstr(buf,"HTTP/1.1 200")==NULL && strstr(buf,"HTTP/1.0 200")==NULL){  

            sleep(2);  

            memset(buf,0,sizeof(buf));  

            conn=open_url(targ->res->file_url);  

            goto begin_down;  

        }  

        //printf("##%s\n",body);  

        body=strstr(buf,"\r\n\r\n")+4;  

        if(body!=NULL){  

            i=i-(body-buf);  

            fwrite(body,sizeof(char),i,f);  

            //printf("@@@@%x\n",buf);  

            fflush(f);  

            ct+=i;  

            pthread_mutex_lock(&g_mut);  

            g_downloaded+=i;  

            pthread_mutex_unlock(&g_mut);  

              

            while(ct< targ->limit){  

                i=recv(conn.sock,buf,sizeof(buf),0);  

                if(i==0){  

                    fclose(f);  

                    conn.avaliable=0;  

                    while(!conn.avaliable){  

                        sleep(2);  

                        //printf("waiting\n");  

                        conn=open_url(targ->res->file_url);  

                    }  

                    memset(pack,0,sizeof(pack));  

                    memset(buf,0,sizeof(buf));  

                    sprintf(pack,"GET %s HTTP/1.1\nHost: %s\nAccept: */*\nReferer: http://%s\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)\nRange: bytes=%d-%d\nPragma: no-cache\nCache-Control: no-cache\n\n",conn.url_info.file,conn.url_info.host_name,conn.url_info.host_name,targ->start_pos+ct,targ->start_pos+targ->limit-1);  

                    f=fopen(targ->res->file_name,"rb+");  

                    fseek(f,targ->start_pos+ct,0);  

                    goto begin_down;  

                }  

                  

                fwrite(buf,sizeof(char),i,f);  

                fflush(f);  

                ct+=i;  

                pthread_mutex_lock(&g_mut);  

                g_downloaded+=i;  

                g_breakpoint.tasks[targ->no][0]=targ->start_pos+ct;  

                g_breakpoint.tasks[targ->no][1]=targ->limit-ct;  

                g_breakpoint.downloaded=g_downloaded;  

                pthread_mutex_unlock(&g_mut);  

            }  

            fclose(f);  

            g_breakpoint.downloaded=g_downloaded;  

            close(conn.sock);  

        }  

    }  

    pthread_exit(NULL);  

}  

void* monitor(void* args){  

    float p;  

    int i,j,z,old;  

    FILE* f;  

    char cfgName[256];  

    strcpy(cfgName,(char*)args);  

    strcat(cfgName,".cfg");  

      

    while(1){  

        p=g_downloaded/(g_total+0.0);  

        if(g_downloaded>=g_total)  

                break;  

        i=p*100/10;  

        if(old!=g_downloaded){  

              

  

            printf("\r");  

            for(j=0;j<i;j++){  

                printf("==");  

            }  

            printf("%2.0f%%",p*100);  

            fflush(stdout);  

          

            store_breakpoint(cfgName);  

            old=g_downloaded;  

        }  

    }  

    printf("\r====================100%%\n");  

    remove(cfgName);  

    pthread_exit(NULL);  

}  

  

  

void download(const char* url,int thread_amount,const char* file_name)  

{  

    ThreadArg targs[MAX_THREAD];  

    pthread_attr_t * thAttr = NULL;  

    pthread_t tids[MAX_THREAD],monitor_id,controler_id;  

    Resource res;  

    int i,block_size,t_start_pos,t_limit;  

    FILE* f;  

    char cfgName[256]={0};  

      

    if(thread_amount>MAX_THREAD)  

        return;  

    res=get_resource(url);  

      

    if(!strcmp(res.file_url,""))  

        return;  

      

    if(strcmp(file_name,""))  

        strcpy(res.file_name,file_name);  

      

    if(!strcmp(res.file_name,""))  

        strcpy(res.file_name,"default_down");  

      

    if(res.file_size<1000000)  

        thread_amount=1;  

      

    block_size=res.file_size/thread_amount;  

    pthread_mutex_init(&g_mut,NULL);  

      

    strcpy(cfgName,res.file_name);  

    strcat(cfgName,".cfg");  

    printf("downloading %s,%d bytes \n",res.file_name,res.file_size);  

      

    if(fopen(cfgName,"r")==NULL){  

new_task:         

        f=fopen(res.file_name,"wb");  

        if(f==NULL){  

            strcpy(res.file_name,"default_down");  

            f=fopen(res.file_name,"wb");  

        }  

        fclose(f);  

        g_total=res.file_size;  

  

        for(i=0;i<thread_amount;i++){  

            targs[i].res=&res;  

            targs[i].start_pos=block_size*i;  

            targs[i].limit=block_size;  

            if(i==thread_amount-1)  

                targs[i].limit+= (res.file_size%thread_amount);  

              

            targs[i].no=i;  

            g_breakpoint.tasks[i][0]=targs[i].start_pos;  

            g_breakpoint.tasks[i][1]=block_size;  

            pthread_create(&tids[i], thAttr, download_part, (void *)&targs[i]);  

        }  

          

    }  

    else{  

        f=fopen(cfgName,"r");  

        if(fscanf(f,"%d",&g_downloaded)==-1)  

            goto new_task;  

        //printf("#%d\n",g_downloaded);  

        g_total=res.file_size;  

        fscanf(f,"%d",&thread_amount);  

        for(i=0;i<thread_amount;i++){  

            fscanf(f,"%d-%d",&t_start_pos,&t_limit);  

            targs[i].res=&res;  

            targs[i].start_pos=t_start_pos;  

            targs[i].limit=t_limit;  

            targs[i].no=i;  

            g_breakpoint.tasks[i][0]=targs[i].start_pos;  

            g_breakpoint.tasks[i][1]=t_limit;  

            pthread_create(&tids[i], thAttr, download_part, (void *)&targs[i]);  

        }  

        fclose(f);  

    }  

      

    pthread_create(&monitor_id,NULL,monitor,(void *)res.file_name);  

    g_breakpoint.thread_amount=thread_amount;  

    g_breakpoint.downloaded=g_downloaded;  

    //printf("#%d\n",g_downloaded);  

    /*for(i=0;i<thread_amount;i++){  

        pthread_join(tids[i],NULL);  

    }*/  

  

    pthread_join(monitor_id,NULL);  

}  

  

  

  

int main (int ac, char * av[])  

{  

  int thread_amount=5;  

  char file_name[256]={0};  

  if(ac<2){  

        printf("usage: qdown URL [thread_amount] [save as]\n");  

        printf("example: qdown http://www.baidu.com/img/logo.gif 5 /home/sunjoy/log.gif\n");  

  }  

  else{  

        if(ac>=3)  

            sscanf(av[2],"%d",&thread_amount);  

        if(ac>=4){  

            strcpy(file_name,av[3]);  

        }  

        download(av[1],thread_amount,file_name);  

          

  }      
  return 0;  

}
