#include "loginwindow.h"
#include "ui_loginwindow.h"
#include<QtDebug> //调试输出到控制台头文件
#include"widget.h"
//#include<QSettings>

#pragma execution_character_set("utf-8");//可以显示汉字

loginWindow::loginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loginWindow)
{
    ui->setupUi(this);

    QImage Image;
    Image.load(":/images/image/Login.jpg");
    QPixmap pixmap = QPixmap::fromImage(Image);
    ui->backGround->setPixmap(pixmap);//设置背景图

    //connect(ui->Login, &QPushButton::clicked, this, &loginWindow::on_Login_clicked);  //按钮名称 ； 触发方式：click 点击 ； ；功能函数
    //connect(ui->Exit, &QPushButton::clicked, this, &loginWindow::on_Exit_clicked);
}

loginWindow::~loginWindow()
{
    delete ui;
}

void loginWindow::on_Login_clicked()
{
    user = ui->accountNumber->text(); //账号
    password = ui->passWord->text(); //密码

    if((user == "admin") && (password == "fh123456")){
        QMessageBox::information(this, "提示", "登陆成功");
        //writeSettings(); //保存设置
        this->hide();
        //emit showmain();  //emit 自定义的一个信号
        Widget *w = new Widget(); //创建新的ui界面指针
        w->setWindowTitle("实时展示界面");
        w->show();//显示新的ui界面
    }
    else if((user == "") || (password == "")){
        QMessageBox::information(this, "提示", "用户名或密码不能为空");
        ui->accountNumber->clear();
        ui->accountNumber->setFocus(); //将焦点移至指定的控件或窗体
    }

    else if(user != "admin"){
        QMessageBox::information(this, "提示", "用户名不存在，请重新输入");
        ui->accountNumber->clear();
        ui->accountNumber->setFocus();
    }

    else if((user == "admin") && (password!= "fh123456")){
        QMessageBox::information(this, "提示", "用户名密码不匹配，请重新输入");
        ui->accountNumber->setFocus();
    }
}

//登录信息写入注册表
/*void loginWindow::writeSettings(){
    QSettings settings("AXB-Qt", "Qt_HK_camera");
    settings.setValue("userName", "admin");
    settings.setValue("passWord", "fh123456");
    settings.setValue("save", ui->rememberPassword->isChecked());
}*/

//读取存储的用户和密码
/*void loginWindow::readSettings(){
    QString organization = "AXB-Qt";//用于注册表
    QString appName = "Qt_HK_camera";

    QSettings settings(organization, appName); //创建

    //bool saved = settings.value("save", false).toBool();//读取saved键的值

}*/

void loginWindow::on_Exit_clicked()
{
    this->close();
}

//设置回车响应函数
void loginWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        emit ui->Login->click(); //emit 自定义的一个信号
    }
}

