#include "widget.h"
#include "ui_widget.h"
#include<QtDebug> //调试输出到控制台头文件
#include"offlinedata.h"
#include <QTimer>

#pragma execution_character_set("utf-8"); // qDebug()可以显示汉字

void CALLBACK MSesGCallback(LONG lCommand, NET_DVR_ALARMER* pAlarmer, char* pAlarmInfo, DWORD dwBufLen, void* pUser);

//构造函数
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget) //新建一个Widget界面
{
    ui->setupUi(this); //在一个指定的窗口建立起ui
    //this->resize(800,600);
    userID = -1; //默认值为注销状态
    previewID = -1; //默认关闭预览

    /*connect(ui->LoginButton, &QPushButton::clicked, this, &Widget::on_LoginButton_clicked);  //按钮名称 ； 触发方式：click 点击 ； ；功能函数
    connect(ui->LogoutButton, &QPushButton::clicked, this, &Widget::on_LogoutButton_clicked);
    connect(ui->PreviewButton, &QPushButton::clicked, this, &Widget::on_PreviewButton_clicked);
    connect(ui->StopPreviewButton, &QPushButton::clicked, this, &Widget::on_StopPreviewButton_clicked);*/

    if(initSDK()){
        qDebug() << "SDK init Success!" << endl;
    }

    putRoad();

    cameraTimer = new QTimer;
    connect(cameraTimer,SIGNAL(timeout()),this,SLOT(extractCameraRealTime())); //执行extractCameraRealTime()函数
}

Widget::~Widget()
{
    if(initSDK()){
        qDebug() << "SDK init Success!" << endl;
    }
    delete ui;
}


void Widget::on_offlinedata_clicked()
{
    this->hide();
    offlineData *o = new offlineData(); //创建新的ui界面指针
    o->setWindowTitle("离线展示界面");
    o->show();//显示新的ui界面
}


