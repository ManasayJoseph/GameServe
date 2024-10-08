// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QDebug>
#include <QStyle>
#include "speedometerwidget.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serverManager(new ServerManager(this))
{
    ui->setupUi(this);

    QIcon startIcon = style()->standardIcon(QStyle::SP_DialogApplyButton );
    ui->startButton->setIcon(startIcon);
    ui->startButton->setIconSize(QSize(100, 100));  // Adjust size


    QPixmap pixmap("C:/Users/ADAM SYSTEMS/Downloads/GameServe.png"); // Replace with the actual path to your image

    // Check if the image loaded successfully
    if (!pixmap.isNull()) {
        // Set the QPixmap as the content of the QLabel
        QPixmap scaledPixmap = pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->Logo->setPixmap(scaledPixmap);

        // Optionally, resize the QLabel to match the size of the image
        // ui->Logo->resize(pixmap.size());
    } else {
        // Handle error if the image could not be loaded
        ui->Logo->setText("Failed to load image");
    }




    speedometer->setRange(0, 2048); // Set min and max values
    speedometer->setValue(0);

    QVBoxLayout* layout = new QVBoxLayout(ui->speedometerPlaceholder);
    layout->addWidget(speedometer);

    connect(serverManager, &ServerManager::outputReceived, this, &MainWindow::handleOutput);
    connect(serverManager, &ServerManager::errorOccurred, this, &MainWindow::handleError);
    connect(serverManager, &ServerManager::resourceUsageUpdated, this, &MainWindow::handleResourceUsageUpdated); // Connect the new signal

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::toggleServerState);
}




MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startServer()
{
    serverManager->startServer("D:/Manasay/ServerHoster/MCJAVA/server.jar");
}

void MainWindow::stopServer(){
    serverManager->executeCommand("stop");
    speedometer->setValue(0);
}

void MainWindow::toggleServerState()
{
    if (serverManager->isRunning()) {
        stopServer();

        // Set Start Icon
        QIcon startIcon = style()->standardIcon(QStyle::SP_MediaPlay);
        ui->startButton->setIcon(startIcon);
    } else {
        startServer();

        // Set Stop Icon
        QIcon stopIcon = style()->standardIcon(QStyle::SP_MediaStop);
        ui->startButton->setIcon(stopIcon);
    }


}


void MainWindow::on_executeButton_clicked()
{
    QString command = ui->commandInput->text();
    if (serverManager->isRunning()) {
        serverManager->executeCommand(command);
        ui->commandInput->clear();
    } else {
        ui->logTextEdit->append("Error: Server is not running.");
    }
}

void MainWindow::handleOutput(const QString &output)
{
    ui->logTextEdit->append(output);
}

void MainWindow::handleError(const QString &error)
{
    ui->logTextEdit->append("Error: " + error);
}

void MainWindow::handleResourceUsageUpdated(double cpuUsage, double ramUsage)
{
    speedometer->setValue(ramUsage);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    serverManager->stopServer();
    event->accept();
}
