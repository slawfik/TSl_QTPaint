#include "tsl_qtpaint.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>

TSL_QTPaint::TSL_QTPaint(QWidget *parent)
    : QMainWindow(parent) ,myCanvas(new Canvas(this))

{
    setCentralWidget(myCanvas);

    initAction();
    buildMenu();

    setFixedSize(DEFAULT_WIDTH,DEFAULT_HIGHT);
}

TSL_QTPaint::~TSL_QTPaint()
{

}

void TSL_QTPaint::closeEvent(QCloseEvent *event)
{
    Toolbar* myToolbar = Toolbar::createToolbarInstance();
    myToolbar->close();
}

void TSL_QTPaint::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Shift){
        myCanvas->setHoldShiftKey(true);
    }
}

void TSL_QTPaint::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Shift){
        myCanvas->setHoldShiftKey(false);
    }
}

void TSL_QTPaint::s_showToolbar()
{
    Toolbar *toolbar = Toolbar::createToolbarInstance(this);
    if(!toolbar->isVisible()) {
        toolbar->show();
        qDebug() << "Show toolbar";
    }
}

void TSL_QTPaint::s_saveImageAs()
{
    QString initialPath = QDir::currentPath();
    QByteArray png_form("png");
    QByteArray jpg_form("jpg");

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),initialPath,
                                                    tr("%1 Files (*.%2);;%3 Files (*.%4);;All Files (*)")
                                                    .arg(QString::fromLatin1(png_form.toUpper()))
                                                    .arg(QString::fromLatin1(png_form))
                                                    .arg(QString::fromLatin1(jpg_form.toUpper()))
                                                    .arg(QString::fromLatin1(jpg_form)));

    if (!fileName.isEmpty()) {
        if(myCanvas->saveImage(fileName,png_form.constData())){
            QMessageBox msgBox;
            msgBox.setText("Kresba bola úspešne uložená!");
            msgBox.exec();
            return;
        }
    }
    QMessageBox msgBox;
    msgBox.setText("Kresbu sa nepodarilo uložiť!");
    msgBox.exec();
    qDebug() << fileName;
}

void TSL_QTPaint::buildMenu()
{
    menu_View = new QMenu(QString("View"),this);
    menu_View->addAction(actToolbar);

    menu_File = new QMenu(QString("File"),this);
    menu_File->addAction(actSaveAs);
    menu_File->addAction(actBrushPixmap);

    menuBar()->addMenu(menu_File);
    menuBar()->addMenu(menu_View);
    menuBar()->addAction(undoAction);
    menuBar()->addAction(redoAction);
}

void TSL_QTPaint::initAction()
{
    actToolbar = new QAction(QString("Show toolbar"),this);
    connect(actToolbar,SIGNAL(triggered()),this,SLOT(s_showToolbar()));

    actSaveAs = new QAction(QString("Save as"),this);
    connect(actSaveAs,SIGNAL(triggered()),this,SLOT(s_saveImageAs()));

    actBrushPixmap = new QAction(QString("Nový štetc"),this);
    connect(actBrushPixmap,SIGNAL(triggered()),myCanvas,SLOT(s_setNewBrush()));

    undoAction = new QAction(this);
    undoAction->setShortcuts(QKeySequence::Undo);
    undoAction->setIcon(QIcon(":/brush/Brush/undo.png"));
    //connect(undoAction,SIGNAL(triggered()),myCanvas,SLOT(s_readFrom_undoStackBack()));
    connect(undoAction,&QAction::triggered,myCanvas,[this](){
        myCanvas->s_readFrom_undoStackBack();
    });
    addAction(undoAction);

    redoAction = new QAction(this);
    redoAction->setShortcuts(QKeySequence::Redo);
    redoAction->setIcon(QIcon(":/brush/Brush/redo.png"));
    connect(redoAction,SIGNAL(triggered()),myCanvas,SLOT(s_readFrom_undoStackForwar()));
    addAction(redoAction);

    Toolbar* myToolbar = Toolbar::createToolbarInstance();
    connect(myToolbar->getColorRed(), SIGNAL(clicked()), myCanvas, SLOT(s_changeRedColor()));
    connect(myToolbar->getColorBlue(), SIGNAL(clicked()), myCanvas, SLOT(s_changeBlueColor()));
    connect(myToolbar->getColorGreen(), SIGNAL(clicked()), myCanvas, SLOT(s_changeGreenColor()));
    connect(myToolbar->getColorAny(), SIGNAL(clicked()), myCanvas, SLOT(s_setPenColor()));
    connect(myToolbar->getClearImage(), SIGNAL(clicked()), myCanvas, SLOT(s_clearImage()));
    //QTimer
    //QDateTime
}



