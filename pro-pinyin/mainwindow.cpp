#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "vkeyboard.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_toolButton_clicked()
{
    VKeyboard *vk = new VKeyboard();
    vk->show ();
}
