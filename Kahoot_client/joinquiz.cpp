#include "joinquiz.h"
#include "ui_joinquiz.h"

joinQuiz::joinQuiz(QMainWindow* m,QWidget *parent) :
    QWidget(parent), mainWindow(m),
    ui(new Ui::joinQuiz)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    closeSocket = true;
    ui->codeEdit->setValidator( new QIntValidator(0, 9999, this) );

    QFile file(":/config.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->buttonBox->setEnabled(true);
        QMessageBox::information(0,"error",file.errorString());
        return;
    }
    QTextStream in(&file);
    adres = in.readLine();
    port = in.readLine().toInt();
    connectToServer();

}

void joinQuiz::accept() {
    if(ui->nameEdit->text() == "" || ui->codeEdit->text() == "") return;
    ui->buttonBox->setEnabled(false);
    connect(sock, &QTcpSocket::readyRead, this, &joinQuiz::joinResponse);
    QString request = QString("joinGame:") + QString(ui->codeEdit->text());
    sock->write(request.toUtf8());

}

void joinQuiz::reject() {
    mainWindow->show();
    this->close();
}

joinQuiz::~joinQuiz() {
    if(closeSocket && sock)
        sock->close();
    delete ui;
}

void joinQuiz::socketConnected(){
    connTimeoutTimer->stop();
    connTimeoutTimer->deleteLater();
    connTimeoutTimer=nullptr;
    ui->buttonBox->setEnabled(true);

}

void joinQuiz::socketDisconnected(){
    mainWindow->show();
    this->close();
}

void joinQuiz::socketError(QTcpSocket::SocketError err){
    if(err == QTcpSocket::RemoteHostClosedError)
        return;
    if(connTimeoutTimer){
        connTimeoutTimer->stop();
        connTimeoutTimer->deleteLater();
        connTimeoutTimer=nullptr;
    }
    QMessageBox::critical(this, "Error", sock->errorString());
    ui->buttonBox->setEnabled(true);
}

void joinQuiz::joinResponse(){
    QByteArray ba = sock->readAll();
    port = ba.toInt();
    sock->close();
    connectToServer();
    ui->codeEdit->setEnabled(false);
    connect(sock, &QTcpSocket::readyRead, this, &joinQuiz::nameResponse);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(acceptName()));
    sock->write(ui->nameEdit->text().toUtf8());

}

void joinQuiz::nameResponse(){
    QByteArray ba = sock->readAll();
    QString response = QString(ba);
    if(response == "accepted") {
        closeSocket = false;
        QWidget *wdg = new Lobby(mainWindow, sock);
        wdg->show();
        this->close();
    } else {
        QMessageBox::critical(this,"Error", "Invalid name");
        ui->buttonBox->setEnabled(true);
    }
}

void joinQuiz::acceptName(){
    sock->write(ui->nameEdit->text().toUtf8());
    ui->buttonBox->setEnabled(false);
}

void joinQuiz::connectToServer(){
    if(sock)
        delete sock;
    sock = new QTcpSocket(this);
    connTimeoutTimer = new QTimer(this);
    connTimeoutTimer->setSingleShot(true);
    connect(connTimeoutTimer, &QTimer::timeout, [&]{
        sock->abort();
        sock->deleteLater();
        sock = nullptr;
        connTimeoutTimer->deleteLater();
        connTimeoutTimer=nullptr;
        QMessageBox::critical(this, "Error", "Connect timed out");
    });

    connect(sock, &QTcpSocket::connected, this, &joinQuiz::socketConnected);
    connect(sock, &QTcpSocket::disconnected, this, &joinQuiz::socketDisconnected);
    connect(sock, &QTcpSocket::errorOccurred, this, &joinQuiz::socketError);

    sock->connectToHost(QString(adres), port);
    connTimeoutTimer->start(3000);
}
