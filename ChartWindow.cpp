#include "ChartWindow.hpp"
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QTextStream>
#include <QDateTime>
#include <QHBoxLayout>

QT_CHARTS_USE_NAMESPACE

struct ChartWindow::PImpl
{
    QChartView *chartView1;
    QChartView *chartView2;
};

ChartWindow::~ChartWindow() = default; // Destructor definition

namespace
{
    void plotSomeData(QChartView &chartView)
    {
        // Create a bar set
        QBarSet *set0 = new QBarSet("Data 1");
        *set0 << 5 << 10 << 15 << 20 << 25;

        // Create a bar series and append the set
        QBarSeries *series = new QBarSeries();
        series->append(set0);

        // Create a chart and add the series
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Simple Bar Chart Example");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        // Create an axis for the categories
        QStringList categories;
        categories << "A" << "B" << "C" << "D" << "E";
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignLeft);

        // Set a vertical axis
        chart->createDefaultAxes();

        // Set the chart view
        chartView.setChart(chart);
        chartView.setMinimumSize(640, 480);
    }
}

ChartWindow::ChartWindow(QWidget *parent)
    : QWidget(parent)
{
    pImpl = std::make_unique<PImpl>();
    pImpl->chartView1 = new QChartView(this);
    pImpl->chartView2 = new QChartView(this);
    // Create a bar set
    if (!pImpl->chartView1 || !pImpl->chartView2)
    {
        qWarning("Failed to create chart view");
        return;
    }
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(pImpl->chartView1);
    layout->addWidget(pImpl->chartView2);
    setLayout(layout);

    plotSomeData(*(pImpl->chartView1));
    plotDataFromCSV("data/bitcoin_prices.csv", *(pImpl->chartView2));
}

void ChartWindow::plotDataFromCSV(const QString &filePath, QChartView &chartView)
{
    // Create a series to hold the data
    QLineSeries *series = new QLineSeries();

    // Open the CSV file
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("Cannot open file for reading");
        return;
    }

    // Create a text stream to read the file
    QTextStream in(&file);
    bool firstLine = true;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (firstLine)
        {
            firstLine = false;
            continue; // Skip the header line
        }

        // Split the line by comma
        QStringList fields = line.split(',');

        // Parse the timestamp and price
        qint64 timestamp = fields[0].toLongLong();
        double price = fields[1].toDouble();

        // Convert timestamp to a readable date (optional)
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(timestamp);
        qreal x = dateTime.toMSecsSinceEpoch(); // Use milliseconds as x-axis value

        // Append the point to the series
        series->append(x, price);
    }

    file.close();

    // Create a chart and add the series
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Bitcoin Price Over Time");

    // Set the chart view
    chartView.setChart(chart);
    chartView.setMinimumSize(640, 480);
}
