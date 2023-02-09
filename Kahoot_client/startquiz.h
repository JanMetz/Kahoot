#ifndef STARTQUIZ_H
#define STARTQUIZ_H

#include <QWidget>
#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include "gamestate.h"

namespace Ui {
class StartQuiz;
}

class StartQuiz : public QWidget
{
    Q_OBJECT

public:
    explicit StartQuiz(QMainWindow* m, QTcpSocket* s, int c, QWidget *parent = nullptr);
    ~StartQuiz();

private:
    Ui::StartQuiz *ui;
    QMainWindow* mainWindow;
    void addPlayer(QString player);
    void removePlayer(QString player);
    QTcpSocket * sock ;
    void socketDisconnected();
    void socketReadable();
    int code;
};

#endif // STARTQUIZ_H
