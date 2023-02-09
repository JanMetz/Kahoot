#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>
#include "scores.h"

namespace Ui {
class GameState;
}

class GameState : public QWidget
{
    Q_OBJECT

public:
    explicit GameState(QMainWindow* m, QTcpSocket* s, int num, QWidget *parent = nullptr);
    ~GameState();

private:
    Ui::GameState *ui;
    QMainWindow* mainWindow;
    QTcpSocket * sock {nullptr};
    int numPlayers;
    QStringList finalList;
    void socketDisconnected();
    void socketReadable();
    void addPlayer(QString player);
};

#endif // GAMESTATE_H
