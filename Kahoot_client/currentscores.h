#ifndef CURRENTSCORES_H
#define CURRENTSCORES_H

#include <QWidget>

namespace Ui {
class CurrentScores;
}

class CurrentScores : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentScores(QStringList scores, QWidget *parent = nullptr);
    ~CurrentScores();

private:
    Ui::CurrentScores *ui;
    void addPlayer(QString player);
};

#endif // CURRENTSCORES_H
