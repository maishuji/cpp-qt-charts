// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

#include "ChartWindow.hpp"  // Include your existing chart window

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

 private slots:
    void showVersion();

 private:
    ChartWindow *chartWindow;

    struct PImpl;
    std::unique_ptr<PImpl> m_pimpl;
    QMenuBar *menuBar;
};

#endif  // MAINWINDOW_H
