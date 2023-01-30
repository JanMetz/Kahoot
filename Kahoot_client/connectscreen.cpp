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
    i = 0;
    num = 0;
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
        qDebug() << code;
        qDebug() << port;
        sock->close();
        if(sock)
            delete sock;
        sock = new QTcpSocket(this);
        connect(sock, &QTcpSocket::connected, this, &ConnectScreen::sendQuestions);
        connectToServer();
    });
    qDebug() << "createGame";
    sock->write(QString("createGame:").toUtf8());
}

void ConnectScreen::sendQuestions(){
    sock->write(QString("nick:creator").toUtf8());
    QString msg = QString::number(numberOfQuestions) + ":" + QString::number(time) + ":";
    qDebug() << "sending info";
    sock->write(msg.toUtf8());
    connect(sock, &QTcpSocket::readyRead, this, &ConnectScreen::socketReadable);
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

void ConnectScreen::socketReadable(){
    QByteArray ba =sock->readAll();
    QString msg = QString(ba);
    qDebug() << msg;
    if (msg != "sendQuestion:" && msg != "sendAnswer:" &&
            msg != "provideCorrectMsgIndex") {
         qDebug() << "return";
        return;
    }
    qDebug() <<"sending";
    if(i == 0) {
        QString q = QString("question:") + questions[num][i];
        qDebug() << q;
        sock->write(q.toUtf8());
        i++;
    } else if (i > 0 && i < 5) {
        QString q = QString("answer:") + questions[num][i];
        qDebug() << q;
        sock->write(q.toUtf8());
        i++;
    } else {
        QString q = QString("correctAnswerIndex:") + questions[num][i];
        qDebug() << q;
        sock->write(q.toUtf8());
        num++;
        i = 0;
        if (num > numberOfQuestions) {
            QWidget *wdg = new StartQuiz(mainWindow, sock, code);
            wdg->show();
            this->close();
        }
    }
}
