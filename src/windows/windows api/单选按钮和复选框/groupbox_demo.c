#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR szCmdLine,
    int iCmdShow
){
    static TCHAR szClassName[] = TEXT("Win32Demo");  //��������
    HWND     hwnd;  //���ھ��
    MSG      msg;  //��Ϣ
    WNDCLASS wndclass;  //������

    hInst = hInstance;

    /**********�ڢٲ���ע�ᴰ����**********/
    //Ϊ������ĸ����ֶθ�ֵ
    wndclass.style = CS_HREDRAW | CS_VREDRAW;  //���ڷ��
    wndclass.lpfnWndProc  = WndProc;  //���ڹ���
    wndclass.cbClsExtra   = 0;  //��ʱ����Ҫ���
    wndclass.cbWndExtra   = 0;  //��ʱ����Ҫ���
    wndclass.hInstance    = hInstance;  //��ǰ���ھ��
    wndclass.hIcon        = LoadIcon (NULL, IDI_APPLICATION);  //����ͼ��
    wndclass.hCursor      = LoadCursor (NULL, IDC_ARROW);  //�����ʽ
    wndclass.hbrBackground= (HBRUSH) GetSysColorBrush(COLOR_BTNFACE);  //���ڱ�����ˢ����ɫ��
    wndclass.lpszMenuName = NULL ;  //���ڲ˵�
    wndclass.lpszClassName= szClassName;  //��������
    //ע�ᴰ��
    RegisterClass(&wndclass);

    /*****�ڢڲ�����������(���ô�����ʾ����)*****/
    hwnd = CreateWindow(
        szClassName,  //��������
        TEXT("Welcome"),  //���ڱ��⣨�����ڱ�������
        WS_OVERLAPPEDWINDOW,  //���ڷ��
        CW_USEDEFAULT,  //��ʼ��ʱx���λ��
        CW_USEDEFAULT,  //��ʼ��ʱy���λ��
        800,  //���ڿ��
        500,  //���ڸ߶�
        NULL,  //�����ھ��
        NULL,  //���ڲ˵����
        hInstance,  //��ǰ���ڵľ��
        NULL  //��ʹ�ø�ֵ
    );

    //��ʾ����
    ShowWindow (hwnd, iCmdShow);
    //���£����ƣ�����
    UpdateWindow (hwnd);

    /**********�ڢ۲�����Ϣѭ��**********/
    while( GetMessage(&msg, NULL, 0, 0) ){
        TranslateMessage(&msg);  //������Ϣ
        DispatchMessage (&msg);  //������Ϣ
    }

    return msg.wParam;
}

//���ڹ���
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    PAINTSTRUCT ps;
    HDC hdc;

    static HFONT hFont;  //�߼�����

    //һ�鵥ѡ��ť--�Ա�
    static HWND hGroupSex;  //GroupBox �ؼ�
    static HWND hRadioMale;  //��ѡ��ť--��
    static HWND hRadioFemale;  //��ѡ��ť--Ů

    //һ�鵥ѡ��ť--����״��
    static HWND hGroupMarriage;
    static HWND hRadioMarried;  //��ѡ��ť--�ѻ�
    static HWND hRadioSingle;  //��ѡ��ť--δ��
    static HWND hRadioSecrecy;  //��ѡ��ť--����

    //һ�鸴ѡ��--����
    static HWND hGroupPet;
    static HWND hCheckboxDog;  //��ѡ��--��
    static HWND hCheckboxCat;  //��ѡ��--è
    static HWND hCheckboxFish;  //��ѡ��--��
    static HWND hCheckboxOther;  //��ѡ��--����

    switch (message){
        case  WM_CREATE:
            //��������
            hFont = CreateFont(
                -14, -7, 0, 0, 400,
                FALSE, FALSE, FALSE,DEFAULT_CHARSET,
                OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
                FF_DONTCARE, TEXT("΢���ź�")
            );

            //һ�鵥ѡ��ť--�Ա�
            hGroupSex = CreateWindow(
                TEXT("button"), TEXT("����Ա�"),
                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                20, 20, 300, 70,
                hWnd, (HMENU)1, hInst, NULL
            );
            hRadioMale = CreateWindow(
                TEXT("button"), TEXT("��"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON /*��ѡ��ť*/,
                15, 30, 50, 26,
                hGroupSex /*������Ϊ GroupBox �ؼ�*/, (HMENU)2, hInst, NULL
            );
            hRadioFemale = CreateWindow(
                TEXT("button"), TEXT("Ů"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                80, 30, 50, 26,
                hGroupSex, (HMENU)3, hInst, NULL
            );

            //һ�鵥ѡ��ť--����״��
            hGroupMarriage = CreateWindow(
                TEXT("button"), TEXT("����״��"),
                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                20, 120, 300, 70,
                hWnd, (HMENU)4, hInst, NULL
            );
            hRadioMarried = CreateWindow(
                TEXT("button"), TEXT("�ѻ�"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                15, 30, 50, 26,
                hGroupMarriage, (HMENU)5, hInst, NULL
            );
            hRadioSingle = CreateWindow(
                TEXT("button"), TEXT("δ��"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                80, 30, 50, 26,
                hGroupMarriage, (HMENU)6, hInst, NULL
            );
            hRadioSecrecy = CreateWindow(
                TEXT("button"), TEXT("����"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                145, 30, 50, 26,
                hGroupMarriage, (HMENU)7, hInst, NULL
            );

            //һ�鸴ѡ��--����
            hGroupPet = CreateWindow(
                TEXT("button"), TEXT("��ĳ���"),
                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                20, 220, 300, 70,
                hWnd, (HMENU)8, hInst, NULL
            );
           
            hCheckboxDog = CreateWindow(
                TEXT("button"), TEXT("��"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX /*��ѡ��*/,
                15, 30, 50, 26,
                hGroupPet, (HMENU)9, hInst, NULL
            );

            hCheckboxCat = CreateWindow(
                TEXT("button"), TEXT("è"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX,
                80, 30, 50, 26,
                hGroupPet, (HMENU)10, hInst, NULL
            );

            hCheckboxFish = CreateWindow(
                TEXT("button"), TEXT("��"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX,
                145, 30, 50, 26,
                hGroupPet, (HMENU)11, hInst, NULL
            );
            hCheckboxOther = CreateWindow(
                TEXT("button"), TEXT("����"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX,
                210, 30, 65, 26,
                hGroupPet, (HMENU)12, hInst, NULL
            );
            
            //�������ÿؼ�����
            SendMessage(hGroupSex, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(hRadioMale, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(hRadioFemale, WM_SETFONT, (WPARAM)hFont, NULL);

            SendMessage(hGroupMarriage, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(hRadioMarried, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(hRadioSingle, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(hRadioSecrecy, WM_SETFONT, (WPARAM)hFont, NULL);

            SendMessage(hGroupPet, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(hCheckboxDog, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(hCheckboxCat, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(hCheckboxFish, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(hCheckboxOther, WM_SETFONT, (WPARAM)hFont, NULL);

            break;

        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            // TODO:  �ڴ���������ͼ����...
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}