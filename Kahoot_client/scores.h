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
    explicit scores(QMainWindow *m, QTcpSocket* s, QWidget *parent = nullptr);
    ~scores();

private:
    Ui::scores *ui;
    QMainWindow *mainWindow;
    QTcpSocket * sock ;
    void socketDisconnected();
    void socketReadable();
    void addPlayer(QString player);
    void removePlayer(QString player);
};

#endif // SCORES_H
