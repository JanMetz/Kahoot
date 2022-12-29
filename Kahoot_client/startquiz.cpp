#include "startquiz.h"
#include "ui_startquiz.h"

StartQuiz::StartQuiz(QMainWindow* m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartQuiz)
{
    ui->setupUi(this);
    mainWindow = m;
    connect(ui->startButton, &QPushButton::clicked, this, [&]{
        QWidget *wdg = new GameState(mainWindow);
        wdg->show();
        this->close();
    });
    connect(ui->cancelButton, &QPushButton::clicked, this, [&]{
        mainWindow->show();
        this->close();
    });    
}

StartQuiz::~StartQuiz()
{
    delete ui;
}

void StartQuiz::addPlayer(QString player){
    QListWidgetItem *item;
    item = new QListWidgetItem(player);
    ui->listWidget->addItem(item);
}

void StartQuiz::removePlayer(QString player){
    QList<QListWidgetItem *> l;
    l = ui->listWidget->findItems(player, Qt::MatchExactly);;
    for(auto x: l){
        ui->listWidget->removeItemWidget(x);
        delete x;
    }
}
