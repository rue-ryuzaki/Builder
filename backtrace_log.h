#ifdef __GNUC__
#include <csignal>
#include <iostream>
#ifndef _WIN32
#include <execinfo.h>
#endif // _WIN32
#include <unistd.h>

void printTrace(int signum)
{
    std::cerr << "CAUGHT SIGNAL: " << signum << std::endl;
#ifndef _WIN32
    const int COUNT = 1024;
    void* trace[COUNT];
    int actual = backtrace(trace, COUNT);
    backtrace_symbols_fd(trace, actual, 2);
#endif // _WIN32
}

void printInfo(int signum)
{
    printTrace(signum);
    signal(signum, SIG_DFL);
    std::raise(signum);
}
#endif // __GNUC__
