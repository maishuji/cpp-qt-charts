cmake_minimum_required(VERSION 3.5)

project(MyQtChartsApp)

set(DATA_FILE ${CMAKE_SOURCE_DIR}/data/bitcoin_prices.csv)

# Configure file to copy to the binary directory
if(EXISTS ${DATA_FILE})
    configure_file(${DATA_FILE} ${CMAKE_BINARY_DIR}/data/bitcoin_prices.csv COPYONLY)
else()
    message(WARNING "Data file ${DATA_FILE} does not exist.")
endif()

# Enable automatic moc processing
set(CMAKE_AUTOMOC ON)

# Find Qt
find_package(Qt5 REQUIRED COMPONENTS Widgets Charts)

# Add the executable
add_executable(MyQtChartsApp main.cpp ChartWindow.cpp)

# Link Qt libraries
target_link_libraries(MyQtChartsApp Qt5::Widgets Qt5::Charts)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
