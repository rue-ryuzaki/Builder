#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>

#include <QThread>

#include "worker.h"

namespace builder {
// -----------------------------------------------------------------------------
const QString buildName = "Builder";
const uint32_t buildNum = 20180321;
// -----------------------------------------------------------------------------
QComboBox* newArch()
{
    QComboBox* result = new QComboBox;
    result->addItems({ toString(Arch::x86),
                       toString(Arch::x64),
                       toString(Arch::unknown) });
    return result;
}

// -----------------------------------------------------------------------------
QComboBox* newCompiler()
{
    QComboBox* result = new QComboBox;
    result->addItems({ toString(Compiler::gcc),
                       toString(Compiler::clang),
                       toString(Compiler::mingw),
                       toString(Compiler::msvc),
                       toString(Compiler::unknown) });
    return result;
}

// -----------------------------------------------------------------------------
QComboBox* newConfig()
{
    QComboBox* result = new QComboBox;
    result->addItems({ toString(Config::Debug),
                       toString(Config::Release),
                       toString(Config::unknown) });
    return result;
}

// -----------------------------------------------------------------------------
QComboBox* newSystem()
{
    QComboBox* result = new QComboBox;
    result->addItems({ toString(System::Linux),
                       toString(System::MaxOS),
                       toString(System::Windows),
                       toString(System::unknown) });
    return result;
}

// -----------------------------------------------------------------------------
// -- MainWindow ---------------------------------------------------------------
// -----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_configurations(),
      m_builder(),
      m_isRunning(false),
      m_boxSystem(),
      m_boxArch(),
      m_boxCompiler(),
      m_boxConfig(),
      m_table(new QTableWidget),
      m_btnAdd(),
      m_btnDel(),
      m_btnBuild(),
      m_log(),
      m_curFile(),
      m_menuFile(),
      m_menuSettings(),
      m_menuHelp(),
      m_actNew(this),
      m_actOpen(this),
      m_actSave(this),
      m_actSaveAs(this),
      m_actExit(this),
      m_actAbout(this),
      m_actAboutQt(this)
{
    setWindowTitle(tr("%1. build %2").arg(buildName).arg(buildNum));

    // actions
    m_actNew.setText(tr("&New"));
    m_actNew.setStatusTip(tr("Create a new project"));
    m_actNew.setShortcuts(QKeySequence::New);
    connect(&m_actNew, SIGNAL(triggered()), this, SLOT(newFile()));

    m_actOpen.setText(tr("&Open..."));
    m_actOpen.setStatusTip(tr("Open an existing file"));
    m_actOpen.setShortcuts(QKeySequence::Open);
    connect(&m_actOpen, SIGNAL(triggered()), this, SLOT(open()));

    m_actSave.setText(tr("&Save"));
    m_actSave.setStatusTip(tr("Save the document to disk"));
    m_actSave.setShortcuts(QKeySequence::Save);
    connect(&m_actSave, SIGNAL(triggered()), this, SLOT(save()));

    m_actSaveAs.setText(tr("Save &As..."));
    m_actSaveAs.setStatusTip(tr("Save the document under a new name"));
    m_actSaveAs.setShortcuts(QKeySequence::SaveAs);
    connect(&m_actSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

    m_actExit.setText(tr("E&xit"));
    m_actExit.setShortcuts(QKeySequence::Quit);
    connect(&m_actExit, SIGNAL(triggered()), this, SLOT(close()));

    m_actAbout.setText(tr("&About"));
    connect(&m_actAbout, SIGNAL(triggered()), this, SLOT(about()));

    m_actAboutQt.setText(tr("About Qt"));
    connect(&m_actAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    // menu
    m_menuFile = menuBar()->addMenu(tr("Menu"));
    m_menuFile->addAction(&m_actNew);
    m_menuFile->addAction(&m_actOpen);
    m_menuFile->addAction(&m_actSave);
    m_menuFile->addAction(&m_actSaveAs);
    m_menuFile->addSeparator();
    m_menuFile->addAction(&m_actExit);

    m_menuSettings = menuBar()->addMenu(tr("Settings"));

    m_menuHelp = menuBar()->addMenu(tr("Help"));
    m_menuHelp->addAction(&m_actAbout);
    //
    QStringList tableHeaders;
    tableHeaders << tr("Arch");
    tableHeaders << tr("BuildDir");
    tableHeaders << tr("Cmake");
    tableHeaders << tr("Compiler");
    tableHeaders << tr("CompilerPath");
    tableHeaders << tr("Project");
    tableHeaders << tr("ProjectPath");
    tableHeaders << tr("Config");
    tableHeaders << tr("System");
    tableHeaders << tr("Build");
    tableHeaders << tr("Progress");
    m_table->setColumnCount(tableHeaders.size());
    m_table->setHorizontalHeaderLabels(tableHeaders);
    updateConfigurations();
    m_log.setReadOnly(true);

    int32_t fixed4 = 100;

    m_btnAdd.setText(tr("Add"));
    m_btnDel.setText(tr("Del"));
    m_btnAdd.setToolTip(tr("Add new configuration"));
    m_btnDel.setToolTip(tr("Del last configuration"));
    m_btnAdd.setFixedWidth(fixed4);
    m_btnDel.setFixedWidth(fixed4);
    connect(&m_btnAdd, SIGNAL(pressed()), this, SLOT(addItem()));
    connect(&m_btnDel, SIGNAL(pressed()), this, SLOT(delItem()));

    m_btnBuild.setText(tr("Build"));
    m_btnBuild.setFixedWidth(fixed4);
    connect(&m_btnBuild, SIGNAL(pressed()), this, SLOT(build()));
    //
    QGridLayout* centralLayout = new QGridLayout;

    centralLayout->addWidget(m_table, 0, 0, 4, 2);
    centralLayout->addWidget(&m_btnAdd, 0, 2);
    centralLayout->addWidget(&m_btnDel, 1, 2);
    centralLayout->addWidget(&m_btnBuild, 3, 2, Qt::AlignRight);
    centralLayout->addWidget(new QLabel("Log"), 4, 0);
    centralLayout->addWidget(&m_log, 5, 0, 1, 3);

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);
    //
    m_builder = new QProcess;
    m_builder->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_builder, SIGNAL(readyReadStandardOutput()), this, SLOT(readIO()));

    statusBar();

    resize(1000, 500);
}

// -----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
}

