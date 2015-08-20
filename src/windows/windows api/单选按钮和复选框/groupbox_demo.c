#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR szCmdLine,
    int iCmdShow
){
    static TCHAR szClassName[] = TEXT("Win32Demo");  //窗口类名
    HWND     hwnd;  //窗口句柄
    MSG      msg;  //消息
    WNDCLASS wndclass;  //窗口类

    hInst = hInstance;

    /**********第①步：注册窗口类**********/
    //为窗口类的各个字段赋值
    wndclass.style = CS_HREDRAW | CS_VREDRAW;  //窗口风格
    wndclass.lpfnWndProc  = WndProc;  //窗口过程
    wndclass.cbClsExtra   = 0;  //暂时不需要理解
    wndclass.cbWndExtra   = 0;  //暂时不需要理解
    wndclass.hInstance    = hInstance;  //当前窗口句柄
    wndclass.hIcon        = LoadIcon (NULL, IDI_APPLICATION);  //窗口图标
    wndclass.hCursor      = LoadCursor (NULL, IDC_ARROW);  //鼠标样式
    wndclass.hbrBackground= (HBRUSH) GetSysColorBrush(COLOR_BTNFACE);  //窗口背景画刷（灰色）
    wndclass.lpszMenuName = NULL ;  //窗口菜单
    wndclass.lpszClassName= szClassName;  //窗口类名
    //注册窗口
    RegisterClass(&wndclass);

    /*****第②步：创建窗口(并让窗口显示出来)*****/
    hwnd = CreateWindow(
        szClassName,  //窗口名字
        TEXT("Welcome"),  //窗口标题（出现在标题栏）
        WS_OVERLAPPEDWINDOW,  //窗口风格
        CW_USEDEFAULT,  //初始化时x轴的位置
        CW_USEDEFAULT,  //初始化时y轴的位置
        800,  //窗口宽度
        500,  //窗口高度
        NULL,  //父窗口句柄
        NULL,  //窗口菜单句柄
        hInstance,  //当前窗口的句柄
        NULL  //不使用该值
    );

    //显示窗口
    ShowWindow (hwnd, iCmdShow);
    //更新（绘制）窗口
    UpdateWindow (hwnd);

    /**********第③步：消息循环**********/
    while( GetMessage(&msg, NULL, 0, 0) ){
        TranslateMessage(&msg);  //翻译消息
        DispatchMessage (&msg);  //分派消息
    }

    return msg.wParam;
}

//窗口过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    PAINTSTRUCT ps;
    HDC hdc;

    static HFONT hFont;  //逻辑字体

    //一组单选按钮--性别
    static HWND hGroupSex;  //GroupBox 控件
    static HWND hRadioMale;  //单选按钮--男
    static HWND hRadioFemale;  //单选按钮--女

    //一组单选按钮--婚姻状况
    static HWND hGroupMarriage;
    static HWND hRadioMarried;  //单选按钮--已婚
    static HWND hRadioSingle;  //单选按钮--未婚
    static HWND hRadioSecrecy;  //单选按钮--保密

    //一组复选框--宠物
    static HWND hGroupPet;
    static HWND hCheckboxDog;  //复选框--狗
    static HWND hCheckboxCat;  //复选框--猫
    static HWND hCheckboxFish;  //复选框--鱼
    static HWND hCheckboxOther;  //复选框--其他

    switch (message){
        case  WM_CREATE:
            //创建字体
            hFont = CreateFont(
                -14, -7, 0, 0, 400,
                FALSE, FALSE, FALSE,DEFAULT_CHARSET,
                OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
                FF_DONTCARE, TEXT("微软雅黑")
            );

            //一组单选按钮--性别
            hGroupSex = CreateWindow(
                TEXT("button"), TEXT("你的性别"),
                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                20, 20, 300, 70,
                hWnd, (HMENU)1, hInst, NULL
            );
            hRadioMale = CreateWindow(
                TEXT("button"), TEXT("男"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON /*单选按钮*/,
                15, 30, 50, 26,
                hGroupSex /*父窗口为 GroupBox 控件*/, (HMENU)2, hInst, NULL
            );
            hRadioFemale = CreateWindow(
                TEXT("button"), TEXT("女"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                80, 30, 50, 26,
                hGroupSex, (HMENU)3, hInst, NULL
            );

            //一组单选按钮--婚姻状况
            hGroupMarriage = CreateWindow(
                TEXT("button"), TEXT("婚姻状况"),
                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                20, 120, 300, 70,
                hWnd, (HMENU)4, hInst, NULL
            );
            hRadioMarried = CreateWindow(
                TEXT("button"), TEXT("已婚"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                15, 30, 50, 26,
                hGroupMarriage, (HMENU)5, hInst, NULL
            );
            hRadioSingle = CreateWindow(
                TEXT("button"), TEXT("未婚"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                80, 30, 50, 26,
                hGroupMarriage, (HMENU)6, hInst, NULL
            );
            hRadioSecrecy = CreateWindow(
                TEXT("button"), TEXT("保密"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                145, 30, 50, 26,
                hGroupMarriage, (HMENU)7, hInst, NULL
            );

            //一组复选框--宠物
            hGroupPet = CreateWindow(
                TEXT("button"), TEXT("你的宠物"),
                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                20, 220, 300, 70,
                hWnd, (HMENU)8, hInst, NULL
            );
           
            hCheckboxDog = CreateWindow(
                TEXT("button"), TEXT("狗"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX /*复选框*/,
                15, 30, 50, 26,
                hGroupPet, (HMENU)9, hInst, NULL
            );

            hCheckboxCat = CreateWindow(
                TEXT("button"), TEXT("猫"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX,
                80, 30, 50, 26,
                hGroupPet, (HMENU)10, hInst, NULL
            );

            hCheckboxFish = CreateWindow(
                TEXT("button"), TEXT("鱼"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX,
                145, 30, 50, 26,
                hGroupPet, (HMENU)11, hInst, NULL
            );
            hCheckboxOther = CreateWindow(
                TEXT("button"), TEXT("其他"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX,
                210, 30, 65, 26,
                hGroupPet, (HMENU)12, hInst, NULL
            );
            
            //依次设置控件字体
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
            // TODO:  在此添加任意绘图代码...
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