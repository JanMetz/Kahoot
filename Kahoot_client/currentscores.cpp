#include "currentscores.h"
#include "ui_currentscores.h"

CurrentScores::CurrentScores(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentScores)
{
    ui->setupUi(this);
}

CurrentScores::~CurrentScores()
{
    delete ui;
}


void CurrentScores::addPlayer(QString player){
    QListWidgetItem *item;
    item = new QListWidgetItem(player);
    ui->listWidget->addItem(item);
}

void CurrentScores::removePlayer(QString player){
    QList<QListWidgetItem *> l;
    l = ui->listWidget->findItems(player, Qt::MatchExactly);;
    for(auto x: l){
        ui->listWidget->removeItemWidget(x);
        delete x;
    }
}
