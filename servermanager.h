// servermanager.h
#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QProcess>
#include <QTimer>

class ServerManager : public QObject
{
    Q_OBJECT

public:
    explicit ServerManager(QObject *parent = nullptr);
    ~ServerManager();

    void startServer(const QString &jarPath);
    void stopServer();
    bool isRunning() const;
    void executeCommand(const QString &command);

signals:
    void outputReceived(const QString &output);
    void errorOccurred(const QString &error);
    void resourceUsageUpdated(double cpuUsage, double ramUsage); // Signal for resource usage

private slots:
    void readOutput();
    void readError();
    void updateResourceUsage(); // New slot to update resource usage

private:
    QProcess *serverProcess;
    QTimer *resourceTimer; // Timer to monitor resources
};

#endif // SERVERMANAGER_H
