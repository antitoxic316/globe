
#include <QWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

    central_ = ui_->centralwidget;

    QWidget *canvasContainer = ui_->canvasContainer;

    globeCanv_ = new Canvas(canvasContainer);

    setWindowTitle("Globe");
    resize(800, 600);
}

MainWindow::~MainWindow()
{
    delete ui_;
}
