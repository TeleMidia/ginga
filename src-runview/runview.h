#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include <QWidget>

namespace Ui {
  class RunView;
}

class RunView : public QWidget
{
  Q_OBJECT

public:
  explicit RunView(QWidget *parent = 0);
  virtual ~RunView();

  void start (const std::string &);
  void stop ();

protected:
  void paintEvent (QPaintEvent *);

protected Q_SLOTS:
  void redrawGinga ();

private:
  Ui::RunView *_ui;
  QImage img;
};

#endif // WIDGET_H
