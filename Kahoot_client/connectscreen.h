#ifndef CONNECTSCREEN_H
#define CONNECTSCREEN_H

#include <QTcpSocket>
#include <QWidget>
#include <QMainWindow>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <vector>
#include <array>
#include "startquiz.h"

namespace Ui {
class ConnectScreen;
}

class ConnectScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectScreen(QMainWindow* m, int n, int t, std::vector<std::array<QString, 6>> q, QWidget *parent = nullptr);
    ~ConnectScreen();
public slots:
    void accept();
    void reject();
private:
    Ui::ConnectScreen *ui;
    QMainWindow* mainWindow;
    int numberOfQuestions;
    int time;
    int port;
    int code;
    int i;
    int num;
    std::vector<std::array<QString, 6>> questions;
    QString adres;
    QTcpSocket * sock {nullptr};
    QTimer * connTimeoutTimer{nullptr};
    void socketConnected();
    void socketDisconnected();
    void socketError(QTcpSocket::SocketError);
    void connectToServer();
    void sendQuestions();
    void socketReadable();
};

#endif // CONNECTSCREEN_H
