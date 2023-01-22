#include "questionscreen.h"
#include "ui_questionscreen.h"

QuestionScreen::QuestionScreen(QMainWindow *m, QTcpSocket* s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuestionScreen)
{
    ui->setupUi(this);
    mainWindow = m;
    sock = s;
    connect(sock, &QTcpSocket::disconnected, this, &QuestionScreen::socketDisconnected);
    connect(sock, &QTcpSocket::readyRead, this, &QuestionScreen::socketReadable);
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
        mainWindow->show();
        this->close();
    });
}

QuestionScreen::~QuestionScreen()
{
    delete ui;
}

void QuestionScreen::socketDisconnected(){

}



void QuestionScreen::socketReadable(){
    QByteArray ba = sock->readAll();

}
