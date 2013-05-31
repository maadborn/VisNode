/*
 * main.cpp
 *
 * Contains the main() function of the VisNode tool
 *
 * Mats Adborn, 2013-05-01
 */

#include "visnode.h"
#include <QApplication>
#include <QStringList>

int main (int argc, char* argv[])
{
    QApplication app(argc, argv);

    QStringList args;

    if (argc > 0)
        args = QApplication::arguments();

    VisNode visnode(args);
    visnode.run();

    return app.exec();
}
