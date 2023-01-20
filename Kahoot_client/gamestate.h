#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class GameState;
}

class GameState : public QWidget
{
    Q_OBJECT

public:
    explicit GameState(QMainWindow* m, QTcpSocket* s, QWidget *parent = nullptr);
    ~GameState();

private:
    Ui::GameState *ui;
    QMainWindow* mainWindow;
    QTcpSocket * sock {nullptr};

    void socketDisconnected();
    void socketReadable();
};

#endif // GAMESTATE_H
