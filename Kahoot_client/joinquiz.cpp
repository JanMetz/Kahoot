#include "joinquiz.h"
#include "ui_joinquiz.h"

joinQuiz::joinQuiz(QMainWindow* m,QWidget *parent) :
    QWidget(parent), mainWindow(m),
    ui(new Ui::joinQuiz)
{
    ui->setupUi(this);    
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
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
    port0 = port;
    sock = new QTcpSocket(this);
    connectToServer();
}

void joinQuiz::accept() {
    if(ui->nameEdit->text() == "" || ui->codeEdit->text() == "") return;
    ui->buttonBox->setEnabled(false);
    connect(sock, &QTcpSocket::readyRead, this, &joinQuiz::joinResponse);
    QString request = QString("joinGame:") + QString(ui->codeEdit->text());
    qDebug() << "send: " << request;
    sock->write(request.toUtf8());
}

void joinQuiz::reject() {
    if(sock) {
        sock->disconnectFromHost();
        sock->close();
    }
    mainWindow->show();
    this->close();
}

joinQuiz::~joinQuiz() {
    delete ui;
}

void joinQuiz::socketConnected(){
    connTimeoutTimer->stop();
    connTimeoutTimer->deleteLater();
    connTimeoutTimer=nullptr;
    ui->buttonBox->setEnabled(true);
}

void joinQuiz::socketDisconnected(){
    qDebug() << "disconnected";
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
    QString message = QString(sock->readAll());
    qDebug() << "joinReponse: " << message;
    QStringList list = message.split(":");
    if (list[0] != "gamePort"){
        if(list[0] == "invalidCode")
            QMessageBox::critical(this, "Error", "Invalid code");
        ui->buttonBox->setEnabled(true);
        return;
    }
    port = list[1].toInt();
    sock->disconnectFromHost();
    sock->close();
    sock = new QTcpSocket(this);
    connect(sock, &QTcpSocket::readyRead, this, &joinQuiz::nameResponse);
    connect(sock, &QTcpSocket::connected, this,[&]{sock->write(ui->nameEdit->text().toUtf8());});
    connectToServer();
}

void joinQuiz::nameResponse(){
    QString message = QString(sock->readAll());
    qDebug() << "nameResponse: " << message;
    QStringList list = message.split(":");
    if(list[0] == "accepted") {
        disconnect(sock, &QTcpSocket::readyRead, this, &joinQuiz::nameResponse);
        disconnect(sock, &QTcpSocket::connected, this, &joinQuiz::socketConnected);
        disconnect(sock, &QTcpSocket::disconnected, this, &joinQuiz::socketDisconnected);
        disconnect(sock, &QTcpSocket::errorOccurred, this, &joinQuiz::socketError);
        QStringList nicks;
        for(int i = 3; i < list.length(); i++) {
            if(list[i] == "allNicks")
                break;
            nicks.append(list[i]);
        }
        QWidget *wdg = new Lobby(mainWindow, sock, ui->nameEdit->text(), nicks);
        wdg->show();
        this->close();
    } else {
        sock->disconnectFromHost();
        sock->close();
        QMessageBox::critical(this,"Error", "Invalid name");
        port = port0;
        sock = new QTcpSocket(this);
        connect(sock, &QTcpSocket::connected, this,[&]{ui->buttonBox->setEnabled(true);});
        connectToServer();
    }
}

void joinQuiz::connectToServer(){
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
