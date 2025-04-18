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
find_package(Qt5 REQUIRED COMPONENTS Widgets Charts Network)

# Add the executable
add_executable(MyQtChartsApp 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChartWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CoreUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MainWindow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ChartWindow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CoreUtils.hpp
)

# Link Qt libraries
target_link_libraries(MyQtChartsApp 
    PRIVATE 
        Qt5::Widgets 
        Qt5::Charts 
        Qt5::Network)

# Include directories
target_include_directories(MyQtChartsApp 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
