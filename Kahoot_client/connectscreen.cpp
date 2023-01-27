#include "connectscreen.h"
#include "ui_connectscreen.h"

ConnectScreen::ConnectScreen(QMainWindow* m, int n, int t,
        std::vector<std::array<QString, 6>> q, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectScreen)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mainWindow = m;
    numberOfQuestions = n;
    time = t;
    questions = q;
    QFile file(":/config.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->buttonBox->setEnabled(true);
        QMessageBox::information(0,"error",file.errorString());
        return;
    }
    QTextStream in(&file);
    adres = in.readLine();
    port = in.readLine().toInt();
    if(sock)
        delete sock;
    sock = new QTcpSocket(this);
    connect(sock, &QTcpSocket::connected, this, &ConnectScreen::socketConnected);
    connectToServer();
}

void ConnectScreen::accept(){
    connect(sock, &QTcpSocket::readyRead, this, [&]{
        QByteArray ba = sock->readAll();
        QString msg = QString(ba);
        QStringList list = msg.split(";");
        code = list[0].toInt();
        port = list[1].toInt();
        sock->close();
        if(sock)
            delete sock;
        sock = new QTcpSocket(this);
        connect(sock, &QTcpSocket::connected, this, &ConnectScreen::sendQuestions);
        connectToServer();
    });
    sock->write(QString("createGame:").toUtf8());

}

void ConnectScreen::sendQuestions(){
    //connect(sock, &QTcpSocket::readyRead, this, &ConnectScreen::socketReadable);
    QString msg = "nick:creator";
    sock->write(msg.toUtf8());
    QString n = QString("numOfQuestions:") + QString::number(numberOfQuestions);
    sock->write(n.toUtf8());
    QString t = QString("time:") + QString::number(time);
    sock->write(t.toUtf8());
    for(std::array<QString, 6> &i :questions){
        QString q = QString("question:") + i[0];
        sock->write(q.toUtf8());
        QString a1 = QString("answer:") + i[1];
        sock->write(a1.toUtf8());
        QString a2 = QString("answer:") + i[2];
        sock->write(a2.toUtf8());
        QString a3 = QString("answer:") + i[3];
        sock->write(a3.toUtf8());
        QString a4 = QString("answer:") + i[4];
        sock->write(a4.toUtf8());
        QString c = QString("correctAnswerIndex:") + i[5];
        sock->write(c.toUtf8());
    }
    QWidget *wdg = new StartQuiz(mainWindow, sock, code);
    wdg->show();
    this->close();
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


void ConnectScreen::connectToServer(){

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

    connect(sock, &QTcpSocket::disconnected, this, &ConnectScreen::socketDisconnected);
    connect(sock, &QTcpSocket::errorOccurred, this, &ConnectScreen::socketError);

    sock->connectToHost(QString(adres), port);
    connTimeoutTimer->start(3000);
}
