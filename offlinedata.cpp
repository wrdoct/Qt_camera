#include "offlinedata.h"
#include "ui_offlinedata.h"
#include<QtDebug> //调试输出到控制台头文件
#include"widget.h"

#include<QFileDialog>
#include "qmyvideowidget.h"

#include<QLabel>
#include<QGraphicsProxyWidget>

offlineData::offlineData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::offlineData)
{
    ui->setupUi(this);

    mediaPlayer = new QMediaPlayer;//创建视频播放器
    mediaPlayer->setNotifyInterval(1000);//信息更新周期
    mediaPlayer->setVideoOutput(ui->videoWidget);//视频显示组件
    //ui->videoWidget->setStyleSheet("background:transparent;border:2px solid red;");//窗口初始颜色

    /*mediaPlayer->setMedia(QUrl::fromLocalFile("C:\\Users\\LLW\\Desktop\\Qt_HK_camera\\Qt_HK_camera\\data\\视频.mp4"));//读取离线视频
    ui->videoWidget->show();
    mediaPlayer->play();*/

    ui->videoWidget->setMediaPlayer(mediaPlayer);//设置显示组件的关联播放器
    connect(mediaPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),   //状态
            this, SLOT(onstateChanged(QMediaPlayer::State)));
    connect(mediaPlayer,SIGNAL(durationChanged(qint64)),   //时长
            this, SLOT(onDurationChanged(qint64)));
    connect(mediaPlayer,SIGNAL(positionChanged(qint64)),   //位置
            this, SLOT(onPositionChanged(qint64)));


    QImage Image_OpenFile, ImagePlay, ImagePause, ImageStop;
    Image_OpenFile.load(":/images/image/OpenFile.bmp");
    ImagePlay.load(":/images/image/Play.bmp");
    ImagePause.load(":/images/image/Pause.bmp");
    ImageStop.load(":/images/image/Stop.bmp");
    QPixmap pixmap_openfile = QPixmap::fromImage(Image_OpenFile);
    QPixmap pixmap_play = QPixmap::fromImage(ImagePlay);
    QPixmap pixmap_pause = QPixmap::fromImage(ImagePause);
    QPixmap pixmap_stop = QPixmap::fromImage(ImageStop);
    ui->openfile->setIcon(pixmap_openfile);
    ui->play->setIcon(pixmap_play);
    ui->pause->setIcon(pixmap_stop);//暂停
    ui->stop->setIcon(pixmap_pause);//停止

    offline_putRoad();

    timer1 = new QTimer;
    connect(timer1, SIGNAL(timeout()), this, SLOT(readRadarData()));
    timer2 = new QTimer;
    connect(timer2, SIGNAL(timeout()), this, SLOT(extractVideoTime()));
}

offlineData::~offlineData()
{
    delete ui;
}

void offlineData::on_Return_clicked()
{
    this->hide();
    Widget *w = new Widget(); //创建新的ui界面指针
    w->setWindowTitle("实时展示界面");
    w->show();//显示新的ui界面
}

/*解析radar.json文件*/
void offlineData::on_radarData_clicked()
{
    QFile file(":/data/data/radar.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text); //QIODevice::ReadOnly 以只读方式打开 ； QIODevice::Text 在读取时，将行结束符转换成 \n；在写入时，将行结束符转换成本地格式
    QString value = file.readAll(); //先用QFile类将*.json文件所有内容读出
    file.close();

    QJsonParseError parseJsonErr;//定义错误参数
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8() , &parseJsonErr);//将读出的内容转换为QJsonDocument类     定义读入文件后保存的文件
    if(!(parseJsonErr.error == QJsonParseError::NoError)){
        qDebug() << "Parse Json Error!error number is :" << parseJsonErr.errorString();
        return;
    }
    else{
        qDebug() << "Parse Json Success!";
    }

    //QJsonObject jsonObject = document.object(); //将QJsonDocument类转换为一个具体的对象QJsonObject
    //qDebug() << "jsonObject[RecordTime] ==" << jsonObject["RecordTime"].toString();//获取第一级的一对：键值
    //QJsonValue arrayValue = jsonObject.value(QStringLiteral("xxxxx"));//将获取的对应键的值转化为json数组QJsonArray;将其复制给json数组QJsonArray    //QStringLiteral 从“字符串常量”创建QString对象的宏
    //if(arrayValue.isArray()){
        //QJsonArray array = arrayValue.toArray();//存入数组
    //}
    if(document.isArray()){
        QJsonArray array = document.array();//把document存入数组
        //qDebug()<< array.size()<<endl;
        for(int i = 0;i < array.size() ;i++){
            QJsonValue iconArray = array.at(i); //获取该数组下的第i个元素
            QJsonObject icon = iconArray.toObject(); //将该值转换为QJsonObject对象
            //qDebug()<<"icon="<<icon<<endl;
            QString RecordTime = icon["RecordTime"].toString();
            QDateTime radarTime = QDateTime::fromString(RecordTime," yyyy-MM-ddThh:mm:ss:zzz");//提取时间戳
            QString Px = icon["Px"].toString();//px.append(Px);    /* QString Px[0] = 1  ;  QStringList px[0] = 17122000 */
            QString Py = icon["Py"].toString();
            QString RoadId = icon["RoadId"].toString();
            QString Vx = icon["Vx"].toString();
            QString Vy = icon["Vy"].toString();
            QString CarID = icon["CarID"].toString();
            //ui->radardata->append(RecordTime);//在白框里显示时间
            qDebug() << "RecordTime =" << RecordTime;
            qDebug() << "radarTime =" << radarTime << "Px =" << Px << "Py =" << Py << "RoadId =" << RoadId << "Vx =" << Vx << "Vy =" << Vy << "CarID =" << CarID ;
        }     
    }
    //qDebug()<<"px[1] ="<<px[1]<<endl;
}

