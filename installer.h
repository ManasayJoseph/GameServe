#ifndef INSTALLER_H
#define INSTALLER_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressBar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>
#include <QDebug>
#include <QRegularExpression>
#include <QMessageBox>
#include "ui_installer.h"
#include <QCloseEvent>

class Installer : public QMainWindow {
    Q_OBJECT

public:
    Installer(QWidget *parent = nullptr);
    ~Installer(); // Default destructor

private slots:
    void goToSelectionScreen();
    void goToInstallingScreen();
    void selectInstallLocation();
    void startInstallation();
    void downloadFiles(const QString &url, const QString &location);
    void updateProgress(qint64 bytesRead, qint64 totalBytes);
    void executeJDK();

private:
    Ui::Installer *ui;  // Pointer to the UI
    QStackedWidget *stackedWidget;
    QLabel *selectedPathLabel;
    QString selectedPath;
    QProgressBar *progressBar;
    QNetworkAccessManager *manager;
    QProcess *jdkProcess;

    void closeEvent(QCloseEvent *event);


    // Track total and downloaded bytes for progress
    qint64 totalBytes;
    qint64 downloadedBytes;
    bool isInstalling;
    // Remove this if using the .ui file
    void setupUi();
};

#endif // INSTALLER_H
