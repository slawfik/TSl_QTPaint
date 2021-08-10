#include "canvas.h"
#include <QPainter>
#include <QColorDialog>
#include <QPixmap>
#include <QDebug>

Canvas::Canvas(QWidget *parent) : QWidget(parent), brushPixmap(":/brush/Brush/ring.png")
{
    setMouseTracking(true);

    //setAttribute(Qt::WA_StaticContents);
    drowing = false;
    penWidth = 50;
    penColor = Qt::blue;
    image = new QImage(QSize(defaultWidth,defaultHight),QImage::Format_ARGB32);
    image->fill(qRgb(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF));
    update();
    defBrus = QCursor(brushPixmap,-(penWidth/2), -(penWidth/2));
    this->setCursor(defBrus);
}

Canvas::~Canvas()
{
    delete image;
}

void Canvas::s_setPenColor()
{
    QColor collor = QColorDialog::getColor(Qt::blue,this,QString("Paleta farieb"));
    swapBrushColor(penColor,collor);
    penColor = collor;
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
}

void Canvas::s_changeBlueColor()
{
    swapBrushColor(penColor,Qt::blue);
    penColor = Qt::blue;
}

void Canvas::s_changeGreenColor()
{
    swapBrushColor(penColor,Qt::green);
    penColor = Qt::green;

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
    if(event->button() == Qt::LeftButton)
        drowing = false;
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

    painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(pointLast, pointEnd);

    int rad = (penWidth / 2) + 2;

    update(QRect(pointLast,pointEnd).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));

    pointLast = pointEnd;
}

void Canvas::paintPoint(const QPoint &point)
{
    QPainter painter(image);

    painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
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

bool Canvas::saveImage(const QString &fileName, const char *format)
{
    if(image->save(fileName,format))
        return true;
    else
        return false;
}
