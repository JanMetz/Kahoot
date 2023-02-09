#include "currentscores.h"
#include "ui_currentscores.h"

CurrentScores::CurrentScores(QStringList scores, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentScores)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    for(int i = 3; i < scores.length() - 1; i += 2) {
        addPlayer(scores[i] + QString(":") + scores[i+1]);
    }
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
