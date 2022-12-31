#include "lobby.h"
#include "ui_lobby.h"

Lobby::Lobby(QMainWindow* m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Lobby)
{
    ui->setupUi(this);
    mainWindow = m;
    connect(ui->exitButton, &QPushButton::clicked, this, [&]{
        mainWindow->show();
        this->close();
    });
}

Lobby::~Lobby()
{
    delete ui;
}