// -----------------------------------------------------------------------------
void MainWindow::newFile()
{
    m_curFile.clear();
    m_table->setRowCount(0);
}

// -----------------------------------------------------------------------------
void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString(), QString(),
                                                    "Json (*.json)");
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

// -----------------------------------------------------------------------------
void MainWindow::save()
{
    if (m_curFile.isEmpty()) {
        saveAs();
    } else {
        saveFile(m_curFile);
    }
}

// -----------------------------------------------------------------------------
void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString(), QString(),
                                                    "Json (*.json)");
    if (fileName.isEmpty()) {
        return;
    }
    if (!fileName.endsWith(".json")) {
        fileName.append(".json");
    }
    saveFile(fileName);
}

// -----------------------------------------------------------------------------
void MainWindow::about()
{
    QDialog* aboutDialog = new QDialog(this);
    aboutDialog->setFixedSize(450, 120);
    aboutDialog->setWindowTitle(tr("About %1").arg(buildName));
    QGridLayout* gLayout = new QGridLayout;
    QLabel* description = new QLabel(tr("This program build C++ projects."));
    gLayout->addWidget(description, 0, 0, 1, 2);
    QLabel* imageLabel = new QLabel;
    imageLabel->setPixmap(QPixmap::fromImage(QImage(":/logo_github.png"))
                          .scaledToWidth(200, Qt::SmoothTransformation));
    gLayout->addWidget(imageLabel, 1, 0, 2, 1, Qt::AlignTop);
    QLabel* build = new QLabel(tr("build %1").arg(buildNum));
    gLayout->addWidget(build, 1, 1, Qt::AlignBottom);
    QLabel* company = new QLabel("golubchikov.mihail@gmail.com");
    gLayout->addWidget(company, 2, 1, Qt::AlignBottom);
    aboutDialog->setLayout(gLayout);
    aboutDialog->exec();

    delete aboutDialog;
}

