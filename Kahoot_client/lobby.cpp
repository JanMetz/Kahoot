#include "lobby.h"
#include "ui_lobby.h"

Lobby::Lobby(QMainWindow* m, QTcpSocket* s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Lobby)
{
    ui->setupUi(this);
    mainWindow = m;
    sock = s;
    connect(sock, &QTcpSocket::disconnected, this, &Lobby::socketDisconnected);
    connect(sock, &QTcpSocket::readyRead, this, &Lobby::socketReadable);
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
        if(sock) sock ->close();
        mainWindow->show();
        this->close();
    });
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::socketDisconnected(){

}
void Lobby::socketReadable(){

}
