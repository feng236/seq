#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <iostream>
#include<string>
#define max 210
#define qmax 2000
using namespace std;
typedef struct listnode {
    struct listnode* next;
    int data;
    double price, time;
    string http;
    string way;
}listnode;
typedef struct {
    string nationname;
    string cityname;
    double wlatitude, jlongitude;
    listnode* next;
}headnode;
typedef struct map {
    headnode* citys[max];
    int citynum;
    int arcnum;
}map;
typedef struct
{
    string name;
    int data;
}point;
typedef struct {
    double weight;
    point route[max];
    int mark;
}dijist;
//第一部分读入数据
void readcity(FILE* f, map& mine)
{
    if (!f)
    {
        cout << "read file error " << endl;//文件打开失败
        return;
    }
    char temp[300] = {'\0'};//暂存字符串初始化
    int num = 0;
    double f1, f2;
    while (!feof(f))//最好不使用feof判断，会导致有一次多余输入
    {
        char temp0 = fgetc(f);//输入最后一个后
        if (feof(f))
        {
            break;//进行判断
        }
        mine.citys[num] = new headnode;//初始化，创建空间
        mine.citys[num]->next = NULL;      
        int i = 0;
        while (temp0 != ',')
        {
            temp[i++] = temp0;
            temp0 = fgetc(f);
        }
        temp[i] = '\0';
        string nationname(temp);//读入信息
        mine.citys[num]->nationname = nationname;
        temp0 = fgetc(f);
        i = 0;
        while (temp0 != ',')
        {
            temp[i++] = temp0;
            temp0 = fgetc(f);
        }
        temp[i] = '\0';
        string cityname(temp);
        mine.citys[num]->cityname = cityname;
        fscanf(f, "%lf,%lf", &f1, &f2);
        mine.citys[num]->wlatitude = f1;
        mine.citys[num]->jlongitude = f2;
        temp0 = fgetc(f);
        if (temp0 == '\n')
        {
            num++;
            continue;
        }
    }
    mine.citynum = num;//计入城市数目
}
int locate(string &name, map& mine)//判断城市在数组中的第几个
{
    for (int i = 0; i < mine.citynum; i++)
    {
        if (mine.citys[i]->cityname == name)
        {
            return i;
        }
    }
    return -1;
}
listnode* similar(string &cityname, string &cityname1, map& mine)//判断路径是否重复，如果有重复，返回对应的重复节点
{
     int j = locate(cityname, mine);
     int i = locate(cityname1, mine);
     if (j == -1 || i == -1)
         return nullptr;
     listnode* next = mine.citys[j]->next;//判断
     while (next)
     {
         if (next->data == i)
         {
             return next;
         }
         next = next->next;
     }
     return NULL;
}
void readroute(FILE* f, map& mine)//读取路径信息
{
    if (!f)
    {
        cout << "read file error " << endl;//判断是否为空
        return;
    }
    char temp[1000] = {'\0'};
    int num = 0;
    double f1, f2;
    int arcnum = 0;
    while (!feof(f))
    {
        char temp0 = fgetc(f);
        if (feof(f))
        {
            break;
        }
        int i = 0;
        while (temp0 != ',')//读入信息
        {
            temp[i++] = temp0;
            temp0 = fgetc(f);
        }
        temp[i] = '\0';
        string cityname(temp);
        i = 0;
        temp0 = fgetc(f);
        while (temp0 != ',')
        {
            temp[i++] = temp0;
            temp0 = fgetc(f);
        }
        temp[i] = '\0';
        i = 0;
        string cityname1(temp);
        temp0 = fgetc(f);
        while (temp0 != ',')
        {
            temp[i++] = temp0;
            temp0 = fgetc(f);
        }
        temp[i] = '\0';
        i = 0;
        string way(temp);
        fscanf(f, "%lf,%lf,", &f1, &f2);
        temp0 = fgetc(f);
        while (temp0 != '\n')
        {
            temp[i++] = temp0;
            temp0 = fgetc(f);
        }
        temp[i] = '\0';
        string http(temp);
        if (listnode * tem = similar(cityname, cityname1, mine))//进行判断，如果是重复，需要进行信息的更新
        {
            tem->way = way;//更新信息
            tem->http = http;
            tem->time = f1;
            tem->price = f2;
        }
        else
        {
            arcnum++;
            listnode* tem1 = new listnode;//创建新的节点，存储路径
            tem1->next = NULL;
            tem1->data = locate(cityname1, mine);//存储信息
            tem1->way = way;
            tem1->time = f1;
            tem1->price = f2;
            tem1->http = http;

            int n = locate(cityname, mine);//连接节点
            tem1->next = mine.citys[n]->next;
            mine.citys[n]->next = tem1;
        }
    }
    mine.arcnum = arcnum;//存储边的数目
}
void freeout(map&mine)//释放内存
{
    for (int i = 0; i < mine.citynum; ++i)
    {
        listnode* p = mine.citys[i]->next;
        while (p)//先删除每个头节点连接的表节点
        {
            listnode* tmp = p;
            p = p->next;
            delete tmp;
        }
        delete mine.citys[i];//删除头节点
    }
}
int search(dijist* me,int n)//寻找数组中，权重最小且未被选中的节点
{
    int i,ang;
    double min;
    for (i = 0; i < n; i++)
    {
        if (me[i].mark == 0)
        {
            ang = i;
            min = me[i].weight;
            break;
        }
    }//先进行初始最小值选择
    if (i == n)
    {
        return -1;//如果没有，说明已完成
    }
    for (; i < n; i++)
    {
        if (me[i].mark == 0 && me[i].weight < min)//开始往后找，最小的权重
        {
            min = me[i].weight;
            ang = i;
        }
    }
    return ang;//返回角标
}
int mestrlen(point* me)//制作一个统计长度的函数
{
    for (int i = 0; i < max; i++)
    {
        if (me[i].data == -1)//我的数组的结束标志为-1
        {
            return i;
        }
    }
    return -1;
}
void copy(point* me, point* nw)//将节点信息复制到第一个数组中
{
    for (int i = 0; i < max; i++)
    {
        me[i].data = nw[i].data;
        me[i].name = nw[i].name;
    }
}
dijist* dj(map&mine,int n)//最短路径
{
    dijist* me = new dijist[max];//给出一个数组用于存储到各个节点的权重
    for (int i = 0; i < mine.citynum; i++)//初始化
    {
        me[i].weight = qmax;//权重最大
        me[i].mark = 0;
        for (int j = 0; j < max; j++)//给每个节点对应的路径数组初始化一下
        {
            me[i].route[j].data = -1;
            me[i].route[j].name = "error";
        }
    }
    me[n].weight =0;//对于指定起点更改权重
    int count = mine.citynum;//循环次数，每一次会选中一个城市节点
    while (count)
    {
        int ang = search(me, mine.citynum);//找到最小且未被选中的节点
        if (ang == -1)
        {
            break;//结束判断
        }
        me[ang].mark = 1;//更改标记
        count--;
        listnode* temp = mine.citys[ang]->next;//开始更新路径，给出路径节点
        while (temp)
        {
            if (me[temp->data].mark == 0 && me[ang].weight + temp->time < me[temp->data].weight)//要求未被选中，且权重较小
            {
                me[temp->data].weight = me[ang].weight + temp->time;//更改权重
                int len = mestrlen(me[ang].route);//得到最后一位
                copy(me[temp->data].route, me[ang].route);//将新的路径信息复制进去
                me[temp->data].route[len].data = temp->data;//添加最后一位的节点信息
                me[temp->data].route[len].name = mine.citys[temp->data]->cityname;//给出名字
            }
            temp = temp->next;//循环
        }
    }
    return me;//返回最短路径的头指针
}
int main()
{
    map mine;
    FILE* fs = fopen("readcitys.txt", "r");
    FILE* f = fopen("readroute.txt", "r");//打开文件
    if (!fs || !f)
    {
        cout << "file open error\n";
        return 1;
    }

    readcity(fs, mine);
    readroute(f, mine);//读入数据

    int n,m;
    cout << "Please enter the codes for the starting city and the destination city\n";//请用户输入想要的两个的角标
    cin >> n >> m;
    dijist*me = dj(mine,n);
    cout << mine.citys[n]->cityname << " to " << mine.citys[m]->cityname << " the distance is " << me[m].weight <<" hour " << endl;
    //文字输出一下
    FILE* file = fopen("test.htm", "w");
    if (!file)
     {     //文件异常处理
        cout << "error do not open  test.htm" << endl;
        delete[] me;
        freeout(mine);
        fclose(fs);
        fclose(f);
        return 1;
    }
    //html文件写入
    fprintf(file, "<!DOCTYPE html><html><head><meta charset='utf-8'>" "<style type='text/css'>""body, html { width: 100%%; height: 100%%; margin: 0; font-family: '微软雅黑'; }""#allmap { height: 100%%; width: 100%%; }""</style>""<script type='text/javascript' src='http://api.map.baidu.com/api?v=2.0&ak=nSxiPohfziUaCuONe4ViUP2N'></script>" "<title>从 %s 到 %s 的最短路径</title></head><body><div id='allmap'></div>",mine.citys[n]->cityname.c_str(), mine.citys[m]->cityname.c_str());
    fprintf(file, "<script type='text/javascript'>"
        "var map = new BMap.Map('allmap');"
        "var point = new BMap.Point(%lf, %lf);"//我规定地图的打开的中间点位置
        "map.centerAndZoom(point, 2);"
        "map.enableScrollWheelZoom(true);",
        (mine.citys[n]->jlongitude + mine.citys[m]->jlongitude) / 2,
        (mine.citys[n]->wlatitude + mine.citys[m]->wlatitude) / 2);
    fprintf(file,
        "var marker0 = new BMap.Marker(new BMap.Point(%lf, %lf));"
        "map.addOverlay(marker0);"
        "var infoWindow0 = new BMap.InfoWindow(\"<p style='font-size:14px;'>国家: %s<br/>城市: %s</p>\");"
        "marker0.addEventListener('click', function(){ this.openInfoWindow(infoWindow0); });",
        mine.citys[n]->jlongitude, mine.citys[n]->wlatitude,
        mine.citys[n]->nationname.c_str(), mine.citys[n]->cityname.c_str());//%s打印的是c语言类型的字符串，为了简便，使用string中的这一个函数，将其转换为c语言类型的字符串。
    fprintf(file,
        "var marker1 = new BMap.Marker(new BMap.Point(%lf, %lf));"
        "map.addOverlay(marker1);"
        "var infoWindow1 = new BMap.InfoWindow(\"<p style='font-size:14px;'>国家: %s<br/>城市: %s</p>\");"
        "marker1.addEventListener('click', function(){ this.openInfoWindow(infoWindow1); });",
        mine.citys[m]->jlongitude, mine.citys[m]->wlatitude,
        mine.citys[m]->nationname.c_str(), mine.citys[m]->cityname.c_str());//把起点和终点标记出来


    fprintf(file, "var polyline = new BMap.Polyline([");
    fprintf(file, "new BMap.Point(%lf, %lf)", mine.citys[n]->jlongitude, mine.citys[n]->wlatitude);
    fprintf(file, ",");
    for (int i = 0; me[m].route[i].data != -1; i++) {
        int city_idx = me[m].route[i].data;
        fprintf(file, "new BMap.Point(%lf, %lf)", mine.citys[city_idx]->jlongitude, mine.citys[city_idx]->wlatitude);
        if (me[m].route[i + 1].data != -1) fprintf(file, ",");
    }
    fprintf(file, "], {strokeColor:'blue', strokeWeight:2, strokeOpacity:0.5});"
        "map.addOverlay(polyline);");//制作一个线条



    fprintf(file,
        "var contentString01 = '从 %s, %s 到 %s, %s: 时间 %.2lf小时';"
        "var infoWindowPath = new BMap.InfoWindow(contentString01);"
        "map.addEventListener('click', function(e) {"
        "   map.openInfoWindow(infoWindowPath, map.getCenter());"
        "});",
        mine.citys[n]->cityname.c_str(), mine.citys[n]->nationname.c_str(),
        mine.citys[m]->cityname.c_str(), mine.citys[m]->nationname.c_str(),//制作路径信息
        me[m].weight); 
    fprintf(file, "</script></html>");
 



    delete[]me;//释放内存
    freeout(mine);
    fclose(file);
    fclose(fs);
    fclose(f);
}