// -----------------------------------------------------------------------------
void MainWindow::addItem()
{
    m_configurations.push_back(Configuration());
    updateConfigurations();
}

// -----------------------------------------------------------------------------
void MainWindow::delItem()
{
    if (m_table->rowCount() > 0) {
        m_table->setRowCount(m_table->rowCount() - 1);
        m_configurations.pop_back();
    }
}

// -----------------------------------------------------------------------------
void MainWindow::readIO()
{
    QString responce = QString(m_builder->readAllStandardOutput());
    m_log.append(responce);
}

// -----------------------------------------------------------------------------
void MainWindow::build()
{
    if (!m_isRunning) {
        setLock(true);
        m_isRunning = true;
        m_log.clear();
        for (int32_t row = 0; row < m_table->rowCount(); ++row) {
            Configuration& config = m_configurations.at(row);
            config.needBuild = reinterpret_cast<QCheckBox*>(m_table->cellWidget(row, 9))->isChecked();
            config.arch     = stringToArch(reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 0))->currentText());
            config.compiler = stringToCompiler(reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 3))->currentText());
            config.config   = stringToConfig(reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 7))->currentText());
            config.system   = stringToSystem(reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 8))->currentText());
        }

        QThread* thread= new QThread;
        Worker* worker = new Worker(m_builder, m_configurations);

        worker->moveToThread(thread);

        connect(worker, SIGNAL(sendLock(bool)), this, SLOT(setLock(bool)));
        connect(worker, SIGNAL(sendLog(QString)), this, SLOT(addLog(QString)));
        connect(worker, SIGNAL(sendProgressMax(int, int)), this, SLOT(setProgressMax(int, int)));
        connect(worker, SIGNAL(sendProgressValue(int, int)), this, SLOT(setProgressValue(int, int)));
        connect(thread, SIGNAL(started()), worker, SLOT(doWork()));

        thread->start();
    }
}

// -----------------------------------------------------------------------------
void MainWindow::setLock(bool lock)
{
    m_isRunning = lock;
    m_btnAdd.setEnabled(!lock);
    m_btnDel.setEnabled(!lock);
    m_btnBuild.setEnabled(!lock);
    m_table->setEnabled(!lock);
//    for (int32_t row = 0; row < m_table->rowCount(); ++row) {
//        reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 0))->setEnabled(!lock);
//        reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 3))->setEnabled(!lock);
//        reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 7))->setEnabled(!lock);
//        reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 8))->setEnabled(!lock);
//        reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 9))->setEnabled(!lock);
//    }
}

// -----------------------------------------------------------------------------
void MainWindow::addLog(const QString& value)
{
    m_log.append(value);
}

// -----------------------------------------------------------------------------
void MainWindow::setProgressMax(int row, int value)
{
    reinterpret_cast<QProgressBar*>(m_table->cellWidget(row, 10))->setMaximum(value);
}

// -----------------------------------------------------------------------------
void MainWindow::setProgressValue(int row, int value)
{
    reinterpret_cast<QProgressBar*>(m_table->cellWidget(row, 10))->setValue(value);
}

