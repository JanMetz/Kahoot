#include "lobby.h"
#include "ui_lobby.h"

Lobby::Lobby(QMainWindow* m, QTcpSocket* s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Lobby)
{
    ui->setupUi(this);
    mainWindow = m;
    sock = s;
    closeSocket = true;
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
    if(closeSocket) sock->close();
    delete ui;
}

void Lobby::socketDisconnected(){

}
void Lobby::socketReadable(){
    QByteArray ba = sock->readAll();
    QString msg = QString(ba);
    if(msg == "startTheGame") {
        closeSocket = false;
        QWidget *wdg = new QuestionScreen(mainWindow, sock);
        wdg->show();
        this->close();
    }
    addPlayer(msg);
}

void Lobby::addPlayer(QString player){
    QListWidgetItem *item;
    item = new QListWidgetItem(player);
    ui->listWidget->addItem(item);
}

void Lobby::removePlayer(QString player){
    QList<QListWidgetItem *> l;
    l = ui->listWidget->findItems(player, Qt::MatchExactly);;
    for(auto x: l){
        ui->listWidget->removeItemWidget(x);
        delete x;
    }
}
