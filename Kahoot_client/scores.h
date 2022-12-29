#ifndef SCORES_H
#define SCORES_H

#include <QMainWindow>
#include <QWidget>

namespace Ui {
class scores;
}

class scores : public QWidget
{
    Q_OBJECT

public:
    explicit scores(QMainWindow *m, QWidget *parent = nullptr);
    ~scores();

private:
    Ui::scores *ui;
    QMainWindow *mainWindow;
};

#endif // SCORES_H
