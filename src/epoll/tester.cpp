#include "local.h"
#include "utils.h"

using namespace std;

char message[BUF_SIZE];     //接受服务器信息
list<int> list_of_clients;  //存放所有客户端
int res;
clock_t tStart;

int main(int argc, char *argv[])
{
    int sock; 
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    
    tStart = clock();

    for(int i=0 ; i<EPOLL_SIZE; i++)  //生成EPOLL_SIZE个客户端，这里是10000个，模拟高并发
    {
       CHK2(sock,socket(PF_INET, SOCK_STREAM, 0));
       CHK(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0);
       list_of_clients.push_back(sock);

       bzero(&message, BUF_SIZE);
       CHK2(res,recv(sock, message, BUF_SIZE, 0));
       printf("%s\n", message);
    }
   
    list<int>::iterator it;          //移除所有客户端
    for(it = list_of_clients.begin(); it != list_of_clients.end() ; it++)
       close(*it);

    printf("Test passed at: %.2f second(s)\n", (double)(clock() - tStart)/CLOCKS_PER_SEC); 
    printf("Total server connections was: %d\n", EPOLL_SIZE);
    
    return 0;
}