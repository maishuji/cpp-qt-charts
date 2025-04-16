// mainwindow.cpp
#include "MainWindow.hpp"

#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

#include "ChartWindow.hpp"
#include "version.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    chartWindow = new ChartWindow(this);  //  reuse your existing QWidget
    setCentralWidget(chartWindow);        //  embed it in the main window

    // Create Menu
    menuBar = new QMenuBar(this);
    fileMenu = new QMenu("File", this);
    exitAction = new QAction("Exit", this);
    menuBar->addMenu(fileMenu);

    helpMenu = new QMenu("Help", this);
    versionAction = new QAction("Version", this);
    helpMenu->addAction(versionAction);
    menuBar->addMenu(helpMenu);

    setMenuBar(menuBar);

    connect(versionAction, &QAction::triggered, this, &MainWindow::showVersion);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::showVersion() {
    // Show version using a message box
    QMessageBox::information(this,
                             tr("Version Information"),
                             tr("Version: %1").arg(APP_VERSION_STRING));
}
