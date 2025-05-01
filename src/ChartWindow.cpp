#include "ChartWindow.hpp"

#include <QComboBox>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLayout>
#include <QTextStream>
#include <QTimer>
#include <QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "CoreUtils.hpp"
#include "DraggableLine.hpp"

QT_CHARTS_USE_NAMESPACE

struct ChartWindow::PImpl {
    QChartView *chartView1;
    QChartView *chartView2;
    QChartView *chartView3;
    QChartView *chartView4;
    QComboBox *comboBox;
    QLabel *errorLabel;
};

ChartWindow::~ChartWindow() = default;  // Destructor definition

namespace {

qreal getMaxX(QLineSeries *series) {
    auto maxIt = std::max_element(
        series->points().begin(),
        series->points().end(),
        [](const QPointF &a, const QPointF &b) { return a.x() >= b.x(); });
    return maxIt != series->points().end() ? maxIt->x() : 0.0;
}

qreal getMinX(QXYSeries *series) {
    auto minIt = std::min_element(
        series->points().begin(),
        series->points().end(),
        [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
    return minIt != series->points().end() ? minIt->x() : 0.0;
}

void plotSeries(QLineSeries *series, QChartView &chartView,
                const QString &title) {
    // Find local min and max
    qreal minY = series->at(0).y();
    qreal maxY = minY;
    qreal minX = series->at(0).x();
    qreal maxX = minX;

    for (int i = 1; i < series->count(); ++i) {
        const QPointF &point = series->at(i);
        if (point.y() < minY) minY = point.y();
        if (point.y() > maxY) maxY = point.y();
        if (point.x() < minX) minX = point.x();
        if (point.x() > maxX) maxX = point.x();
    }

    // Create min line
    QLineSeries *minLine = new QLineSeries();
    minLine->append(minX, minY);
    minLine->append(maxX, minY);
    minLine->setName("Min");

    // Create max line
    QLineSeries *maxLine = new QLineSeries();
    maxLine->append(minX, maxY);
    maxLine->append(maxX, maxY);
    maxLine->setName("Max");

    // Create a new chart and configure it
    QChart *chart = new QChart();
    chart->addSeries(series);  // Add the provided QLineSeries to the chart
    chart->addSeries(minLine);
    chart->addSeries(maxLine);
    // chart->createDefaultAxes(); // Automatically create axes based on the
    // series
    chart->setTitle(title);  // Set the chart title

    // Create and configure the DateTime axis
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setRange(
        QDateTime::fromMSecsSinceEpoch(series->at(0).x()),
        QDateTime::fromMSecsSinceEpoch(series->at(series->count() - 1).x()));
    axisX->setFormat("dd/MM/yy-hh:mm");
    axisX->setLabelsAngle(90);
    QFont font("Arial", 10);
    axisX->setLabelsFont(font);
    axisX->setTitleText("Date");
    axisX->setTickCount(10);  // Set the number of ticks to display
    chart->addAxis(axisX, Qt::AlignBottom);  // Attach the X-axis to the series
    // series->attachAxis(
    //     axisX);  // Can only attach to axes after they are added to the chart

    // Create and configure the Y-axis
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Value");
    chart->addAxis(axisY, Qt::AlignLeft);  // Attach the axis to the chart
    // series->attachAxis(axisY);             // Attach the series to the Y-axis

    // Attach series to axes
    for (QLineSeries *s : {series, minLine, maxLine}) {
        s->attachAxis(axisX);
        s->attachAxis(axisY);
    }

    // Set the chart in the chart view
    chartView.setChart(chart);
    chartView.setMinimumSize(640, 480);

    // The line will be set to the y center in the plotAreaChanged event
    DraggableLine *line = new DraggableLine(0, 0, 100, 100);
    chartView.scene()->addItem(line);

    QObject::connect(chart,
                     &QChart::plotAreaChanged,
                     [&chartView, chart, line](const QRectF &plotArea) {
                         QPointF left = chart->mapToScene(plotArea.topLeft());
                         QPointF right = chart->mapToScene(plotArea.topRight());
                         QPointF center = chart->mapToScene(plotArea.center());
                         QPointF start(left.x(), center.y());
                         QPointF end(right.x(), center.y());

                         if (line) {
                             line->setLine(QLineF(start, end));
                         }
                     });
}

void plotSomeData(QChartView &chartView) {
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
}  // namespace

template <typename T>
void setChartWithComboLayout(T &layout, QChartView *chartView,
                             QComboBox *comboBox) {
    static_assert(
        std::is_base_of<QLayout, typename std::remove_pointer<T>::type>::value,
        "T must be derived from QLayout");
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(comboBox);
    vLayout->addWidget(chartView);
    layout.addLayout(vLayout);
}

ChartWindow::ChartWindow(QWidget *parent) :
    QWidget(parent), networkManager(new QNetworkAccessManager(this)) {
    pImpl = std::make_unique<PImpl>();
    pImpl->chartView1 = new QChartView(this);
    pImpl->chartView2 = new QChartView(this);
    pImpl->chartView3 = new QChartView(this);
    pImpl->chartView4 = new QChartView(this);
    pImpl->comboBox = new QComboBox(this);

    pImpl->comboBox->addItems(
        {"Bitcoin", "Ethereum", "Solana", "SUI", "Cardano", "Dogecoin"});

    // Create a bar set
    if (!pImpl->chartView1 || !pImpl->chartView2) {
        qWarning("Failed to create chart view");
        return;
    }
    QHBoxLayout *layout = new QHBoxLayout();

    QVBoxLayout *vLayout1 = new QVBoxLayout();
    setChartWithComboLayout(*vLayout1, pImpl->chartView1, pImpl->comboBox);
    connect(pImpl->comboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) {
                QString cryptoName = pImpl->comboBox->currentText().toLower();
                plotDataFromAPI(*(pImpl->chartView1), cryptoName);
            });

    // vLayout1->addWidget(pImpl->chartView1);
    vLayout1->addWidget(pImpl->chartView3);

    QVBoxLayout *vLayout2 = new QVBoxLayout();
    vLayout2->addWidget(pImpl->chartView2);
    vLayout2->addWidget(pImpl->chartView4);

    layout->addLayout(vLayout1);
    layout->addLayout(vLayout2);

    // Set up the error label
    pImpl->errorLabel = new QLabel(this);
    pImpl->errorLabel->setStyleSheet(
        "color: red; font-weight: bold;");             // Style for visibility
    pImpl->errorLabel->setAlignment(Qt::AlignCenter);  // Center the text
    pImpl->errorLabel->setText("");                    // Start with no text
    vLayout1->addWidget(pImpl->errorLabel);

    setLayout(layout);

    plotSomeData(*(pImpl->chartView1));
    // plotDataFromCSV("data/bitcoin_prices.csv", *(pImpl->chartView2));
    plotDataFromAPI(*(pImpl->chartView2), "sui");
    plotDataFromAPI(*(pImpl->chartView3), "bitcoin");
    plotDataFromAPI(*(pImpl->chartView4), "solana");
}

void ChartWindow::plotDataFromCSV(const QString &filePath,
                                  QChartView &chartView) {
    // Create a series to hold the data
    QLineSeries *series = new QLineSeries();

    // Open the CSV file
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Cannot open file for reading");
        return;
    }

    // Create a text stream to read the file
    QTextStream in(&file);
    bool firstLine = true;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (firstLine) {
            firstLine = false;
            continue;  // Skip the header line
        }

        // Split the line by comma
        QStringList fields = line.split(',');

        // Parse the timestamp and price
        qint64 timestamp = fields[0].toLongLong();
        double price = fields[1].toDouble();

        // Convert timestamp to a readable date (optional)
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(timestamp);
        qreal x =
            dateTime.toMSecsSinceEpoch();  // Use milliseconds as x-axis value

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

void ChartWindow::plotDataFromAPI(QChartView &chartView,
                                  const QString &cryptoName) {
    if (!coreUtils::shouldRefreshCache(cryptoName)) {
        qDebug() << "Using cached data for" << cryptoName;
        QString fn = coreUtils::makeCacheFileName(cryptoName);
        QFile file(fn);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray responseData = file.readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonDoc.object();
            plotJsonData(jsonObject, chartView, cryptoName);
            return;
        } else {
            qWarning("Failed to open cache file for reading");
        }
    }

    // CoinGecko API URL for hourly data over the last 7 days
    // No need of API key from CoinGecko to access this data
    QUrl url(QString("https://api.coingecko.com/api/v3/coins/%1/"
                     "market_chart?vs_currency=usd&days=7")
                 .arg(cryptoName));
    QNetworkRequest request(url);

    // Send the GET request and create a reply
    QNetworkReply *reply = networkManager->get(request);

    // Connect the reply's finished signal to a lambda to handle this request's
    // specific context
    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, reply, &chartView, cryptoName]() {
                onDataReceived(reply, chartView, cryptoName);
            });
}

