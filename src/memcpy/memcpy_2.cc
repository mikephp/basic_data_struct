#include <iostream>
#include <sys/time.h>
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{
	long len = 8192;
	int  loop = 200;
	char* p = new char[len];
	char* q = p;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	for (int i =0; i < loop; ++i)
	{
		char* p = new char[len];
		*p = char(i);
		memcpy(p, q, len);
		delete [] p;
	}
	gettimeofday(&end, NULL);
	cout <<"do memcpy speed:" << ((end.tv_sec - start.tv_sec)*1000 + double(end.tv_usec - start.tv_usec) / (len*loop/1000/1000) ) / loop<<" ms/MB\n";
	cout <<"each thread can do memcpy "<< double(len)*loop/1000/1000 / ((end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec) / 1000/1000) <<" MB\n";

	cout <<"time"<<  ((end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec)/1000/1000) <<"s\n";
}