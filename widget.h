#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QDateTime>
#include"include/HCNetSDK.h"
//#include"Windows.h"
#include<qt_windows.h>
#include<QMessageBox> //消息提示框头文件

#include<QJsonDocument>
#include<QJsonParseError>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonValue>
#include<QFile>
#include <QTimer>

//#include"loginwindow.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    bool initSDK();
    bool uninitSDK();
    long play(HWND hWnd, NET_DVR_PREVIEWINFO struPlayInfo); //NET_DVR_PREVIEWINFO  预览

    void parseRadar();
    void parseFusion();

    //virtual void resizeEvent(QResizeEvent *event); //给窗口添加背景图片  虚函数
    void putRoad(); //增设车道图片

    //void receiveLogin();

    std::string string_To_UTF8(const std::string& str);
    //void CALLBACK MSesGCallback(LONG lCommand, NET_DVR_ALARMER* pAlarmer, char* pAlarmInfo, DWORD dwBufLen, void* pUser);

    void SetMessageCallBack();

private slots:

    void on_offlinedata_clicked();

    void on_LoginButton_clicked();

    void on_LogoutButton_clicked();

    void on_PreviewButton_clicked();

    void on_StopPreviewButton_clicked();

    void on_savedata_clicked();

    void on_stopsave_clicked();

    void extractCameraRealTime();

    void on_SetupAlarm_clicked();

    void on_CloseAlarm_clicked();

    void on_Exit_clicked();

private:
    Ui::Widget *ui;//UI::Widget就是一个Widget界面 , 初始化一个Widget界面指针，其变量名为ui
    long userID;
    long previewID;

    //QLabel * roadLabel;
    int wide;int height;

    QTimer *cameraTimer;//计时器指针

    LONG lHandle;//报警布防/撤防

};

#endif // WIDGET_H
