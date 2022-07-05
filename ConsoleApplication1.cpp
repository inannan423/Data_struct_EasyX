//基于Easy X的用户图形界面的股票查询分析系统
#undef UNICODE  //解决EasyX的编码问题
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include "libxl.h"
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <time.h>
#include <conio.h>
#include<Windows.h>
#include <graphics.h> // 图形库
#include <conio.h>
#include <cstring>
using namespace libxl;
using namespace std;
//const wchar_t*   是一个指向宽字符类型的指针，这个指针指向的内容是个常量，不能改变


#define HASHSIZE 97 // 定义散列表长度
#define N 100
#define Maxfile 201
#define POINTS 61
#define DATES 171   //所有的日期
int A[POINTS][POINTS];
//辅助数组 克鲁斯卡尔 表示各个顶点所属的连通分量

typedef struct Stock{
    string nick;//股票名称
    string stockcode;//股票代码
    char date[100];	//日期
    char openprice[100]; //开盘价 
    char closeprice[100]; //收盘价 
    char maxprice[100];//最高价
    char minprice[100];//最低价 
    char agrees[100]; //成交量
    char agreemount[100]; //成交额
    char exchange[100]; //换手率 
    char updowns[100]; //涨跌额 
    char updrate[100]; //涨跌幅 
};

Stock stock[1000][1000];//创建对象,用于二叉排序树
//创建以stock结构体的全局对象，方便存储文件里的信息Stock stock[1000][1000]
//第一个括号代表存的第几个文件，后面的括号代表文件里面的第某条数据;
typedef struct BNode {
    string nick;//名称
    string stockcode;//股票代码
    struct BNode* lchild, * rchild;//左右孩子
}BNode, * Btree;

typedef struct Shares {
    string code;   //代码
    string nick;//公司简称
    string indus;   //行业代码
    string category1;   //一级门类
    string category2;   //二级门类
    string place;   //上市交易所
    string name;   //公司名称
    string date;    //上市日期
    string province;    //所在省份
    string city;    //所在城市
    string chairman;    //法人
    string address;     //公司地址
    string website;     //公司网址
    string mail;      //公司邮箱
    string tele;    //电话
    string work;    //业务
    string ranges;  //经营范围
    int has;//标记在3.4中已经存在
};
//链表结构体定义,哈希查找，由readexcel读入
typedef struct LNode {
    Shares data;
    struct LNode* next;
}LNode, * linklist;

//单链表查询结构体
typedef struct PNode {
    //建立新链表，用于查询4功能
    struct PNode* next;
    string code;
    string date;
    string nick;
    string str_openprice;
    string str_closeprice;
    string str_updrate;
    //三个string类型为了方便图形界面显示
    double openprice; //开盘价 
    double closeprice; //收盘价 
    double updrate; //涨跌幅 
    //类型与查询4、分析1挂钩
}PNode,*plist;
plist p[Maxfile];
//建立并初始化一个全局“链组”，先将文件信息读入二维数组，遍历二维数组，将其存的每个文件中存的所有日期信息不重复的给链组p；
//p的每个下标代表一个日期。再遍历所有文件，将每只股票（stock第一维度）中与p【n】对应的信息以链表的形式接在p【n】后面，此时存有日期信息的p【n】相当于一个链表的头指针。

//快排最大涨跌幅结构体
typedef struct FNode { //60支股票信息
    struct FNode* next;
    double maxupd; //最大涨跌幅 
    string code;//股票代码
    string date;//日期
    string name;//股票名称
    double score;//评分
    string cate1;//一级行业
    string recent;//最近一日的信息
    double closeprice;//最近一日的收盘价
    double number;//序号
}FNode, * Flist;



//图
typedef struct GNode {
    string name;//股票名称
    double arcs[POINTS][POINTS];
    int n;
    //图的点值
    struct GNode* next;
    double score[61];//Prim排序依据
}GNode,Graphst;

void InitFlist(Flist F);
void Kruskal(GNode g, int* neighbour, Flist l2);
void MiniSTree_Prim(GNode g, Flist l2);
void MiniSTree_show(GNode g, int* neighbour);
void BGraph(GNode g, Flist F);
void Btree_Delete(Btree &T);
void Btree_Delete_diguifind(Btree& T, string code);
void Btree_Delete_core(Btree& T,string code);
void display_screen();
void Floyd_Core(GNode& g, Flist& F);
void selectget1(plist l1, Btree& T, Flist& l, linklist L, int msgs);
double TurnStringDouble(string str, int n);
void display_head(plist &hp,Flist& F,Btree& T,linklist& L);
void Display();
void display_find(plist &hp,Flist &F,Btree &T, linklist &L);
void Hash_BasicInfo(plist &hp,Flist &F,Btree &T,linklist& L);
void ReadTxt(plist &hp,Flist &F,Btree &T,linklist L);
void display_Sort1(plist &hp,Flist &F ,Btree & T, linklist& L);
void display_analyse(plist &hp,Flist& F, Btree& T, linklist& L);
void priceana(plist& P, Flist& F, Btree& T, linklist& L);
void read4_2(Flist& l);
void Maxupd(plist& hp, Flist& l2, linklist& l, Btree& T);
void display_selectsort(plist& P, Btree& T, Flist& F, linklist& L);

//用于存储文件的标题
char title0[99],title1[99], title2[99], title3[99], title4[99],title5[99],title6[99],title7[99],title8[99],title9[99];//存放表头

//二部图着色
int color[500];
int Es[500][500];
//二部图
//克鲁斯卡尔
struct ARC {
    int v;  //起点
    int u;  //终点
    int w;  //权值
}arc2[3000];
//数据需要大一点，避免后面堆栈溢出

wchar_t* StringToWchar(const string& pKey)
{
    const char* pCStrKey = pKey.c_str();
    //第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
    int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
    wchar_t* pWCStrKey = new wchar_t[pSize];
    //第二次调用将单字节字符串转换成双字节字符串
    MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
    return pWCStrKey;
}

char* w2c(char* pcstr, const wchar_t* pwstr, size_t len)
//此函数支持中文读取和显示，需加入否则中文内容乱码
{
    int nlength = wcslen(pwstr);//获取转换后的长度

    int nbytes = WideCharToMultiByte(0, 0, pwstr, nlength, NULL, 0, NULL, NULL); //映射一个unicode字符串到一个多字节字符串

    if (nbytes > len)   nbytes = len;
    // 通过以上得到的结果，转换unicode 字符为ascii 字符
    WideCharToMultiByte(0, 0, pwstr, nlength, pcstr, nbytes, NULL, NULL);

    return pcstr;
}

