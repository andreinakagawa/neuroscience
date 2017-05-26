#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QScreen>

#include "reachingwindow.h"

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

void MainWindow::on_actionReaching_triggered()
{
    ReachingWindow *rw = new ReachingWindow();    
    rw->show();

    //QScreen *screen = QGuiApplication::screens()[1]; // specify which screen to use;
    //rw->move(screen->geometry().x(),screen->geometry().y());
    //rw->resize(screen->geometry().width(), screen->geometry().height());
    //rw->showFullScreen();
}
