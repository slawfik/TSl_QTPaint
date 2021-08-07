#ifndef TSL_QTPAINT_H
#define TSL_QTPAINT_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class TSL_QTPaint; }
QT_END_NAMESPACE

class TSL_QTPaint : public QMainWindow
{
    Q_OBJECT

public:
    TSL_QTPaint(QWidget *parent = nullptr);
    ~TSL_QTPaint();

private:
    Ui::TSL_QTPaint *ui;
};
#endif // TSL_QTPAINT_H
