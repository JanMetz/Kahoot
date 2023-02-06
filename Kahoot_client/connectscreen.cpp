#include "connectscreen.h"
#include "ui_connectscreen.h"

ConnectScreen::ConnectScreen(QMainWindow* m, int n, int t,
        std::vector<std::array<QString, 6>> q, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectScreen)
{
    ui->setupUi(this);
    ui->progressBar->hide();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(n*6);
    ui->progressBar->setValue(0);
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
    connectToServer(sock);
}

void ConnectScreen::accept(){
    ui->buttonBox->setEnabled(false);
    connect(sock, &QTcpSocket::readyRead, this, [&]{
        qDebug() << "reading port";
        QByteArray ba = sock->readAll();
        QString msg = QString(ba);
        QStringList list = msg.split(":");
        code = list[0].toInt();
        port = list[1].toInt();
        qDebug() << "code: " << code;
        qDebug() << "port: " << port;
        sock->disconnectFromHost();
        sock->close();
        //if(sock)
        //    delete sock;
        sock = new QTcpSocket(this);
        connect(sock, &QTcpSocket::connected, this, &ConnectScreen::sendQuestions);
        connectToServer(sock);
    });
    qDebug() << "createGame";
    sock->write(QString("createGame:").toUtf8());
}

void ConnectScreen::sendQuestions(){
    socketConnected();
    qDebug() << "sending nick";
    sock->write(QString("creator").toUtf8());
    connect(sock, &QTcpSocket::readyRead, this, &ConnectScreen::socketAccepted);
    ui->progressBar->show();
}

void ConnectScreen::socketAccepted(){
    QString message = QString(sock->readAll());
    qDebug() << "accepted message: " << message;
    QStringList list = message.split(":");
    if (list[0] == "accepted") {
        disconnect(sock, &QTcpSocket::readyRead, this, &ConnectScreen::socketAccepted);
        connect(sock, &QTcpSocket::readyRead, this, &ConnectScreen::socketReadable);
        QString msg = QString::number(numberOfQuestions) + ":" + QString::number(time) + ":";
        qDebug() << "sending: numberOfQuestions, time";
        sock->write(msg.toUtf8());
    }
}

void ConnectScreen::reject(){
    if(sock)
        sock->close();
    mainWindow->show();
    this->close();
}

ConnectScreen::~ConnectScreen(){
    delete ui;
}

void ConnectScreen::socketConnected(){
    qDebug() << "connected";
    connTimeoutTimer->stop();
    connTimeoutTimer->deleteLater();
    connTimeoutTimer=nullptr;
}

void ConnectScreen::socketDisconnected(){
    qDebug() << "disconnected";
}

void ConnectScreen::socketError(QTcpSocket::SocketError err){
    ui->buttonBox->setEnabled(true);
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


void ConnectScreen::connectToServer(QTcpSocket* socket){
    qDebug() << "connecting";
    connTimeoutTimer = new QTimer(this);
    connTimeoutTimer->setSingleShot(true);
    connect(connTimeoutTimer, &QTimer::timeout, [&]{
        qDebug() << "Connect timed out";
        socket->abort();
        socket->deleteLater();
        socket = nullptr;
        connTimeoutTimer->deleteLater();
        connTimeoutTimer=nullptr;
        QMessageBox::critical(this, "Error", "Connect timed out");
    });
    connect(socket, &QTcpSocket::disconnected, this, &ConnectScreen::socketDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ConnectScreen::socketError);
    socket->connectToHost(QString(adres), port);
    connTimeoutTimer->start(3000);
}

void ConnectScreen::socketReadable(){
    QByteArray ba =sock->readAll();
    QString msg = QString(ba);
    qDebug() << "received message: " << msg;
    if (msg != "sendQuestion:" && msg != "sendAnswer:" &&
            msg != "provideCorrectMsgIndex") {
        qDebug() << "return " << msg;
        return;
    }
    qDebug() <<"sending";
    if(i == 0) {
        QString q = questions[num][i];
        qDebug() << q;
        sock->write(q.toUtf8());
        i++;
    } else if (i > 0 && i < 5) {
        QString q = questions[num][i];
        qDebug() << q;
        sock->write(q.toUtf8());
        i++;
    } else {
        QString q =  questions[num][i];
        qDebug() << q;
        sock->write(q.toUtf8());
        num++;
        i = 0;
        if (num >= numberOfQuestions) {
            QWidget *wdg = new StartQuiz(mainWindow, sock, code);
            wdg->show();
            this->close();
        }
    }
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}