//菜单单元栏的白色样式条
void AddStyle(int a, int b, int c, int d) {
    setfillcolor(WHITE);
    solidroundrect(a, b, c, d, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
}

void Init(linklist& L) {
    //链表初始化
    L = new LNode;
    L->next = NULL;
}

void InitG(Graphst& g) {
    //要对所有边的权值进行初始化
    for (int i = 1; i < 61; i++) {
        for (int j = 0; j < 61; j++) {
            g.arcs[i][j] = g.arcs[j][i] = 99.00;
        }
    }
}

//读取表内容并存入链表
void Read_Excel(linklist &L) {
    linklist t = new LNode;
    t = L;
    t -> next = NULL;
    Book* book = xlCreateXMLBook();
    //创建一个XLSX的实例,在使用前必须先调用这个函数创建操作excel的对象

    book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc");
    //设置相应的key激活相关功能(购买)，否则创建book无法成功

    if (book->load(L"A股公司简介.xlsx"))//文件放入工程目录里
    {
        Sheet* sheetread = book->getSheet(0);//选取第一个表

        if (sheetread)
        {
            for (int row = sheetread->firstRow()+1; row < sheetread->lastRow(); ++row)//行遍历
            {                                   //不读表头
                linklist q = new LNode;
                q->next = NULL;
                //所有列（一行）遍历完创建一个新节点
                for (int col = sheetread->firstCol(); col < sheetread->lastCol(); ++col)//列遍历
                {
                    CellType celltype = sheetread->cellType(row, col);//读取某行某列的单元格的类型。有字符串型，数值型，空值。

                    Format* format = sheetread->cellFormat(row, col);

                    if (celltype == CELLTYPE_STRING)//如果每个单元格内容为字符串类型
                    {
                        const wchar_t* t = sheetread->readStr(row, col);//从单元格中读取字符串及其格式

                        char* pcstr = (char*)malloc(sizeof(char) * (2 * wcslen(t) + 1));//定义空间大小来存放读出的内容，wcslen()函数返回输入参数t的长度

                        memset(pcstr, 0, 2 * wcslen(t) + 1);//初始化清零操作，开辟两倍+1的空间

                        w2c(pcstr, t, 2 * wcslen(t) + 1); //pcstr的内容读出来后要进行中文编码的转换，这个为转换函数

                        //out << pcstr << endl;//输出
                        
                        switch (col)    //根据列判断单元格类型
                        {
                            case 0:
                            {
                                q->data.code = pcstr;//股票代码
                                break;
                            }
                            case 1:
                            {
                                q->data.nick = pcstr;//简称
                                break;
                            }
                            case 2:
                            {
                                q->data.indus = pcstr;//行业代码
                                break;
                            }
                            case 3:
                            {
                                q->data.category1 = pcstr;//一级门类
                                break;
                            }
                            case 4:
                            {
                                q->data.category2 = pcstr;//二级门类
                                break;
                            }
                            case 5:
                            {
                                q->data.place = pcstr;//上市交易所
                                break;
                            }
                            case 6:
                            {
                                q->data.name = pcstr;//公司名称
                                break;
                            }
                            case 7:
                            {
                                q->data.date = pcstr;//上市日期
                                break;
                            }
                            case 8:
                            {
                                q->data.province = pcstr;//所在省份
                                break;
                            }
                            case 9:
                            {
                                q->data.city = pcstr;//城市
                                break;
                            }
                            case 10:
                            {
                                q->data.chairman = pcstr;//法人
                                break;
                            }
                            case 11:
                            {
                                q->data.address = pcstr;//地址
                                break;
                            }
                            case 12:
                            {
                                q->data.website = pcstr;//网址
                                break;
                            }
                            case 13:
                            {
                                q->data.mail = pcstr;//邮箱
                                break;
                            }
                            case 14:
                            {
                                q->data.tele = pcstr;//电话
                                break;
                            }
                            case 15:
                            {
                                q->data.work = pcstr;//主营业务
                                break;
                            }
                            case 16:
                            {
                                q->data.ranges = pcstr;//经营范围
                                break;
                            }
                        default:
                            break;
                        }
                        free(pcstr);
                    }
                    
                    /*
                    else if (celltype == CELLTYPE_NUMBER)//如果每个单元格内容为数值类型
                    {
                        double result = sheetread->readNum(row, col);

                        cout << result << endl;
                    }
                    else if (celltype == CELLTYPE_BLANK)//如果每个单元格内容为空：无值但存在单元格样式
                    {
                        cout << "\t\t" << endl;
                    }
                    else if (celltype == CELLTYPE_EMPTY)//不存在单元格样式
                    {
                        cout << "\t\t" << endl;
                    }
                   */

                }
                t->next = q;
                t = q;
            }
        }
        ////保存excel
        book->save(L"A股公司简介.xlsx");
        book->release();
    }
}

//哈希查找
void Hash_BasicInfo(plist &hp,Flist &F,Btree &T,linklist& L) {
    MOUSEMSG m;
    Read_Excel(L);//读取文件
    initgraph(1000, 700);
    // 设置背景色为白色
    //setbkcolor(WHITE);
    // 用背景色清空屏幕
    IMAGE background;//定义一个图片名.
    loadimage(&background, "图片2.png", 1000, 700, 1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)

    char C[100] = {};
    InputBox(C, 100, "请输入股票代码：");

    string code;
    code = C;
    //股票代码
    //cout << "请输入股票代码：";
    //cin >> code;

    //cout << endl; 


    int codenum = 0;//获取输入字符串的字符个数
    for (int j = 0; j < code.length(); j++) {
        codenum += int(code[j]);//强制类型转换
    }//获取ASCII码值之和
    linklist Hash[HASHSIZE];//97
    for (int i = 0; i < 97; i++) {
        Hash[i] = new LNode;
        Hash[i]->next = NULL;
    }//初始化97个线性表
    linklist q = new LNode;
    q = L->next;
    double asl = 0;//平均查找长度，初始化

    //·····创建哈希表，以下操作是将L的信息存入哈希表
    while (q) {
        //遍历q对应的链表L，在此前文件里的信息已经读入了L
        int sum=0;//字符值之和，初始化
        int stlen = q->data.code.length();
        //获取股票代码字符串的长度
        //字符值之和,string可以视作数组，直接用数组方法读取其每一个字符
        for (int k = 0; k < stlen; k++) {
            sum += int(q->data.code[k]); 
            //强制类型转化ascii码值相加
        }


        //对97取余数，获取关键字
        int key = sum % 97;//对97取余数
        //cout << sum<< "!!!!!!!!!!" << endl;
        linklist p = new LNode;
        p = Hash[key];
        //找到此条数据应该在哈希表中存储的位置
        while (p->next) {
            p = p->next;
            //遍历到该纵位上横位的最后一位，便于尾插
        }

        //建立新节点，尾插法插入到该结点的后面
        linklist n = new LNode;
        n->data = q->data;
        n->next = NULL;
        p->next = n;
        //尾插法
        q = q->next;
        //继续对下一条数据进行操作
    }
    //·····哈希表创建完毕

    //·····哈希表的查找
    
    //···求ASL ASl=查找次数/元素个数
    for (int i = 0; i < 97; i++){
        //平均查找长度的求值
        //遍历每哈希表每一个排头兵
        linklist ln = new LNode;
        double sum1 = 0;//初始化，计算单个排头兵后面的元素数

        ln =Hash[i]->next;//遍历哈希表每个排头兵后面跟的数据结点
        while (ln) {
            sum1++; //每个排头兵后面的节点数计算
            ln = ln->next;
        }
        if (sum1 != 0) {
            asl += ((1 + sum1) * sum1) / 2;//asl加上该排头兵的结点数
        }//asl要在下面除以200
    }
    //···ASL求值结束

    //···查找
    linklist s = new LNode;
    codenum = codenum % 97;
    //获取用户输入的字符串mod97判断应该在的哈希表中的位置
    s = Hash[codenum];//定义指针指向该位置的头节点
    int flag = 0;//定义是否查找成功的标志，初始化
    while (s->next) {
        //在此存储单元（排头兵）后面遍历所有的元素
        if (s->next->data.code == code) {
            //找到之后显示出来
            flag = 1;//已找到
            //显示模块，便于折叠写了判断
            if(1){
            setfillcolor(LIGHTBLUE);
            solidroundrect(0, 650, 800, 150, 0, 0);
            //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
            settextstyle(30, 0, "华文中宋");
            setbkmode(TRANSPARENT);
            setcolor(WHITE);
            outtextxy(20, 160, "找到如下结果");
            AddStyle(800, 200, 210, 150);

            settextstyle(26, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(20, 200, "股票名称:");
            //RECT r = { 0, 0, 639, 479 };
            char* C = new char[1024];
            strcpy(C, s->next->data.name.c_str());
            outtextxy(20, 230, C);

            settextstyle(26, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(20, 270, "股票代码:");
            strcpy(C, s->next->data.code.c_str());
            outtextxy(20, 300, C);

            settextstyle(26, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(20, 340, "所属一级行业:");
            strcpy(C, s->next->data.category1.c_str());
            outtextxy(20, 370, C);

            settextstyle(26, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(20, 410, "所属二级行业:");
            strcpy(C, s->next->data.category2.c_str());
            outtextxy(20, 440, C);

            settextstyle(26, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(20, 480, "主营业务:");
            strcpy(C, s->next->data.work.c_str());
            outtextxy(20, 510, C);
            }
            /*
            cout << "为您找到如下结果：" << endl;
            cout << "股票名称:" << s->next->data.name << endl;
            cout << "股票代码:" << s->next->data.code << endl;
            cout << "所属一级行业:" << s->next->data.category1 << endl;
            cout << "所属二级行业:" << s->next->data.category2 << endl;
            cout << "主营业务:" << s->next->data.work << endl;
            cout << "ASL:" << asl << endl;
            flag = 1;//查找成功*/
            break;
        }
        s = s->next;//遍历下一个结点
    }
    //·····查找结束
    //输出ASl
    settextstyle(26, 0, "楷体");
    setbkmode(TRANSPARENT);
    outtextxy(20, 550, "ASL:");
    sprintf(C, "%.3f", asl/200.0);
    outtextxy(20, 580, C);

    //未找到的提示
    if (flag == 0) {
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 280, 800, 150, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        outtextxy(20, 160, "未找到结果");
        AddStyle(800, 200, 210, 150);
    }
    //显示返回按钮
    setfillcolor(WHITE);
    solidroundrect(0, 50, 200, 110, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 64, "返回");
    setcolor(WHITE);
    //判断鼠标操作
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 200 && m.y >= 58 && m.y <= 110) {
            //查找
            //setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                display_find(hp,F,T,L);
                break;
            }
        }
    }
    closegraph();//关闭界面
}

//KMP字符串匹配
void Kmp_FindWeb(plist &hp,Flist &F,Btree &T,linklist& L) {
    //绘图
    MOUSEMSG m;
    initgraph(1000, 700);
    IMAGE background;//定义一个图片名.
    loadimage(&background, "图片2.png", 1000, 700, 1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
    int scrolly = 0;//用于翻页

    //用KMP算法查找website
    int flag = 0;//标记是否查找成功
    string webs;
    char C[100] = {};
    InputBox(C, 100, "请输入您想查询企业的英文名称或简写：");
    webs = C;
    //以上获取用户输入
    
    //下标从1开始，往后腾一位，方便主串从1开始匹配
    for (int o = webs.length(); o > 0; o--) {
        webs[o] = webs[o - 1];
    }

up:
    //翻页操作后重新绘图
    loadimage(&background, "图片2.png", 1000, 700, 1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
    setfillcolor(WHITE);
    solidroundrect(0, 50, 200, 110, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 64, "返回");
    setcolor(WHITE);


    //·····计算next数组
    int next[N] = { 0 };//初始化next数组
    next[1] = 0;//定义其第一位为0
    int i = 1;int j = 0;
    //i代表主串所在位置，j代表模式串所指位置，主串从1开始
    while (i < webs.length()) {
        //在未到尾部时
        if (j == 0 || webs[i] == webs[j]) {
            //此情况时，满足情况继续比较后继字符
            ++i; ++j;
            next[i] = j;
        }
        else j = next[j];//模式串向右移动
    }
    //·····next数组计算结束
    
    //·····KMP模式匹配算法
    linklist p = new LNode;
    p = L->next;
    //定义指向L的指针（L）中存有股票的基本信息

    int Height = 0;//界面用，用于循环后画间隔
    while (p) {
        //遍历表L
        int pos = 1;//主串定位指针
        j = 1;//模式串定位指针
        while (pos <= p->data.website.length() && j <= webs.length()) {
            //当两者均未比较到末尾
            if (j == 0 || p->data.website[pos] == webs[j]) {
                pos++;  
                j++;   //向后比较
            }
            else j = next[j];//模式串右移
        }
        //·····KMP模式匹配算法结束
        //判断是否查找成功
        if (j > webs.length()) {
            //j遍历完
            flag = 1;//查找成功
             //显示模块，为了折叠写的判断
            if (1) {
            //设置模块颜色为紫色
            setfillcolor(LIGHTBLUE);
            solidroundrect(0, 0, 1000, 160, 0, 0);
            settextstyle(26, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(20, 200+Height+ scrolly, "股票名称:");
            //RECT r = { 0, 0, 639, 479 };
            char* C = new char[1024];
            strcpy(C, p->data.name.c_str());
            outtextxy(20, 230 + Height+ scrolly, C);

            settextstyle(26, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(20, 270 + Height+ scrolly, "股票代码:");
            strcpy(C, p->data.code.c_str());
            outtextxy(20, 300 + Height+ scrolly, C);
            }
            //break; 
            //此处不应该跳出，因为一个关键字可能会有很多匹配项
            Height += 150;
        }
        p = p->next;//遍历下一条数据
    }
    if (flag == 0) {
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 280, 800, 150, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        outtextxy(20, 160, "未找到结果");
        AddStyle(800, 200, 210, 150);
    }
    setfillcolor(WHITE);
    solidroundrect(0, 50, 200, 110, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 64, "返回");
    setcolor(WHITE);

    if (flag) {//有结果才显示翻页
        setfillcolor(WHITE);
        solidroundrect(1200, 300, 900, 350, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(930, 310, "上翻");
        setcolor(WHITE);

        //这些地方可以简写，我都是复制的，所以懒得删了
        setfillcolor(WHITE);
        solidroundrect(1200, 370, 900, 420, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(930, 380, "下翻");
        setcolor(WHITE);
    }
    //判断鼠标操作
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 200 && m.y >= 58 && m.y <= 110) {
            //查找
            //setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                display_find(hp,F,T,L);
                break;
            }
        }
        else {
            if (flag) {
                //有结果才反应翻页指令
                if (m.x >= 890 && m.x <= 1300 && m.y >= 290 && m.y <= 350) {
                    //上翻指令捕获 solidroundrect(1200, 300, 900, 350, 0, 0);
                    //左上右下
                    if (m.uMsg == WM_LBUTTONDOWN && scrolly < 0) {		//如果按下鼠标左键实现相应功能.
                        scrolly = scrolly + 600;
                        //上翻，每条股票的位置均下移500,注意scrolly有个封顶值，不然会一直翻页空白页面
                        goto up;//修改scrolly相当于修改了上面的Height，回到原处继续执行
                        break;
                    }
                }
                else if (m.x >= 890 && m.x <= 1300 && m.y >= 360 && m.y <= 430) {
                    //下翻捕获指令 solidroundrect(1200, 370, 900, 420, 0, 0);;
                    //左上右下
                    if (m.uMsg == WM_LBUTTONDOWN && scrolly > -94300) {		//如果按下鼠标左键实现相应功能.
                        scrolly = scrolly - 600;
                        goto up;
                        break;
                    }
                }
            }
        }
    }
}

double BtreeASL(Btree T, double x) {
    if (T != NULL){
        x++;
        double num = x;
        if (T->lchild != NULL)
            x += BtreeASL(T->lchild, num);
        if (T->rchild != NULL)
            x += BtreeASL(T->rchild, num);
        return x;
    }
}

//二叉排序树·建树·递归
void InsertTree(Btree& t, string bstcode,string name) {
    //插入，根据股票代码大小排序
    if (!t) {
        //如t空，直接生成新节点
        Btree s = new BNode;//生成新节点
        s->stockcode = bstcode;
        s->nick = name;
        s->lchild = s->rchild = NULL;//新节点初始化左右子树为空
        t = s;//每次建立一个结点，s成为新的关键节点
        //ASll++;
    }

    if (bstcode < t->stockcode) {
        //左小右大，小的插入左子树
        //ASll++;
        InsertTree(t->lchild, bstcode,name);//递归插入
    }

    else if (bstcode > t->stockcode) {
        //大的插入右子树
        //ASll++;
        InsertTree(t->rchild, bstcode,name);//递归插入
    }
}
int flags = 1;
//二叉排序树·查找模块
int Findtree(Btree t, string s, double aasl) {
    int i;
    Btree ti = new BNode;
    
    ti = t;
    
    if (!t) {
        //树空报错
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 280, 800, 150, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        outtextxy(20, 160, "错误");
        AddStyle(800, 200, 210, 150);
        return -1;
    }
    else {//在建好的树中遍历
        if (s == t->stockcode) {
            //相等就刚好找到
            for (i = 0; i < 200; i++) {
                if (s == stock[i][0].stockcode)
                {//和读出来的二维数组比对股票代码，比对成功则输出
                    setfillcolor(LIGHTBLUE);
                    solidroundrect(0, 560, 800, 150, 0, 0);
                    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
                    settextstyle(30, 0, "华文中宋");
                    setbkmode(TRANSPARENT);
                    setcolor(WHITE);
                    char* C = new char[1024];
                    strcpy(C, stock[i][0].stockcode.c_str());
                    outtextxy(20, 160, C);
                    strcpy(C, stock[i][0].nick.c_str());
                    outtextxy(200, 160, C);
                    
                   // sprintf(C, "%.4f",ASll/ 200);
                    
                    AddStyle(800, 200, 410, 150);

                    settextstyle(26, 0, "楷体");
                    setbkmode(TRANSPARENT);
                    outtextxy(20, 300, "开盘价:");
                    outtextxy(20, 330, stock[i][0].openprice);

                    settextstyle(26, 0, "楷体");
                    setbkmode(TRANSPARENT);
                    outtextxy(20, 370, "收盘价:");
                    outtextxy(20, 400, stock[i][0].closeprice);

                    settextstyle(26, 0, "楷体");
                    setbkmode(TRANSPARENT);
                    outtextxy(20, 440, "涨跌幅:");
                    outtextxy(20, 470, stock[i][0].updrate);
                   
                }
            }
        }
        else if (s < t->stockcode) {
            //左小右大，在左子树中查找
            Findtree(t->lchild, s, aasl);
        }
        else if (s > t->stockcode) {
            //在右子树中查找
            Findtree(t->rchild, s,aasl);
        }
    }
    char* C = new char[1024];
    outtextxy(20, 200, "ASL:");
    sprintf(C, "%.4f", aasl);
    outtextxy(100, 200, C);
    return 0;
    closegraph();
}

//二叉排序树·主
void BuildTree(plist &hp,Flist &F,Btree& t, linklist L) {//二叉排序树的创建，股票代码从原来的链表中获取
    Display();
    ReadTxt(hp,F,t,L);
    //读取股票价格数据文件进二维数组
    MOUSEMSG m1;//获取鼠标表动作
    //二叉树的初始化
    t = new BNode;
    t = NULL;

    //·····以下遍历链表L将股票代码和名称插入到树中（建立二叉树）
    linklist p = new LNode;
    p = L->next;
    while (p){
        //遍历结束，每个节点都要插入到树中
        InsertTree(t, p->data.code,p->data.nick);
        p = p->next;//继续遍历下一个
    }
    //·····二叉树建立结束

    int flag = 1;
    while (flag) {
        //循环，直到找到为止，循环停止
        char C[100] = {};
        InputBox(C, 100, "请输入股票代码：");//获取用户输入
        string temp;
        temp = C;
        //以上获取用户输入
        double aasl = BtreeASL(t, 0) / 200.0;
        flag = Findtree(t, temp,aasl);//在建立好的二叉树中查找temp对应的股票代码
        //···以下，渲染删除功能的入口
        if (flag == 0) {
            solidroundrect(0, 50, 200, 110, 0, 0);
            settextstyle(30, 0, "华文中宋");
            setbkmode(TRANSPARENT);
            setcolor(LIGHTBLUE);
            outtextxy(20, 64, "返回");
            setcolor(WHITE);
            solidroundrect(220, 50, 400, 110, 0, 0);
            settextstyle(30, 0, "华文中宋");
            setbkmode(TRANSPARENT);
            setcolor(LIGHTBLUE);
            outtextxy(240, 64, "删除");
            setcolor(WHITE);
            //判断鼠标操作
            while (1) {
                m1 = GetMouseMsg();
                if (m1.x >= 0 && m1.x <= 200 && m1.y >= 58 && m1.y <= 110) {
                    if (m1.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                        display_find(hp,F,t, L);
                        break;
                    }
                }
                else if (m1.x >= 220 && m1.x <= 420 && m1.y >= 58 && m1.y <= 110) {
                    if (m1.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                        closegraph();
                        Btree_Delete(t);
                        break;
                    }
                }
            }
            closegraph();
        }
    }
    closegraph();
}

//读价格文件
//二叉排序树
 //创建以stock结构体的全局对象，方便存储文件里的信息Stock stock[1000][1000]，第一个括号代表存的第几个文件，后面的括号代表文件里面的第某条数据;
void ReadTxt(plist &hp,Flist &F,Btree &T,linklist L) {
    MOUSEMSG m1;
    linklist p = new LNode;
    p = L->next;
    char filep[Maxfile] = { 0 };
    int k = 0;
    int m = 0;//第m个文件
    while (p){
        string s ="股票交易日志\\"+p->data.code + ".txt";
        strcpy(filep, s.c_str());
        //c_str()函数用于转化string为char【】
        FILE* fp;//文件指针
        if ((fp = fopen(filep, "r")) == NULL){
            Display();
            setfillcolor(LIGHTBLUE);
            solidroundrect(0, 280, 800, 150, 0, 0);
            //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
            settextstyle(30, 0, "华文中宋");
            setbkmode(TRANSPARENT);
            setcolor(WHITE);
            outtextxy(20, 160, "文件错误");
            AddStyle(800, 200, 210, 150);
            setfillcolor(WHITE);
            solidroundrect(0, 50, 200, 110, 0, 0);
            settextstyle(30, 0, "华文中宋");
            setbkmode(TRANSPARENT);
            setcolor(LIGHTBLUE);
            outtextxy(20, 64, "返回");
            setcolor(WHITE);
            //判断鼠标操作
            while (1) {
                m1 = GetMouseMsg();
                if (m1.x >= 0 && m1.x <= 200 && m1.y >= 58 && m1.y <= 110) {
                    if (m1.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                        display_find(hp,F,T, L);
                        break;
                    }
                }
            }
            closegraph();
        }
        else{
            int line = 0;//n表示文件的第几行
            stock[m][0].stockcode = p->data.code;//链表的将股票代码赋值给price结构体数组里的股票代码
            stock[m][0].nick = p->data.nick;
            //读取文件对应的股票代码信息和名称信息
            int ff=fscanf(fp, "%s%s%s%s%s%s%s%s%s%s", title0, title1, title2, title3,title4, title5, title6, title7, title8, title9);
            //不读标题
            //以下，将文件里面的数据信息读入二维数组中
            while ((fscanf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
                stock[m][line].date, stock[m][line].openprice, stock[m][line].closeprice,
                stock[m][line].maxprice, stock[m][line].minprice, stock[m][line].agrees,
                stock[m][line].agreemount, stock[m][line].exchange, stock[m][line].updowns,
                stock[m][line].updrate)) != EOF){
                line++;//行++
            }
            fclose(fp);
        }
        p = p->next;
        m++;
    }
}


//把价格数据存入单链表,从readtolist独立出来的
int Creatlist(plist &hp,Flist &F,Btree& T, linklist L) {
    ReadTxt(hp,F,T, L);//将价格信息读入二维数组stock
    //建立一个全局的链组，这个链组序号代表其代表的文件
    for (int i = 0; i < Maxfile; i++) {
        p[i] = new PNode;
        p[i]->next = NULL;
    }//初始化链组，下一个结点置空
    //date为char类型数组

    int sumday = 0;//判断日期个数

    //·····遍历所有的文档，查找所有的日期
    for (int i = 0; i < Maxfile; i++) {
        //i标记是第几个文件
        for (int j = 0; j < Maxfile; j++) {
            //j标记是第i个文件中的第j行
            int flag = 1;//标记该日期未存在
            for (int h = 0; h < sumday; h++) {
                //判断该日期是否已经存在
                if (p[h]->date == stock[i][j].date) {
                    //与二维数组遍历比较，确定待查找的日期是否存在
                    flag = 0;//flag=0表示该日期存在
                    break;
                }
            }
            char voiding[10] = { '\0' };
            //判断是否为空行
            if (flag == 1 && strcmp(stock[i][j].date, voiding) != 0) {
                            //屏蔽文件中的空行信息
                p[sumday]->date = stock[i][j].date;
                //将第sumday个日期从全局二维数组中添加给链表
                sumday++;
            }

        }
    }
    //·····日期遍历完毕

    //以下将p【w】（日期）作为排头兵，把这个日期后的所有数据存到这个排头兵的后面
    int y;//y表示某个文件中（某只股票）存有的数据第几行
    for (int w = 0; w < sumday; w++) {
        //遍历L中存有的所有的日期
        for (int x = 0; x < Maxfile; x++) {
            //x标记这是那个文件（哪个股票）
            int fg = 0;//标记这个文件里面是否有这个日期
            for (y = 0; y < Maxfile; y++) {
                //遍历所有行
                if (p[w]->date == stock[x][y].date) {
                    fg = 1;//存在这个日期
                    break;
                }
            }

            if (fg == 1) {
                //如果存在这个日期
                //···以下建立新节点，将该日期对应的所有股票的有关信息存在这个日期的排头兵后面
                plist n = new PNode;
                n = p[w];
                while (n->next) {
                    n = n->next;
                    //遍历到该纵位上横位的最后一位
                }
                plist n1 = new PNode;
                n1->str_openprice = stock[x][y].openprice;
                n1->str_closeprice = stock[x][y].closeprice;
                n1->nick = stock[x][0].nick;
                n1->str_updrate = stock[x][y].updrate;
                n1->code = stock[x][0].stockcode;

                //1表示含%，0表示不含
                n1->updrate = TurnStringDouble(stock[x][y].updrate, 1);
                n1->openprice = TurnStringDouble(stock[x][y].openprice, 0);
                n1->closeprice = TurnStringDouble(stock[x][y].closeprice, 0);


                n1->next = NULL;
                n->next = n1;//尾插到地址的后面
                n = n1;
                //尾插法
            }
        }
    }
    return sumday;
}

//将string转为double类型
double TurnStringDouble(string str,int n) {
    string str2;
    if (n == 1) {
        str2 = str.replace(str.find("%"), 1, "");
            //string库函数，替换字符串中的%为空
    }
    else if (n == 0) {
        str2 = str;
    }
    double db= atof(const_cast<const char*>(str2.c_str()));
    //atof函数，将字符串或char数组转为double类型
    return db;
}

//3.4 单链表查询
//把价格数据存入单链表，并写入所有日期到单链表中
void readtolist(plist &hp,Flist &F,Btree &T,linklist L) {
    MOUSEMSG m;
    initgraph(1000, 700);
    IMAGE background;//定义一个图片名.
    loadimage(&background, "图片3.png", 1000, 700, 1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)

    int sumday=Creatlist(hp,F,T, L);
    //建表，建立一个全局链组，以日期为标杆，将日期对应的股票信息跟在这个标杆后面

    /*输出来看看
    cout << sumday << endl;
    for (int d = 0; d < sumday; d++) {
        cout << p[d]->date << endl;
        cout << "!" << endl;
   }*/

    //以下获取用户输入
    string day;
    char C[100] = {};
    InputBox(C, 100, "请输入您想查询的日期：");
    day = C;
    int scrolly = 0;//界面用，翻页时使用

wup:
    int isfind = 0;//判断是否找到
    //···以下判断此链表是否有此日期
    linklist lc;
    lc = L->next;
    linklist lo;
    lo = lc;
    while (lc) {
        lc->data.has = 0;
        lc = lc->next;
    }
    
    initgraph(1000, 700);
    loadimage(&background, "图片3.png", 1000, 700, 1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 0, 800, 160, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(WHITE);
    strcpy(C, day.c_str());
    outtextxy(20, 90, C);
    //AddStyle(800, 200, 210, 150);

    setfillcolor(WHITE);
    solidroundrect(0, 20, 200, 80, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 34, "返回");
    setcolor(WHITE);

    setfillcolor(WHITE);
    solidroundrect(1200, 300, 900, 350, 0, 0);
    //左上右下
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(930, 310, "上翻");
    setcolor(WHITE);

    setfillcolor(WHITE);
    solidroundrect(1200, 370, 900, 420, 0, 0);
    //左上右下
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(930, 380, "下翻");
    setcolor(WHITE);

    setfillcolor(WHITE);
    solidroundrect(1200, 440, 900, 490, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(930, 460, "继续");
    setcolor(WHITE);
    int ht = 200;
    linklist lu = lo;
    for (int sl = 0; sl < sumday; sl++) {
        if (p[sl]->date == day) {
            while (lo) {
                plist om = new PNode;
                om = p[sl]->next;
                while (om) {
                if (lo->data.nick == om->nick) {
                    cout << lo->data.nick << endl;
                    lo->data.has = 1;
                }
                om = om->next;
                }
                lo = lo->next;
            }
        }
    }
    while (lu) {
        if (lu->data.has == 0) {
            strcpy(C, lu->data.nick.c_str());
            settextstyle(20, 0, "楷体");
            outtextxy(25, ht+ scrolly, C);
            outtextxy(120, ht+ scrolly, "此日期不存在这个股票");
            cout <<lu->data.nick <<"此日期不存在这个股票"<<endl;
            ht += 30;
        }
        lu = lu->next;
    }
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 0, 800, 160, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(WHITE);
    strcpy(C, day.c_str());
    outtextxy(20, 90, C);
    //AddStyle(800, 200, 210, 150);

    setfillcolor(WHITE);
    solidroundrect(0, 20, 200, 80, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 34, "返回");
    setcolor(WHITE);

    setfillcolor(WHITE);
    solidroundrect(1200, 300, 900, 350, 0, 0);
    //左上右下
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(930, 310, "上翻");
    setcolor(WHITE);

    setfillcolor(WHITE);
    solidroundrect(1200, 370, 900, 420, 0, 0);
    //左上右下
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(930, 380, "下翻");
    setcolor(WHITE);

    setfillcolor(WHITE);
    solidroundrect(1200, 440, 900, 490, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(930, 450, "继续");
    setcolor(WHITE);
    //判断鼠标操作
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
            //返回 solidroundrect(0, 20, 200, 80, 0, 0);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                display_find(hp, F, T, L);
                break;
            }
        }
        else if (m.x >= 890 && m.x <= 1300 && m.y >= 290 && m.y <= 350) {
            //上翻指令捕获 solidroundrect(1200, 300, 900, 350, 0, 0);
            //左上右下
            if (m.uMsg == WM_LBUTTONDOWN && scrolly < 0) {		//如果按下鼠标左键实现相应功能.
                scrolly = scrolly + 500;
                goto wup;
                break;
            }
        }
        else if (m.x >= 890 && m.x <= 1300 && m.y >= 360 && m.y <= 430) {
            //下翻捕获指令 solidroundrect(1200, 370, 900, 420, 0, 0);;
            //左上右下
            if (m.uMsg == WM_LBUTTONDOWN && scrolly > -14300) {		//如果按下鼠标左键实现相应功能.
                scrolly = scrolly - 500;
                goto wup;
                break;
            }
        }
        else if (m.x >= 890 && m.x <= 1300 && m.y >= 430 && m.y <= 500) {
            //下翻捕获指令 solidroundrect(1200, 370, 900, 420, 0, 0);;
            //左上右	//如果按下鼠标左键实现相应功能.
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                break;
            }
        }
    }
//翻页用重新渲染标签
    scrolly = 0;
up:
    initgraph(1000, 700);
    loadimage(&background, "图片3.png", 1000, 700, 1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 0, 800, 160, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(WHITE);
    strcpy(C, day.c_str());
    outtextxy(20, 90, C);
    //AddStyle(800, 200, 210, 150);

    setfillcolor(WHITE);
    solidroundrect(0, 20, 200, 80, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 34, "返回");
    setcolor(WHITE);

    setfillcolor(WHITE);
    solidroundrect(1200, 300, 900, 350, 0, 0);
    //左上右下
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(930, 310, "上翻");
    setcolor(WHITE);

    setfillcolor(WHITE);
    solidroundrect(1200, 370, 900, 420, 0, 0);
    //左上右下
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(930, 380, "下翻");
    setcolor(WHITE);
    for (int b = 0; b < sumday; b++) {
        //遍历链组里面所有的日期
        if (p[b]->date == day) {
            //查找到与用户输入的日期对应的日期
            isfind = 1;//标记已经找到
            //建图，绘制相关UI信息，翻页按钮、返回按钮，为了折叠而写的判断
            if(1){
            settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(25, 140, "股票代码");

            //settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(179, 140, "股票名称");

            //settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(287, 140, "开盘价");

            //settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(387, 140, "收盘价");

            //settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(491, 140, "涨跌幅");
            }

            int Height = 160+ scrolly;//标记每条信息绘制的高度

            
            
            //定义新指针，以这个日期对应的标杆为头节点
            plist op = new PNode;
            op = p[b]->next;
            //输出这个日期后面所有的信息
            while (op) {
                //股票代码
                strcpy(C, op->code.c_str());
                outtextxy(25, Height, C);

                //股票名称
                strcpy(C, op->nick.c_str());
                outtextxy(179,Height,C);

                //开盘价
                strcpy(C, op->str_openprice.c_str());
                outtextxy(287, Height, C);

                //收盘价
                strcpy(C, op->str_closeprice.c_str());
                outtextxy(387, Height, C);

                //涨跌幅
                strcpy(C, op->str_updrate.c_str());
                outtextxy(491, Height, C);
                

                Height += 20;
                op = op->next;
                //cout << "\t" << op->nick << "\t" << op->openprice << "\t" << op->closeprice << "\t" << op->updrate << endl;
            }
            //再次绘制UI，避免翻页后原UI被覆盖，为折叠写的判断
            if (1) {            
            setfillcolor(LIGHTBLUE);
            solidroundrect(0, 0, 800, 160, 0, 0);
            //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
            settextstyle(30, 0, "华文中宋");
            setbkmode(TRANSPARENT);
            setcolor(WHITE);
            strcpy(C, day.c_str());
            outtextxy(20, 90, C);
            setfillcolor(WHITE);
            solidroundrect(0, 20, 200, 80, 0, 0);
            settextstyle(30, 0, "华文中宋");
            setbkmode(TRANSPARENT);
            setcolor(LIGHTBLUE);
            outtextxy(20, 34, "返回");
            setcolor(WHITE);
            settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(25, 140, "股票代码");

            //settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(179, 140, "股票名称");

            //settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(287, 140, "开盘价");

            //settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(387, 140, "收盘价");

            //settextstyle(20, 0, "楷体");
            setbkmode(TRANSPARENT);
            outtextxy(491, 140, "涨跌幅");
            }
            //判断鼠标操作
            while (1) {
                m = GetMouseMsg();
                if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
                    //返回 solidroundrect(0, 20, 200, 80, 0, 0);
                    if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                        display_find(hp,F,T, L);
                        break;
                    }
                }
                else if (m.x >= 890 && m.x <= 1300 && m.y >= 290 && m.y <= 350) {
                    //上翻指令捕获 solidroundrect(1200, 300, 900, 350, 0, 0);
                    //左上右下
                    if (m.uMsg == WM_LBUTTONDOWN && scrolly < 0) {		//如果按下鼠标左键实现相应功能.
                        scrolly = scrolly + 500;
                        goto up;
                        break;
                    }
                }
                else if (m.x >= 890 && m.x <= 1300 && m.y >= 360 && m.y <= 430) {
                    //下翻捕获指令 solidroundrect(1200, 370, 900, 420, 0, 0);;
                    //左上右下
                    if (m.uMsg == WM_LBUTTONDOWN && scrolly > -4300) {		//如果按下鼠标左键实现相应功能.
                        scrolly = scrolly - 500;
                        goto up;
                        break;
                    }
                }
            }

            break;
        }
    }
    
    //未找到显示提示Ui
    if (isfind == 0) {
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 280, 800, 150, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        outtextxy(20, 160, "未找到结果");
        AddStyle(800, 200, 210, 150);
        setfillcolor(WHITE);
        solidroundrect(0, 20, 200, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(20, 34, "返回");
        setcolor(WHITE);
        while (1) {
            m = GetMouseMsg();
            if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 80) {
                //返回
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    display_find(hp,F,T, L);
                    break;
                }
            }
        }
    }
    closegraph();
}
//封装建图
void Display() {
    // 初始化绘图窗口
    initgraph(1000, 700);
    // 设置背景色为白色
    //setbkcolor(WHITE);
    // 用背景色清空屏幕
    IMAGE background;//定义一个图片名.
    loadimage(&background, "图片1.png", 1000, 700, 1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
}

//排序1写入文件
void WriteToFile(plist &t,int line,int how) {
    //how判断采取何种关键字排序
    int i = line;
    //获取文件的行数
    t = t->next;
    Book* book = xlCreateXMLBook();//创建一个XLSX的实例,在使用前必须先调用这个函数创建操作excel的对象
    book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc");//设置用户注册信息，是否正版就在这里验证，否则第一行无法操作
    if (book) {
        Sheet* sheetwrite = book->addSheet(L"sheet1");//生成第一个表
        if (sheetwrite)
        {
            int n = 6;//列数 
            for (int row = 0; row < i; ++row){
                //行对应股票
                for (int col = 0; col < n; ++col){
                    //列对应标题
                    if (col == 0) {
                        sheetwrite->writeNum(row, col, row);
                    }
                    else if (col == 1) {
                        //第一列 股票代码
                        wchar_t* wchar = StringToWchar(t->code);
                        sheetwrite->writeStr(row, col, wchar);
                        delete[]wchar;
                    }
                    else if (col == 2) {
                        //第2列 股票名
                        wchar_t* wchar = StringToWchar(t->nick);
                        sheetwrite->writeStr(row, col, wchar);
                        delete[]wchar;
                    }
                    else if (col == 3) {
                        //第3列 开盘价
                        sheetwrite->writeNum(row, col, t->openprice);
                    }
                    else if (col == 4) {
                        //第4列 收盘价
                        sheetwrite->writeNum(row, col, t->closeprice);
                    }
                    else if (col == 5) {
                        //第5列 涨跌幅
                        //为数据添加百分号
                        double upra = t->updrate;
                        char ups[50] = { '\0' };
                        sprintf_s(ups, "%.2f", upra);
                        string str1 = ups;
                        string str2 = str1 + '%';
                        wchar_t* str3 = StringToWchar(str2);
                        sheetwrite->writeStr(row, col, str3);
                    }
                }
                t = t->next;
            }
        }
        //保存excel
        if (how == 1&&book->save(L"开盘价直接插入排序结果.xlsx"))
        {
            //::ShellExecute(NULL, "open", "example.xls", NULL, NULL, SW_SHOW);
        }
        else if (how == 2 && book->save(L"收盘价直接插入排序结果.xlsx"))
        {
            //::ShellExecute(NULL, "open", "example.xls", NULL, NULL, SW_SHOW);
        }
        if (how == 3 && book->save(L"涨跌幅直接插入排序结果.xlsx"))
        {
            //::ShellExecute(NULL, "open", "example.xls", NULL, NULL, SW_SHOW);
        }
        else
        {
            std::cout << book->errorMessage() << std::endl;
        }
    }
    book->release();
}


//分析1 插入排序
void InsertSort(plist hp, Flist& F, Btree& T, linklist& L, int how) {
    //how标记是用什么为关键字排序
    Display();
    Creatlist(hp,F,T,L);//建表,创建全局链组p
    
    MOUSEMSG m;
    string dates;//输入的日期
    int z;  //作为标志查找是否存在这个日期
    int flag = 0;  //是否存在标志
    //获取用户输入
    char C[100] = {};
    InputBox(C, 100, "请输入日期：");
    dates = C;

    //遍历查找p【】，直到找到该日期对应的链表
    for (z = 0; z < 200; z++) {//遍历查找
        if (dates == p[z]->date) {//如果日期相等的话
            flag = 1;//找到
            break;
        }
    }

    //该日期不存在，显示提示界面
    if (flag == 0) {//如果不存在的话
        //UIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUI
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 280, 800, 150, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        outtextxy(20, 160, "未找到结果");
        AddStyle(800, 200, 210, 150);
        
        while (1) {
            m = GetMouseMsg();
            if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 80) {
                //返回
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    display_find(hp,F,T, L);
                    break;
                }
            }
        }
        //UIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUI
    }
    else {
        //日期存在
        plist t = new PNode;   //t存表头，即查找到的对应日期的表头
        t = p[z];//t相当于表头
        plist q = new PNode;
        //将t->next作为哨兵位，其后置空
        //q存储原链表中的后面的所有数值
        q = t->next->next;  //已排好序列的头,下面初始化头t->next->next=NULL
        t->next->next = NULL;    //初始化头t->next->next=NULL
        while (q) {
            //遍历q，待排序的数值
            plist t1 = new PNode;
            t1 = t;      //t1是排好序区域的工作指针
            plist t2 = new PNode;
            t2 = q->next;//t2是待排序区域的工作指针
          
            switch (how) {
                //判断排序方式
            case 1: {
                while (t1->next != NULL && t1->next->openprice > q->openprice) {
                    //大排在前面，找到应该插入的位置
                    t1 = t1->next;
                }
                //以下将待排的插入到指定位置
                q->next = t1->next;
                t1->next = q;
                q = t2;
                break;
            }
            case 2: {
                while (t1->next != NULL && t1->next->closeprice > q->closeprice) {
                    //大排在前面
                    t1 = t1->next;
                }
                q->next = t1->next;
                t1->next = q;
                q = t2;
                break;
            }
            case 3: {
                while (t1->next != NULL && t1->next->updrate > q->updrate) {
                    //大排在前面
                    t1 = t1->next;
                }
                q->next = t1->next;
                t1->next = q;
                q = t2;
                break;
            }
            }
        }
        plist js = new PNode;   //t存表头
        js = p[z];
        js = t->next;

        //以下获取数据总行数
        int ho = 0;
        while (js) {
            js = js->next;
            ho++;
        }
        plist ks = new PNode;   //t存表头
        ks = t;
        WriteToFile(ks, ho, how);//写入文件
        int scrolly = 0;

        
    fromhere:
        if (1) {
        //UIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUI
        initgraph(1000, 700);
        IMAGE background;//定义一个图片名.
        loadimage(&background, "图片3.png", 1000, 700, 1);//从图片文件获取图像
        putimage(0, 0, &background);
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 0, 800, 160, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        char* C = new char[1024];
        strcpy(C, dates.c_str());
        outtextxy(20, 90, C);
        //AddStyle(800, 200, 210, 150);

        setfillcolor(WHITE);
        solidroundrect(0, 20, 200, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(20, 34, "返回");
        setcolor(WHITE);

        /*
        setfillcolor(WHITE);
        solidroundrect(220, 20, 420, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(240, 34, "保存");
        setcolor(WHITE);
        */

        setfillcolor(WHITE);
        solidroundrect(1200, 300, 900, 350, 0, 0);
        //左上右下
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(930, 310, "上翻");
        setcolor(WHITE);

        setfillcolor(WHITE);
        solidroundrect(1200, 370, 900, 420, 0, 0);
        //左上右下
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(930, 380, "下翻");
        setcolor(WHITE);

        settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(25, 140, "序号");

        setbkmode(TRANSPARENT);
        outtextxy(109, 140, "股票代码");

        setbkmode(TRANSPARENT);
        outtextxy(247, 140, "股票名称");

        setbkmode(TRANSPARENT);
        outtextxy(387, 140, "开盘价");

        setbkmode(TRANSPARENT);
        outtextxy(491, 140, "收盘价");

        setbkmode(TRANSPARENT);
        outtextxy(594, 140, "涨跌幅");
        }
        int Height = 160 + scrolly;
        //UIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUI

        
        plist pl = new PNode;   //t存表头
        pl = p[z];
        pl = t->next;
        
        int i = 0;
        while (pl) {
            //cout << i << "\t股票代码：" << pl->code << "\t股票名称：" << pl->nick << "\t开盘价：" << pl->openprice << "\t收盘价：" << pl->closeprice << "\t涨跌幅：" << pl->updrate << "%" << endl;
            //序号
            _itoa(i,C, 10);
            //char k = char(i);
            outtextxy(25, Height, C);
            
            //股票代码
            strcpy(C, pl->code.c_str());
            outtextxy(95, Height, C);

            //股票名称
            strcpy(C, pl->nick.c_str());
            outtextxy(239, Height, C);

            //开盘价
            strcpy(C, pl->str_openprice.c_str());
            outtextxy(387, Height, C);

            //收盘价
            strcpy(C, pl->str_closeprice.c_str());
            outtextxy(491, Height, C);

            //涨跌幅
            strcpy(C, pl->str_updrate.c_str());
            outtextxy(571, Height, C);
            Height += 20;
            pl = pl->next;
            i++;
        }
        if(1){
        //UIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUI
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 0, 800, 160, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        //char* C = new char[1024];
        strcpy(C, dates.c_str());
        outtextxy(20, 90, C);
        //AddStyle(800, 200, 210, 150);

        setfillcolor(WHITE);
        solidroundrect(0, 20, 200, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(20, 34, "返回");
        setcolor(WHITE);

        /*
        setfillcolor(WHITE);
        solidroundrect(220, 20, 420, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(240, 34, "保存");
        setcolor(WHITE);
        */

        setfillcolor(WHITE);
        solidroundrect(1200, 300, 900, 350, 0, 0);
        //左上右下
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(930, 310, "上翻");
        setcolor(WHITE);

        setfillcolor(WHITE);
        solidroundrect(1200, 370, 900, 420, 0, 0);
        //左上右下
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(930, 380, "下翻");
        setcolor(WHITE);

        settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(25, 140, "序号");

        setbkmode(TRANSPARENT);
        outtextxy(109, 140, "股票代码");

        setbkmode(TRANSPARENT);
        outtextxy(247, 140, "股票名称");

        setbkmode(TRANSPARENT);
        outtextxy(387, 140, "开盘价");

        setbkmode(TRANSPARENT);
        outtextxy(491, 140, "收盘价");

        setbkmode(TRANSPARENT);
        outtextxy(594, 140, "涨跌幅");
        }
        //判断鼠标操作
        while (1) {
            m = GetMouseMsg();
            if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
                //返回 solidroundrect(0, 20, 200, 80, 0, 0);
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    priceana(hp,F,T,L);
                    break;
                }
            }
            else if (m.x >= 890 && m.x <= 1300 && m.y >= 290 && m.y <= 350) {
                //上翻指令捕获 solidroundrect(1200, 300, 900, 350, 0, 0);
                //左上右下
                if (m.uMsg == WM_LBUTTONDOWN && scrolly < 0) {		//如果按下鼠标左键实现相应功能.
                    scrolly = scrolly + 500;
                    goto fromhere;
                    break;
                }
            }
            else if (m.x >= 890 && m.x <= 1300 && m.y >= 360 && m.y <= 430) {
                //下翻捕获指令 solidroundrect(1200, 370, 900, 420, 0, 0);;
                //左上右下
                if (m.uMsg == WM_LBUTTONDOWN && scrolly > -4300) {		//如果按下鼠标左键实现相应功能.
                    scrolly = scrolly - 500;
                    goto fromhere;
                    break;
                }
            }
            //solidroundrect(220, 20, 420, 80, 0, 0);
        }//UIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUIUI

        closegraph();
    }
}

void display_find(plist &hp,Flist &F,Btree& T,linklist& L) {
    Display();
    
    MOUSEMSG m;
    //基本信息查询 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 200, 200, 150, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 160, "查询基本信息");
    AddStyle(200, 200, 210, 150);

    //网址查询 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 280, 200, 230, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 240, "股票网址查询");
    AddStyle(200, 280, 210, 230);

    //价格查询 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 360, 200, 310, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 320, "最近价格查询");
    AddStyle(200, 360, 210, 310);

    //价格查询 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 440, 200, 390, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 400, "某日价格查询");
    AddStyle(200, 440, 210, 390);

    //返回 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 520, 200, 470, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 480, "返回上一级");
    AddStyle(200, 520, 210, 470);

    settextstyle(80, 0, "华文中宋");
    outtextxy(20, 20, "股票查询 Stock Query");
    settextstyle(20, 0, "仿宋");
    outtextxy(700, 740, "计算机类20-4 201002423 程子涵");
    Init(L);
    Read_Excel(L);
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 300 && m.y >= 130 && m.y <= 210) {
            //哈希查找
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                Hash_BasicInfo(hp,F,T,L);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 210 && m.y <= 290) {
            //KMP查找
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                Kmp_FindWeb(hp,F,T,L); 
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 290 && m.y <= 370) {
            //二叉树查找
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                BuildTree(hp,F,T, L);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 370 && m.y <= 450) {
            //单链表查找
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                readtolist(hp,F,T, L);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 450 && m.y <= 530) {
            //返回上一级
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                display_head(hp,F,T,L);
                break;
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
            //setlinecolor(WHITE);
            //rectangle(90, 95, 310, 160);
        }
    }
}


