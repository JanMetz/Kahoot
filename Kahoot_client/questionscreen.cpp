#include "questionscreen.h"
#include "ui_questionscreen.h"

QuestionScreen::QuestionScreen(QMainWindow *m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuestionScreen)
{
    ui->setupUi(this);
    mainWindow = m;
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
        mainWindow->show();
        this->close();
    });
}

QuestionScreen::~QuestionScreen()
{
    delete ui;
}
