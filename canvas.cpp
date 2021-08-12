#include "canvas.h"
#include <QPainter>
#include <QColorDialog>
#include <QPixmap>
#include <QDebug>
#include <QList>

Canvas::Canvas(QWidget *parent) : QWidget(parent), brushPixmap(":/brush/Brush/ring.png")
{
    setMouseTracking(true);

    drowing = false;
    penWidth = 50;
    penColor = Qt::blue;
    drowingBrush = new QBrush(brushPixmap);
    drowingBrush->setStyle(Qt::SolidPattern);
    drowingBrush->setColor(penColor);

    image = new QImage(QSize(defaultWidth,defaultHight),QImage::Format_RGBA64);
    image->fill(qRgb(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF));
    update();

    defBrus = QCursor(brushPixmap,-(penWidth/2), -(penWidth/2));
    this->setCursor(defBrus);

    for (int i = 0;i<10 ;i++ ) {
        undoStack.push_back(*image);
    }
}

Canvas::~Canvas()
{
    delete image;
    delete drowingBrush;
}

void Canvas::s_setPenColor()
{
    QColor collor = QColorDialog::getColor(Qt::blue,this,QString("Paleta farieb"));
    swapBrushColor(penColor,collor);
    penColor = collor;
    drowingBrush->setColor(penColor);
}

void Canvas::s_clearImage()
{
    image->fill(qRgb(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF));
    update();
}

void Canvas::s_changeRedColor()
{
    swapBrushColor(penColor,Qt::red);
    penColor = Qt::red;
    drowingBrush->setColor(penColor);
}

void Canvas::s_changeBlueColor()
{
    swapBrushColor(penColor,Qt::blue);
    penColor = Qt::blue;
    drowingBrush->setColor(penColor);
}

void Canvas::s_changeGreenColor()
{
    swapBrushColor(penColor,Qt::green);
    penColor = Qt::green;
    drowingBrush->setColor(penColor);
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        drowing = true;
        pointLast = event->pos();
        paintPoint(event->pos());
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        drowing = false;
        insertTo_undoStack();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if(drowing){
        paintLine(event->pos());
    }
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect dirtyRect = event->rect();

    painter.drawImage(dirtyRect, *image, dirtyRect);
}

int Canvas::getPenWidth() const
{
    return penWidth;
}

void Canvas::paintLine(const QPoint &pointEnd)
{
    QPainter painter(image);

    //painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    painter.setPen(QPen(*drowingBrush, penWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    painter.setBrush(*drowingBrush);

    painter.drawLine(pointLast, pointEnd);

    int rad = (penWidth / 2) + 2;

    update(QRect(pointLast,pointEnd).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));

    pointLast = pointEnd;
}

void Canvas::paintPoint(const QPoint &point)
{
    QPainter painter(image);

    //painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    painter.setPen(QPen(*drowingBrush, penWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    painter.setBrush(*drowingBrush);

    painter.setBrush(*drowingBrush);

    painter.drawPoint(point);

    update();
}

void Canvas::swapBrushColor(const QColor &from,QColor to)
{
    QImage brushimage = brushPixmap.toImage();
    for(int y = 0; y < brushimage.height(); y++)
    {
        for(int x= 0; x < brushimage.width(); x++)
        {
            if(brushimage.pixelColor(x,y) == from){
                to.setAlpha(brushimage.pixelColor(x,y).alpha());
                brushimage.setPixelColor(x,y,to);
            }
        }
    }
    brushPixmap = QPixmap::fromImage(brushimage);

    defBrus = QCursor(brushPixmap,-(penWidth/2), -(penWidth/2));
    this->setCursor(defBrus);
}

void Canvas::insertTo_undoStack()
{
    if(undoStack_insertPosition >= undoStackMax){
        undoStack_insertPosition = 0;
    }

    qDebug() << QString("undoStack_insertPosition = ") <<QString::number(undoStack_insertPosition);
    undoStack.replace(undoStack_insertPosition ,*image);
    undoStack_readPosition = undoStack_insertPosition++;  //read is lover

    if(stepBack < 9)
        stepBack++;
    stepForward = 0;
}

void Canvas::s_readFrom_undoStackBack()
{
    if(stepBack == 0)
        return;
    if(undoStack_readPosition == 0)
        undoStack_readPosition = 10;

    qDebug() << QString("BACK: undoStack_readPosition = ") <<QString::number(undoStack_readPosition) <<
                QString("undoStack_insertPosition = ") << QString::number(undoStack_insertPosition);

    --undoStack_readPosition;
    *image = undoStack.at(undoStack_readPosition);
    update();
    stepBack--;
    stepForward++;

    if((undoStack_insertPosition-1) == -1)
        undoStack_insertPosition = 9;
    else
        undoStack_insertPosition--;
}

void Canvas::s_readFrom_undoStackForwar()
{
    if(stepForward == 0)
        return;
    if(undoStack_readPosition == 9)
        undoStack_readPosition = -1;

    qDebug() << QString("FORWARD: undoStack_readPosition = ") <<QString::number(undoStack_readPosition) <<
                QString("undoStack_insertPosition = ") << QString::number(undoStack_insertPosition);

    ++undoStack_readPosition;
    *image = undoStack.at(undoStack_readPosition);
    update();
    stepBack++;
    stepForward--;

    if((undoStack_insertPosition) == 10)
        undoStack_insertPosition = 0;
    else
        undoStack_insertPosition++;
}

bool Canvas::saveImage(const QString &fileName, const char *format)
{
    if(image->save(fileName,format))
        return true;
    else
        return false;
}
