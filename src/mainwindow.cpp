#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QDir::setCurrent(qApp->applicationDirPath());
}

MainWindow::~MainWindow()
{
    delete ui;
}
