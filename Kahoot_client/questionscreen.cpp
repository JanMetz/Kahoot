#include "questionscreen.h"
#include "ui_questionscreen.h"

QuestionScreen::QuestionScreen(QMainWindow *m, QTcpSocket* s, QString n, int num, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuestionScreen)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    mainWindow = m;
    sock = s;
    nick = n;
    numPlayers = num;
    qDebug()<<numPlayers;
    if(sock->bytesAvailable() > 0) socketReadable();
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
    int bytesToRead = sock->bytesAvailable();
    QStringList p = QString(sock->peek(bytesToRead)).split(":");
    qDebug() << p;
    if(p[0] == "question" && p.size() < 7){
        return;
    } else if (p[0] == "question") {
        QString buffer = "";
        for(int i = 0; i < 7; i++) {
            buffer += p[i] + ":";
        }
        bytesToRead = buffer.size();
    }
    if(p[0] == "punctation" && p.size() < 2 * numPlayers + 1) {
       return;
    } else if (p[0] == "punctation") {
        QString buffer = "";
        for(int i = 0; i < 2 * numPlayers + 3; i++) {
            buffer += p[i] + ":";
        }
        bytesToRead = buffer.size();
    }
    QByteArray ba = sock->read(bytesToRead);
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
        finalList = list;
        if(scoresScreen) {
            scoresScreen->close();
            delete scoresScreen;
        }
        scoresScreen = new CurrentScores(this, list);
        this->hide();
        scoresScreen->show();
    }
    else if (list[0] == "theGameEnds") {
        sock->disconnectFromHost();
        sock->close();
        if(scoresScreen) {
            scoresScreen->close();
            delete scoresScreen;
        }
        scoresScreen = new scores(mainWindow, finalList);
        scoresScreen->show();
        this->close();
    }
    if(sock->bytesAvailable() > 0) socketReadable();
}

void QuestionScreen::enableButtons(bool f){
    ui->buttonA->setEnabled(f);
    ui->buttonB->setEnabled(f);
    ui->buttonC->setEnabled(f);
    ui->buttonD->setEnabled(f);
}
