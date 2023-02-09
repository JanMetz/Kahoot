#ifndef QUESTIONSCREEN_H
#define QUESTIONSCREEN_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>
#include "scores.h"
#include "currentscores.h"

namespace Ui {
class QuestionScreen;
}

class QuestionScreen : public QWidget
{
    Q_OBJECT

public:
    explicit QuestionScreen(QMainWindow *m, QTcpSocket* s, QString n, QWidget *parent = nullptr);
    ~QuestionScreen();

private:
    Ui::QuestionScreen *ui;
    QMainWindow *mainWindow;
    QTcpSocket * sock ;
    QString nick;
    bool end;
    void socketDisconnected();
    void socketReadable();
    QString generateAnswer(QString answerBody);
    void enableButtons(bool f);
};

#endif // QUESTIONSCREEN_H
