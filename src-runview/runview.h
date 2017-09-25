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

protected:
  void paintEvent(QPaintEvent *);

private:
  Ui::RunView *_ui;
};

#endif // WIDGET_H
