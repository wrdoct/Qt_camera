#include "widget.h"
#include <QApplication>
#include"loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Widget w;
    //w.show();

    loginWindow login;

    login.setWindowTitle("登录界面");
    login.show();
    //test3

    //QObject::connect(&login, SIGNAL(showmain()), &w, SLOT(receiveLogin())); //第一个参数sender为发送对象；第二个参数为发送对象里面的一个信号；第三个参数为接收对象；第四个参数为接收对象里面的槽函数

    return a.exec();
}
