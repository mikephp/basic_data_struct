#include "local.h"
#include "utils.h"

using namespace std;

char message[BUF_SIZE];

/*
    流程：
        调用fork产生两个进程，两个进程通过管道进行通信
        子进程：等待客户输入，并将客户输入的信息通过管道写给父进程
        父进程：接受服务器的信息并显示，将从子进程接受到的信息发送给服务器
*/
int main(int argc, char *argv[])
{
    int sock, pid, pipe_fd[2], epfd;

    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_HOST);

    static struct epoll_event ev, events[2]; 
    ev.events = EPOLLIN | EPOLLET;

    //退出标志
    int continue_to_work = 1;

    CHK2(sock,socket(PF_INET, SOCK_STREAM, 0));
    CHK(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0);

    CHK(pipe(pipe_fd));
    
    CHK2(epfd,epoll_create(EPOLL_SIZE));

    ev.data.fd = sock;
    CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev));

    ev.data.fd = pipe_fd[0];
    CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, pipe_fd[0], &ev));

    // 调用fork产生两个进程
    CHK2(pid,fork());
    switch(pid)
    {
        case 0:                   // 子进程
            close(pipe_fd[0]);    // 关闭读端
            printf("Enter 'exit' to exit\n");
            while(continue_to_work)
            {
                bzero(&message, BUF_SIZE);
                fgets(message, BUF_SIZE, stdin);

                // 当收到exit命令时，退出
                if(strncasecmp(message, CMD_EXIT, strlen(CMD_EXIT)) == 0)
                {
                    continue_to_work = 0;
                }
                else
                {            
                    CHK(write(pipe_fd[1], message, strlen(message) - 1));
                }
            }
            break;
        default:                 // 父进程
            close(pipe_fd[1]);   // 关闭写端
            int epoll_events_count, res;
            while(continue_to_work) 
            {
                CHK2(epoll_events_count,epoll_wait(epfd, events, 2, EPOLL_RUN_TIMEOUT));

                for(int i = 0; i < epoll_events_count ; i++)
                {
                    bzero(&message, BUF_SIZE);
                    if(events[i].data.fd == sock)   //从服务器接受信息
                    {
                        CHK2(res,recv(sock, message, BUF_SIZE, 0));
                        if(res == 0)               //服务器已关闭
                        {
                            CHK(close(sock));
                            continue_to_work = 0;
                        }
                        else 
                        {
                            printf("%s\n", message);
                        }
                    }
                    else        //从子进程接受信息
                    {
                        CHK2(res, read(events[i].data.fd, message, BUF_SIZE));
                        if(res == 0)
                        {
                            continue_to_work = 0; 
                        }
                        else
                        {
                            CHK(send(sock, message, BUF_SIZE, 0));
                        }
                    }
                }
            }
    }
    if(pid)
    {
        close(pipe_fd[0]);
        close(sock);
    }else
    {
        close(pipe_fd[1]);
    }

    return 0;
}