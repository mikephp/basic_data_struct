// dihkstra.cpp : 定义控制台应用程序的入口点。
// building envirenment vs 2010
//

#include "stdafx.h"

/*int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}*/
#include <iostream>

using namespace std;

#define MAX 9999999

#define LEN 210

int map[LEN][LEN]; //某点到某点两点间的的距离
int dist[LEN]; //记录当前点到源点的最短路径长度
int mark[LEN]; //加入进来的点的集合

 

    //初始化map为正无穷大
    void init()
    {
       int i,j;
       for(i=0;i<LEN;i++)
     {
              for(j=0;j<LEN;j++)
             {
                     map[i][j]=MAX;
              }
       }
    }

    //n:多少条路 start:起始点 
    //dist[i],最后存储着start 到i点的最短距离
    void myDijstra(int n,int start)
    {
       int i,j,min,pos;
       for(i=1;i<=n;i++)
     {
            mark[i]=0;//没有点加入
            dist[i]=map[start][i];//把start 附近点 dis[]初始化 
       }

       mark[start]=1;//把起始点加进来
       dist[start]=0;

       for(i=1;i<=n;i++)
     {

              min=MAX;
              for(j=1;j<=n;j++)
             {
                     if(!mark[j] && dist[j]<min)
                     { //取出不在mark里的最小的dist[i]
                         min=dist[j];
                         pos=j;//标记
                     }

              }
                
             if(min==MAX)//已经不能通了
                  break;

                mark[pos]=1;//把K加进来

              //做松弛操作
              for(j=1;j<=n;j++)
             {
                    if(!mark[j] && dist[j]>dist[pos]+map[pos][j])//start->j or start->pos,pos->j 
                    {
                       dist[j]=dist[pos]+map[pos][j];//这步跟prim算法有点不同
                    }
              }
        }
    }

int main()
{
       int i,n,line;
       int a,b,d;
       scanf("%d%d",&n,&line);//输入点和
       init();
       for(i=0;i<line;i++)
		{

			scanf("%d%d%d",&a,&b,&d); //输入各边的权值
			// 这个判断是防止重边的，因为也许会对同一条路给出两个值，这时就保存较小的值 
			if(map[a][b]>d)
			{
			map[a][b]=map[b][a]=d;
			}
		}

       myDijstra(n,1);//调用方法(点数，起始点)

       //输出1到5的最短路径
       cout<<dist[5]<<endl;

       return 0;
}

/*
5 7
1 2 10
1 4 30
1 5 100
2 3 50
3 5 10
4 3 20
4 5 60
60
*/