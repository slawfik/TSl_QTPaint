#include "tsl_qtpaint.h"
#include "ui_tsl_qtpaint.h"

TSL_QTPaint::TSL_QTPaint(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TSL_QTPaint)
{
    ui->setupUi(this);
}

TSL_QTPaint::~TSL_QTPaint()
{
    delete ui;
}

