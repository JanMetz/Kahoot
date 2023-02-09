#include "scores.h"
#include "ui_scores.h"

scores::scores(QMainWindow *m, QStringList list, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::scores)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    mainWindow = m;
    for(int i = 3; i < list.length() - 1; i += 2) {
        addPlayer(list[i] + QString(":") + list[i+1].left(4));
    }
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
       mainWindow->show();
       this->close();
    });
}

scores::~scores()
{
    delete ui;
}


void scores::addPlayer(QString player){
    QListWidgetItem *item;
    item = new QListWidgetItem(player);
    ui->listWidget->addItem(item);
}