/*解析fusion.json文件*/
void offlineData::on_fusionData_clicked()
{
    QFile file(":/data/data/fusion.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text); //QIODevice::ReadOnly 以只读方式打开 ； QIODevice::Text 在读取时，将行结束符转换成 \n；在写入时，将行结束符转换成本地格式
    QString value = file.readAll(); //先用QFile类将*.json文件所有内容读出
    file.close();

    QJsonParseError parseJsonErr;//定义错误参数
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8() , &parseJsonErr);//将读出的内容转换为QJsonDocument类     定义读入文件后保存的文件
    if(!(parseJsonErr.error == QJsonParseError::NoError)){
        qDebug() << "Parse Json Error!error number is :" << parseJsonErr.errorString();
        return;
    }
    else{
        qDebug() << "Parse Json Success!";
    }

    if(document.isArray()){
        QJsonArray array = document.array();
        for(int i = 0;i < array.size(); i++){
            QJsonValue iconArray = array.at(i);
            QJsonObject icon = iconArray.toObject();
            QString RecordTime = icon["RecordTime"].toString();
            QDateTime Time = QDateTime::fromString(RecordTime," yyyy-MM-ddThh:mm:ss:zzz");//提取时间戳
            QString Px = icon["Px"].toString();
            QString Py = icon["Py"].toString();
            QString RoadId = icon["RoadId"].toString();
            QString Vx = icon["Vx"].toString();
            QString Vy = icon["Vy"].toString();
            QString CarID = icon["CarID"].toString();//雷达数据
            QString snapFirstPicTime = icon["snapFirstPicTime"].toString();QDateTime cameraTime = QDateTime::fromString(snapFirstPicTime, "yyyy-MM-dd hh:mm:ss:z");//提取时间戳
            QString carNum = icon["carNum"].toString();
            QString carType = icon["carType"].toString();
            QString driveChan = icon["driveChan"].toString();
            QString carDirectiontype = icon["carDirectiontype"].toString();
            QString carSpeed = icon["carSpeed"].toString();
            QString PilotSafebelt = icon["PilotSafebelt"].toString();
            QString IllegalType = icon["IllegalType"].toString();
            QString byPendant = icon["byPendant"].toString();
            QString CopilotSafebelt = icon["CopilotSafebelt"].toString();
            QString PilotCall = icon["PilotCall"].toString();
            QString CarColor = icon["CarColor"].toString();
            QString VehicleLogoRecog = icon["VehicleLogoRecog"].toString();//相机数据

            qDebug() << "RecordTime =" << RecordTime << endl;
            qDebug() << "Time =" << Time << endl;
            qDebug() << "Px =" << Px << endl;
            qDebug() << "Py =" << Py << endl;
            qDebug() << "RoadId =" << RoadId << endl;
            qDebug() << "Vx =" << Vx << endl;
            qDebug() << "Vy =" << Vy << endl;
            qDebug() << "CarID =" << CarID << endl;
            qDebug() << "snapFirstPicTime =" << snapFirstPicTime << endl;qDebug() << "cameraTime =" << cameraTime << endl;
            qDebug() << "carNum =" << carNum << endl;
            qDebug() << "carType =" << carType << endl;
            qDebug() << "driveChan =" << driveChan << endl;
            qDebug() << "carDirectiontype =" << carDirectiontype << endl;
            qDebug() << "carSpeed =" << carSpeed << endl;
            qDebug() << "PilotSafebelt =" << PilotSafebelt << endl;
            qDebug() << "IllegalType =" << IllegalType << endl;
            qDebug() << "byPendant =" << byPendant << endl;
            qDebug() << "CopilotSafebelt =" << CopilotSafebelt << endl;
            qDebug() << "PilotCall =" << PilotCall << endl;
            qDebug() << "CarColor =" << CarColor << endl;
            qDebug() << "VehicleLogoRecog =" << VehicleLogoRecog << endl;
        }
    }
}