//日期分析菜单
void priceana(plist &P,Flist &F,Btree &T,linklist &L) {
    Display();

    MOUSEMSG m;
    //分析1  CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 200, 200, 150, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 160, "直接插入排序");
    AddStyle(200, 200, 210, 150);

    //相关性计算 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 280, 200, 230, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 240, "快速排序");
    AddStyle(200, 280, 210, 230);

    //基金筛选 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 360, 200, 310, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 320, "简单选择排序");
    AddStyle(200, 360, 210, 310);

    //返回 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 440, 200, 390, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 400, "返回上一级");
    AddStyle(200, 440, 210, 390);

    settextstyle(80, 0, "华文中宋");
    outtextxy(20, 20, "股票价格分析");
    settextstyle(20, 0, "仿宋");
    outtextxy(700, 740, "计算机类20-4 201002423 程子涵");
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 300 && m.y >= 130 && m.y <= 210) {
            //价格分析
            setlinecolor(LIGHTBLUE);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                display_Sort1(P,F,T, L);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 210 && m.y <= 290) {
            //快速排序
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                Maxupd(P,F, L, T);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 290 && m.y <= 370) {
            //简单选择排序
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                display_selectsort(P, T, F, L);
                //Flist& F, Btree& T, linklist& L
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 370 && m.y <= 450) {
            //返回上一级
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                display_analyse(P,F,T, L);
                break;
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
            //setlinecolor(WHITE);
            //rectangle(90, 95, 310, 160);
        }
    }
}

