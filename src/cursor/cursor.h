// �����Ļ  
#define CLEAR() printf("\033[2J")  
  
// ���ƹ��  
#define MOVEUP(x) printf("\033[%dA", (x))  
  
// ���ƹ��  
#define MOVEDOWN(x) printf("\033[%dB", (x))  
  
// ���ƹ��  
#define MOVELEFT(y) printf("\033[%dD", (y))  
  
// ���ƹ��  
#define MOVERIGHT(y) printf("\033[%dC",(y))  
  
// ��λ���  
#define MOVETO(x,y) printf("\033[%d;%dH", (x), (y))  
  
// ��긴λ  
#define RESET_CURSOR() printf("\033[H")  
  
// ���ع��  
#define HIDE_CURSOR() printf("\033[?25l")  
  
// ��ʾ���  
#define SHOW_CURSOR() printf("\033[?25h")  
  
//����  
#define HIGHT_LIGHT() printf("\033[7m")  
#define UN_HIGHT_LIGHT() printf("\033[27m") 