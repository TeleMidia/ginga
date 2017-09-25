#include "runview.h"
#include <QApplication>

int main(int argc, char **argv)
{
  QApplication a(argc, argv);
  RunView w;
  w.show();

  w.start ("/home/roberto/workspaces/cpp/ginga/tests-ncl/test-lua-4grid.ncl");

  return a.exec();
}
