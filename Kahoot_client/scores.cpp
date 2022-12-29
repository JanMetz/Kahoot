#include "scores.h"
#include "ui_scores.h"

scores::scores(QMainWindow *m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::scores)
{
    ui->setupUi(this);
    mainWindow = m;
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
       mainWindow->show();
       this->close();
    });
}

scores::~scores()
{
    delete ui;
}
