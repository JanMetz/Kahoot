#ifndef QUESTIONSCREEN_H
#define QUESTIONSCREEN_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>
#include "scores.h"

namespace Ui {
class QuestionScreen;
}

class QuestionScreen : public QWidget
{
    Q_OBJECT

public:
    explicit QuestionScreen(QMainWindow *m, QTcpSocket* s, QWidget *parent = nullptr);
    ~QuestionScreen();

private:
    Ui::QuestionScreen *ui;
    QMainWindow *mainWindow;
    QTcpSocket * sock ;
    void socketDisconnected();
    void socketReadable();
};

#endif // QUESTIONSCREEN_H
