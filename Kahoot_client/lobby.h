#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QWidget>

namespace Ui {
class Lobby;
}

class Lobby : public QWidget
{
    Q_OBJECT

public:
    explicit Lobby(QMainWindow* m, QWidget *parent = nullptr);
    ~Lobby();

private:
    Ui::Lobby *ui;
    QMainWindow* mainWindow;
};

#endif // LOBBY_H