void display_analyse(plist &hp,Flist &F,Btree &T,linklist &L) {
    Display();

    MOUSEMSG m;
    //分析 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 200, 200, 150, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 160, "股票价格分析");
    AddStyle(200, 200, 210, 150);

    //相关性计算 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 280, 200, 230, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 240, "相关性计算");
    AddStyle(200, 280, 210, 230);

    //基金筛选 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 360, 200, 310, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 320, "股票基金筛选");
    AddStyle(200, 360, 210, 310);

    //返回 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 440, 200, 390, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 400, "返回上一级");
    AddStyle(200, 440, 210, 390);

    settextstyle(80, 0, "华文中宋");
    outtextxy(20, 20, "股票分析 Stock Analyse");
    settextstyle(20, 0, "仿宋");
    outtextxy(700, 740, "计算机类20-4 201002423 程子涵");
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 300 && m.y >= 130 && m.y <= 210) {
            //价格分析
            setlinecolor(LIGHTBLUE);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                plist pu = new PNode;
                pu->next = NULL;
                Flist f = new FNode;
                InitFlist(f);
                Btree tr=new BNode;
                linklist li;
                Init(li);
                Read_Excel(li);
                read4_2(f);
                priceana(pu,f,tr,li);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 210 && m.y <= 290) {
            //相关性计算
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                GNode g;
                InitG(g);
                Flist f = new FNode;
                InitFlist(f);
                Floyd_Core(g, f);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 290 && m.y <= 370) {
            //基金筛选
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                display_screen();
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 370 && m.y <= 450) {
            //返回上一级
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                plist pu = new PNode;
                pu->next = NULL;
                Flist f = new FNode;
                InitFlist(f);
                Btree tr = new BNode;
                linklist li;
                Init(li);
                display_head(pu,f,tr, li);
                break;
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
            //setlinecolor(WHITE);
            //rectangle(90, 95, 310, 160);
        }
}
}

//4.1的三个排序菜单
void display_Sort1(plist &hp,Flist &F,Btree& T, linklist& L) {
    Display();

    MOUSEMSG m;
    //分析1  CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 200, 200, 150, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 160, "开盘价降序");
    AddStyle(200, 200, 210, 150);

    //相关性计算 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 280, 200, 230, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 240, "收盘价降序");
    AddStyle(200, 280, 210, 230);

    //基金筛选 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 360, 200, 310, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 320, "涨跌幅降序");
    AddStyle(200, 360, 210, 310);

    //返回 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 440, 200, 390, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 400, "返回上一级");
    AddStyle(200, 440, 210, 390);

    settextstyle(80, 0, "华文中宋");
    outtextxy(20, 20, "直接插入排序");
    settextstyle(20, 0, "仿宋");
    outtextxy(700, 740, "计算机类20-4 201002423 程子涵");
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 300 && m.y >= 130 && m.y <= 210) {
            //价格分析
            setlinecolor(LIGHTBLUE);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                InsertSort(hp,F,T, L, 1);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 210 && m.y <= 290) {
            //相关性计算
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                InsertSort(hp,F,T, L, 2);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 290 && m.y <= 370) {
            //基金筛选
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                InsertSort(hp,F,T, L, 3);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 370 && m.y <= 450) {
            //返回上一级
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                priceana(hp,F,T, L);
                break;
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
            //setlinecolor(WHITE);
            //rectangle(90, 95, 310, 160);
        }
    }
}

void Exit_0() {
    Display();
    setfillcolor(RED);
    solidroundrect(0, 200, 200, 150, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 320, "感谢使用");
}

void InitFlist(Flist f) {
    //链表初始化
    f = new FNode;
    f->next = NULL;
}

