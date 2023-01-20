#include "gamestate.h"
#include "ui_gamestate.h"

GameState::GameState(QMainWindow* m, QTcpSocket* s ,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameState)
{
    ui->setupUi(this);
    mainWindow = m;
    sock = s;
}

GameState::~GameState()
{
    delete ui;
    if(sock) sock->close();;
}

void GameState::socketDisconnected(){

}
void GameState::socketReadable(){

}
