#include "startquiz.h"
#include "ui_startquiz.h"

StartQuiz::StartQuiz(QMainWindow* m, QTcpSocket* s, int c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartQuiz)
{
    ui->setupUi(this);
    mainWindow = m;
    sock = s;
    code = c;
    connect(sock, &QTcpSocket::disconnected, this, &StartQuiz::socketDisconnected);
    connect(sock, &QTcpSocket::readyRead, this, &StartQuiz::socketReadable);
    connect(ui->startButton, &QPushButton::clicked, this, [&]{
        QString msg = QString("startTheGame");
        sock->write(msg.toUtf8());
        QWidget *wdg = new GameState(mainWindow, sock);
        wdg->show();
        this->close();
    });
    connect(ui->cancelButton, &QPushButton::clicked, this, [&]{
        if(sock)
            sock->close();
        mainWindow->show();
        this->close();
    });    
    ui->YourCode->setText(QString::number(code));
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

void StartQuiz::socketDisconnected(){
    mainWindow->show();
    this->close();
}



void StartQuiz::socketReadable(){
    QByteArray ba = sock->readAll();
    addPlayer(QString(ba));
}
