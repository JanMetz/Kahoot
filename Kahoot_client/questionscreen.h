#ifndef QUESTIONSCREEN_H
#define QUESTIONSCREEN_H

#include <QMainWindow>
#include <QWidget>
#include "scores.h"

namespace Ui {
class QuestionScreen;
}

class QuestionScreen : public QWidget
{
    Q_OBJECT

public:
    explicit QuestionScreen(QMainWindow *m, QWidget *parent = nullptr);
    ~QuestionScreen();

private:
    Ui::QuestionScreen *ui;
    QMainWindow *mainWindow;
};

#endif // QUESTIONSCREEN_H