void offlineData::on_openfile_clicked()
{//打开文件
    QString curPath=QDir::homePath();//获取系统当前目录
    QString dlgTitle="选择视频文件"; //对话框标题
    QString filter="mp4文件(*.mp4);;wmv文件(*.wmv);;所有文件(*.*)"; //文件过滤器
    QString aFile=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

    if (aFile.isEmpty())
      return;

    QFileInfo fileInfo(aFile);
    ui->LabCurMedia->setText(fileInfo.fileName());//显示当前播放视频名称

    mediaPlayer->setMedia(QUrl::fromLocalFile(aFile));//设置播放文件
    //mediaPlayer->play();

    //timer1->start(1);//定时器1每间隔一毫秒去对比雷达数据
    //timer2->start(100);//定时器2每100毫秒去获取视频时间进行校准
    /*timer1 = new QTimer;
    connect(timer1, SIGNAL(timeout()), this, SLOT(readRadarData()));
    timer2 = new QTimer;
    connect(timer2, SIGNAL(timeout()), this, SLOT(extractVideoTime()));*/

    offline_realTime = offline_startTime;
    //开始读取雷达数据
    QFile file(":/data/data/radar.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text); //QIODevice::ReadOnly 以只读方式打开 ； QIODevice::Text 在读取时，将行结束符转换成 \n；在写入时，将行结束符转换成本地格式
    QString value = file.readAll(); //先用QFile类将*.json文件所有内容读出
    file.close();

    QJsonParseError parseJsonErr;//定义错误参数
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8() , &parseJsonErr);//将读出的内容转换为QJsonDocument类     定义读入文件后保存的文件
    if(!(parseJsonErr.error == QJsonParseError::NoError)){
        qDebug() << "Parse Json Error!error number is :" << parseJsonErr.errorString();
        return;
    }
    else{
        qDebug() << "Parse Json Success!";
    }

    if(document.isArray()){
        array = document.array();//把document存入数组
        //qDebug()<< array.size()<<endl;//array.size()为{}的组数
        for(int i = 0;i < array.size() ;i++){
            iconArray = array.at(i); //获取该数组下的第i个元素(即第i个{})
            icon = iconArray.toObject(); //将该值转换为QJsonObject对象 即一个{xxxxxx}
            //qDebug()<<"icon="<<icon<<endl;
            QString RecordTime = icon["RecordTime"].toString();
            QDateTime radarTime = QDateTime::fromString(RecordTime," yyyy-MM-ddThh:mm:ss:zzz");//提取时间戳
            QString Px = icon["Px"].toString(); px.append(Px);
            QString Py = icon["Py"].toString();py.append(Px);
            QString RoadId = icon["RoadId"].toString();roadid.append(Px);
            QString Vx = icon["Vx"].toString();vx.append(Px);
            QString Vy = icon["Vy"].toString();vy.append(Px);
            QString CarID = icon["CarID"].toString();carid.append(Px);
         }
    }

    while(offline_realTime != QDateTime::fromString(array.at(offline_dataIndex).toObject()["RecordTime"].toString()," yyyy-MM-ddThh:mm:ss:zzz")){
        offline_dataIndex++;
    }
    //跳出while循环之后可以找到开始读取雷达数据的起始时间所对应的索引数 即视频起始时间对应的数据起始位置
    //qDebug()<<"offline_dataIndex="<<offline_dataIndex<<endl;
}

void offlineData::on_play_clicked()
{
    mediaPlayer->play();

    timer1->start(1);//定时器1每间隔一毫秒去对比雷达数据
    timer2->start(100);//定时器2每100毫秒去获取视频时间进行校准
}

void offlineData::on_pause_clicked()
{//暂停
    mediaPlayer->pause();

    timer1->stop();
    timer2->stop();
}

void offlineData::on_stop_clicked()
{//停止
    mediaPlayer->stop();

    //timer1->stop();
    //timer2->stop();
}

