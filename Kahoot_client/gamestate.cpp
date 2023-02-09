#include "gamestate.h"
#include "ui_gamestate.h"

GameState::GameState(QMainWindow* m, QTcpSocket* s, int num, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameState)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    mainWindow = m;
    sock = s;
    numPlayers = num;
    connect(sock, &QTcpSocket::readyRead, this, &GameState::socketReadable);
    connect(sock, &QTcpSocket::disconnected, this, &GameState::socketDisconnected);
}

GameState::~GameState()
{
    delete ui;
}

void GameState::socketDisconnected(){
    qDebug() << "disconnected";
}
void GameState::socketReadable(){
    int bytesToRead = sock->bytesAvailable();
    QStringList p = QString(sock->peek(bytesToRead)).split(":");
    qDebug() << p;
    if(p[0] == "theGameStarts") {
        bytesToRead = 14;
    }
    if(p[0] == "question" && p.size() < 7){
        return;
    } else if (p[0] == "question") {
        QString buffer = "";
        for(int i = 0; i < 7; i++) {
            buffer += p[i] + ":";
        }
        bytesToRead = buffer.size();
    }
    if (p[0] == "punctation") {
         QString buffer = "";
         for(int i = 0; i < 2 * numPlayers + 3; i++) {
             buffer += p[i] + ":";
            }
         bytesToRead = buffer.size();
    }
    if (p[0] == "allNicks" && p.size() < numPlayers + 1) {
        return;
    } else if (p[0] == "allNicks") {
        numPlayers--;
        QString buffer = "";
        for(int i = 0; i < numPlayers + 2; i++) {
            buffer += p[i] + ":";
        }
        bytesToRead = buffer.size();
    }
    QByteArray ba = sock->read(bytesToRead);
    QString msg = QString(ba);
    qDebug() << msg;
    QStringList list = msg.split(":");
    if (list[0] == "punctation"){
        finalList = list;
        ui->listWidget->clear();
        for(int i = 3; i < list.length() - 1; i += 2) {
            addPlayer(list[i] + QString(":") + list[i+1]);
        }
    }
    else if (list[0] == "theGameEnds") {
        sock->disconnectFromHost();
        sock->close();
        QWidget *wdg = new scores(mainWindow, finalList);
        wdg->show();
        this->close();
    }
    if(sock->bytesAvailable() > 0) socketReadable();
}

void GameState::addPlayer(QString player){
    QListWidgetItem *item;
    item = new QListWidgetItem(player);
    ui->listWidget->addItem(item);
}
