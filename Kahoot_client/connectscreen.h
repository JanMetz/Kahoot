#ifndef CONNECTSCREEN_H
#define CONNECTSCREEN_H

#include <QTcpSocket>
#include <QWidget>
#include <QMainWindow>
#include <QTimer>
#include <QMessageBox>
#include "startquiz.h"

namespace Ui {
class ConnectScreen;
}

class ConnectScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectScreen(QMainWindow* m, int n, int t, QWidget *parent = nullptr);
    ~ConnectScreen();
public slots:
    void accept();
    void reject();
private:
    Ui::ConnectScreen *ui;
    QMainWindow* mainWindow;
    int numberOfQuestions;
    int time;
    QTcpSocket * sock {nullptr};
    QTimer * connTimeoutTimer{nullptr};
    void socketConnected();
    void socketDisconnected();
    void socketError(QTcpSocket::SocketError);
    void socketReadable();
};

#endif // CONNECTSCREEN_H
