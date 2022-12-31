#include "joinquiz.h"
#include "ui_joinquiz.h"

joinQuiz::joinQuiz(QMainWindow* m,QWidget *parent) :
    QWidget(parent), mainWindow(m),
    ui(new Ui::joinQuiz)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

void joinQuiz::accept() {
    QWidget *wdg = new Lobby(mainWindow);
    wdg->show();
    this->close();
}

void joinQuiz::reject() {
    mainWindow->show();
    this->close();
}

joinQuiz::~joinQuiz() {
    delete ui;
}
