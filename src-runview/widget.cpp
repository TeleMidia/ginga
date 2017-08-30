#include "widget.h"
#include "ui_widget.h"
/*
#include <cairo.h>
#include <glib.h>
#include <stdlib.h>
#include <glib/gstdio.h>

#include "ginga.h"*/

// Global formatter.
//static Ginga *GINGA = nullptr;

Widget::Widget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Widget)
{
  ui->setupUi(this);
}

Widget::~Widget()
{
  delete ui;
}
