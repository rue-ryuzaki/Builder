#ifndef BUILDER_MAINWINDOW_H
#define BUILDER_MAINWINDOW_H

#include <QMainWindow>

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QMenu>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>

#include <vector>

#include "configuration.h"

namespace builder {
// -----------------------------------------------------------------------------
// -- MainWindow ---------------------------------------------------------------
// -----------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // actions
    void        newFile();
    void        open();
    void        save();
    void        saveAs();
    void        about();
    // table
    void        addItem();
    void        delItem();
    //
    void        readIO();
    void        build();
    void        setLock(bool);
    void        addLog(const QString& value);
    void        setProgressMax(int row, int value);
    void        setProgressValue(int row, int value);

private:
    void        updateConfigurations();
    void        writeIO(const QString& command);
    void        loadFile(const QString& fileName);
    void        saveFile(const QString& fileName) const;
    //
    std::vector<Configuration> m_configurations;
    QProcess*   m_builder;
    bool        m_isRunning;
    // GUI
    QComboBox   m_boxSystem;
    QComboBox   m_boxArch;
    QComboBox   m_boxCompiler;
    QComboBox   m_boxConfig;
    QTableWidget* m_table;
    QPushButton m_btnAdd;
    QPushButton m_btnDel;
    QPushButton m_btnBuild;
    QTextEdit   m_log;
    //
    QString     m_curFile;
    // Menu
    QMenu*      m_menuFile;
    QMenu*      m_menuSettings;
    QMenu*      m_menuHelp;
    // Actions
    QAction     m_actNew;
    QAction     m_actOpen;
    QAction     m_actSave;
    QAction     m_actSaveAs;
    QAction     m_actExit;
    QAction     m_actAbout;
    QAction     m_actAboutQt;

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator =(const MainWindow&) = delete;
};
} // buider

#endif // BUILDER_MAINWINDOW_H
