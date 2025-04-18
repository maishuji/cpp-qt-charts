#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QChart>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <memory>

QT_CHARTS_USE_NAMESPACE

class ChartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWindow(QWidget *parent = nullptr);
    ~ChartWindow(); // Destructor declaration

    void plotDataFromCSV(const QString &filePath, QChartView &chartView);
    void plotDataFromAPI(QChartView &chartView, const QString &cryptoName);
    void plotJsonData(QJsonObject &jsonObject, QChartView &chartView, const QString &cryptoName);

public slots:
    void onDataReceived(QNetworkReply *reply, QChartView &chartView, const QString &cryptoName);

private:
    struct PImpl;
    std::unique_ptr<class PImpl> pImpl;
    QNetworkAccessManager *networkManager;
};

#endif // CHARTWINDOW_H
