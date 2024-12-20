#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QChart>

#include <memory>

QT_CHARTS_USE_NAMESPACE

class ChartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWindow(QWidget *parent = nullptr);
    ~ChartWindow(); // Destructor declaration

    void plotDataFromCSV(const QString &filePath, QChartView &chartView);

private:
    struct PImpl;
    std::unique_ptr<class PImpl> pImpl;
};

#endif // CHARTWINDOW_H