//4.2 实现快排的交换
void Exchange(Flist p, Flist q) {

    double temp = p->maxupd;
    p->maxupd = q->maxupd;
    q->maxupd = temp;

    string tmp1 = p->date;
    p->date = q->date;
    q->date = tmp1;

    string tmp2 = p->code;
    p->code = q->code;
    q->code = tmp2;

    string tmp3 = p->name;
    p->name = q->name;
    q->name = tmp3;
}
//4.2 快排核心算法
void FastSort(Flist head, Flist tail) {
                //l3->next       t5
    if (head == tail || head == NULL || head == tail->next)
        return; // 递归的出口
    Flist high = new FNode;//工作指针high
    Flist low = new FNode;//工作指针low
    Flist pre = new FNode;//工作指针pre
    high = low = pre = head;//head是枢轴值
    //初始化工作指针为枢轴值

    while (low != tail) {
        //当工作指针low没有到达尾部时
        low = low->next; //low向后遍历
        if (low->maxupd > head->maxupd) {
            //如果low值大于枢轴值则将其放到high的左边（插到前面）
            pre = high;//记录high的值
            high = high->next;//high指针后移
            Exchange(high, low);//只交换数据 不交换指针
        }
    }
    // head为枢轴值,前半部分head->next~high大于枢轴,high->next,tail小于枢轴
    Exchange(head, high); // 将枢轴放在最终位置，最终枢轴前面都比他大，后面都比他小
    FastSort(head, pre);//将原数值列二分，分别工作进行快速排序
    FastSort(high->next, tail);
}
//4.2 快排界面和输出
void Maxupd(plist &hp,Flist& l2, linklist& l, Btree& T) {
    MOUSEMSG m;
    kik://翻页标签
    initgraph(1000, 700);
    IMAGE background;
    loadimage(&background, "图片2.png", 1000, 700, 1);
    putimage(0, 0, &background);
    int scrolly = 0;

    read4_2(l2);//读取评分文件，并用将所有信息用Flist穿起来成为链表
    Init(l);
    Read_Excel(l);
    Creatlist(hp,l2,T, l);//建立全局链组p

    Flist t = new FNode;
    t = l2->next;
    
    //·····以下遍历所有读取的flist链表结点，获取该股票的最大涨跌幅
    while (t) {
        //遍历评分链表
        int i;
        t->maxupd = -999;//初始化最大涨跌幅
        //·····以下该日期所有股票，获取该股票在这一天的最大涨跌幅
        //遍历所有日期后就能获得本支股票的最大涨跌幅
        for (i = 0; i < DATES; i++) {
            //遍历所有的日期，DATE存有171个日期，171也是p链组所有排头兵的个数
            //初始化y标记当前日期的所在的排头兵
            plist  y = new PNode;
            y = p[i]->next;
            while (y) {
                //遍历需要的链表
                if (y->nick == t->name) {
                    //如果股票名能够对应上，并且该结点的涨跌幅超过原最大涨跌幅，则将其赋值给最大涨跌幅
                    if (y->updrate > t->maxupd) {
                        t->date = p[i]->date;
                        t->maxupd = y->updrate;
                    }
                    break;
                }
                y = y->next;//遍历下一个结点
            }
        }
        t = t->next;
    }
    //flist存有评分的链表
    Flist t1 = new FNode;//定义指针去作为flist获取值的工作指针
    //·····以下将行业信息从L基本信息链表中读取到flist2中
    t1 = l2->next;
    while (t1) {
        linklist p = new LNode;//定义头指针
        p = l->next;
        while (p) {
            if (p->data.nick == t1->name) {
                t1->cate1 = p->data.category1;
                break;
            }
            p = p->next;
        }
        t1 = t1->next;
    }

    string category;
    int flag = 1;
    //绘制UI，通过交互界面获取用户需要的行业值
    while (flag) {
        //为方便折叠写的判断语句
        if(1){
        initgraph(1000, 700);
        loadimage(&background, "图片2.png", 1000, 700, 1);
        putimage(0, 0, &background);

        setfillcolor(LIGHTBLUE);
        solidroundrect(0,200, 600, 150, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        outtextxy(20, 160, "请选择一级行业");
        AddStyle(600, 200, 210, 150);
        

        settextstyle(20, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        settextcolor(LIGHTBLUE);
        int up1 = 240;
        int up2 = 210;
        int up3 = 214;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        outtextxy(20, up3, "采矿业、电力、热力、燃气及水生产和供应业");

        up1 += 34;up2 += 34;up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        outtextxy(20, up3, "批发和零售业"); 

        up1 += 34; up2 += 34; up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        outtextxy(20, up3, "卫生和社会工作、文化、体育和娱乐业");

        up1 += 34; up2 += 34; up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        outtextxy(20, up3, "金融业");

        up1 += 34; up2 += 34; up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        outtextxy(20, up3, "信息传输、软件和信息技术服务业");

        up1 += 34; up2 += 34; up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        outtextxy(20, up3, "制造业");

        up1 += 34; up2 += 34; up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        outtextxy(20, up3, "租赁和商务服务业");

        up1 += 34; up2 += 34; up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        outtextxy(20, up3, "房地产业");

        up1 += 34; up2 += 34; up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        outtextxy(20, up3, "建筑业");

        up1 += 34; up2 += 34; up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        outtextxy(20, up3, "交通运输、仓储和邮政业");

        up1 += 34; up2 += 34; up3 += 35;
        setfillcolor(WHITE);
        solidroundrect(0, up1, 600, up2, 0, 0);
        outtextxy(20, up3, "科学研究和技术服务业");

        setfillcolor(WHITE);
        solidroundrect(0, 50, 200, 110, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(20, 64, "返回");
        setcolor(WHITE);
        //判断鼠标操作,给category赋值
        while (1) {
            m = GetMouseMsg();
            up2 = 240;up1 = 210;up3 = 214;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "采矿业、电力、热力、燃气及水生产和供应业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "批发和零售业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "卫生和社会工作、文化、体育和娱乐业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "金融业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "信息传输、软件和信息技术服务业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "制造业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "租赁和商务服务业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "房地产业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "建筑业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "交通运输、仓储和邮政业";
                    break;
                }
            }
            up1 += 34; up2 += 34; up3 += 35;
            if (m.x >= 0 && m.x <= 600 && m.y >= up1 && m.y <= up2) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    category = "科学研究和技术服务业";
                    break;
                }
            }
            if (m.x >= 0 && m.x <= 200 && m.y >= 58 && m.y <= 110) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    priceana(hp,l2,T,l);
                    break;
                }
            }
        }
        }
        closegraph();
        flag = 0;//标记行业的存在性
        
        Flist t4 = new FNode;
        t4 = l2->next;//初始化一个最大涨跌幅链表

        Flist l3 = new FNode;//新表的头结点,作为快排的头部（枢轴）
        l3->next = NULL;
        Flist t5 = new FNode;//新表的首元节点，建立后会成为快排的尾部
        t5 = l3;

    up:
        /*以下建立符合行业需求的涨跌幅链表*/
        while (t4) {
            //遍历建立的最大涨跌幅链表
            if (t4->cate1 == category) {
                //如果某结点与需求节点相同
                flag = 1;//标记已找到（有数据）
                Flist t6 = new FNode;//生成新节点 ,尾插
                t6->maxupd = t4->maxupd;//数据赋值
                t6->cate1 = t4->cate1;
                t6->date = t4->date;
                t6->name = t4->name;
                t6->score = t4->score;
                t6->code = t4->code;
                t6->number = t4->number;
                t6->next = NULL;
                t5->next = t6;//尾插到头结点后面的后面				
                t5 = t5->next;
            }
            t4 = t4->next;
        }

        FastSort(l3->next, t5);//快速排序核心算法
        //分别传入快速排序的枢轴和尾部


        //···········以下是Ui和翻页界面
        Flist p1 = new FNode;//新表的头结点
        p1->next = NULL;
        p1 = l3->next;

        initgraph(1000, 700);
        IMAGE background;//定义一个图片名.
        loadimage(&background, "图片3.png", 1000, 700, 1);//从图片文件获取图像
        putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 0, 800, 160, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        char* C = new char[1024];
        strcpy(C, category.c_str());
        outtextxy(20, 90, C);
        //AddStyle(800, 200, 210, 150);

        setfillcolor(WHITE);
        solidroundrect(0, 20, 200, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(20, 34, "返回");
        setcolor(WHITE);

        setfillcolor(WHITE);
        settextcolor(LIGHTBLUE);
        solidroundrect(1200, 300, 900, 350, 0, 0);
        outtextxy(930, 310, "上翻");
        setcolor(WHITE);

        setfillcolor(WHITE);
        settextcolor(LIGHTBLUE);
        solidroundrect(1200, 370, 900, 420, 0, 0);
        outtextxy(930, 380, "下翻");
        setcolor(WHITE);


        settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(25, 140, "序号");

        //settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(129, 140, "股票代码");

        //settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(287, 140, "股票名称");

        //settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(427, 140, "涨跌幅");

        //settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(565, 140, "日期");
        int Height = 160 + scrolly;
        while (p1 != NULL) {
            //序号
            sprintf(C, "%.0f", p1->number);
            //strcpy(C, p1->number.c_str());
            outtextxy(25, Height, C);

            //股票代码
            strcpy(C, p1->code.c_str());
            outtextxy(149, Height, C);

            //名称
            strcpy(C, p1->name.c_str());
            outtextxy(277, Height, C);

            //涨跌幅
            sprintf(C, "%.2f", p1->maxupd);
            outtextxy(427, Height, C);

            //日期
            strcpy(C, p1->date.c_str());
            outtextxy(541, Height, C);

            Height += 20;
            //cout << p1->number << "\t股票代码:" << p1->code<< "\t涨跌幅:" << p1->maxupd << "\t日期:" << p1->date << "\t股票名称:" << p1->name << endl;
            
            
            //遍历下一结点
            p1 = p1->next;
            flag = 1;
        }

        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 0, 800, 160, 0, 0);
        //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        
        strcpy(C, category.c_str());
        outtextxy(20, 90, C);
        //AddStyle(800, 200, 210, 150);

        setfillcolor(WHITE);
        solidroundrect(0, 20, 200, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(20, 34, "返回");
        setcolor(WHITE);

        setfillcolor(WHITE);
        settextcolor(LIGHTBLUE);
        solidroundrect(1200, 300, 900, 350, 0, 0);
        outtextxy(930, 310, "上翻");
        setcolor(WHITE);

        setfillcolor(WHITE);
        settextcolor(LIGHTBLUE);
        solidroundrect(1200, 370, 900, 420, 0, 0);
        outtextxy(930, 380, "下翻");
        setcolor(WHITE);


        settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(25, 140, "序号");

        //settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(129, 140, "股票代码");

        //settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(287, 140, "股票名称");

        //settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(427, 140, "涨跌幅");

        //settextstyle(20, 0, "楷体");
        setbkmode(TRANSPARENT);
        outtextxy(565, 140, "日期");

        //判断鼠标操作
        while (1) {
            m = GetMouseMsg();
            if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
                //返回 solidroundrect(0, 20, 200, 80, 0, 0);
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    goto kik;
                    break;
                }
            }
            else if (m.x >= 890 && m.x <= 1300 && m.y >= 290 && m.y <= 350) {
                //上翻指令捕获 solidroundrect(1200, 300, 900, 350, 0, 0);
                //左上右下
                if (m.uMsg == WM_LBUTTONDOWN && scrolly < 0) {		//如果按下鼠标左键实现相应功能.
                    scrolly = scrolly + 500;
                    goto up;
                    break;
                }
            }
            else if (m.x >= 890 && m.x <= 1300 && m.y >= 360 && m.y <= 430) {
                //下翻捕获指令 solidroundrect(1200, 370, 900, 420, 0, 0);;
                //左上右下
                if (m.uMsg == WM_LBUTTONDOWN && scrolly > -1000) {		//如果按下鼠标左键实现相应功能.
                    scrolly = scrolly - 500;
                    goto up;
                    break;
                }
            }
        }


        if (flag == 0) {
            setfillcolor(LIGHTBLUE);
            solidroundrect(0, 280, 800, 150, 0, 0);
            //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
            settextstyle(30, 0, "华文中宋");
            setbkmode(TRANSPARENT);
            setcolor(WHITE);
            outtextxy(20, 160, "未找到结果");
            AddStyle(800, 200, 210, 150);
            setfillcolor(WHITE);
            solidroundrect(0, 20, 200, 80, 0, 0);
            settextstyle(30, 0, "华文中宋");
            setbkmode(TRANSPARENT);
            setcolor(LIGHTBLUE);
            outtextxy(20, 34, "返回");
            setcolor(WHITE);
            while (1) {
                m = GetMouseMsg();
                if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 80) {
                    //返回
                    if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                        goto kik;
                        break;
                    }
                }
            }
        }
        }
}
//评分信息读入
void read4_2(Flist& l){
    InitFlist(l);
    Flist p = new FNode;//定义头指针
    p = l;

    p->next = NULL;
    Book* book = xlCreateXMLBook();//创建一个XLSX的实例,在使用前必须先调用这个函数创建操作excel的对象

    book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc");//设置相应的key激活相关功能(购买)，否则创建book无法成功

    if (book->load(L"60支股票信息.xlsx"))//文件放入工程目录里
    {
        Sheet* sheetread = book->getSheet(1);//选取第2个表

        if (sheetread)//col为每一列  row为每一行
        {
            for (int row = sheetread->firstRow() + 1; row < 61; ++row)//行遍历
            {
                Flist q = new FNode;
                q->next = NULL;
                for (int col = sheetread->firstCol(); col < sheetread->lastCol(); ++col)//列遍历
                {
                    CellType celltype = sheetread->cellType(row, col);//读取某行某列的单元格的类型。有字符串型，数值型，空值。

                    Format* format = sheetread->cellFormat(row, col);

                    if (celltype == CELLTYPE_STRING)//如果每个单元格内容为字符串类型
                    {
                        //cout << "1" << endl;
                        const wchar_t* t = sheetread->readStr(row, col);//从单元格中读取字符串及其格式

                        char* pcstr = (char*)malloc(sizeof(char) * (2 * wcslen(t) + 1));//定义空间大小来存放读出的内容，wcslen()函数返回输入参数t的长度

                        memset(pcstr, 0, 2 * wcslen(t) + 1);//初始化清零操作，开辟两倍+1的空间

                        w2c(pcstr, t, 2 * wcslen(t) + 1); //中文编码

                        //cout << pcstr << endl;
                        switch (col) {//选择列
                        case 1: {//股票简称
                            q->name = pcstr;
                            break;
                        }
                        case 2: {//股票编码
                            q->code = pcstr;
                            break;
                        }
                        }
                        free(pcstr);
                    }
                    else if (celltype == CELLTYPE_NUMBER)//如果每个单元格内容为数值类型
                    {
                        //cout << "2" << endl;
                        switch (col)
                        {
                        case 0: {
                            double result = sheetread->readNum(row, col);
                            q->number = result;
                            break;
                        }
                        case 3: {
                            double result = sheetread->readNum(row, col);
                            q->score = result;
                            //cout << "!" << q->score;
                            break;
                        }
                        }
                    }
                }
                //每行内容为一个结点存储
                p->next = q;
                p = q;
            }
        }
        //保存excel
        book->save(L"60支股票信息.xlsx");
        book->release();
    }
}
//选择排序的菜单
void display_selectsort(plist &P, Btree& T, Flist &F, linklist &L) {
    Display();

    MOUSEMSG m;
    //选择排序  CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 200, 200, 150, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 160, "收盘价排序");
    AddStyle(200, 200, 210, 150);

    //相关性计算 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 280, 200, 230, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 240, "评分排序");
    AddStyle(200, 280, 210, 230);

    //返回 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 360, 200, 310, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 320, "返回上一级");
    AddStyle(200, 360, 210, 310);

    settextstyle(80, 0, "华文中宋");
    outtextxy(20, 20, "简单选择排序");
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 300 && m.y >= 130 && m.y <= 210) {
            //按照涨跌幅排序
            setlinecolor(LIGHTBLUE);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                selectget1(P, T, F, L, 1);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 210 && m.y <= 290) {
            //按照评分排序
            setlinecolor(LIGHTBLUE);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                selectget1(P, T, F, L, 2);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 290 && m.y <= 370) {
            //返回上一级
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                priceana(P,F, T,L);
                break;
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
            //setlinecolor(WHITE);
            //rectangle(90, 95, 310, 160);
        }
    }
}

