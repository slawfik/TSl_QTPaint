#ifndef TSL_QTPAINT_H
#define TSL_QTPAINT_H

#include <QMainWindow>
#include <QMenu>
#include "toolbar.h"
#include "canvas.h"

#include <QDebug>

class TSL_QTPaint : public QMainWindow
{
    Q_OBJECT

public:
    TSL_QTPaint(QWidget *parent = nullptr);
    ~TSL_QTPaint();

    void closeEvent(QCloseEvent *event) override;

public slots:
    void s_showToolbar();
    void s_saveImageAs();

private:
    void buildMenu();
    void initAction();

    QMenu *menu_File;
    QMenu *menu_Save;
    //
    QMenu *menu_View;
    QMenu *menu_ShowToolbar;

    QAction *actToolbar;
    QAction *actSaveAs;
    Toolbar *myToolbar;
    Canvas *myCanvas;

};
#endif // TSL_QTPAINT_H
