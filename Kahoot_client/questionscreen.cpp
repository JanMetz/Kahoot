#include "questionscreen.h"
#include "ui_questionscreen.h"

QuestionScreen::QuestionScreen(QMainWindow *m, QTcpSocket* s, QString n, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuestionScreen)
{
    ui->setupUi(this);
    mainWindow = m;
    sock = s;
    nick = n;
    end = false;
    connect(sock, &QTcpSocket::disconnected, this, &QuestionScreen::socketDisconnected);
    connect(sock, &QTcpSocket::readyRead, this, &QuestionScreen::socketReadable);
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
        mainWindow->show();
        this->close();
    });
    connect(ui->buttonA, &QPushButton::clicked, this, [&]{
        QString answer = generateAnswer(ui->buttonA->text());
        sock->write(answer.toUtf8());
    });
    connect(ui->buttonB, &QPushButton::clicked, this, [&]{
       QString answer = generateAnswer(ui->buttonB->text());
        sock->write(answer.toUtf8());
    });
    connect(ui->buttonC, &QPushButton::clicked, this, [&]{
        QString answer = generateAnswer(ui->buttonC->text());
        sock->write(answer.toUtf8());
    });
    connect(ui->buttonD, &QPushButton::clicked, this, [&]{
        QString answer = generateAnswer(ui->buttonD->text());
        sock->write(answer.toUtf8());
    });
}

QString QuestionScreen::generateAnswer(QString answerBody){
    enableButtons(false);
    float timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    QString answer = QString("answer:") + answerBody + QString(":") + nick + QString(":") + QString::number(timestamp);
    qDebug() << "answer: " << answer;
    return answer;
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
    QStringList p = QString(sock->peek(sock->bytesAvailable())).split(":");
    if(p[0] == "question"){
        if(p.size() < 7) return;
    }
    QByteArray ba = sock->readAll();
    QString msg = QString(ba);
    qDebug() << msg;
    QStringList list = msg.split(":");
    if(list[0] == "question") {
        ui->textQuestion->setText(list[1]);
        ui->buttonA->setText(list[3]);
        ui->buttonB->setText(list[4]);
        ui->buttonC->setText(list[5]);
        ui->buttonD->setText(list[6]);
        enableButtons(true);
    }
    else if (list[0] == "punctation"){
        if(end) {
            sock->disconnectFromHost();
            sock->close();
            QWidget *wdg = new scores(mainWindow, list);
            wdg->show();
            this->close();
        } else {
            QWidget *wdg = new CurrentScores(list);
            wdg->show();
        }
    }
    else if (list[0] == "theGameEnds") {
        end = true;
    }
}

void QuestionScreen::enableButtons(bool f){
    ui->buttonA->setEnabled(f);
    ui->buttonB->setEnabled(f);
    ui->buttonC->setEnabled(f);
    ui->buttonD->setEnabled(f);
}
