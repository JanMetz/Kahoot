#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "createquiz.h"
#include "joinquiz.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->createButton, &QPushButton::clicked, this, [&]{
        QWidget *wdg = new createQuiz(this);
        hide();
        wdg->show();
    });
    connect(ui->joinButton, &QPushButton::clicked, this, [&]{
        QWidget *wdg = new joinQuiz(this);
        hide();
        wdg->show();
    });

    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
         QApplication::quit();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}
