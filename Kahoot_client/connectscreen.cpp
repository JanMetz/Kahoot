#include "connectscreen.h"
#include "ui_connectscreen.h"

ConnectScreen::ConnectScreen(QMainWindow* m, int n, int t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectScreen)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mainWindow = m;
    numberOfQuestions = n;
    time = t;
}

void ConnectScreen::accept(){
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

    connect(sock, &QTcpSocket::connected, this, &ConnectScreen::socketConnected);
    connect(sock, &QTcpSocket::disconnected, this, &ConnectScreen::socketDisconnected);
    connect(sock, &QTcpSocket::errorOccurred, this, &ConnectScreen::socketError);
    connect(sock, &QTcpSocket::readyRead, this, &ConnectScreen::socketReadable);

    sock->connectToHost(QString("127.0.0.1"), 80);
    connTimeoutTimer->start(3000);

}

void ConnectScreen::reject(){
    if(sock)
        sock->close();
    mainWindow->show();
    this->close();
}

ConnectScreen::~ConnectScreen()
{
    delete ui;
}

void ConnectScreen::socketConnected(){
    connTimeoutTimer->stop();
    connTimeoutTimer->deleteLater();
    connTimeoutTimer=nullptr;
}

void ConnectScreen::socketDisconnected(){

}

void ConnectScreen::socketError(QTcpSocket::SocketError err){
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

void ConnectScreen::socketReadable(){
    QByteArray ba = sock->readAll();
    int code = ba.toInt();
    QWidget *wdg = new StartQuiz(mainWindow, sock, code);
    wdg->show();
    this->close();
}
