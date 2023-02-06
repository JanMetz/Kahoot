#ifndef JOINQUIZ_H
#define JOINQUIZ_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <QIntValidator>

#include "lobby.h"

namespace Ui {
class joinQuiz;
}

class joinQuiz : public QWidget
{
    Q_OBJECT

public:
    explicit joinQuiz(QMainWindow* m, QWidget *parent = nullptr);
    ~joinQuiz();
public slots:
    void accept();
    void reject();
    //void acceptName();
private:
    Ui::joinQuiz *ui;
    QMainWindow* mainWindow;
    bool closeSocket;
protected:
    QTcpSocket * sock {nullptr};
    QTimer * connTimeoutTimer{nullptr};
    QString adres;
    int port;
    int port0;
    void connectToServer();
    void socketConnected();
    void socketDisconnected();
    void socketError(QTcpSocket::SocketError);
    void joinResponse();
    void nameResponse();
};

#endif // JOINQUIZ_H
