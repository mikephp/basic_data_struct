void init_daemon()
{
int pid;
int i;
pid=fork();
if(pid<0)
    exit(1);  //���������˳�
else if(pid>0) //�������˳�
    exit(0);

setsid(); //ʹ�ӽ��̳�Ϊ�鳤
pid=fork();
if(pid>0)
    exit(0); //�ٴ��˳���ʹ���̲����鳤���������̾Ͳ���򿪿����ն�
else if(pid<0)
    exit(1);

//�رս��̴򿪵��ļ����
for(i=0;i<NOFILE;i++)
    close(i);
chdir("/root/test");  //�ı�Ŀ¼
umask(0);//�����ļ�����������
return;
}