// -----------------------------------------------------------------------------
void MainWindow::updateConfigurations()
{
    m_table->setRowCount(0);
    for (Configuration& config : m_configurations) {
        int row = m_table->rowCount();
        m_table->setRowCount(row + 1);
        m_table->setCellWidget(row, 0, newArch());
        m_table->setItem(row, 1, new QTableWidgetItem(config.buildDir));
        m_table->setItem(row, 2, new QTableWidgetItem(config.path.cmake));
        m_table->setItem(row, 3, new QTableWidgetItem(toString(config.compiler)));
        m_table->setCellWidget(row, 3, newCompiler());
        m_table->setItem(row, 4, new QTableWidgetItem(config.path.compiler));
        m_table->setItem(row, 5, new QTableWidgetItem(config.project));
        m_table->setItem(row, 6, new QTableWidgetItem(config.projectPath));
        m_table->setCellWidget(row, 7, newConfig());
        m_table->setCellWidget(row, 8, newSystem());
        m_table->setCellWidget(row, 9, new QCheckBox);
        m_table->setCellWidget(row, 10, new QProgressBar);
        reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 0))->setCurrentText(toString(config.arch));
        reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 3))->setCurrentText(toString(config.compiler));
        reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 7))->setCurrentText(toString(config.config));
        reinterpret_cast<QComboBox*>(m_table->cellWidget(row, 8))->setCurrentText(toString(config.system));
    }
    m_table->resizeColumnsToContents();
}

// -----------------------------------------------------------------------------
void MainWindow::writeIO(const QString& command)
{
    m_builder->write(command.toUtf8());
    m_builder->waitForReadyRead();
}

// -----------------------------------------------------------------------------
void MainWindow::loadFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot read file '%1':\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }
    QJsonParseError parseErr;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseErr);
    file.close();
    if (parseErr.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Error '%1' json parse file '%2'")
                             .arg(parseErr.error)
                             .arg(fileName));
        return;
    }
    m_curFile = fileName;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QJsonObject object = document.object();
    QVariantMap map = object.toVariantMap();
    QVariantList list = map["configurations"].toList();
    m_configurations.clear();
    foreach (QVariant value, list) {
        QVariantMap configMap = value.toMap();
        Configuration config;
        config.arch     = stringToArch(configMap["arch"].toString());
        config.compiler = stringToCompiler(configMap["compiler"].toString());
        config.config   = stringToConfig(configMap["config"].toString());
        config.system   = stringToSystem(configMap["system"].toString());

        config.path.cmake   = configMap["path.cmake"].toString();
        config.path.compiler= configMap["path.compiler"].toString();
        config.path.make    = configMap["path.make"].toString();

        config.buildDir   = configMap["buildDir"].toString();
        config.project    = configMap["project"].toString();
        config.projectPath= configMap["projectPath"].toString();
        m_configurations.emplace_back(config);
    }
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("File '%1' loaded").arg(m_curFile), 5000);

    updateConfigurations();
}

// -----------------------------------------------------------------------------
void MainWindow::saveFile(const QString& fileName) const
{
    QVariantMap map;
    QVariantList configurations;
    for (const Configuration& config : m_configurations) {
        QVariantMap value;
        value.insert("arch",    toString(config.arch));
        value.insert("compiler", toString(config.compiler));
        value.insert("config",  toString(config.config));
        value.insert("system",  toString(config.system));

        value.insert("path.cmake",  config.path.cmake);
        value.insert("path.compiler", config.path.compiler);
        value.insert("path.make",   config.path.make);

        value.insert("buildDir",    config.buildDir);
        value.insert("project",     config.project);
        value.insert("projectPath", config.projectPath);
        configurations << value;
    }
    map.insert("configurations", configurations);
    QJsonObject object = QJsonObject::fromVariantMap(map);
    QJsonDocument document;
    document.setObject(object);
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
        QMessageBox::warning(const_cast<MainWindow*>(this), tr("Recent Files"),
                             tr("Cannot write file '%1':\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }
    file.write(document.toJson());
    file.close();

    const_cast<QString&>(m_curFile) = fileName;
}
} // buider
