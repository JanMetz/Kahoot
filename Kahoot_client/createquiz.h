#ifndef CREATEQUIZ_H
#define CREATEQUIZ_H

#include <QWidget>
#include <QMainWindow>
#include "createquestion.h"

namespace Ui {
class createQuiz;
}

class createQuiz : public QWidget
{
    Q_OBJECT
public:
    explicit createQuiz(QMainWindow* m, QWidget *parent = nullptr);
    ~createQuiz();
public slots:
    void accept();
    void reject();
private:
    Ui::createQuiz *ui;
    QMainWindow* mainWindow;
};

#endif // CREATEQUIZ_H
