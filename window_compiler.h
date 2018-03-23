#ifndef BUILDER_WINDOW_COMPILER_H
#define BUILDER_WINDOW_COMPILER_H

#include <QDialog>

#include "configuration.h"

namespace builder {
class WindowCompiler : public QDialog
{
public:
    WindowCompiler(QWidget* parent = nullptr)
        : QDialog(parent) { }

private:
};
} // builder

#endif // BUILDER_WINDOW_COMPILER_H
