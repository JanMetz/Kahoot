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
    explicit Lobby(QMainWindow* m, QTcpSocket* s, QString name, QStringList nicks, QWidget *parent = nullptr);
    ~Lobby();

private:
    Ui::Lobby *ui;
    QMainWindow* mainWindow;
    QTcpSocket * sock ;
    void socketDisconnected();
    void socketReadable();
    void addPlayer(QString player);
    void removePlayer(QString plyayer);
    QString yourNick;
};

#endif // LOBBY_H