void offlineData::onstateChanged(QMediaPlayer::State state)
{//播放器状态变化
    ui->play->setEnabled(!(state==QMediaPlayer::PlayingState));
    ui->pause->setEnabled(state==QMediaPlayer::PlayingState);
    ui->stop->setEnabled(state==QMediaPlayer::PlayingState);
}

void offlineData::onDurationChanged(qint64 duration)
{//文件时长变化
    ui->sliderPosition->setMaximum(duration);

    int   secs=duration/1000;//秒
    int   mins=secs/60; //分钟
    secs=secs % 60;//余数秒
    durationTime=QString::asprintf("%d:%d",mins,secs);
    ui->LabRatio->setText(positionTime+"/"+durationTime);
}

void offlineData::onPositionChanged(qint64 position)
{//文件播放位置变化
    if (ui->sliderPosition->isSliderDown())
        return; //如果正在拖动滑条，退出

    ui->sliderPosition->setSliderPosition(position);//

    int   secs=position/1000;//秒
    int   mins=secs/60; //分钟
    secs=secs % 60;//余数秒
    positionTime=QString::asprintf("%d:%d",mins,secs);
    ui->LabRatio->setText(positionTime+"/"+durationTime);
}

void offlineData::on_sliderPosition_valueChanged(int value)
{//播放位置
   mediaPlayer->setPosition(value);
}

void offlineData::offline_putRoad(){
    /*QImage Image;
    Image.load(":/images/image/offline_road.png");
    QPixmap pixmap = QPixmap::fromImage(Image);

    offline_wide = ui->offline_RoadLabel->width();
    offline_height = ui->offline_RoadLabel->height();
    QPixmap fitpixmap = pixmap.scaled(offline_wide, offline_height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); //按比例缩放  饱满填充
    ui->offline_RoadLabel->setPixmap(fitpixmap);*/
    //roadLabel->setScaledContents(true); //设置图片自适应控件大小
    //roadLabel->resize(wide, height);

    ui->offline_Road_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//设置水平滚动条
    ui->offline_Road_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//设置垂直滚动条
    scene = new QGraphicsScene();//创建显示容器
    offline_wide = ui->offline_Road_graphicsView->width();
    offline_height = ui->offline_Road_graphicsView->height();
    scene->setSceneRect(0, 0, offline_wide, offline_height);//设置场景边框
    ui->offline_Road_graphicsView->setScene(scene);//借助graphicsView（QGraphicsView类）控件显示容器的内容
    scene->addPixmap(QPixmap(":/images/image/offline_road.png").scaled(offline_wide, offline_height)); //按比例缩放  饱满填充
}

void offlineData::addCar(int num, QStringList px, QStringList py, QStringList roadid, QStringList vx, QStringList vy, QStringList carid){
    //qDebug()<<"num ="<<num<<"carlistNew.size() ="<<carlistNew.size()<<endl;
    for(int i = 0;i < carlistNew.size();i++){
        QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget;
        proxy = scene->addWidget(carlistNew[i]);

        /*scene->removeItem(carlistNew[i]->graphicsProxyWidget()); //removeItem 从场景中移除项目项
        scene->removeItem(proxy);
        delete carlistNew[i];
        delete proxy;*/
    }

    qDeleteAll(carlistNew); //从场景中移除car并清除内存   经过测试仅用clear()是无法清除内存的
    carlistNew.clear();

    for(int k = 0;k < num;k++){
        car = new QLabel;
        carlistNew.append(car);

        carlistNew[k]->setAttribute(Qt::WA_TranslucentBackground); //设定该窗口透明显示
        carlistNew[k]->setPixmap(QPixmap(":/images/image/1.png").scaled(100,100));
        carlistNew[k]->setMinimumSize(100,100);
        carlistNew[k]->setMaximumSize(100,100);

        //move函数中第一个参数向右移动，第二个参数向下移动
        //carlistNew[k]->move((15000000-px[offline_dataIndex+k].toInt())/100000.0 * 4.0 - 30, (py[offline_dataIndex+k].toInt()+650000)/100000.0*20.0-30);
//        carlistNew[k]->move((py[offline_dataIndex + k].toInt() + 650000) / 100000 * 20 -30, 150 - px[offline_dataIndex + k].toInt() / 100000 - 30);

        //car->move(py[offline_dataIndex + k].toInt() / 100000, 150 - px[offline_dataIndex + k].toInt() / 100000);
        scene->addWidget(carlistNew[k]);
    }
    //qDebug()<<"num ="<<num<<endl;
}

