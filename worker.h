#ifndef BUILDER_WORKER_H
#define BUILDER_WORKER_H

#include <QObject>
#include <QProcess>

#include <vector>

#include "configuration.h"

namespace builder {
// -----------------------------------------------------------------------------
class Worker : public QObject
{
    Q_OBJECT

public:
    Worker(QProcess* process, std::vector<Configuration>& configurations);

public slots:
    void    doWork();

signals:
    void    sendLock(bool);
    void    sendLog(const QString&);
    void    sendProgressMax(int, int);
    void    sendProgressValue(int, int);

private:
    QProcess*   m_process;
    std::vector<Configuration>& m_configurations;

    Worker(const Worker&) = delete;
    Worker& operator =(const Worker&) = delete;
};
} // builder

#endif // BUILDER_WORKER_H
