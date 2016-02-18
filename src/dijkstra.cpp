// dihkstra.cpp : �������̨Ӧ�ó������ڵ㡣
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

int map[LEN][LEN]; //ĳ�㵽ĳ�������ĵľ���
int dist[LEN]; //��¼��ǰ�㵽Դ������·������
int mark[LEN]; //��������ĵ�ļ���

 

    //��ʼ��mapΪ�������
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

    //n:������· start:��ʼ�� 
    //dist[i],���洢��start ��i�����̾���
    void myDijstra(int n,int start)
    {
       int i,j,min,pos;
       for(i=1;i<=n;i++)
     {
            mark[i]=0;//û�е����
            dist[i]=map[start][i];//��start ������ dis[]��ʼ�� 
       }

       mark[start]=1;//����ʼ��ӽ���
       dist[start]=0;

       for(i=1;i<=n;i++)
     {

              min=MAX;
              for(j=1;j<=n;j++)
             {
                     if(!mark[j] && dist[j]<min)
                     { //ȡ������mark�����С��dist[i]
                         min=dist[j];
                         pos=j;//���
                     }

              }
                
             if(min==MAX)//�Ѿ�����ͨ��
                  break;

                mark[pos]=1;//��K�ӽ���

              //���ɳڲ���
              for(j=1;j<=n;j++)
             {
                    if(!mark[j] && dist[j]>dist[pos]+map[pos][j])//start->j or start->pos,pos->j 
                    {
                       dist[j]=dist[pos]+map[pos][j];//�ⲽ��prim�㷨�е㲻ͬ
                    }
              }
        }
    }

int main()
{
       int i,n,line;
       int a,b,d;
       scanf("%d%d",&n,&line);//������
       init();
       for(i=0;i<line;i++)
		{

			scanf("%d%d%d",&a,&b,&d); //������ߵ�Ȩֵ
			// ����ж��Ƿ�ֹ�رߵģ���ΪҲ����ͬһ��·��������ֵ����ʱ�ͱ����С��ֵ 
			if(map[a][b]>d)
			{
			map[a][b]=map[b][a]=d;
			}
		}

       myDijstra(n,1);//���÷���(��������ʼ��)

       //���1��5�����·��
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