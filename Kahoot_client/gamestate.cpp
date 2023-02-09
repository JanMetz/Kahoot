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
    end = false;
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
    //TODO:theGameStarts, question
    if (p[0] == "punctation") {
         QString buffer = "";
         for(int i = 0; i < 2 * numPlayers + 1; i++) {
             buffer += p[i] + ":";
            }
         bytesToRead = buffer.size();
    }
    QByteArray ba = sock->read(bytesToRead);
    QString msg = QString(ba);
    qDebug() << msg;
    QStringList list = msg.split(":");
    if (list[0] == "punctation"){
        if(end) {
            sock->disconnectFromHost();
            sock->close();
            QWidget *wdg = new scores(mainWindow, list);
            wdg->show();
            this->close();
        } else {
            ui->listWidget->clear();
            for(int i = 3; i < list.length() - 1; i += 2) {
                addPlayer(list[i] + QString(":") + list[i+1]);
            }
        }
    }
    else if (list[0] == "theGameEnds") {
        end = true;
    }
}

void GameState::addPlayer(QString player){
    QListWidgetItem *item;
    item = new QListWidgetItem(player);
    ui->listWidget->addItem(item);
}
