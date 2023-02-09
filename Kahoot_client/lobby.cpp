#include "lobby.h"
#include "ui_lobby.h"

Lobby::Lobby(QMainWindow* m, QTcpSocket* s, QString name, QStringList nicks, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Lobby)
{
    ui->setupUi(this);
    mainWindow = m;
    sock = s;
    ui->label->setText(name);
    yourNick = name;
    for(int i = 0; i < nicks.length(); i++) {
        addPlayer(nicks[i]);
    }
    connect(sock, &QTcpSocket::disconnected, this, &Lobby::socketDisconnected);
    connect(sock, &QTcpSocket::readyRead, this, &Lobby::socketReadable);
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
        if(sock) {
            sock->disconnectFromHost();
            sock ->close();
        }
        mainWindow->show();
        this->close();
    });
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::socketDisconnected(){
    qDebug() << "disconnected";
}
void Lobby::socketReadable(){
    QByteArray ba = sock->readAll();
    QString msg = QString(ba);
    qDebug() << msg;
    QStringList list = msg.split(":");
    if(list[0] == "theGameStarts") {
        disconnect(sock, &QTcpSocket::disconnected, this, &Lobby::socketDisconnected);
        disconnect(sock, &QTcpSocket::readyRead, this, &Lobby::socketReadable);
        QWidget *wdg = new QuestionScreen(mainWindow, sock, yourNick);
        wdg->show();
        this->close();
    }
    if(list[0] == "Error! Terminating game") {
        if(sock)
            sock->close();
        mainWindow->show();
        this->close();
    }
    if(list[0] == "allNicks") {
        ui->listWidget->clear();
        for(int i = 2; i < list.length(); i++) {
            if(list[i] == "allNicks") break;
            addPlayer(list[i]);
        }
    }
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