/*
void offlineData::addCar(int i, QStringList Px, QStringList Py, QStringList Vx, QStringList Vy, QStringList Id, QStringList Road)
{
    for (int l=0;l<carlistNew.size();l++)
    {
        QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget;
        proxy = scene->addWidget(carlistNew[l]);

        //remove item
        scene->removeItem(carlistNew[l]->graphicsProxyWidget());
        scene->removeItem(proxy);
        delete carlistNew[l];
        delete proxy;
    }
    for (int l=0;l<carInfolist.size();l++)
    {
        QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget;
        proxy = scene->addWidget(carInfolist[l]);

        //remove item
        scene->removeItem(carInfolist[l]->graphicsProxyWidget());
        scene->removeItem(proxy);
        delete carInfolist[l];
        delete proxy;
    }

 QLabel *car;
 QLabel *carinfo;
    carlistnew.clear();
    carInfolist.clear();
    // qDebug()<<"sizeb:"<<carlistnew.size();
    for (int k=0;k<i;k++) {

        car=new QLabel;
        carinfo =new QLabel;
        carlistnew.append(car);

        carlistnew[k]->setAttribute(Qt::WA_TranslucentBackground);   //设定该窗口透明显示
        carlistnew[k]->setPixmap(QPixmap(":/images/images/car1.png").scaled(100,100));
        carlistnew[k]->setMinimumSize(100,100);
        carlistnew[k]->setMaximumSize(100,100);
       // car->move(100,100);
        //carlistnew[k]->setNum(10);
       // scene->addWidget(car);
        carlistnew[k]->move((15000000-px[dataIdex+k].toInt())/100000.0*4.0-30,(py[dataIdex+k].toInt()+650000)/100000.0*20.0-30);
        carInfolist.append(carinfo);
        //carInfolist[k]->setAttribute(Qt::WA_TranslucentBackground);
        carInfolist[k]->move((15000000-px[dataIdex+k].toInt())/100000.0*4.0-100,(py[dataIdex+k].toInt()+650000)/100000.0*20.0-30);
        carInfolist[k]->setText("Px: "+px[dataIdex+k]+"\n"+
                                "Py: "+py[dataIdex+k]+"\n"+
                                "Vx: "+vx[dataIdex+k]+"\n"+
                                "Vy: "+vy[dataIdex+k]+"\n"+
                                "Id: "+id[dataIdex+k]+"\n");

       //  qDebug()<<"pos:"<<car->pos()<<"x"<<px[dataIdex+k].toInt();
         //car->move(100,100);
       // carlistnew.append(car);
        scene->addWidget(carlistnew[k]);
        scene->addWidget(carInfolist[k]);

    }
    qDebug()<<i;
   // qDebug()<<"sizea:"<<carlistnew.size();

//    carlistold=carlistnew;
}
*/


void offlineData::readRadarData(){
    int num = 0;//总共要出现的车辆个数 即从视频时间开始到雷达数据结束的{}个数  初始化
    while(offline_realTime == QDateTime::fromString(array.at(offline_dataIndex).toObject()["RecordTime"].toString()," yyyy-MM-ddThh:mm:ss:zzz")){
        QJsonValue value = array.at(offline_dataIndex);
        icon = value.toObject();
        QString time = icon["RecordTime"].toString();
        QDateTime datatime = QDateTime::fromString(time, " yyyy-MM-ddThh:mm:ss:zzz");
        //qDebug()<<"offline_dataIndex="<<offline_dataIndex<<endl;
        offline_dataIndex++;
        num++;//从视频时间起始到雷达数据结束的{}个数
        //qDebug()<<"num="<<num<<endl;
        if(offline_realTime != QDateTime::fromString(array.at(offline_dataIndex).toObject()["RecordTime"].toString(), " yyyy-MM-ddThh:mm:ss:zzz")){
            addCar(num, px, py, roadid, vx, vy, carid);
        }
    }

    offline_realTime = offline_realTime.addMSecs(1); //addMSecs() 在当前时间对象上加多少毫秒

}

void offlineData::extractVideoTime(){
    offline_realTime = offline_startTime.addMSecs(mediaPlayer->position());//加上视频已走的时间得到新的时间
    while(offline_realTime.toTime_t() > (QDateTime::fromString(array.at(offline_dataIndex).toObject()["RecordTime"].toString(), " yyyy-MM-ddThh:mm:ss:zzz")).toTime_t()){
         offline_dataIndex++;//视频时间大于雷达数据时间 则雷达数据时间对应的索引增加
    }
    //qDebug()<<"offline_dataIndex="<<offline_dataIndex<<endl;
    offline_realTime = QDateTime::fromString(array.at(offline_dataIndex).toObject()["RecordTime"].toString(), " yyyy-MM-ddThh:mm:ss:zzz");//更新时间
}
