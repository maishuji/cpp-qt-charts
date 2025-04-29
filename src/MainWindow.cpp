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

struct MainWindow::PImpl {
    QAction *exitAction = nullptr;
    QAction *versionAction = nullptr;
    QMenu *fileMenu = nullptr;
    QMenu *helpMenu = nullptr;
};

MainWindow::~MainWindow() = default;  // Destructor definition

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), m_pimpl(std::make_unique<PImpl>()) {
    chartWindow = new ChartWindow(this);  //  reuse your existing QWidget
    setCentralWidget(chartWindow);        //  embed it in the main window

    // Create Menu
    menuBar = new QMenuBar(this);

    //  File Menu
    m_pimpl->fileMenu = new QMenu("File", this);
    m_pimpl->exitAction = new QAction("Exit", this);
    m_pimpl->fileMenu->addAction(m_pimpl->exitAction);
    m_pimpl->exitAction->setShortcut(QKeySequence::Quit);

    menuBar->addMenu(m_pimpl->fileMenu);

    //  Help Menu
    m_pimpl->helpMenu = new QMenu("Help", this);
    m_pimpl->versionAction = new QAction("Version", this);
    m_pimpl->helpMenu->addAction(m_pimpl->versionAction);
    menuBar->addMenu(m_pimpl->helpMenu);

    setMenuBar(menuBar);

    connect(m_pimpl->versionAction,
            &QAction::triggered,
            this,
            &MainWindow::showVersion);
    connect(m_pimpl->exitAction, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::showVersion() {
    // Show version using a message box
    QMessageBox::information(this,
                             tr("Version Information"),
                             tr("Version: %1").arg(APP_VERSION_STRING));
}
