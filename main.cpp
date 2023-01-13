#include "editorview.h"
#include <QApplication>

/**
 * Group: Kimochi
 * Class: CS3505
 * Date: November 6, 2022
 *
 * Main class to start up Kimochi Pixlr
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Model model;
    EditorView w(model);
    w.show();
    return a.exec();
}
