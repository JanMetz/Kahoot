#ifndef STARTQUIZ_H
#define STARTQUIZ_H

#include <QWidget>
#include <QMainWindow>
#include "gamestate.h"

namespace Ui {
class StartQuiz;
}

class StartQuiz : public QWidget
{
    Q_OBJECT

public:
    explicit StartQuiz(QMainWindow* m, QWidget *parent = nullptr);
    ~StartQuiz();

private:
    Ui::StartQuiz *ui;
    QMainWindow* mainWindow;
    void addPlayer(QString player);
    void removePlayer(QString player);
};

#endif // STARTQUIZ_H
