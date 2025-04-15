// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ChartWindow.hpp" // Include your existing chart window

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void showVersion();

private:
    ChartWindow *chartWindow;
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QAction *exitAction;
    QMenu *helpMenu;
    QAction *versionAction;
};

#endif // MAINWINDOW_H
