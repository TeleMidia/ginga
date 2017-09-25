#include "runview.h"
#include "ui_runview.h"

#include <cairo.h>
#include <glib.h>
#include <stdlib.h>
#include <glib/gstdio.h>
#include "ginga.h"

// Global formatter.
static Ginga *GINGA = nullptr;

RunView::RunView(QWidget *parent) :
  QWidget(parent),
  _ui(new Ui::RunView)
{
  _ui->setupUi(this);
}

RunView::~RunView()
{
  delete _ui;
}