//简单选择排序的文件写入
void select_write(Flist& l,int msgs) {
    Flist t3 = new FNode;
    t3 = l->next;

    Book* book = xlCreateXMLBook();//创建一个XLSX的实例,在使用前必须先调用这个函数创建操作excel的对象
    book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc");//设置用户注册信息，是否正版就在这里验证，否则第一行无法操作
    if (book) {
        Sheet* sheetwrite = book->addSheet(L"sheet1");//生成第一个表
        if (sheetwrite)
        {
            for (int row = 0; row < 60; ++row)//按行遍历
            {
                for (int col = 0; col < 5; ++col)//按列遍历
                {
                    if (col == 0) {
                        sheetwrite->writeNum(row, col, t3->number);
                    }
                    else if (col == 1) {
                        wchar_t* wchar = StringToWchar(t3->code);
                        sheetwrite->writeStr(row, col, wchar);
                        delete[]wchar;
                    }
                    else if (col == 2) {
                        wchar_t* wchar = StringToWchar(t3->name);
                        sheetwrite->writeStr(row, col, wchar);
                        delete[]wchar;
                    }
                    else if (col == 3) {
                        sheetwrite->writeNum(row, col, t3->closeprice);
                    }
                    else if (col == 4) {
                        sheetwrite->writeNum(row, col, t3->score);
                    }
                }
                t3 = t3->next;
            }
        }
        //保存
        if (msgs == 1 && book->save(L"收盘价简单选择排序.xlsx"))
        {
            //::ShellExecute(NULL, "open", "example.xls", NULL, NULL, SW_SHOW);
        }
        if (msgs == 2 && book->save(L"评分简单选择排序.xlsx"))
        {
            //::ShellExecute(NULL, "open", "example.xls", NULL, NULL, SW_SHOW);
        }
        else
        {
            std::cout << book->errorMessage() << std::endl;
        }
    }
    book->release();
}

//简单选择排序 按照两种不同的排序方式
void SelectSort_CP(plist& P, Btree& T, Flist& l,linklist &Lk,int m) {
    string pxfs;
    //排序方式标记
    MOUSEMSG ms;//鼠标动作
    for (int j = 0; j < 60; j++){
        Flist p = new FNode;
        p = l->next;
        switch (m) {
        case 1: {
            //收盘价排序
            for (int i = 0; i < 59; i++) {
                pxfs = "按照收盘价排序";
                if (p->closeprice < p->next->closeprice){
                    double temp;
                    temp = p->closeprice;
                    p->closeprice = p->next->closeprice;
                    p->next->closeprice = temp;

                    string str1;
                    str1 = p->code;
                    p->code = p->next->code;
                    p->next->code = str1;

                    string str2;
                    str2 = p->name;
                    p->name = p->next->name;
                    p->next->name = str2;

                    double str3;
                    str3 = p->score;
                    p->score = p->next->score;
                    p->next->score = str3;
                }
                p = p->next;
            }break;
        }
        case 2: {
            //评分排序
            pxfs = "按照评分排序";
            for (int i = 0; i < 59; i++) {
                if (p->score < p->next->score){
                    double temp;
                    temp = p->closeprice;
                    p->closeprice = p->next->closeprice;
                    p->next->closeprice = temp;

                    string str1;
                    str1 = p->code;
                    p->code = p->next->code;
                    p->next->code = str1;

                    string str2;
                    str2 = p->name;
                    p->name = p->next->name;
                    p->next->name = str2;

                    double str3;
                    str3 = p->score;
                    p->score = p->next->score;
                    p->next->score = str3;
                }
                p = p->next;
            }
            break;
        }     
        }
    }
    select_write(l,m);//保存文件

    int scrolly = 0;

up:
    initgraph(1000, 700);
    // 设置背景色为白色
    //setbkcolor(WHITE);
    // 用背景色清空屏幕
    IMAGE background;//定义一个图片名.
    loadimage(&background, "图片3.png", 1000, 700, 1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 0, 800, 160, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(WHITE);
    char* C = new char[1024];
    strcpy(C, pxfs.c_str());
    outtextxy(20, 90, C);

    setfillcolor(WHITE);
    solidroundrect(0, 20, 200, 80, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 34, "返回");
    setcolor(WHITE);

    setfillcolor(WHITE);
    settextcolor(LIGHTBLUE);
    solidroundrect(1200, 300, 900, 350, 0, 0);
    outtextxy(930, 310, "上翻");
    setcolor(WHITE);

    setfillcolor(WHITE);
    settextcolor(LIGHTBLUE);
    solidroundrect(1200, 370, 900, 420, 0, 0);
    outtextxy(930, 380, "下翻");
    setcolor(WHITE);
    settextstyle(20, 0, "楷体");
    setbkmode(TRANSPARENT);
    outtextxy(25, 140, "序号");
    setbkmode(TRANSPARENT);
    outtextxy(129, 140, "股票代码");
    setbkmode(TRANSPARENT);
    outtextxy(287, 140, "股票名称");
    setbkmode(TRANSPARENT);
    outtextxy(427, 140, "收盘价");
    setbkmode(TRANSPARENT);
    outtextxy(547, 140, "评分");
    int Height = 160 + scrolly;


    Flist t3 = new FNode;
    t3 = l->next;//从新表的表头结点开始
    while (t3) {
        //序号
        sprintf(C, "%.0f", t3->number);
        outtextxy(25, Height, C);

        //股票代码
        strcpy(C, t3->code.c_str());
        outtextxy(149, Height, C);

        //名称
        strcpy(C, t3->name.c_str());
        outtextxy(277, Height, C);

        //收盘价
        sprintf(C, "%.2f", t3->closeprice);
        outtextxy(427, Height, C);

        //评分
        sprintf(C, "%.2f", t3->score);
        outtextxy(547, Height, C);
        Height += 20;
        //cout << "股票代码:" << t3->code << "\t收盘价:" << t3->closeprice << "\t评分:" << t3->score << "\t股票名称:" << t3->name << endl;
        t3 = t3->next;
    }
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 0, 800, 160, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(WHITE);
    //char* C = new char[1024];
    strcpy(C, pxfs.c_str());
    outtextxy(20, 90, C);

    setfillcolor(WHITE);
    solidroundrect(0, 20, 200, 80, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 34, "返回");
    setcolor(WHITE);

    setfillcolor(WHITE);
    settextcolor(LIGHTBLUE);
    solidroundrect(1200, 300, 900, 350, 0, 0);
    outtextxy(930, 310, "上翻");
    setcolor(WHITE);

    setfillcolor(WHITE);
    settextcolor(LIGHTBLUE);
    solidroundrect(1200, 370, 900, 420, 0, 0);
    outtextxy(930, 380, "下翻");
    setcolor(WHITE);
    settextstyle(20, 0, "楷体");
    setbkmode(TRANSPARENT);
    outtextxy(25, 140, "序号");
    setbkmode(TRANSPARENT);
    outtextxy(129, 140, "股票代码");
    setbkmode(TRANSPARENT);
    outtextxy(287, 140, "股票名称");
    setbkmode(TRANSPARENT);
    outtextxy(427, 140, "收盘价");
    setbkmode(TRANSPARENT);
    outtextxy(547, 140, "评分");
    //int Height = 160 + scrolly;

    //判断鼠标操作
    while (1) {
        ms = GetMouseMsg();
        if (ms.x >= 0 && ms.x <= 200 && ms.y >= 20 && ms.y <= 90) {
            //返回 solidroundrect(0, 20, 200, 80, 0, 0);
            if (ms.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                display_selectsort(P,T,l,Lk);
                break;
            }
        }
        else if (ms.x >= 890 && ms.x <= 1300 && ms.y >= 290 && ms.y <= 350) {
            //上翻指令捕获 solidroundrect(1200, 300, 900, 350, 0, 0);
            //左上右下
            if (ms.uMsg == WM_LBUTTONDOWN && scrolly < 0) {		//如果按下鼠标左键实现相应功能.
                scrolly = scrolly + 500;
                goto up;
                break;
            }
        }
        else if (ms.x >= 890 && ms.x <= 1300 && ms.y >= 360 && ms.y <= 430) {
            //下翻捕获指令 solidroundrect(1200, 370, 900, 420, 0, 0);;
            //左上右下
            if (ms.uMsg == WM_LBUTTONDOWN && scrolly > -1000) {		//如果按下鼠标左键实现相应功能.
                scrolly = scrolly - 500;
                goto up;
                break;
            }
        }
    }
}

//4.3 简单选择排序
void selectget1(plist l1, Btree &T,Flist& l,linklist L,int msgs) {
    //msgs用于判断以什么为关键字排序
    initgraph(1000, 700);
    // 设置背景色为白色
    //setbkcolor(WHITE);
    // 用背景色清空屏幕
    IMAGE background;//定义一个图片名.
    loadimage(&background, "图片3.png", 1000, 700, 1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)

    read4_2(l);//读取评分文件
    Creatlist(l1,l,T,L);
    ReadTxt(l1,l, T, L);
    //先进行一系列读操作，确保有值

    Flist t1 = new FNode;
    t1 = l->next;//从新表的表头结点开始
    plist t2 = new PNode;//从p[0]开始

    int flag = 1;

    while (t1) {
        //遍历整个有评分的F链表
        t2 = p[0]->next;
        //每次所有的日期
        while (t2) {
            if (t2->nick == t1->name) {
                //如果匹配，则
                t1->recent = p[0]->date;//初始化日期为最初始的日期，最近的日期
                t1->closeprice = t2->closeprice;
                t1 = t1->next;
                flag = 0;//标记已经遍历
                break;
            }
            t2 = t2->next;//遍历下一个
        }
    }
    SelectSort_CP(l1,T,l,L,msgs);
}

//读点的边权信息
int read_3(Graphst &g)
{  
    Book* book = xlCreateXMLBook();//创建一个XLSX的实例,在使用前必须先调用这个函数创建操作excel的对象
    book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc");//设置相应的key激活相关功能(购买)，否则创建book无法成功
    if (book->load(L"60支股票信息.xlsx"))//文件放入工程目录里
    {
        Sheet* sheetread = book->getSheet(0);//选取第2个表
        if (sheetread)//col为每一列  row为每一行
        {
            for (int row = 1; row < 84; ++row)//行遍历
            {
                double x = 0; double y = 0;
                int a = 0; int b = 0;
                for (int col = 0; col < 3; ++col)//列遍历
                {
                    CellType celltype = sheetread->cellType(row, col);//读取某行某列的单元格的类型。有字符串型，数值型，空值。
                    Format* format = sheetread->cellFormat(row, col);

                    if (celltype == CELLTYPE_NUMBER)//如果每个单元格内容为数值类型
                    {
                        switch (col)
                        {
                        case 0: {//第0列，点1
                            double result = sheetread->readNum(row, col);
                            x = result;
                            break;
                        }
                        case 1: {//第1列，点2
                            double result = sheetread->readNum(row, col);
                            y = result;
                            break;
                        }
                        case 2: {//第三列，权值
                            double result = sheetread->readNum(row, col);
                            a = (int)x;
                            b = (int)y;//强制类型转换，转换为int
                            g.arcs[a][b] = g.arcs[b][a] = result;
                            //通路a、b的权值就是第三列的result
                            break;
                        }
                        }
                    }
                }
            }
        }
       	
        book->save(L"60支股票信息.xlsx");
        book->release();
    }
    return 0;
}
//绘制首页
void display_head(plist &hp,Flist &F,Btree &T,linklist& L) {
    Display();
    MOUSEMSG m;
    //功能1 查找 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(RED);
    solidroundrect(0, 200, 200, 150, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 160, "股票查询");
    AddStyle(200, 200, 210, 150);
    

    //功能2 分析 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(RED);
    solidroundrect(0, 280, 200, 230, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 240, "股票分析");
    AddStyle(200, 280, 210, 230);

    //功能3 退出 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(RED);
    solidroundrect(0, 360, 200, 310, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 320, "退出程序");
    AddStyle(200, 360, 210, 310);

    settextstyle(80, 0, "华文中宋");
    outtextxy(20, 20, "股票查询与分析系统");
    settextstyle(20, 0, "仿宋");
    outtextxy(700, 610, "计算机类20-4 201002423 程子涵");
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 300 && m.y >= 130 && m.y <= 210) {
            //查找
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                display_find(hp,F,T,L);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 210 && m.y <= 290) {
            //分析
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                display_analyse(hp,F,T,L);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 210 && m.y <= 370) {
            setlinecolor(LIGHTBLUE);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                exit(0);
                break;
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
            //setlinecolor(WHITE);
            //rectangle(90, 95, 310, 160);
        }
    }
}

int jol = 0;//标记几次进入递归

int fg = 0;
int ct = 0;

void Floyd_minpath(int path[][POINTS], int i, int j,Flist &F,int Mic)
{
    //二分，前面一个递归调用是i到k的路径，最后的那个递归调用的是k到j的路径
    jol++;
    if (jol % 2 == 0) {
        fg = 0;
    }
    else {
        fg = 230;
    }
    int k;
    k = path[i][j];
    if (k == -1)  return;
    Mic += 70;
    Floyd_minpath(path, i, k,F,Mic);
    
    //递归
    //printf("%d", k);
    Flist f = new FNode;
    f = F->next;
    while (f) {
        if (f->number == k) {
            cout << f->name;
            f->name = f->name + "↑";
            outtextxy(60+fg, 310+Mic, f->name.c_str());
        }
        f = f->next;
    }
    char C[100] = { "\0" };
    sprintf(C, "%d", k);
    //将int转为string
   outtextxy(30+fg, 310 + Mic, C);
   sprintf(C, "%d", ct++);
   outtextxy(0 + fg, 310 + Mic,C);
   outtextxy(15 + fg, 310 + Mic, ":");
    //Flist f = new FNode;
    Floyd_minpath(path, k, j,F,Mic);
    //Mic += 50;
}

