#include "worker.h"

#include <QDir>
#include <QFile>

namespace builder {
Worker::Worker(QProcess* process, std::vector<Configuration>& configurations)
    : m_process(process),
      m_configurations(configurations)
{
}

void Worker::doWork()
{
    for (int32_t row = 0; row < int32_t(m_configurations.size()); ++row) {
        Configuration& config = m_configurations.at(row);
        if (!config.needBuild) {
            continue;
        }
        emit sendProgressValue(row, 0);
        config.buildName = QString("%1-%2_%3_%4-%5")
                .arg(config.project)
                .arg(toString(config.system))
                .arg(toString(config.compiler))
                .arg(toString(config.arch))
                .arg(toString(config.config));
        if (!QFile(config.path.compiler).exists()) {
            emit sendLog("Check compiler ... Fail");
            continue;
        }
        emit sendLog("Check compiler ... OK");
        if (!QFile(config.path.cmake).exists()) {
            emit sendLog("Check cmake ... Fail");
            continue;
        }
        emit sendLog("Check cmake ... OK");
        emit sendLog("Building...");
        QDir dir = QDir(config.buildDir + QDir::separator() + config.buildName);
        if (dir.exists()) {
            emit sendLog("Clean");
            dir.removeRecursively();
        }
        emit sendLog("Create build directory");
        dir.mkpath(".");
        if (dir.exists()) {
            if (!dir.isReadable()) {
                emit sendLog("Build directory is RO");
                continue;
            }
            if (config.config == Config::unknown) {
                emit sendLog("Invalid build configuration");
                continue;
            }
            emit sendProgressMax(row, 0);

            m_process->setWorkingDirectory(dir.absolutePath());
            m_process->start(config.path.cmake, { QString("-DCMAKE_BUILD_TYPE=%1").arg(toString(config.config)),
                                                  config.projectPath });
            m_process->waitForFinished();
            m_process->close();
            m_process->start(config.path.make);

//            m_builder->write(QString("cd %1").arg(dir.absolutePath()).toUtf8());
//            writeIO(QString("%1 %2").arg(config.path.cmake).arg(config.projectPath));
//            writeIO(QString("%1").arg(config.path.make));

            // TODO
            m_process->waitForFinished();
            m_process->close();
            emit sendLog("Build done");
            emit sendProgressMax(row, 100);
            emit sendProgressValue(row, 100);
        } else {
            emit sendLog("Build directory not found");
        }
    }
    emit sendLock(false);
}
} // builder
