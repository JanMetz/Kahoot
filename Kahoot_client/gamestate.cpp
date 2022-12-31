#include "gamestate.h"
#include "ui_gamestate.h"

GameState::GameState(QMainWindow* m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameState)
{
    ui->setupUi(this);
    mainWindow = m;
}

GameState::~GameState()
{
    delete ui;
}
