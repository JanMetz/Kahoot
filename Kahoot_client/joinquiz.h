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
private:
    Ui::joinQuiz *ui;
    QMainWindow* mainWindow;
    bool closeSocket;
protected:
    QTcpSocket * sock {nullptr};
    QTimer * connTimeoutTimer{nullptr};
    void connectBtnHit();
    void socketConnected();
    void socketDisconnected();
    void socketError(QTcpSocket::SocketError);
    void socketReadable();
};

#endif // JOINQUIZ_H
