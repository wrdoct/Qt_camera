#ifndef QMYVIDEOWIDGET_H
#define QMYVIDEOWIDGET_H

#include <QWidget>
#include<QVideoWidget>
#include<QMediaPlayer>

#include<QTimer>

class QmyVideoWidget : public QVideoWidget
{
    Q_OBJECT
public:
    explicit QmyVideoWidget(QWidget *parent = nullptr);

    void    setMediaPlayer(QMediaPlayer *mediaPlayer);

signals:

public slots:

private:
    QMediaPlayer  *thePlayer;

    QTimer *timer1;//定时器1每间隔一毫秒去对比雷达数据
    QTimer *timer2;//定时器2每100毫秒去获取视频时间进行校准

protected:
    void mousePressEvent(QMouseEvent *event);

};

#endif // QMYVIDEOWIDGET_H
