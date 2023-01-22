#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>

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
    void socketDisconnected();
    void socketReadable();
};

#endif // LOBBY_H
