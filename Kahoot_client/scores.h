#ifndef SCORES_H
#define SCORES_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class scores;
}

class scores : public QWidget
{
    Q_OBJECT

public:
    explicit scores(QMainWindow *m, QStringList list, QWidget *parent = nullptr);
    ~scores();

private:
    Ui::scores *ui;
    QMainWindow *mainWindow;
    void addPlayer(QString player);
};

#endif // SCORES_H
