// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "servermanager.h"
#include "speedometerwidget.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void toggleServerState();  // Slot to handle the button press
    void on_executeButton_clicked();
    void handleOutput(const QString &output);
    void handleError(const QString &error);
    void handleResourceUsageUpdated(double cpuUsage, double ramUsage); // New slot for resource usage
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    ServerManager *serverManager;
    void startServer();
    void stopServer();
    SpeedometerWidget* speedometer = new SpeedometerWidget();

};

#endif // MAINWINDOW_H
