#ifndef JOINQUIZ_H
#define JOINQUIZ_H

#include <QMainWindow>
#include <QWidget>
#include "lobby.h"

namespace Ui {
class joinQuiz;
}

class joinQuiz : public QWidget
{
    Q_OBJECT

public:
    explicit joinQuiz(QMainWindow* m, QWidget *parent = nullptr);
    ~joinQuiz();
public slots:
    void accept();
    void reject();
private:
    Ui::joinQuiz *ui;
    QMainWindow* mainWindow;
};

#endif // JOINQUIZ_H
