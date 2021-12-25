#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include<QKeyEvent> //QT键盘事件
//#include<thread> //多线程
#include<QMessageBox> //消息提示框头文件
//#include"widget.h"

namespace Ui {
class loginWindow;
}

class loginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit loginWindow(QWidget *parent = nullptr);
    ~loginWindow();

    void keyPressEvent(QKeyEvent*); //设置回车响应函数

    //void writeSettings(); //登录信息写入注册表
    //void readSettings(); //读取存储的用户名和密码

signals:
    void showmain();

private slots:
    void on_Login_clicked();

    void on_Exit_clicked();

private:
    Ui::loginWindow *ui;
    QString user;
    QString password;
};

#endif // LOGINWINDOW_H