//初始化
bool Widget::initSDK(){
    bool isok = NET_DVR_Init();
    if(isok == false){
        qDebug() << "NET_DVR_Init error;error number is " << NET_DVR_GetLastError();
        QMessageBox::warning(this, "Warning","NET_DVR_Init error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
        return isok;
    }

    //设置连接时间与重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);

    return isok;
}

//退出初始化 //释放SDK资源
bool Widget::uninitSDK(){
    bool isok = NET_DVR_Cleanup();
    if(isok == false){
        qDebug() << "NET_DVR_Cleanup error;error number is " << NET_DVR_GetLastError();
        QMessageBox::warning(this, "Warning","NET_DVR_Cleanup error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
        return isok;
    }

    return isok;
}

long Widget::play(HWND hWnd, NET_DVR_PREVIEWINFO struPlayInfo){ //NET_DVR_PREVIEWINFO 预览
    //启动预览并设置回调数据流
    LONG lRealPlayHandle;

    struPlayInfo = {0};
    struPlayInfo.hPlayWnd = hWnd;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
    struPlayInfo.lChannel     = 1;       //预览通道号
    struPlayInfo.dwStreamType = 0;       //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
    struPlayInfo.dwLinkMode   = 0;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP
    struPlayInfo.bBlocked     = 1;       //0- 非阻塞取流，1- 阻塞取流

    lRealPlayHandle = NET_DVR_RealPlay_V40(userID, &struPlayInfo, NULL, NULL);
    if (lRealPlayHandle < 0)
    {
        qDebug() << "NET_DVR_RealPlay_V40 error;error number is " << NET_DVR_GetLastError();
        QMessageBox::warning(this, "Warning","NET_DVR_RealPlay_V40 error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
        NET_DVR_Logout(userID);
        userID = -1; //注销
        return -1;
    }

    return lRealPlayHandle; //lRealPlayHandle 记为 previewID
}

//登录设备
void Widget::on_LoginButton_clicked()
{
    //initSDK();
    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    memset(&struDeviceInfo, 0, sizeof(NET_DVR_DEVICEINFO_V30));//存放设备参数的结构体
    userID = NET_DVR_Login_V30(const_cast<char*>("192.168.1.66"), 8000, const_cast<char*>("admin"), const_cast<char*>("fh123456"), &struDeviceInfo);

    /*//相机参数
    char sDVRIP[20] = "192.168.1.66";
    short wDVRPort = 8000;
    char sUserName[20] = "admin";
    char sPassword[20] = "fh123456";
    //注册摄像机设备
    userID = NET_DVR_Login_V30(sDVRIP, wDVRPort, sUserName, sPassword, &struDeviceInfo);*/  

    if (userID < 0){
        qDebug() << "NET_DVR_DEVICEINFO_V30 error;error number is " << NET_DVR_GetLastError();
        QMessageBox::warning(this, "Warning","NET_DVR_DEVICEINFO_V30 error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
        return;
    }

    qDebug() << "Login Success , userID: " << userID << endl;
    //uninitSDK();
}

//注销设备
void Widget::on_LogoutButton_clicked()
{
    //initSDK();
    NET_DVR_Logout(userID);//注销用户
    /*if(userID != -1){
        if( NET_DVR_Logout(userID) == false){
            qDebug() << "NET_DVR_Logout error;error number is " << NET_DVR_GetLastError();
            QMessageBox::warning(this, "Warning","NET_DVR_Logout error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
            return;
        }
        userID = -1; //注销
    }*/

    //uninitSDK();
    qDebug() << "Logout Success!" << userID << endl;
    ui->graphicsView->close();
    //this->close();
}

//开始预览
void Widget::on_PreviewButton_clicked()
{
    if(userID == -1){
        qDebug() << "Need Login!" << endl;
        QMessageBox::warning(this , "error" , "Need Login!");
        return;
    }

    HWND hWnd = (HWND)ui->graphicsView->winId();        //HWND是windows api的窗体句柄类型
    NET_DVR_PREVIEWINFO struPlayInfo;
    previewID = play(hWnd, struPlayInfo);
    if(previewID == -1){
        return;
    }

    qDebug() << "Preview Success!" << userID << endl;

    cameraTimer->start(1000);//预览之后开始计时器  1000ms触发一次slots槽函数   timeout()信号
}

//停止预览
void Widget::on_StopPreviewButton_clicked()
{
    //关闭预览
    NET_DVR_StopRealPlay(previewID);
    /*if(previewID != -1){
        if(NET_DVR_StopRealPlay(previewID) == false){
            qDebug() << "NET_DVR_StopRealPlay error;error number is " << NET_DVR_GetLastError();
            QMessageBox::warning(this, "Warning","NET_DVR_StopRealPlay error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
            return;
        }
        previewID = -1;
    }*/

    qDebug() << "Stop Preview Success!" << userID << endl;
    //ui->graphicsView->close();
}

void Widget::putRoad(){
    QImage Image;
    Image.load("C:\\Users\\LLW\\Desktop\\Qt_HK_camera\\Qt_HK_camera\\image\\Road.png");
    QPixmap pixmap = QPixmap::fromImage(Image);

    //roadLabel = ui->RoadLabel;
    //roadLabel->setPixmap(QPixmap("C:\\Users\\LLW\\Desktop\\Qt_HK_camera\\Qt_HK_camera\\image\\Road.png"));
    //roadLabel->setScaledContents(true); //设置图片自适应控件大小
    //roadLabel->resize(this->size());
    wide = ui->RoadLabel->width();
    height = ui->RoadLabel->height();
    QPixmap fitpixmap = pixmap.scaled(wide, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); //按比例缩放  饱满填充
    ui->RoadLabel->setPixmap(fitpixmap);
    //roadLabel->setScaledContents(true); //设置图片自适应控件大小
    //roadLabel->resize(wide, height);

}

/*void Widget::receiveLogin(){
    this->close();
    Widget *w = new Widget();
    w->show();
}*/

void Widget::on_savedata_clicked()
{
    //HWND hWnd = (HWND)ui->graphicsView->winId();        //HWND是windows api的窗体句柄类型
    //NET_DVR_PREVIEWINFO struPlayInfo;
    //previewID = play(hWnd , struPlayInfo);
    bool save_ReturnValue = NET_DVR_SaveRealData_V30(previewID, 0x1, const_cast<char*>("C:\\Users\\LLW\\Desktop\\Qt_HK_camera\\Qt_HK_camera\\data\\test1.mp4"));// 0x1 : PS文件层，主要用于存储，也可用于传输  // 0x2 : 3GPP文件层，用于存储
    if(save_ReturnValue == false){
        qDebug() << "NET_DVR_SaveRealData_V30 error;error number is " << NET_DVR_GetLastError();
        QMessageBox::warning(this, "Warning","NET_DVR_SaveRealData_V30 error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
        return;
    }

    qDebug() << "SaveRealData Success!" << userID << endl;
}

void Widget::on_stopsave_clicked()
{
    NET_DVR_StopSaveRealData(previewID);
    /*if(NET_DVR_StopSaveRealData(previewID) == false){
        qDebug() << "NET_DVR_StopSaveRealData error;error number is " << NET_DVR_GetLastError();
        QMessageBox::warning(this, "Warning","NET_DVR_StopSaveRealData error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
        return;
    }
    NET_DVR_StopSaveRealData(previewID) == true;*/

    qDebug() << "StopSaveRealData Success!" << userID << endl;
}

//获取相机实时视频流时间
void Widget::extractCameraRealTime(){
    //NET_DVR_TIME_V50 CameraRealTime;
    bool iRet;
    DWORD dwReturnLeo;
    NET_DVR_TIME CameraRealTime = { 0 };

    iRet = NET_DVR_GetDVRConfig(userID, NET_DVR_GET_TIMECFG, 1, &CameraRealTime, sizeof(NET_DVR_TIME), &dwReturnLeo);
    if (!iRet){
        printf("NET_DVR_GetDVRConfig NET_DVR_GET_TIMECFG error.\n");
        NET_DVR_Logout(userID);
        NET_DVR_Cleanup();
    }
    /*qDebug() <<CameraRealTime.dwYear<<"年"<<CameraRealTime.dwMonth<<"月"<<CameraRealTime.dwDay<<"日"
             <<CameraRealTime.dwHour<<":"<<CameraRealTime.dwMinute<<":"<<CameraRealTime.dwSecond<<endl;*/

    return;
}

//普通string类型转UTF-8编码格式字符串
std::string string_To_UTF8(const std::string& str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

    wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
    ZeroMemory(pwBuf, nwLen * 2 + 2);

    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char* pBuf = new char[nLen + 1];
    ZeroMemory(pBuf, nLen + 1);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr(pBuf);

    delete[]pwBuf;
    delete[]pBuf;

    pwBuf = NULL;
    pBuf = NULL;

    return retStr;
}

//报警回调函数及HTTP传输
void CALLBACK MSesGCallback(LONG lCommand, NET_DVR_ALARMER* pAlarmer, char* pAlarmInfo, DWORD dwBufLen, void* pUser)
{
    //qDebug()<<"MSesGCallback Succeed!";
    int i = 0;
    char filename[100];
    FILE *fSnapPic = NULL;
    FILE *fSnapPicPlate = NULL;
    //cout << "lCommand=" << lCommand << endl;

    switch (lCommand)
    {
    //交通抓拍结果(新报警消息)
        case COMM_ITS_PLATE_RESULT:
        {
            NET_ITS_PLATE_RESULT struITSPlateResult = { 0 };

            memcpy(&struITSPlateResult, pAlarmInfo, sizeof(struITSPlateResult));

            //数据内容
            char hkDataTime[100] = { 0 };
            sprintf(hkDataTime, "%02d-%02d-%02d% 02d:%02d:%02d:%02d", struITSPlateResult.struSnapFirstPicTime.wYear, struITSPlateResult.struSnapFirstPicTime.byMonth, struITSPlateResult.struSnapFirstPicTime.byDay, struITSPlateResult.struSnapFirstPicTime.byHour, struITSPlateResult.struSnapFirstPicTime.byMinute, struITSPlateResult.struSnapFirstPicTime.bySecond, struITSPlateResult.struSnapFirstPicTime.wMilliSec);
            char hkcarNum[100] = { 0 };
            sprintf(hkcarNum, "%s", struITSPlateResult.struPlateInfo.sLicense);
           qDebug()<<hkcarNum<<endl;
            char hkcarType[100] = { 0 };
            sprintf(hkcarType, "%d", struITSPlateResult.byVehicleType);
            char hkdriveChan[100] = { 0 };
            sprintf(hkdriveChan, "%d", struITSPlateResult.byDriveChan);
            char hkcarDirectiontype[100] = { 0 };
            sprintf(hkcarDirectiontype, "%d", struITSPlateResult.byCarDirectionType);
            char hkcarSpeed[100] = { 0 };
            sprintf(hkcarSpeed, "%d", struITSPlateResult.struVehicleInfo.wSpeed);
            char hkPilotSafebelt[100] = { 0 };
            sprintf(hkPilotSafebelt, "%d", struITSPlateResult.byPilotSafebelt);
            char hkIllegalType[100] = { 0 };
            sprintf(hkIllegalType, "%d", struITSPlateResult.wIllegalType);
            char hkPendant[100] = { 0 };
            sprintf(hkPendant, "%d", struITSPlateResult.byPendant);
            char hkCopilotSafebelt[100] = { 0 };
            sprintf(hkCopilotSafebelt, "%d", struITSPlateResult.byCopilotSafebelt);
            char hkPilotCall[100] = { 0 };
            sprintf(hkPilotCall, "%d", struITSPlateResult.byPilotCall);
            char hkCarColor[100] = { 0 };
            sprintf(hkCarColor, "%d", struITSPlateResult.struVehicleInfo.byColor);
            char hkVehicleLogoRecog[100] = { 0 };
            sprintf(hkVehicleLogoRecog, "%d", struITSPlateResult.struVehicleInfo.byVehicleLogoRecog);

            char postParams[1024];
            memset(postParams, 0, sizeof(postParams));
            sprintf(postParams, "{\n\"snapFirstPicTime\":\"%03d-%02d-%02d% 02d:%02d:%02d:%02d\",\n\"carNum\":\"%s\",\n\" carType\":\"%d\",\n\" driveChan\":\"%d\",\n\" carDirectiontype\":\"%d\",\n\" carSpeed\":\"%d\",\n\" PilotSafebelt\":\"%d\",\n\" IllegalType\":\"%d\",\n\" byPendant\":\"%d\",\n\" CopilotSafebelt\":\"%d\",\n\" PilotCall\":\"%d\",\n\" CarColor\":\"%d\",\n\" VehicleLogoRecog\":\"%d\"\n}",
                struITSPlateResult.struSnapFirstPicTime.wYear, struITSPlateResult.struSnapFirstPicTime.byMonth, struITSPlateResult.struSnapFirstPicTime.byDay, struITSPlateResult.struSnapFirstPicTime.byHour, struITSPlateResult.struSnapFirstPicTime.byMinute, struITSPlateResult.struSnapFirstPicTime.bySecond, struITSPlateResult.struSnapFirstPicTime.wMilliSec, struITSPlateResult.struPlateInfo.sLicense, struITSPlateResult.byVehicleType,
                struITSPlateResult.byDriveChan, struITSPlateResult.byCarDirectionType, struITSPlateResult.struVehicleInfo.wSpeed, struITSPlateResult.byPilotSafebelt, struITSPlateResult.wIllegalType, struITSPlateResult.byPendant, struITSPlateResult.byCopilotSafebelt, struITSPlateResult.byPilotCall, struITSPlateResult.struVehicleInfo.byColor, struITSPlateResult.struVehicleInfo.byVehicleLogoRecog);
            std::string JsonData = string_To_UTF8((std::string) postParams);
            //qDebug()<<JsonData<<endl;
            qDebug()<<postParams<<endl;

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
            for (i = 0; i < struITSPlateResult.dwPicNum; i++)
            {
                //保存场景图
                //struITSPlateResult.struPicInfo[i].byType：数据类型，0 - 车牌图、1 - 场景图、2 - 合成图、3 - 特写图
                //struPicInfo[6];    //图片信息,单张回调，最多6张图，由序号区分
                if ((struITSPlateResult.struPicInfo[i].dwDataLen != 0) && (struITSPlateResult.struPicInfo[i].byType == 1) || (struITSPlateResult.struPicInfo[i].byType == 2))
                {
                    sprintf(filename, "C:\\Users\\LLW\\Desktop\\Qt_HK_camera\\Qt_HK_camera\\pic\\%s_%d.jpg", struITSPlateResult.struPlateInfo.sLicense, i);
                    fSnapPic = fopen(filename, "wb");   //w是以文本方式打开文件，wb是二进制方式打开文件
                    fwrite(struITSPlateResult.struPicInfo[i].pBuffer, struITSPlateResult.struPicInfo[i].dwDataLen, 1, fSnapPic);
                    //iNum++;
                    fclose(fSnapPic);
                }

                //车牌图
                if (struITSPlateResult.struPicInfo[i].dwDataLen != 0 && struITSPlateResult.struPicInfo[i].byType == 0)
                {
                    sprintf(filename, "C:\\Users\\LLW\\Desktop\\Qt_HK_camera\\Qt_HK_camera\\pic\\%s.jpg", struITSPlateResult.struPlateInfo.sLicense);
                    fSnapPicPlate = fopen(filename, "wb");
                    fwrite(struITSPlateResult.struPicInfo[i].pBuffer, struITSPlateResult.struPicInfo[i].dwDataLen, 1, fSnapPicPlate);
                    //iNum++;
                    fclose(fSnapPicPlate);
                }
            }	//其他信息处理
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

            break;
        }
        default:
        {
            //std::cout << lCommand << endl;
            break;
        }
    }

    return;
}

void Widget::SetMessageCallBack(){
    qDebug()<<"SetMessageCallBack Succeed!";

    NET_DVR_SetDVRMessageCallBack_V30(MSesGCallback, NULL);//设置报警回调函数
    if(NET_DVR_SetDVRMessageCallBack_V30(MSesGCallback, NULL) == false){
            qDebug() << "NET_DVR_SetDVRMessageCallBack_V30 error;error number is " << NET_DVR_GetLastError();
            QMessageBox::warning(this, "Warning","NET_DVR_SetDVRMessageCallBack_V30 error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
            return;
        }

    return;
}

void Widget::on_SetupAlarm_clicked()
{
   //启用布防
   NET_DVR_SETUPALARM_PARAM struSetupParam = { 0 };
   struSetupParam.dwSize = sizeof(NET_DVR_SETUPALARM_PARAM);
   struSetupParam.byLevel = 1; //布防优先级：0- 一等级（高），1- 二等级（中）
   struSetupParam.byAlarmInfoType = 1; //上传报警信息类型: 0- 老报警信息(NET_DVR_PLATE_RESULT), 1- 新报警信息(NET_ITS_PLATE_RESULT)

   lHandle = NET_DVR_SetupAlarmChan_V41(userID, &struSetupParam);
   if (lHandle < 0)
   {
       qDebug()<<"NET_DVR_SetupAlarmChan_V41 failed, error code: %d\n"<< NET_DVR_GetLastError();
       QMessageBox::warning(this, "Warning","NET_DVR_SetupAlarmChan_V41 error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
       NET_DVR_Logout(userID);
       NET_DVR_Cleanup();
       return;
   }
   else
   {
       qDebug()<<"SetupAlarm Succeed!\n";
   }

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
   NET_DVR_SetDVRMessageCallBack_V30(MSesGCallback, NULL);//设置报警回调函数
   if(NET_DVR_SetDVRMessageCallBack_V30(MSesGCallback, NULL) == false){
           qDebug() << "NET_DVR_SetDVRMessageCallBack_V30 error;error number is " << NET_DVR_GetLastError();
           QMessageBox::warning(this, "Warning","NET_DVR_SetDVRMessageCallBack_V30 error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
           return;
       }
   //Sleep(60000);
}

void Widget::on_CloseAlarm_clicked()
{
//撤销布防上传通道
    NET_DVR_CloseAlarmChan_V30(lHandle);

    /*if (NET_DVR_CloseAlarmChan_V30(lHandle) == false) //布防句柄lHandle
    {
        qDebug() << "NET_DVR_CloseAlarmChan_V30 Failed! Error number：" << NET_DVR_GetLastError();
        QMessageBox::warning(this, "Warning","NET_DVR_CloseAlarmChan_V30 error;error number " + QString::number(NET_DVR_GetLastError()));//弹出提示框
        NET_DVR_Logout(userID);
        NET_DVR_Cleanup();
        return;
    }*/

    lHandle = -1;//布防句柄

    qDebug()<<"CloseAlarm Succeed!\n";
}

void Widget::on_Exit_clicked()
{
    this->close();
}
