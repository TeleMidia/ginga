#include "runview.h"
#include <QApplication>

int main(int argc, char **argv)
{
  QApplication a (argc, argv);
  RunView w;
  w.show ();

  if (argc)
    w.start (argv[1]);

  return a.exec();
}
