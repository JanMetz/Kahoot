#include "createquestion.h"
#include "ui_createquestion.h"

createQuestion::createQuestion(QMainWindow* m, int n, int t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createQuestion)
{
    mainWindow = m;
    numberOfQuestions = n;
    time = t;
    questionNumber = 1;
    ui->setupUi(this);
    ui->questionNumber->setText(QStringLiteral("Question %1").arg(questionNumber));
    ui->buttonGroup->setId(ui->radioButtonA, 1);
    ui->buttonGroup->setId(ui->radioButtonB, 2);
    ui->buttonGroup->setId(ui->radioButtonC, 3);
    ui->buttonGroup->setId(ui->radioButtonD, 4);
    connect(ui->buttonGroup, &QButtonGroup::buttonClicked, this, [&]{
        ui->nextButton->setEnabled(true);
        answerId = ui->buttonGroup->checkedId();
    });
    connect(ui->nextButton, &QPushButton::clicked, this, [&]{
        if(ui->Question->document()->isEmpty() || ui->answerA->document()->isEmpty() ||
            ui->answerB->document()->isEmpty() || ui->answerC->document()->isEmpty() ||
                ui->answerD->document()->isEmpty()) {
            QMessageBox msgBox;
            msgBox.setInformativeText("Text fields cannot be empty.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet("background:white");
            msgBox.exec();
            return;
        }
        questions.push_back({ui->Question->toPlainText(),
            ui->answerA->toPlainText(), ui->answerB->toPlainText(),
            ui->answerC->toPlainText(), ui->answerD->toPlainText(),
            QString::number(answerId)});
        questionNumber++;
        ui->questionNumber->setText(QStringLiteral("Question %1").arg(questionNumber));
        ui->Question->clear();
        ui->answerA->clear();
        ui->answerB->clear();
        ui->answerC->clear();
        ui->answerD->clear();
        ui->nextButton->setEnabled(false);
        ui->buttonGroup->setExclusive(false);
        ui->buttonGroup->button(answerId)->setChecked(false);
        ui->buttonGroup->setExclusive(true);
        if(questionNumber > numberOfQuestions) {
            QWidget *wdg = new ConnectScreen(mainWindow, numberOfQuestions, time, questions);
            wdg->show();
            this->close();
        }
    });
}

createQuestion::~createQuestion()
{
    delete ui;
}
