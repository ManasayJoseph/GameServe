#include "installer.h"
#include "ui_installer.h"  // Ensure this is generated properly

Installer::Installer(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Installer),
    manager(new QNetworkAccessManager(this)),
    totalBytes(0),
    downloadedBytes(0),
    isInstalling(false)
{
    ui->setupUi(this);  // Set up the UI

    // Set the default path to Downloads/gameserve
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    selectedPath = downloadsPath + "/gameserve";  // This is the base folder for gameserve
    connect(ui->nextButton1, &QPushButton::clicked, this, &Installer::goToSelectionScreen);
    ui->selectedPathLabel->setText(selectedPath); // Use ui-> to access UI elements
    // In Installer constructor
    connect(ui->browse, &QPushButton::clicked, this, &Installer::selectInstallLocation);
    connect(ui->nextButton2, &QPushButton::clicked, this, &Installer::goToInstallingScreen);
}

Installer::~Installer() {
    delete ui; // Clean up the UI
}

void Installer::goToSelectionScreen() {
    ui->stackedWidget->setCurrentIndex(1); // Access UI elements with ui->
}

void Installer::goToInstallingScreen() {
    startInstallation();
}

void Installer::selectInstallLocation() {
    QString folder = QFileDialog::getExistingDirectory(this, "Select Installation Folder");
    if (folder.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No folder selected. Please select an installation folder.");
    } else {
        selectedPath = folder + "/gameserve";  // Updated to base folder for gameserve
        ui->selectedPathLabel->setText(selectedPath); // Access UI elements
    }
}

void Installer::startInstallation() {
    QDir dir(selectedPath);
    if (!dir.exists()) {
        dir.mkpath(".");  // Create the directory if it doesn't exist
    }

    // Save the selected path in a JSON settings file inside the gameserve folder
    QJsonObject settings;
    settings["install_path"] = selectedPath;

    QString settingsFilePath = selectedPath + "/settings.json";  // settings.json is now in the base folder
    QFile settingsFile(settingsFilePath);
    if (settingsFile.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(settings);
        settingsFile.write(doc.toJson());
        settingsFile.close();
    }

    // Move to the progress screen
    ui->stackedWidget->setCurrentIndex(2);

    totalBytes = 0;
    downloadedBytes = 0;

    bool skipJDKDownload = false;
    bool skipPlayitDownload = false;

    // Check if Java 16 or higher is installed
    QProcess javaProcess;
    javaProcess.start("java -version");
    javaProcess.waitForFinished();

    QString output = javaProcess.readAllStandardError();  // Java version info is usually in standard error
    QRegularExpression versionRegex(R"(version \"(\d+)\.(\d+))");
    QRegularExpressionMatch match = versionRegex.match(output);

    if (match.hasMatch()) {
        int majorVersion = match.captured(1).toInt();
        if (majorVersion >= 16) {
            QMessageBox::information(this, "Java Detected", "Java " + QString::number(majorVersion) + " is already installed. Skipping Java installation.");
            skipJDKDownload = true;  // Skip downloading JDK if Java 16 or above is found
        } else {
            QMessageBox::critical(this, "Java Version Too Low", "Java " + QString::number(majorVersion) + " is installed. Please uninstall this version and run the program again.");
            return;  // Stop the installation process if Java version is below 16
        }
    }

    // Check if the JDK file is already in the selected folder (under gameserve/minecraft)
    QString jdkPath = selectedPath + "/minecraft/jdk.exe";  // JDK is now inside the minecraft folder
    if (QFile::exists(jdkPath)) {
        QMessageBox::information(this, "JDK Installer Found", "JDK installer found in the installation folder. Skipping download.");
        skipJDKDownload = true;
    }

    // Check if playit.exe is already in the selected folder (under gameserve/minecraft)
    QString playitPath = selectedPath + "/minecraft/playit.exe";  // playit.exe is now inside the minecraft folder
    if (QFile::exists(playitPath)) {
        QMessageBox::information(this, "Playit Installer Found", "playit.exe found in the installation folder. Skipping download.");
        skipPlayitDownload = true;
    }

    // Download the JDK if it’s not present locally and Java 16+ is not installed
    if (!skipJDKDownload) {
        ui->lineEdit->setText("Downloading Java");
        downloadFiles("https://github.com/ManasayJoseph/GameServe/releases/download/requisites/jdk.exe", jdkPath);
    }

    // Always ensure playit.exe is downloaded if not found in the folder
    if (!skipPlayitDownload) {
        ui->lineEdit->setText("Downloading Playit");
        downloadFiles("https://github.com/ManasayJoseph/GameServe/releases/download/requisites/playit.exe", playitPath);
    }
    executeJDK();
}

void Installer::executeJDK() {
    QString jdkPath = selectedPath + "/minecraft/jdk.exe";  // JDK path is now inside minecraft folder

    if (!QFile::exists(jdkPath)) {
        qDebug() << "JDK installer not found at path:" << jdkPath;
        return;
    }
    ui->lineEdit->setText("Installing Java 21 to Device");
    isInstalling = true;  // Set flag to true to indicate installation is in progress

    jdkProcess = new QProcess(this);  // Initialize jdkProcess as a member

    connect(jdkProcess, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "JDK installation finished with exit code:" << exitCode;
        if (exitStatus == QProcess::NormalExit) {
            qDebug() << "JDK installed successfully.";
            ui->stackedWidget->setCurrentIndex(3);
        } else {
            qDebug() << "JDK installation encountered an issue.";
            ui->stackedWidget->setCurrentIndex(3);
        }
        isInstalling = false;  // Reset flag when installation completes
        jdkProcess->deleteLater();  // Clean up the process once done
    });

    // Start jdk.exe with silent mode
    jdkProcess->start(jdkPath, QStringList() << "/s");

    if (!jdkProcess->waitForStarted()) {
        qDebug() << "Failed to start JDK installation. Error:" << jdkProcess->errorString();
    } else {
        qDebug() << "JDK installation started successfully with silent mode.";
    }
}

// Override closeEvent to prevent closing during installation
void Installer::closeEvent(QCloseEvent *event) {
    if (isInstalling) {
        QMessageBox::warning(this, "Installation in Progress", "The installation is currently in progress. Please wait until it completes.");
        event->ignore();  // Prevent closing
    } else {
        event->accept();  // Allow closing if no installation is running
    }
}

void Installer::downloadFiles(const QString &url, const QString &location) {
    QUrl qurl(url);
    QNetworkRequest request(qurl);

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::downloadProgress, this, &Installer::updateProgress);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Download error:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QFile file(location);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "Could not open file for writing:" << location;
            reply->deleteLater();
            return;
        }

        file.write(reply->readAll());
        file.close();
        reply->deleteLater();

        downloadedBytes += file.size();  // Update total downloaded bytes
        qDebug() << "Download completed successfully for:" << location;
    });

    connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal) {
        totalBytes += bytesTotal;  // Update the total bytes for all downloads
        updateProgress(bytesReceived, bytesTotal);
    });
    ui->lineEdit->setText(" ");
}

void Installer::updateProgress(qint64 bytesRead, qint64 totalBytes) {
    if (totalBytes > 0) {
        ui->progressBar->setMaximum(totalBytes);
        ui->progressBar->setValue(downloadedBytes + bytesRead);  // Update the current progress
    }
}
