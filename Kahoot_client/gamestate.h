#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QMainWindow>
#include <QWidget>

namespace Ui {
class GameState;
}

class GameState : public QWidget
{
    Q_OBJECT

public:
    explicit GameState(QMainWindow* m, QWidget *parent = nullptr);
    ~GameState();

private:
    Ui::GameState *ui;
    QMainWindow* mainWindow;

};

#endif // GAMESTATE_H
