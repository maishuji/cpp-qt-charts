#include "ChartWindow.hpp"
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>

QT_CHARTS_USE_NAMESPACE

struct ChartWindow::PImpl
{
    QChartView *chartView1;
    QChartView *chartView2;
    QChartView *chartView3;
    QChartView *chartView4;
    QLabel *errorLabel;
};

ChartWindow::~ChartWindow() = default; // Destructor definition

namespace
{

    void plotSeries(QLineSeries *series, QChartView &chartView, const QString &title)
    {
        // Create a new chart and configure it
        QChart *chart = new QChart();
        chart->addSeries(series);   // Add the provided QLineSeries to the chart
        chart->createDefaultAxes(); // Automatically create axes based on the series
        chart->setTitle(title);     // Set the chart title

        // Set the chart in the chart view
        chartView.setChart(chart);
        chartView.setMinimumSize(640, 480);
    }

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
    : QWidget(parent),
      networkManager(new QNetworkAccessManager(this))
{
    pImpl = std::make_unique<PImpl>();
    pImpl->chartView1 = new QChartView(this);
    pImpl->chartView2 = new QChartView(this);
    pImpl->chartView3 = new QChartView(this);
    pImpl->chartView4 = new QChartView(this);
    // Create a bar set
    if (!pImpl->chartView1 || !pImpl->chartView2)
    {
        qWarning("Failed to create chart view");
        return;
    }
    QHBoxLayout *layout = new QHBoxLayout();

    QVBoxLayout *vLayout1 = new QVBoxLayout();
    vLayout1->addWidget(pImpl->chartView1);
    vLayout1->addWidget(pImpl->chartView3);

    QVBoxLayout *vLayout2 = new QVBoxLayout();
    vLayout2->addWidget(pImpl->chartView2);
    vLayout2->addWidget(pImpl->chartView4);

    layout->addLayout(vLayout1);
    layout->addLayout(vLayout2);

    // Set up the error label
    pImpl->errorLabel = new QLabel(this);
    pImpl->errorLabel->setStyleSheet("color: red; font-weight: bold;"); // Style for visibility
    pImpl->errorLabel->setAlignment(Qt::AlignCenter);                   // Center the text
    pImpl->errorLabel->setText("");                                     // Start with no text
    vLayout1->addWidget(pImpl->errorLabel);

    setLayout(layout);

    plotSomeData(*(pImpl->chartView1));
    // plotDataFromCSV("data/bitcoin_prices.csv", *(pImpl->chartView2));
    plotDataFromAPI(*(pImpl->chartView2), "sui");
    plotDataFromAPI(*(pImpl->chartView3), "bitcoin");
    plotDataFromAPI(*(pImpl->chartView4), "solana");
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
    chart->setTitle("Bitcoin Price Over Time (1)");

    // Set the chart view
    chartView.setChart(chart);
    chartView.setMinimumSize(640, 480);
}

void ChartWindow::plotDataFromAPI(QChartView &chartView, const QString &cryptoName)
{
    // CoinGecko API URL for hourly data over the last 7 days
    // No need of API key from CoinGecko to access this data
    QUrl url(QString("https://api.coingecko.com/api/v3/coins/%1/market_chart?vs_currency=usd&days=7").arg(cryptoName));
    QNetworkRequest request(url);

    // Send the GET request and create a reply
    QNetworkReply *reply = networkManager->get(request);
    // connect(networkManager, &QNetworkAccessManager::finished, this, &ChartWindow::onDataReceived);
    // networkManager->get(request);

    // Connect the reply's finished signal to a lambda to handle this request's specific context
    connect(reply, &QNetworkReply::finished, this, [this, reply, &chartView, cryptoName]()
            { onDataReceived(reply, chartView, cryptoName); });
}

void ChartWindow::onDataReceived(QNetworkReply *reply, QChartView &chartView, const QString &cryptoName)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning("Error fetching data: %s", qPrintable(reply->errorString()));
        pImpl->errorLabel->setText("Error fetching data: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    // Parse the JSON response
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray prices = jsonObject["prices"].toArray();

    // Create a series to hold the data
    QLineSeries *series = new QLineSeries();

    // Populate the series with data (timestamp and price)
    for (const QJsonValue &value : prices)
    {
        QJsonArray priceData = value.toArray();
        qint64 timestamp = priceData[0].toVariant().toLongLong();
        double price = priceData[1].toDouble();

        // Convert timestamp to QDateTime
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(timestamp / 1000); // Convert from milliseconds
        qreal x = dateTime.toMSecsSinceEpoch();                               // Use milliseconds for x-axis
        series->append(x, price);
    }

    plotSeries(series, chartView, cryptoName + " Price Over Time");

    reply->deleteLater(); // Clean up the reply object
}
