/*
tab 选项卡 SysTabControl32
listbox 列表框 ListBox
progress 进度条 msctls_progress32
ScrollBar 滚动条 ScrollBar
listview 列表视图 SysListView32
treeview 树状图 SysTreeView32
updown 滚动选框 msctls_updown32
*/

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
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    //设置缓冲区
    static TCHAR szBufSex[10];
    static TCHAR szBufMarriage[10];
    static TCHAR szBufPet[20];
    static TCHAR szBufSubmit[100];
    static TCHAR szBufTmp[10];

    static HFONT hFont;  //逻辑字体
    //一组单选按钮
    static HWND labSex;  //静态文本框--性别
    static HWND radioMale;  //单选按钮--男
    static HWND radioFemale;  //单选按钮--女
    //一组单选按钮
    static HWND labMarriage;  //静态文本框--婚姻状况
    static HWND radioMarried;  //单选按钮--已婚
    static HWND radioSingle;  //单选按钮--未婚
    static HWND radioSecrecy;  //单选按钮--保密

    //一组复选框
    static HWND labPet;  //静态文本框--你的宠物
    static HWND checkboxDog;  //复选框--狗
    static HWND checkboxCat;  //复选框--猫
    static HWND checkboxFish;  //复选框--鱼
    static HWND checkboxOther;  //复选框--其他

    static HWND btnSubmit;  //按钮控件

    switch (message){
        case  WM_CREATE:
            hFont = CreateFont(
                -14, -7, 0, 0, 400,
                FALSE, FALSE, FALSE,DEFAULT_CHARSET,
                OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
                FF_DONTCARE, TEXT("微软雅黑")
            );

            //选择性别
            labSex = CreateWindow(
                TEXT("static"), TEXT("你的性别："),
                WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT/*文字居右*/,
                10, 10, 80, 26,
                hWnd, (HMENU)1, hInst, NULL
            );

            radioMale = CreateWindow(
                TEXT("button"), TEXT("男"),
                WS_CHILD | WS_VISIBLE | BS_LEFT/*文字居左*/ | BS_AUTORADIOBUTTON /*单选按钮*/ | WS_GROUP,
                95, 10, 50, 26,
                hWnd, (HMENU)2, hInst, NULL
            );

            radioFemale = CreateWindow(
                TEXT("button"), TEXT("女"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                150, 10, 50, 26,
                hWnd, (HMENU)2, hInst, NULL
            );

            //选择婚姻状况
            labMarriage = CreateWindow(
                TEXT("static"), TEXT("婚姻状况："),
                WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT,
                10, 40, 80, 26,
                hWnd, (HMENU)4, hInst, NULL
            );

            radioMarried = CreateWindow(
                TEXT("button"), TEXT("已婚"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON | WS_GROUP,
                95, 40, 65, 26,
                hWnd, (HMENU)5, hInst, NULL
            );

            radioSingle = CreateWindow(
                TEXT("button"), TEXT("未婚"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                165, 40, 65, 26,
                hWnd, (HMENU)6, hInst, NULL
            );

            radioSecrecy = CreateWindow(
                TEXT("button"), TEXT("保密"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
                235, 40, 100, 26,
                hWnd, (HMENU)7, hInst, NULL
            );

            //你的宠物
            labPet = CreateWindow(
                TEXT("static"), TEXT("你的宠物："),
                WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT,
                10, 70, 80, 26,
                hWnd, (HMENU)8, hInst, NULL
            );
           
            checkboxDog = CreateWindow(
                TEXT("button"), TEXT("狗"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX/*复选框*/,
                95, 70, 50, 26,
                hWnd, (HMENU)9, hInst, NULL
            );

            checkboxCat = CreateWindow(
                TEXT("button"), TEXT("猫"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX,
                150, 70, 50, 26,
                hWnd, (HMENU)10, hInst, NULL
            );

            checkboxFish = CreateWindow(
                TEXT("button"), TEXT("鱼"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX,
                205, 70, 50, 26,
                hWnd, (HMENU)11, hInst, NULL
            );
            checkboxOther = CreateWindow(
                TEXT("button"), TEXT("其他"),
                WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX,
                260, 70, 65, 26,
                hWnd, (HMENU)12, hInst, NULL
            );

            //创建按钮控件
            btnSubmit = CreateWindow(TEXT("button"), TEXT("提 交"),
                WS_CHILD | WS_VISIBLE | WS_BORDER | BS_FLAT,
                95, 110, 200, 36,
                hWnd, (HMENU)13, hInst, NULL
            );

            SendMessage(labSex, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(radioMale, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(radioFemale, WM_SETFONT, (WPARAM)hFont, NULL);

            SendMessage(labMarriage, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(radioMarried, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(radioSingle, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(radioSecrecy, WM_SETFONT, (WPARAM)hFont, NULL);

            SendMessage(labPet, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(checkboxDog, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(checkboxCat, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(checkboxFish, WM_SETFONT, (WPARAM)hFont, NULL);
            SendMessage(checkboxOther, WM_SETFONT, (WPARAM)hFont, NULL);

            SendMessage(btnSubmit, WM_SETFONT, (WPARAM)hFont, NULL);

            break;

        case WM_COMMAND:
            wmId    = LOWORD(wParam);
            wmEvent = HIWORD(wParam);
            if( wmEvent==  BN_CLICKED){
                switch (wmId){
                    //获取性别
                    case 2:
                    case 3:
                        GetWindowText((HWND)lParam, szBufSex, 10);
                        break;
                    //获取婚姻状况
                    case 5:
                    case 6:
                    case 7:
                        GetWindowText((HWND)lParam, szBufMarriage, 10);
                        break;
                    //获取宠物
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                        memset(szBufPet, 0, sizeof(szBufPet));
                        //是否选中狗
                        if( SendMessage(checkboxDog, BM_GETCHECK, 0, 0) == BST_CHECKED ){
                            GetWindowText(checkboxDog, szBufTmp,  10);
                            wsprintf(szBufPet, TEXT("%s  %s"), szBufPet, szBufTmp);
                        }
                        //是否选中猫
                        if( SendMessage(checkboxCat, BM_GETCHECK, 0, 0) == BST_CHECKED ){
                            GetWindowText(checkboxCat, szBufTmp, 10);
                            wsprintf(szBufPet, TEXT("%s  %s"), szBufPet, szBufTmp);
                        }
                        //是否选中鱼
                        if( SendMessage(checkboxFish, BM_GETCHECK, 0, 0) == BST_CHECKED ){
                            GetWindowText(checkboxFish, szBufTmp, 10);
                            wsprintf(szBufPet, TEXT("%s  %s"), szBufPet, szBufTmp);
                        }
                        //是否选中其他
                        if( SendMessage(checkboxOther, BM_GETCHECK, 0, 0) == BST_CHECKED ){
                            GetWindowText(checkboxOther, szBufTmp, 10);
                            wsprintf(szBufPet, TEXT("%s  %s"), szBufPet, szBufTmp);
                        }
                        break;

                    case 13:
                        wsprintf(szBufSubmit, TEXT("你的性别：%s\n婚姻状况：%s\r\n你的宠物：%s"), szBufSex, szBufMarriage, szBufPet);
                        MessageBox(hWnd, szBufSubmit, TEXT("信息提示"), MB_ICONINFORMATION);
                        break;

                    default:
                        return DefWindowProc(hWnd, message, wParam, lParam);
                }
            }
            return DefWindowProc(hWnd, message, wParam, lParam);

        case WM_DESTROY:
            DeleteObject(hFont);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}