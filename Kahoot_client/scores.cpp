#include "scores.h"
#include "ui_scores.h"

scores::scores(QMainWindow *m, QTcpSocket* s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::scores)
{
    ui->setupUi(this);
    mainWindow = m;
    sock = s;
    connect(sock, &QTcpSocket::disconnected, this, &scores::socketDisconnected);
    connect(sock, &QTcpSocket::readyRead, this, &scores::socketReadable);
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
       mainWindow->show();
       this->close();
    });
}

scores::~scores()
{
    delete ui;
}

void scores::socketDisconnected(){

}

void scores::addPlayer(QString player){
    QListWidgetItem *item;
    item = new QListWidgetItem(player);
    ui->listWidget->addItem(item);
}

void scores::removePlayer(QString player){
    QList<QListWidgetItem *> l;
    l = ui->listWidget->findItems(player, Qt::MatchExactly);;
    for(auto x: l){
        ui->listWidget->removeItemWidget(x);
        delete x;
    }
}

void scores::socketReadable(){
    QByteArray ba = sock->readAll();

}
