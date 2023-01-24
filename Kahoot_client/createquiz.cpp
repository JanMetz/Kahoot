#include "createquiz.h"
#include "ui_createquiz.h"

createQuiz::createQuiz(QMainWindow* m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createQuiz), mainWindow(m)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}
void createQuiz::accept(){
    int n = ui->questionsN->value();
    int time = ui->time->value();
    QWidget *wdg = new createQuestion(mainWindow, n, time);
    wdg->show();
    this->close();
}
void createQuiz::reject(){
    mainWindow->show();
    this->close();
}
createQuiz::~createQuiz()
{
    delete ui;
}