void Floyd_GetOut(int A[][POINTS], int path[][POINTS], int n, Flist &F)
{
    int Mic = 0;
    int flag = 1;
    while (flag) {
        flag = 0;
        int i, j;

        //获取用户的输入
        char Tu[100] = {};
        InputBox(Tu, 100, "请输入两点的序号(0-60)，先输入第一个点的序号：");
        i = atoi(Tu);
        InputBox(Tu, 100, "输入第二个点的序号：");
        j = atoi(Tu);
        //cin >> i >> j;
obs:
        if (i > 60 || i < 1 || j>60 || j < 1) {
            InputBox(Tu, 100, "ERROR!输入两点的序号必须在0-60之间，请重新输入第一个点的序号：");
            i = atoi(Tu);
            InputBox(Tu, 100, "重新输入第二个点的序号：");
            j = atoi(Tu);
            goto obs;//跳回去在检查一遍
            //cin >> i >> j;
        }


        MOUSEMSG m;
        initgraph(1000, 700);
        IMAGE background;//定义一个图片名
        loadimage(&background, "图片3.png", 1000,700, 1);//从图片文件获取图像
        putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)


        string name1;
        string name2;
        Flist t1 = new FNode;
        t1 = F->next;
        //通过F链表和图的序号（点）比对，获取企业名称
        while (t1) {
            if (t1->number == i) {
                name1 = t1->name;
                break;
            }
            t1 = t1->next;
        }

        Flist t2 = new FNode;
        t2 = F->next;
        while (t2) {
            if (t2->number == j) {
                name2 = t2->name;
                break;
            }
            t2 = t2->next;
        }
        //设置模块颜色为紫色
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 0, 1500, 160, 0, 0);
        //无边框的圆角矩形 

        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        char* C = new char[1024];
        outtextxy(20, 90, "股票相关性分析");

        setfillcolor(WHITE);
        solidroundrect(0, 20, 200, 80, 0, 0);
        setcolor(LIGHTBLUE);
        settextcolor(LIGHTBLUE);
        outtextxy(20, 34, "返回");

        setcolor(LIGHTBLUE);
        strcpy(C, name1.c_str());
        outtextxy(10, 190, C);
        outtextxy(150, 190, "与");
        strcpy(C, name2.c_str());
        outtextxy(210, 190, C);

        if (A[i][j] != 999 && i != j)
        {
            sprintf(C,"%d", i);
            //将int转为string
            outtextxy(10, 250, "从");
            outtextxy(40, 250, C);

            outtextxy(90, 250, "到");
            sprintf(C, "%d", j);
            outtextxy(130, 250, C);
            outtextxy(200, 250, "路径长度为");
            sprintf(C, "%d", A[i][j]);//先转化成char*
            outtextxy(360, 250, C);
            //printf("从%d到%d路径长度:", i, j);
            //printf("%d", i);
            sprintf(C, "%d", i);
            //将int转为string
            outtextxy(10, 310, C);
            Flist f = new FNode;
            f = F->next;
            while (f) {
                if (f->number == i) {
                    //cout << f->name;
                    outtextxy(50, 310, f->name.c_str());
                }
                f = f->next;
            }
            outtextxy(170, 310, "→");
            //outtextxy(400, 310, "需要经过下列路径");
            sprintf(C, "%d", j);
            //将int转为string
            outtextxy(230, 310, C);
            
            Floyd_minpath(path, i, j,F,Mic);
            
            if (jol == 0) {
                outtextxy(400, 310, "无中间路径");
            }
            else {
                outtextxy(400, 310, "需要经过下列路径");
            }

            f = F->next;
            while (f) {
                if (f->number == j) {
                    //cout << f->name;
                    outtextxy(290, 310, f->name.c_str());
                }
                f = f->next;
            }
            
        }
        if (i == j) {
            sprintf(C, "%d", i);
            //将int转为string
            outtextxy(10, 250, "从");
            outtextxy(40, 250, C);

            outtextxy(90, 250, "到");
            sprintf(C, "%d", j);
            outtextxy(130, 250, C);
            outtextxy(200, 250, "路径长度为∞");
            /*
            cout << name1 << "与" << name2  << endl;
            printf("从%d到%d的", i, j);
            printf("路径长度为:0");*/
        }

        while (1) {
            m = GetMouseMsg();
            if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    //上一级
                    closegraph();
                
                    plist hp;
                    Btree T;
                    linklist L;//为保证传参准确
                    Mic = 0;
                    jol = 0;
                    ct = 0;
                    display_analyse(hp, F, T, L);
                    break;
                }
            }
        }
    }
    
    closegraph();
}

//弗洛伊德核心算法
//使用带权的邻接矩阵arcs来表示有向网G，求vi到vj的最短路径
void Floyd_Core(GNode & g, Flist& F) {
    read_3(g);//点关系和权值读入
    read4_2(F);//评分读入
    g.n = 61;//点数赋值，图中建立61个点
    Display();
    int D[POINTS][POINTS]={0};//建立并初始化点阵，记录最短路径的长度
    int path[POINTS][POINTS]={0};//最短路径上顶点vj的前一个顶点的序号
    int i, j, k, n = g.n;
    //·····初始化各对顶点之间初始化已知路径和距离
    for (i = 1; i < n; i++)					
        for (j = 1; j < n; j++)
        {
            D[i][j] = g.arcs[i][j];     //如果i和j之间有弧，则将j的前驱置1
            path[i][j] = -1;            //如果i和j之间无弧，则将j的前驱置为-1
        }

    for (k = 1; k < n; k++)					
    {
        for (i = 1; i < n; i++)
            for (j = 1; j < n; j++)
                if (D[i][j] > (D[i][k] + D[k][j]))  //从i经过k到j的一条路径更短
                {
                    D[i][j] = D[i][k] + D[k][j];    //更新D[i][j]
                    path[i][j] = k;                 //更改j的前驱为k
                }
    }
    Floyd_GetOut(D, path, n, F);
}

//筛选菜单
void display_screen() {
    Display();

    MOUSEMSG m;
    //最小生成树筛选  CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 200, 200, 150, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 160, "Prim筛选");
    AddStyle(200, 200, 210, 150);

    //二部图筛选 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 280, 200, 230, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 240, "二部图筛选");
    AddStyle(200, 280, 210, 230);

    //Kruskal基金筛选 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 360, 200, 310, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 320, "Kruskal筛选");
    AddStyle(200, 360, 210, 310);

    //返回 CZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZHCZH
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 440, 200, 390, 0, 0);
    //无边框的圆角矩形 参数：左 上 右 下（坐标） 圆角宽度 圆角宽度 
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    outtextxy(20, 400, "返回上一级");
    AddStyle(200, 440, 210, 390);

    settextstyle(80, 0, "华文中宋");
    outtextxy(20, 20, "基金筛选");
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 300 && m.y >= 130 && m.y <= 210) {
            setlinecolor(LIGHTBLUE);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                //Prim
                GNode g;
                InitG(g);
                int ps[61] = {};
                Flist l2 = new FNode;
                InitFlist(l2);
                MiniSTree_Prim(g, l2);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 210 && m.y <= 290) {
            setlinecolor(LIGHTBLUE);

            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                //二部图
                GNode g;
                InitG(g);
                Flist F=new FNode;
                InitFlist(F);
                BGraph(g,F);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 290 && m.y <= 370) {
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                //Kar
                //Flist& F, Btree& T, linklist& L
                int neighbour[61] = {};
                GNode g;
                InitG(g);
                int ps[61] = {};
                Flist f = new FNode;
                InitFlist(f);
                Kruskal(g, neighbour, f);
                break;
            }
        }
        else if (m.x >= 0 && m.x <= 300 && m.y >= 370 && m.y <= 450) {
            //返回上一级
            setlinecolor(LIGHTBLUE);
            // rectangle(90, 95, 310, 160);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                plist hp;Flist F;Btree T;linklist L;
                display_analyse(hp, F, T, L);
                break;
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
            //setlinecolor(WHITE);
            //rectangle(90, 95, 310, 160);
        }
    }
}

//展示删除操作后的左右子树信息
void Btree_Delete_show(Btree& T,string code) {
    Display();
    //设置模块颜色为紫色
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 0, 800, 160, 0, 0);
    //无边框的圆角矩形 

    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(WHITE);

    //注：输出也必须是char【】类型，需要转化
    char* C = new char[1024];
    strcpy(C, code.c_str());//string转化为char【】
    outtextxy(20, 90, C);

    setfillcolor(WHITE);
    solidroundrect(0, 20, 200, 80, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 34, "退出");
    setcolor(WHITE);

    
    int left, uph;
    left = 20;
    uph = 180;
    if (!T) {
        //原结点是叶子节点
        outtextxy(left, uph, "该新结点是叶子结点，无左右子树！");
    }
    if (T->rchild == NULL && T->lchild != NULL) {
        outtextxy(left, uph, "新结点信息");
        strcpy(C, T->stockcode.c_str());
        outtextxy(left+200, uph, C);
        outtextxy(left, uph+60, "左孩子信息");
        strcpy(C, T->lchild->stockcode.c_str());
        outtextxy(left+200, uph+120, C);
        strcpy(C, T->lchild->nick.c_str());
        outtextxy(left+400, uph + 180, C);
        cout << "\n左孩子信息：" << endl;
        cout << T->lchild->stockcode << endl;
    }
    else if (T->rchild != NULL && T->lchild == NULL) {
        outtextxy(left, uph, "新结点信息");
        strcpy(C, T->stockcode.c_str());
        outtextxy(left+200, uph, C);
        outtextxy(left, uph+60, "右孩子信息");
        strcpy(C, T->rchild->stockcode.c_str());
        outtextxy(left+200, uph + 60, C);
        strcpy(C, T->rchild->nick.c_str());
        outtextxy(left + 400, uph + 60, C);
        cout << "\n右孩子信息：" << endl;
        cout << T->rchild->stockcode << endl;

    }
    else if (T->rchild == NULL && T->lchild == NULL) {
        //原结点不是叶子节点，新节点是
        outtextxy(left, uph, "该新结点是叶子结点，无左右子树！");
    }
    /*  结点删除后进本函数会产生冲突，故不在此显示
    else if (T->rchild == NULL && T->lchild == NULL && isem == 1) {
        //原结点是叶子节点
        outtextxy(left, uph, "该新结点是叶子结点，无左右子树！");
    }*/
    else {
        outtextxy(left, uph, "新结点信息");
        strcpy(C, T->stockcode.c_str());
        outtextxy(left+200, uph, C);
        outtextxy(left, uph+60, "左孩子信息");
        strcpy(C, T->lchild->stockcode.c_str());
        outtextxy(left+200, uph+60, C);
        strcpy(C, T->lchild->nick.c_str());
        outtextxy(left + 400, uph+60, C);

        outtextxy(left, uph+120, "右孩子信息");
        strcpy(C, T->rchild->stockcode.c_str());
        outtextxy(left+200, uph+120, C);
        strcpy(C, T->rchild->nick.c_str());
        outtextxy(left + 400, uph + 120, C);
        cout << "\n左孩子信息：" << endl;
        cout << T->lchild->stockcode << endl;
        
        cout << "\n右孩子信息：" << endl;
        cout << T->rchild->stockcode << endl;
    }
    MOUSEMSG m;
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
            //返回 solidroundrect(0, 20, 200, 80, 0, 0);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                exit(0);
                break;
            }
        }
    }
    closegraph();
}

//二叉树的删除
void Btree_Delete(Btree &T) {
    Display();
    string code;
    char C[100] = {};
    InputBox(C, 100, "请输入您想删除的股票代码：");
    code = C;
    //cout << "请输入股票代码";
    //cin >> code;
    Btree_Delete_diguifind(T, code);
}

//递归寻找待删除的点
void Btree_Delete_diguifind(Btree& T, string code) {
    
    if (!T) {
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 0, 800, 160, 0, 0);
        //无边框的圆角矩形 

        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);
        outtextxy(20, 180, "错误！");
        setfillcolor(WHITE);
        solidroundrect(0, 20, 200, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(20, 34, "退出");
        setcolor(WHITE);
        MOUSEMSG m;
        while (1) {
            m = GetMouseMsg();
            if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
                //返回 solidroundrect(0, 20, 200, 80, 0, 0);
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    exit(0);
                    break;
                }
            }
        }
    }
    if (code == T->stockcode) {
        Btree_Delete_core(T,code);
        //删除的核心算法函数
    }
    else if (code < T->stockcode)
        Btree_Delete_diguifind(T->lchild, code);
    //递归向左子树查找
    else if (code > T->stockcode)
        Btree_Delete_diguifind(T->rchild, code);
    //递归向右子树查找
}

//二叉排序树删除的核心算法
void Btree_Delete_core(Btree &T,string code) {
    Btree q, s;
    Display();
    MOUSEMSG m;
    int isempty = 0;//判断删除后是否为空
    if (T->rchild != NULL && T->lchild != NULL) {
        //左右子树均不空
        q = T;
        s = T->lchild;
        while (s->rchild) {
            //在T的左子树中继续寻找前驱节点，即最右下的结点
            q = s;
            s = s->rchild;//向右直到尽头
        }
        T->stockcode = s->stockcode;
        //用s删除被删除的结点
        if (q != T) {
            //重接q的右子树，接上删除的地方
            q->rchild = s->lchild;
        }
        else {
            q->lchild = s->lchild;
            //重接q的左子树
        }
        delete s;
        //删除
    }
    else if (T->rchild == NULL && T->lchild != NULL) {
        //没有右子树但是有左子树
        q = T;
        T = T->lchild;
        delete q;//删除原T
    }
    else if (T->rchild != NULL && T->lchild == NULL) {
        //没有左子树但有右子树
        q = T;
        T = T->rchild;
        delete q;
    }
    else if (T->rchild == NULL && T->lchild == NULL) {
        //没有左子树也没有右子树
        q = T;
        delete q;
        isempty = 1;//删除后为空
        setfillcolor(LIGHTBLUE);
        solidroundrect(0, 0, 800, 160, 0, 0);
        //无边框的圆角矩形 

        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);

        setfillcolor(WHITE);
        solidroundrect(0, 20, 200, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(20, 34, "退出");
        setcolor(WHITE);

        char* C = new char[1024];
        strcpy(C, code.c_str());//string转化为char【】
        outtextxy(20, 90, C);
        //原结点是叶子节点
        outtextxy(20, 180, "该结点是叶子结点，删除后不存在新结点及左右子树");
        while (1) {
            m = GetMouseMsg();
            if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
                //返回 solidroundrect(0, 20, 200, 80, 0, 0);
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    exit(0);
                    break;
                }
            }
        }
        closegraph();
    }
    if(isempty==0)
        Btree_Delete_show(T,code);
}

// 染色法 深度优先
int Bgraph_DFS(int v, int x) {
    color[v] = x;
    //给v染色，染成传入的值，初始化为1
    for (int i = 0; i < 10; i++) {
        //遍历相邻结点
        if (Es[v][i] == 1) {
            //检查染色情况
            if (color[i] == x)
            //此点与旁边的点的颜色相同，不能构成二部图
                return 0;
            if (color[i] == 0 && !Bgraph_DFS(i, -x))    
            //该相邻点未着色，则染相反的颜色
                return 0; 
        }
    }
    return 1;
    //如果所有顶点涂完色，并且没有出现同色的相邻顶点，就返回1
}

string BGraph_Findname(Flist l2, int i) {//通过序号，返回名字
    Flist f = new FNode;
    f = l2->next;
    while (f) {
        if (f->number == i) {
            return f->name;
        }
        f = f->next;
    }
}

