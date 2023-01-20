#ifndef CREATEQUESTION_H
#define CREATEQUESTION_H

#include <QWidget>
#include <QMainWindow>
#include "connectscreen.h"
#include <QMessageBox>

namespace Ui {
class createQuestion;
}

class createQuestion : public QWidget
{
    Q_OBJECT

public:
    explicit createQuestion(QMainWindow* m, int n, int t, QWidget *parent = nullptr);
    ~createQuestion();

private:
    Ui::createQuestion *ui;
    QMainWindow* mainWindow;
    int numberOfQuestions;
    int questionNumber;
    int time;
    int answerId;
};

#endif // CREATEQUESTION_H
