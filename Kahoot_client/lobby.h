#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>
#include "questionscreen.h"

namespace Ui {
class Lobby;
}

class Lobby : public QWidget
{
    Q_OBJECT

public:
    explicit Lobby(QMainWindow* m, QTcpSocket* s ,QWidget *parent = nullptr);
    ~Lobby();

private:
    Ui::Lobby *ui;
    QMainWindow* mainWindow;
    QTcpSocket * sock ;
    bool closeSocket;
    void socketDisconnected();
    void socketReadable();
    void addPlayer(QString player);
    void removePlayer(QString plyayer);
};

#endif // LOBBY_H
