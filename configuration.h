#ifndef BUILDER_CONFIGURATION_H
#define BUILDER_CONFIGURATION_H

#include <QString>

namespace builder {
// -----------------------------------------------------------------------------
// -- Build architecture -------------------------------------------------------
// -----------------------------------------------------------------------------
enum class Arch {
    x86, // i386
    x64, // x64_86

    unknown
};

inline Arch stringToArch(const QString& value)
{
    if (value == "x86") {
        return Arch::x86;
    } else if (value == "x64") {
        return Arch::x64;
    } else {
        return Arch::unknown;
    }
}

inline QString toString(Arch arch)
{
    switch (arch) {
        case Arch::x86 :
            return "x86";
        case Arch::x64 :
            return "x64";
        default :
            return "unknown";
    }
}

// -----------------------------------------------------------------------------
// -- Build compiler -----------------------------------------------------------
// -----------------------------------------------------------------------------
enum class Compiler {
    clang,
    gcc,
    mingw,
    msvc,

    unknown
};

inline Compiler stringToCompiler(const QString& value)
{
    if (value == "clang") {
        return Compiler::clang;
    } else if (value == "gcc") {
        return Compiler::gcc;
    } else if (value == "mingw") {
        return Compiler::mingw;
    } else if (value == "msvc") {
        return Compiler::msvc;
    } else {
        return Compiler::unknown;
    }
}

inline QString toString(Compiler compiler)
{
    switch (compiler) {
        case Compiler::clang :
            return "clang";
        case Compiler::gcc :
            return "gcc";
        case Compiler::mingw :
            return "mingw";
        case Compiler::msvc :
            return "msvc";
        default :
            return "unknown";
    }
}

// -----------------------------------------------------------------------------
// -- Build config -------------------------------------------------------------
// -----------------------------------------------------------------------------
enum class Config {
    Release,
    Debug,

    unknown
};

inline Config stringToConfig(const QString& value)
{
    if (value == "Release") {
        return Config::Release;
    } else if (value == "Debug") {
        return Config::Debug;
    } else {
        return Config::unknown;
    }
}

inline QString toString(Config config)
{
    switch (config) {
        case Config::Release :
            return "Release";
        case Config::Debug :
            return "Debug";
        default :
            return "unknown";
    }
}

// -----------------------------------------------------------------------------
// -- Build Operation System ---------------------------------------------------
// -----------------------------------------------------------------------------
enum class System {
    Linux,
    MaxOS,
    Windows,

    unknown
};

inline System stringToSystem(const QString& value)
{
    if (value == "linux") {
        return System::Linux;
    } else if (value == "maxOS") {
        return System::MaxOS;
    } else if (value == "windows") {
        return System::Windows;
    } else {
        return System::unknown;
    }
}

inline QString toString(System system)
{
    switch (system) {
        case System::Linux :
            return "linux";
        case System::MaxOS :
            return "maxOS";
        case System::Windows :
            return "windows";
        default :
            return "unknown";
    }
}

// -----------------------------------------------------------------------------
// -- Pathes -------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct Pathes
{
    Pathes()
        : cmake(),
          compiler(),
          make()
    { }
    QString cmake;
    QString compiler;
    QString make;
};

// -----------------------------------------------------------------------------
// -- Total configuration ------------------------------------------------------
// -----------------------------------------------------------------------------
struct Configuration
{
    Configuration()
        : arch(Arch::unknown),
          buildDir(),
          buildName(),
          compiler(Compiler::unknown),
          project(),
          projectPath(),
          config(Config::unknown),
          system(System::unknown),
          path(),
          needBuild(false)
    { }

    Arch        arch;
    QString     buildDir;
    QString     buildName;
    Compiler    compiler;
    QString     project;
    QString     projectPath;
    Config      config;
    System      system;
    Pathes      path;
    bool        needBuild;
};
} // builder

#endif // BUILDER_CONFIGURATION_H
