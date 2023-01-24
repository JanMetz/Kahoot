#ifndef CREATEQUESTION_H
#define CREATEQUESTION_H

#include <QWidget>
#include <QMainWindow>
#include "connectscreen.h"
#include <QMessageBox>
#include <vector>
#include <array>

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
    std::vector<std::array<QString, 6>> questions;
};

#endif // CREATEQUESTION_H
