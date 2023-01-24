#include "questionscreen.h"
#include "ui_questionscreen.h"

QuestionScreen::QuestionScreen(QMainWindow *m, QTcpSocket* s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuestionScreen)
{
    ui->setupUi(this);
    mainWindow = m;
    sock = s;
    connect(sock, &QTcpSocket::disconnected, this, &QuestionScreen::socketDisconnected);
    connect(sock, &QTcpSocket::readyRead, this, &QuestionScreen::socketReadable);
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
        mainWindow->show();
        this->close();
    });
    connect(ui->buttonA, &QPushButton::clicked, this, [&]{
        ui->buttonA->setEnabled(false);
        ui->buttonB->setEnabled(false);
        ui->buttonC->setEnabled(false);
        ui->buttonD->setEnabled(false);
        float timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        QString answer = QString("answerBody:") + ui->buttonA->text() +
                QString(" timestamp:") + QString::number(timestamp);
        sock->write(answer.toUtf8());
    });
    connect(ui->buttonB, &QPushButton::clicked, this, [&]{
        ui->buttonA->setEnabled(false);
        ui->buttonB->setEnabled(false);
        ui->buttonC->setEnabled(false);
        ui->buttonD->setEnabled(false);
        float timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        QString answer = QString("answerBody:") + ui->buttonB->text() +
                QString(" timestamp:") + QString::number(timestamp);
        sock->write(answer.toUtf8());
    });
    connect(ui->buttonC, &QPushButton::clicked, this, [&]{
        ui->buttonA->setEnabled(false);
        ui->buttonB->setEnabled(false);
        ui->buttonC->setEnabled(false);
        ui->buttonD->setEnabled(false);
        float timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        QString answer = QString("answerBody:") + ui->buttonC->text() +
                QString(" timestamp:") + QString::number(timestamp);
        sock->write(answer.toUtf8());
    });
    connect(ui->buttonD, &QPushButton::clicked, this, [&]{
        ui->buttonA->setEnabled(false);
        ui->buttonB->setEnabled(false);
        ui->buttonC->setEnabled(false);
        ui->buttonD->setEnabled(false);
        float timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        QString answer = QString("answerBody:") + ui->buttonD->text() +
                QString(" timestamp:") + QString::number(timestamp);
        sock->write(answer.toUtf8());
    });
}

QuestionScreen::~QuestionScreen()
{
    delete ui;
}

void QuestionScreen::socketDisconnected(){
    mainWindow->show();
    this->close();
}



void QuestionScreen::socketReadable(){
    QByteArray ba = sock->readAll();

}
