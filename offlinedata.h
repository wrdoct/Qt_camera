#ifndef OFFLINEDATA_H
#define OFFLINEDATA_H

#include <QWidget>
#include<QDateTime>
#include<QMessageBox> //消息提示框头文件

#include<QJsonDocument>
#include<QJsonParseError>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonValue>
#include<QFile>

#include<QMediaPlayer>
#include<QVideoWidget>
#include<QMediaPlaylist>
#include<QUrl>

#include "qmyvideowidget.h"

//#include<QMouseEvent>
#include<QTimer>
#include<QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsRectItem>

namespace Ui {
class offlineData;
}

class offlineData : public QWidget
{
    Q_OBJECT

public:
    explicit offlineData(QWidget *parent = nullptr);
    ~offlineData();

    void offline_putRoad(); //增设车道图片

    void addCar(int num, QStringList px, QStringList py, QStringList roadid, QStringList vx, QStringList vy, QStringList carid);

/*protected:
    void mousePressEvent(QMouseEvent *event);*/

private slots:

    void on_Return_clicked();

    void on_radarData_clicked();

    void on_fusionData_clicked();

    void on_openfile_clicked();

    void on_play_clicked();

    void on_pause_clicked();

    void on_stop_clicked();

    //自定义槽函数
    void onstateChanged(QMediaPlayer::State state);
    void onDurationChanged(qint64 duration);
    void onPositionChanged(qint64 position);

    void on_sliderPosition_valueChanged(int value);//播放位置

    void readRadarData();
    void extractVideoTime();

private:
    Ui::offlineData *ui;

    QMediaPlayer *mediaPlayer;//视频播放器
    QString  durationTime;//
    QString  positionTime;//
    //QMediaPlaylist *mediaPlaylist;
    //QVideoWidget *videoWidget;

    //QLabel * offline_roadLabel;
    int offline_wide;int offline_height;

    QTimer *timer1;//定时器1每间隔一毫秒去对比雷达数据
    QTimer *timer2;//定时器2每100毫秒去获取视频时间进行校准
    QDateTime offline_startTime =  QDateTime::fromString(" 2021-12-02T15:32:13:397", " yyyy-MM-ddThh:mm:ss:zzz");
    QDateTime offline_realTime;

    QJsonArray array;
    QJsonValue iconArray;
    QJsonObject icon;

    QStringList px;
    QStringList py;
    QStringList roadid;
    QStringList vx;
    QStringList vy;
    QStringList carid;

    int offline_dataIndex = 0;

    //全局变量
    //QList<QLabel*> carlistOld;
    QList<QLabel*> carlistNew;
    //QList<QLabel*> carInfolist;
    QLabel *car;
    QGraphicsScene *scene; //QGraphicsScene类用于在场景中操作大量的2D图形元素
};

#endif // OFFLINEDATA_H
