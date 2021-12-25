#include "qmyvideowidget.h"
#include<QVideoWidget>
#include<QMouseEvent>

#include<QTimer>

QmyVideoWidget::QmyVideoWidget(QWidget *parent) : QVideoWidget(parent)
{

}

void QmyVideoWidget::mousePressEvent(QMouseEvent *event)
{//鼠标事件，单击控制暂停和继续播放
    if(event->button() == Qt::LeftButton){ //按下左键
        if(thePlayer->state() == QMediaPlayer::PlayingState){ //如果指针在播放
            thePlayer->pause(); //暂停

            //timer1->stop();
            //timer2->stop();
        }
        else{
            thePlayer->play(); //播放

            //timer1->start(1);//定时器1每间隔一毫秒去对比雷达数据
            //timer2->start(100);//定时器2每100毫秒去获取视频时间进行校准
        }
    }
    QVideoWidget::mousePressEvent(event); //再次判断事件（循环）
}

void QmyVideoWidget::setMediaPlayer(QMediaPlayer *mediaPlayer)
{//设置播放器
    thePlayer=mediaPlayer;
}
