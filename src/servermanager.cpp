// servermanager.cpp
#include "servermanager.h"
#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>
#include <QOperatingSystemVersion>

ServerManager::ServerManager(QObject *parent) :
    QObject(parent),
    serverProcess(new QProcess(this)),
    resourceTimer(new QTimer(this)) // Initialize the timer
{
    connect(serverProcess, &QProcess::readyReadStandardOutput, this, &ServerManager::readOutput);
    connect(serverProcess, &QProcess::readyReadStandardError, this, &ServerManager::readError);

    connect(resourceTimer, &QTimer::timeout, this, &ServerManager::updateResourceUsage); // Connect timer to slot
}

ServerManager::~ServerManager()
{
    stopServer();
}

void ServerManager::startServer(const QString &jarPath)
{
    if (isRunning()) {
        qDebug() << "Server is already running.";
        return;
    }

    QStringList arguments;
    arguments << "-Xmx1024M" << "-Xms1024M" << "-jar" << jarPath << "nogui";

    // Set the working directory to the location of the JAR file
    serverProcess->setWorkingDirectory(QFileInfo(jarPath).absolutePath());
    serverProcess->start("java", arguments);

    if (!serverProcess->waitForStarted()) {
        emit errorOccurred("Failed to start the server.");
        return;
    }
    emit outputReceived("Minecraft server started successfully.");

    // Start the timer to monitor resource usage
    resourceTimer->start(1000); // Update every second
}

void ServerManager::stopServer()
{
    if (isRunning()) {
        serverProcess->terminate();
        serverProcess->waitForFinished();
        emit outputReceived("Minecraft server stopped.");
    }
    resourceTimer->stop(); // Stop the resource monitoring timer
}

bool ServerManager::isRunning() const
{
    return serverProcess->state() == QProcess::Running;
}

void ServerManager::readOutput()
{
    QString output = serverProcess->readAllStandardOutput();
    emit outputReceived(output);
}

void ServerManager::readError()
{
    QString error = serverProcess->readAllStandardError();
    emit errorOccurred(error);
}

void ServerManager::executeCommand(const QString &command)
{
    if (isRunning()) {
        serverProcess->write(command.toUtf8() + "\n"); // Send command to server
    } else {
        emit errorOccurred("Cannot execute command: Server is not running.");
    }
}


void ServerManager::updateResourceUsage() {
    if (!isRunning()) {
        emit resourceUsageUpdated(0.0,0.0);
        return;
    }

    QProcess taskList;
    taskList.start("tasklist", QStringList() << "/FI" << "imagename eq java.exe" << "/FO" << "CSV");

    if (!taskList.waitForFinished()) {
        emit errorOccurred("Failed to get resource usage.");
        return;
    }

    QString output = taskList.readAllStandardOutput();
    qDebug() << "Tasklist output: " << output;  // Debug: Print the entire output

    QStringList lines = output.split("\n");
    double totalRamUsageMB = 0.0;

    for (int i = 1; i < lines.size(); ++i) { // Start from 1 to skip the header
        QString dataLine = lines[i].trimmed(); // Trim whitespace
        if (dataLine.isEmpty()) continue; // Skip empty lines

        qDebug() << "Data Line: " << dataLine;  // Debug: Print the extracted line


        // Split the data line at the commas
        QStringList splitFields = dataLine.split(",", Qt::SkipEmptyParts);

        // Create a new QStringList to hold the final data fields
        QStringList dataFields;

        // Take the first four elements from splitFields
        for (int i = 0; i < 4 && i < splitFields.size(); ++i) {
            dataFields.append(splitFields[i].trimmed()); // Trim to remove any surrounding whitespace
        }

        // Append the last element concatenated if it exists
        if (splitFields.size() > 4) {
            QString lastField = splitFields.mid(4).join(","); // Join the remaining elements
            dataFields.append(lastField.trimmed()); // Add the remaining data as a single field
        }

        // Print the data fields for debugging
        qDebug() << "Data Fields: " << dataFields;

        if (dataFields.size() >= 5) {
            // Get the memory usage string, clean it properly
            QString memUsageStr = dataFields[4].remove("\"").remove(" K"); // Remove quotes and ' K'
            qDebug() << "Memory usage string (raw): " << memUsageStr;  // Debug: Print the memory usage string

            // Remove commas for conversion
            memUsageStr.replace(",", "");  // Remove commas

            bool ok;
            double ramUsageKB = memUsageStr.toDouble(&ok); // Convert to double
            if (ok) {
                double ramUsageMB = ramUsageKB / 1024.0; // Convert to MB
                totalRamUsageMB += ramUsageMB; // Accumulate memory usage
                qDebug() << "Current RAM usage in MB: " << ramUsageMB;  // Debug: Print each instance's RAM usage
            } else {
                qDebug() << "Failed to convert memory usage to number: " << memUsageStr;
            }
        } else {
            qDebug() << "Unexpected data format in tasklist output: " << dataLine;
        }
    }

    emit resourceUsageUpdated(0.0, totalRamUsageMB);
    qDebug() << "Total RAM usage in MB: " << totalRamUsageMB; // Debug: Print total RAM usage
}