//检查着色情况
void BGraph_check(int a[], Flist l2) {
    MOUSEMSG m;
    if (1) {//为方便折叠写的判断语句
        initgraph(1500, 1300);
        IMAGE background;//定义一个图片名.
        loadimage(&background, "图片3.png", 1500, 1300, 1);//从图片文件获取图像
        putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)


        //设置模块颜色为紫色
        //setfillcolor(LIGHTBLUE);
        //solidroundrect(0, 0, 800, 160, 0, 0);
        //无边框的圆角矩形 

        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(WHITE);

        setfillcolor(WHITE);
        solidroundrect(0, 20, 200, 80, 0, 0);
        settextstyle(30, 0, "华文中宋");
        setbkmode(TRANSPARENT);
        setcolor(LIGHTBLUE);
        outtextxy(20, 34, "返回");
        setcolor(WHITE);
    }
    
    for (int i = 0; i < 10; i++) {
        //遍历输入的10个点
        if (color[i] == 0) {
            //color==0，未着色，在if中着色
            if (Bgraph_DFS(i, 1)==0) {
                outtextxy(20, 90, "无法构成二部图");
                //cout << "不能构成二部图" << endl;
                while (1) {
                    m = GetMouseMsg();
                    if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
                        //返回 solidroundrect(0, 20, 200, 80, 0, 0);
                        if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                            display_screen();//返回筛选菜单
                            break;
                        }
                    }
                }
                return;
            }
        }
    }

    outtextxy(20, 90, "可以构成二部图");
    int left = 0;char C[90] = {};//界面用

    //······以下两个循环分别输出不同色的点的信息
    for (int i = 0; i < 10; i++) {
        if (color[i] == 1) {
            //cout << a[i] << " " << BGraph_Findname(l2, a[i]) << "\t";
            sprintf(C, "%d", a[i]);
            outtextxy(20, 150+left, C);
            outtextxy(50, 150+left, BGraph_Findname(l2, a[i]).c_str());
            //找到该点对应的股票名称
            left += 60;
        }
    }
    //cout << endl;
    left = 0;
    for (int i = 0; i < 10; i++) {
        if (color[i] == -1) {
            sprintf(C, "%d", a[i]);
            outtextxy(220 , 210+ left, C);
            outtextxy(250 , 210+ left, BGraph_Findname(l2, a[i]).c_str());
            left += 60;
            //cout << a[i] << " " << BGraph_Findname(l2, a[i]) << "\t";
        }
    }

    while (1) {
        m = GetMouseMsg();
            if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
                //返回 solidroundrect(0, 20, 200, 80, 0, 0);
                if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                    display_screen();
                    break;
                }
            }
    }
    closegraph();
}

void Initsz(int a[]) {
    int leng = sizeof(a);
    for (int o = 0; o < leng; o++) {
        a[o] = 0;
        //将数组初始化为-1
    }
}
//二部图
void BGraph(GNode g, Flist l2) {
    Display();
    read_3(g);//将点权信息读入
    read4_2(l2);//将flist信息读入
    //读进结构体
    int flag = 1;
    int a[10]={-1};//初始化输入数组
    while (flag) {
        flag = 0;
        Initsz(color);//初始化color数组
        memset(Es, 0, sizeof(Es));//初始化二维数组
        //将原数组的所有空间初始化为0
        int MK;
        
        //10个数字的输入，含鲁棒性检验
        if (1) {
            //为方便折叠写的判断
        //第一个数字
        char C[100] = {};
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第1个：");
        MK = atoi(C);

        ck1:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第1个：");
                MK = atoi(C);
                goto ck1;
            }
        }
        a[0] = MK;

        //第二个数字
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第2个：");
        MK = atoi(C);

    ck2:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第2个：");
                MK = atoi(C);
                goto ck2;
            }
        }
        a[1] = MK;

        //第三个数字
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第3个：");
        MK = atoi(C);

    ck3:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第3个：");
                MK = atoi(C);
                goto ck3;
            }
        }
        a[2] = MK;


        //第四个数字
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第4个：");
        MK = atoi(C);

    ck4:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第4个：");
                MK = atoi(C);
                goto ck4;
            }
        }
        a[3] = MK;

        //第5个数字
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第5个：");
        MK = atoi(C);

    ck5:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第5个：");
                MK = atoi(C);
                goto ck5;
            }
        }
        a[4] = MK;

        //第6个数字
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第6个：");
        MK = atoi(C);

    ck6:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第6个：");
                MK = atoi(C);
                goto ck6;
            }
        }
        a[5] = MK;


        //第7个数字
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第7个：");
        MK = atoi(C);

    ck7:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第7个：");
                MK = atoi(C);
                goto ck7;
            }
        }
        a[6] = MK;

        //第8个数字
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第8个：");
        MK = atoi(C);

    ck8:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第8个：");
                MK = atoi(C);
                goto ck8;
            }
        }
        a[7] = MK;

        //第9个数字
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第9个：");
        MK = atoi(C);

    ck9:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第9个：");
                MK = atoi(C);
                goto ck9;
            }
        }
        a[8] = MK;

        //第9个数字
        InputBox(C, 100, "请输入（1-61）中10个数字且不允许重复，请输入第10个：");
        MK = atoi(C);

    ck10:
        for (int ck = 0; ck < 10; ck++) {
            if (a[ck] == MK || MK > 61 || MK < 0) {
                InputBox(C, 100, "错误！请输入（1-61）中10个数字且不允许重复！请重新输入第10个：");
                MK = atoi(C);
                goto ck10;
            }
        }
        a[9] = MK;
    }
        
        //以下建立生成子图
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (g.arcs[a[i]][a[j]] != 99) {
                    //输入两边之间的权值不等于99
                    Es[i][j] = 1;//则代表这两个点之间有边,邻接矩阵中的该点值为1
                }
            }
        }
        BGraph_check(a, l2);
    }
}


//Prim和Kruskal共用的输出
void Prim_Scorefirst(GNode g, int* neighbour, Flist l2,int k) {
    //评分优先排序,k用于判断是K算法还是P算法
    //Getscore(g);//获取得分信息
    //将得分信息写入g
    Display();
    MOUSEMSG m;
    Flist F = new FNode;
    F->next = NULL;
    read4_2(F);//将链表信息读入，准备评分信息
    Flist f = new FNode;
    f = F->next;
    for (int u = 1; u < 61; u++) {
        if (u == f->number) {
            g.score[u] = f->score;
            f = f->next;
        }
    }//写入得分信息
    if (1) {//为方便折叠写的选择
    setfillcolor(LIGHTBLUE);
    solidroundrect(0, 0, 800, 160, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(WHITE);

    //显示不同标题
    if (k == 1) {
        outtextxy(20, 90, "Prim股票筛选");
    }
    else if (k == 0) {
        outtextxy(20, 90, "Kruskal股票筛选");
    }

    setfillcolor(WHITE);
    solidroundrect(0, 20, 200, 80, 0, 0);
    settextstyle(30, 0, "华文中宋");
    setbkmode(TRANSPARENT);
    setcolor(LIGHTBLUE);
    outtextxy(20, 34, "返回");
    setcolor(WHITE);
    }
    char C[1024] = {};
    int node_num = 0;
    int visited[61] = { 0 };
    //标记已经使用过的股票
    int x = 0, y = 0;
    int Height = 0;//界面用
    while (node_num < 6)//根据股票评分选出符合条件的6个点
    {
        int min = 999;//初始化最小值为999
        for (int i = 1; i <= 60; i++){
            for (int j = 1; j <= 60; j++){
                if (A[i][j] <= min && (visited[i] == 0 || visited[j] == 0) && A[i][j] != 999)
                //边中有一个点没有被访问过就可以构成条件
                {
                    if (A[i][j] < min)
                    {//选出剩余边中值最小的
                        min = A[i][j];
                        x = i;
                        y = j;//保存该边的两个端点
                    }
                    else if (A[i][j] == min && (g.score[i] + g.score[j]) > (g.score[x] + g.score[y]))
                    //权值相同的情况下，比较两边两顶点的分数值之和，分数值之和高的优先
                    {
                        min = A[i][j];
                        x = i;
                        y = j;//保存该边的两个端点
                    }
                }
            }
        }
        if (visited[x] == 0){
            //标记该点已经被选中
            visited[x] = 1;
            node_num++;//总选中数增加
        }
        if (visited[y] == 0){
            visited[y] = 1;
            node_num++;
        }
        //获取点对应的股票名称
        settextcolor(BLACK);
        outtextxy(20, 170+Height, "边的权值");
        sprintf(C, "%d", min);
        outtextxy(140, 170 + Height, C);
        outtextxy(170, 170 + Height, "边的结点1：");
        strcpy(C, BGraph_Findname(l2, x).c_str());
        outtextxy(310, 170 + Height, C);
        outtextxy(450, 170 + Height, "边的结点2：");
        strcpy(C, BGraph_Findname(l2, y).c_str());
        outtextxy(590, 170 + Height, C);
        Height += 50;
    }
    while (1) {
        m = GetMouseMsg();
        if (m.x >= 0 && m.x <= 200 && m.y >= 20 && m.y <= 90) {
            //返回 solidroundrect(0, 20, 200, 80, 0, 0);
            if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
                closegraph();
                display_screen();
                break;
            }
        }
    }
    closegraph();
}

void MiniSTree_Minpath(GNode g, Flist l2) {
    //floyd算法从每对顶点之间的最短路径
    for (int i = 1; i < 61; i++)		
        for (int j = 1; j < 61; j++) {
            A[i][j] = g.arcs[i][j];
            //将g存的的边权信息赋值给A[i][j]
        }
    //判断在ij之间权值确定的情况下是否或经由k从i到j的权值更小，如有，更新为这一节点
    for (int k = 1; k < 61; k++) {
        for (int i = 1; i < 61; i++)
            for (int j = 1; j <61; j++)
                if (A[i][j] > (A[i][k] + A[k][j])){
                    A[i][j] = A[i][k] + A[k][j];
             }
    }
}

//显示生成的最小二叉树信息-不用显示
void MiniSTree_show(GNode g, int* neighbour) {
    int Dots = 61;//一共有61个顶点
    cout << "MST的边为:" << endl;
    int sum = 0;
    for (int i = 2; i < Dots; i++) {//他的上一个边和这个点
        cout << neighbour[i] << "—" << i << "\t权值" << A[neighbour[i]][i] << endl;
        sum += A[neighbour[i]][i];
    }
    cout << sum << endl;
}

#define Dots 61   //所有点数
//Prim最小二叉树
void MiniSTree_Prim(GNode g,Flist l2) {
    read_3(g);
    read4_2(l2);
    //读需要的文件
    MiniSTree_Minpath(g, l2);
    //V为所有顶点，U为最小生成树的所有结点
    Display();
    int MinValue[61] = {};         //MinValue[i]保存编号为i的顶点到U中所有顶点的最小权值
    int MinValueDots[61] = {};    //MinValueDots[i]保存U到V-U中编号为i的顶点权值最小的顶点的编号
    int used[61] = {};
    int min;
    int neighbour[61] = {};
    //·····以下初始化权值、点信息
    for (int i = 1; i < Dots; i++) {
        MinValue[i] = A[1][i];
        //初始化每个结点到U中所有顶点的距离为到1的距离
        MinValueDots[i] = 1;
        //初始化权值最小点为1，从1找起
        used[i] = 0;
        //初始化判断这个点是否已经用过加入选点中
        neighbour[i] = -1;
        //边的另一个顶点初始化为无
    }
    //先从1找起，将其标记为选过
    used[1] = 1;
    //遍历所有的顶点
    for (int i = 1; i < Dots; i++) {
        int j = 1;
        min = 999;//初始化最小权值计数器

        //···此遍历找出权值最小的边（此边即将加入树中）
        for (int k = 1; k < Dots; k++) {
            //找到V-U中的与U中顶点组成的最小权值的边的顶点编号
            if (used[k] == 0 && MinValue[k] < min) {
                //确保点k未被使用过 k的最小权值小于目前的最小权值
                min = MinValue[k];//把这个最小权值赋值给min
                j = k;//保存这个最小权值的编号
            }
        }
        //以上找出了权值最小的边的权值
        neighbour[j] = MinValueDots[j];//j的另一个结点就是这个让其权值最小的结点
        used[j] = 1;//标记这个点已经用过
        //·····以下以j作为基点寻找下一条边
        for (int m = 1; m < Dots; m++) {
            //由于j顶点加入U中，更新MinValue和MinValueDots数组中的元素
            //检测V-U中的顶点到j顶点的权值是否比j加入U之前的MinValue数组的元素小  
            if (used[m] == 0 && A[j][m] < MinValue[m]) {
                MinValue[m] = A[j][m];//更新j后，初始化各点的最小权值为到j边的最小权值
                MinValueDots[m] = j;//初始化未经遍历的各点的最小权值边为j
            }
        }
    }
    //MiniSTree_show(g, neighbour);//显示生成的最小生成树
    Prim_Scorefirst(g, neighbour, l2,1);//按照评分优先筛选股票
}

int f[105];//记录某个点根的值

int Kruskal_findfather(int a) {
    //寻找祖先结点
    if (f[a] == a)
        return a;
    else {
        f[a] = Kruskal_findfather(f[a]);
        //递归 直至找到祖先根
    }
    return f[a];
}

bool Kruskal_Root(int a, int b) {
    //归并，让两个顶点归并到同一个边下
    int t1 = Kruskal_findfather(a);
    int t2 = Kruskal_findfather(b);
    //获取a、b的根
    if (t1 == t2) {
        //两个顶点的根相同时，则可以形成环，不允许
        return false;
    }
    else {
        f[t2] = f[t1];
        //两个根不相同时，不成环，满足 
        return true;
    }
}

//Kruskal核心算法 归并边的过程
void Kruskal(GNode g, int* neighbour,Flist l2 ) {
    read_3(g);
    read4_2(l2);
    //读需要的文件
    for (int i = 0; i <= 100; i++) {
        f[i] = i;
    }//初始化全局数组，其存的结点就是自己序号本身，用于判断根和祖先
    MiniSTree_Minpath(g, l2);//获取最短路
    int n, ts = 0, sum = 0;
    n = 60;
    int s = 1;
    for (int i = 1; i <= n; i++) {
        for (int j = i; j <= n; j++) {
            if (sum != 1831) {
                //((60+1)*60)*0.5
                arc2[s].v = i;      //初始化起点    
                arc2[s].u = j;      //初始化终点
                arc2[s].w = A[i][j];    //初始化权值
                s++;//边数增加
            }
        }
    }
    //······以下使用冒泡排序对权值排序
    for (int i = 1; i <= 1830; i++) {
        for (int j = 1; j <= 1830 - i; j++) {
            if (arc2[j + 1].w < arc2[j].w) {
                int AR1;
                AR1 = arc2[j].u;
                arc2[j].u = arc2[j + 1].u;
                arc2[j + 1].u = AR1;

                int AR2;
                AR2 = arc2[j].v;
                arc2[j].v = arc2[j + 1].v;
                arc2[j + 1].v = AR2;

                int AR3;
                AR3 = arc2[j].w;
                arc2[j].w = arc2[j + 1].w;
                arc2[j + 1].w = AR3;
            }
        }
    }
       
        for (int i = 1; i <= 1830; i++) { 
            //·····以下判断添加的边是否成环
            if (Kruskal_Root(arc2[i].v, arc2[i].u))//如果不成环，加入到生成树中 成环就跳过
            {
                ts++;
                //59个
                if (ts == n - 1) break;
            }
        }
        //评分优先排序
        Prim_Scorefirst(g, neighbour, l2,0);
}

int main(){
    cout << "Loading..." << endl;
    linklist L;
    Btree T;
    Flist f=new FNode;
    InitFlist(f);
    plist q;
    Init(L);
    GNode g;
    InitG(g);
    q = NULL;
    Creatlist(q,f,T, L);
    Read_Excel(L);
    //display_analyse(T, L);
    //display_head(f,T,L);
    
    //display_selectsort(q, T, f, L);
    //selectget1(q, T, f, L,1);
    //Maxupd(f, L,T);
    //readtolist(T,L);
    //InsertSort(T,L,1);
    //display_Sort1(T, L);
    display_head(q,f,T,L);//显示首页
    //Floyd_Core(g, f);
    //Btree_Delete(T);
    //BGraph(g, f);
    
    //int neighbour[61] = {0};
   //MiniSTree_Prim(g, neighbour, f);
    return 0;
}
