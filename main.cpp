#include "mainwindow.h"

#include <QApplication>

#include "backtrace_log.h"

int main(int argc, char* argv[])
{
#ifdef  __GNUC__
    signal(SIGINT, printInfo);  // Ctrl + C pressed in term
    signal(SIGABRT, printInfo); // throw exception
    signal(SIGTERM, printInfo); // terminated (Qt -> stop running program)
    signal(SIGSEGV, printInfo); //
#endif // __GNUC__

    QApplication a(argc, argv);

    builder::MainWindow window;
    window.show();

    return a.exec();
}
