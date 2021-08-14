#include "canvas.h"
#include <QPainter>
#include <QColorDialog>
#include <QPixmap>
#include <QDebug>
#include <QList>
// compression library
#include "lzo/lzoconf.h"
#include <lzo/lzo1x.h>

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

    /*for (int i = 0;i<10 ;i++ ) {
        undoStack.push_back(*image);
    }*/
    insertTo_undoStack(); //insert first image

    //init LZO compression library
    if (lzo_init() != LZO_E_OK)
    {
        printf("internal error - lzo_init() failed !!!\n");
        printf("(this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
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
    insertTo_undoStack();
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
    //ssss
    lzo_uint inLen = image->sizeInBytes();
    lzo_uint outLen = inLen+inLen/16+64+3; //def from library
    unsigned char tempOut[outLen];
    lzo_voidp wrkmem = (lzo_voidp) malloc(LZO1X_1_MEM_COMPRESS);

    QByteArray arr;
    QDataStream ds(&arr, QIODevice::WriteOnly);

    //COMPRESS
    int r= lzo1x_1_compress(image->bits(),inLen,tempOut,&outLen,wrkmem);
    if (r == LZO_E_OK)
        qDebug() << QString("compressed %1 bytes into %2 bytes\n").arg(inLen).arg(outLen);
    ds.writeRawData((const char*)tempOut, outLen);
    uStack.push_back(arr);

    //++++
    /*QByteArray arr;
    QDataStream ds(&arr, QIODevice::ReadWrite);
    ds.writeRawData((const char*)image->bits(), image->byteCount());
     ds.device()->seek(0);

        *image = QImage(defaultWidth, defaultHight, image->format());
        ds.readRawData((char*)image->bits(), image->byteCount()); // We read the data directly into the image buffer
    //++++**/
    update();

    uStackPosition++;
    free(wrkmem);
    //ssss

    /*if(undoStack_insertPosition >= undoStackMax){
        undoStack_insertPosition = 0;
    }

    qDebug() << QString("undoStack_insertPosition = ") <<QString::number(undoStack_insertPosition);
    undoStack.replace(undoStack_insertPosition ,*image);
    undoStack_readPosition = undoStack_insertPosition++;  //read is lover

    if(stepBack < 9)
        stepBack++;
    stepForward = 0;*/
}

void Canvas::s_readFrom_undoStackBack()
{
    //ssss
    if(uStackPosition == 0)
        return;
    lzo_uint new_len;
    lzo_uint imageLen = image->sizeInBytes();
    lzo_uint inLen = uStack.operator[](--uStackPosition).size();
    unsigned char* tempIn = reinterpret_cast<unsigned char*> (uStack.operator[](uStackPosition).data());
    lzo_bytep tempOut = (lzo_bytep) malloc(imageLen);

    //DECOMPRESS
    new_len = imageLen;
    int r = lzo1x_decompress(tempIn, inLen, tempOut, &new_len, NULL);
    if (r == LZO_E_OK && new_len == imageLen)
        qDebug() << QString("decompressed %1 bytes back into %2 bytes\n").arg(inLen).arg(imageLen);
    else
    {
        qDebug() << QString("decompress failed");
    }

    *image = QImage(defaultWidth, defaultHight, image->format());
    memcpy((char*)image->bits(),tempOut,new_len);

    update();
    free(tempOut);
    //ssss

    /*if(stepBack == 0)
        return;
    if(undoStack_readPosition == 0)
        undoStack_readPosition = 10;

    #ifdef QT_DEBUG
        qDebug() << QString("BACK: undoStack_readPosition = ") <<QString::number(undoStack_readPosition) <<
                    QString("undoStack_insertPosition = ") << QString::number(undoStack_insertPosition);
    #endif

    --undoStack_readPosition;
    *image = undoStack.at(undoStack_readPosition);
    update();
    stepBack--;
    stepForward++;

    if((undoStack_insertPosition-1) == -1)
        undoStack_insertPosition = 9;
    else
        undoStack_insertPosition--;*/
}

void Canvas::s_readFrom_undoStackForwar()
{
    /*if(stepForward == 0)
        return;
    if(undoStack_readPosition == 9)
        undoStack_readPosition = -1;

    #ifdef QT_DEBUG
        qDebug() << QString("FORWARD: undoStack_readPosition = ") <<QString::number(undoStack_readPosition) <<
                    QString("undoStack_insertPosition = ") << QString::number(undoStack_insertPosition);
    #endif

    ++undoStack_readPosition;
    *image = undoStack.at(undoStack_readPosition);
    update();
    stepBack++;
    stepForward--;

    if((undoStack_insertPosition) == 10)
        undoStack_insertPosition = 0;
    else
        undoStack_insertPosition++;*/
}

bool Canvas::saveImage(const QString &fileName, const char *format)
{
    if(image->save(fileName,format))
        return true;
    else
        return false;
}