void ChartWindow::onDataReceived(QNetworkReply *reply, QChartView &chartView,
                                 const QString &cryptoName) {
    if (reply->error() != QNetworkReply::NoError) {
        qWarning("Error fetching data: %s", qPrintable(reply->errorString()));
        pImpl->errorLabel->setText("Error fetching data: " +
                                   reply->errorString());
        reply->deleteLater();
        return;
    }

    // Parse the JSON response
    QByteArray responseData = reply->readAll();
    if (!responseData.isEmpty()) {
        QString fn = coreUtils::makeCacheFileName(cryptoName);
        QFile file(fn);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(responseData);
            file.close();
        } else {
            qWarning("Failed to open cache file for writing");
        }
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonDoc.object();
    plotJsonData(jsonObject, chartView, cryptoName);

    reply->deleteLater();  // Clean up the reply object
}

void ChartWindow::plotJsonData(QJsonObject &jsonObject, QChartView &chartView,
                               const QString &cryptoName) {
    QJsonArray prices = jsonObject["prices"].toArray();

    // Create a series to hold the data
    QLineSeries *series = new QLineSeries();
    // Populate the series with data (timestamp and price)
    for (const QJsonValue &value : prices) {
        QJsonArray priceData = value.toArray();
        qint64 timestamp = priceData[0].toVariant().toLongLong();
        double price = priceData[1].toDouble();

        // Convert timestamp to QDateTime
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(
            timestamp / 1000);  // Convert from milliseconds
        qreal x = dateTime.toMSecsSinceEpoch();

        // Use milliseconds for x-axis
        series->append(x, price);
    }

    plotSeries(series, chartView, cryptoName + " Price Over Time");
}
