#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

// 参数结构体 
struct argument
{
  int num;
  char string[30];
  int *fd;
};

// 声明两个线程函数
void *thread1_func( void * );

int main(int argc, char *argv[])
{
  char	buf[128];
  //定义两个线程标识符
  pthread_t thread1;
  //定义用来接收两个线程退出后的返回值,用作pthread_join的第二个参数
  void *thread1_return;
  //传递的参数结构体
  struct argument arg1;
  int i;
  int wait_thread_end; //判断线程退出成功与否
  //参数结构体值初始化
  arg1.num = 1949;
  strcpy( arg1.string, "中华人民共和国" ); 

  int fd[2];

  if(pipe(fd)<0){printf("create pipe error\n");}
  arg1.fd = &fd[0];
  // 创建两个线程
  pthread_create(&thread1, NULL, thread1_func, (void*)&arg1 );
  
    printf("我是最初的进程！\n");
    sleep(1);  //主统线程睡眠，调用其他线程
  
  strcpy(buf,"hello ljy");
  write(fd[1],buf,9);
  sleep(1);
  strcpy(buf,"q");
  write(fd[1],buf,1);
  sleep(1);
  //等待第一个线程退出，并接收它的返回值(返回值存储在thread1_return)
/*  wait_thread_end = pthread_join( thread1, &thread1_return );
  if( wait_thread_end != 0 ) 
  {
    printf("调用 pthread_join 获取线程1的返回值出现错误!\n");
  }
  else
  {
    printf("调用 pthread_join 成功！线程1退出后的返回值是 %d\n", (int)thread1_return);
  }
 */
  return 0;
}

/**
 *线程1函数实现 
 */
 void *thread1_func( void *arg )
{
  int i,cc;
  struct argument *arg_thread1; // 接收传递过来的参数结构体 
  arg_thread1 = ( struct argument * )arg;
  char buf[128];
  while(1)
  {
  	memset(buf,0,128);
    cc=read(*(int *)arg_thread1->fd,buf,128);
	if(cc){
    	printf( "我来自线程1，传递给我的参数是 %d, %s, %s\n", arg_thread1->num, arg_thread1->string,buf);
    }//sleep(1); // 投入睡眠，调用其它线程
	if('q'==buf[0]){
		break;
 		return (void *)1234;
	}
  }
}
