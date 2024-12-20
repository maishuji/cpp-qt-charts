# Qt Chart Project

This project demonstrates how to use `Qt Charts` to display data from a CSV file in a simple Qt application. It creates a chart that plots data such as Bitcoin prices over time. The project includes multiple charts displayed side-by-side in a single window.

## Features

- Reads Bitcoin price data from a CSV file with timestamp and price.
- Displays two charts side by side in the same window.
- Uses `QChartView` and `QLineSeries` to plot the data.
- Automatically handles widget deletion with Qt's parent-child memory management.

## Requirements

- Qt 5.15 or later (QtCharts module).
- CMake for project configuration.


## Building the Project
```bash
mkdir build
cd build
cmake ..
make
```

## License
This project is licensed under MIT License 
