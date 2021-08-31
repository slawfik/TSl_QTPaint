#include "canvas.h"
#include <QPainter>
#include <QColorDialog>
#include <QPixmap>
#include <QDebug>
#include <QList>
#include <QFileDialog>
#include <QDateTime>
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

    image = new QImage(QSize(DEFAULT_WIDTH,DEFAULT_HIGHT),QImage::Format_RGBA64);
    image->fill(qRgb(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF));
    update();

    cursorBrus = QCursor(brushPixmap,-(penWidth/2), -(penWidth/2));
    this->setCursor(cursorBrus);

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

void Canvas::s_setNewBrush()
{
    QString filename = QFileDialog::getOpenFileName(this,("Open File"),QDir::currentPath(), ( "All files (*.*) ;; PNG files (*.png)"));
    qDebug() << filename;

    if( !filename.isNull() ) {
        brushPixmap = QPixmap(filename);
        penColor = Qt::blue;
        drowingBrush->setStyle(Qt::SolidPattern);
        drowingBrush->setColor(penColor);

        cursorBrus = QCursor(brushPixmap,-(penWidth/2), -(penWidth/2));
        this->setCursor(cursorBrus);
    }
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
    if(event->button() == Qt::LeftButton && !holdShiftKey){
        drowing = true;
        pointLast = event->pos();
        paintPoint(event->pos());
    } else if (event->button() == Qt::LeftButton && holdShiftKey) {
        drowing = true;
        paintLine(event->pos());
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

void Canvas::setHoldShiftKey(bool newHoldShiftKey)
{
    holdShiftKey = newHoldShiftKey;
}

int Canvas::getPenWidth() const
{
    return penWidth;
}

void Canvas::paintLine(const QPoint &pointEnd)
{
    QPainter painter(image);

    painter.setBrush(*drowingBrush);
    painter.setPen(QPen(*drowingBrush, penWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));

    //painter.drawLine(pointLast, pointEnd);
    /*pointLast.setX(pointLast.rx() - brushPixmap.width()/2);
    pointLast.setY(pointLast.ry() - brushPixmap.height()/2);

    int xStep = (pointLast.rx() - (pointEnd.x()- brushPixmap.width()/2))/15;
    int yStep = (pointLast.ry() - (pointEnd.y()- brushPixmap.height()/2))/15;
    for (int i=0;i<15;i++) {
        pointLast.setX(pointLast.rx() + xStep);
        pointLast.setY(pointLast.ry() + yStep);
        painter.drawPixmap(pointLast,brushPixmap);
    }

    painter.drawPixmap(pointEnd,brushPixmap);
    update();*/

    painter.drawLine(pointLast,pointEnd);

    int rad = (penWidth / 2) + 2;

    update(QRect(pointLast,pointEnd).normalized().adjusted(-rad, -rad, +rad, +rad));

    pointLast = pointEnd;
}

void Canvas::paintPoint(const QPoint &point)
{
    QPainter painter(image);

    painter.setBrush(*drowingBrush);
    painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    //painter.setPen(QPen(*drowingBrush, penWidth, Qt::SolidLine));

    /*QPoint tempPoint = point;
    tempPoint.setX(tempPoint.rx()- brushPixmap.width()/2);
    tempPoint.setY(tempPoint.ry()- brushPixmap.height()/2);

    painter.drawPixmap(tempPoint,brushPixmap);*/
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

    cursorBrus = QCursor(brushPixmap,-(penWidth/2), -(penWidth/2));
    this->setCursor(cursorBrus);
}

void Canvas::insertTo_undoStack()
{
#ifdef QT_DEBUG
    clock_t start,end;
    start = clock();
#endif

    lzo_uint inLen = image->sizeInBytes();
    lzo_uint outLen = inLen+inLen/16+64+3; //def from library
    unsigned char *tempOut = new unsigned char[outLen];
    lzo_voidp wrkmem = (lzo_voidp) malloc(LZO1X_1_MEM_COMPRESS);
    QByteArray arr;
    QDataStream ds(&arr, QIODevice::WriteOnly);

    if(!undoStack.isEmpty()){
        if(undoStack.at(undoStackPosition) != undoStack.last()){
            int countForDelete = undoStack.size() - (undoStackPosition+1);
            qDebug() << QString("ustack.size %1 uStackPosition %2 \n").arg(undoStack.size()).arg(undoStackPosition+1);
            for (int i = 0;i<countForDelete;i++ ) {
                undoStack.removeLast();
            }
        }
    }

    //COMPRESS
    int r= lzo1x_1_compress(image->bits(),inLen,tempOut,&outLen,wrkmem);
    if (r == LZO_E_OK)
        qDebug() << QString("compressed %1 bytes into %2 bytes\n").arg(inLen).arg(outLen);

    //PUSH IMAGE TO STACK
    ds.writeRawData((const char*)tempOut, outLen);
    //ds.writeRawData((const char *)image->bits(), inLen);
    undoStack.push_back(arr);

    undoStackPosition++;
    free(wrkmem);
    delete[] tempOut;

#ifdef QT_DEBUG
    end = clock();
    QString text;
    text.sprintf("--TIMER-- insertTo_undoStack --> %0.6f sec", ((float) end-start)/CLOCKS_PER_SEC);
    qDebug() << text;
#endif
}

void Canvas::s_readFrom_undoStackBack()
{
#ifdef QT_DEBUG
    clock_t start,end;
    start = clock();
#endif

    if(undoStackPosition == 0)
        return;
    lzo_uint new_len;
    lzo_uint imageLen = image->sizeInBytes();
    lzo_uint inLen = undoStack.operator[](--undoStackPosition).size(); //DECREMENT uStackPosition
    unsigned char* tempIn = reinterpret_cast<unsigned char*> (undoStack.operator[](undoStackPosition).data());
    lzo_bytep tempOut = (lzo_bytep) malloc(imageLen);

    //DECOMPRESS
    //new_len = imageLen;
    int r = lzo1x_decompress(tempIn, inLen, tempOut, &new_len, NULL);
    if (r == LZO_E_OK && new_len == imageLen)
        qDebug() << QString("decompressed %1 bytes back into %2 bytes\n").arg(inLen).arg(imageLen);
    else {
        qDebug() << QString("decompress failed");
    }

    //UPDATE IMAGE
    *image = QImage(DEFAULT_WIDTH, DEFAULT_HIGHT, image->format());
    memcpy((char*)image->bits(),tempOut,new_len);

    update();
    free(tempOut);

#ifdef QT_DEBUG
    end = clock();
    QString text;
    text.sprintf("--TIMER-- s_readFrom_undoStackBack --> %0.6f sec", ((float) end-start)/CLOCKS_PER_SEC);
    qDebug() << text;
#endif
}

void Canvas::s_readFrom_undoStackForwar()
{
#ifdef QT_DEBUG
    clock_t start,end;
    start = clock();
#endif

    if(undoStack.at(undoStackPosition) == undoStack.last())
        return;
    qDebug() << QString("Forward --> %1 _").arg(undoStackPosition);
    lzo_uint lenAfterDecompress;
    lzo_uint imageLen = image->sizeInBytes();
    lzo_uint inputLen_Compress = undoStack.operator[](++undoStackPosition).size(); //INCREMENT uStackPosition
    unsigned char* input_Compress = reinterpret_cast<unsigned char*> (undoStack.operator[](undoStackPosition).data());
    lzo_bytep tempOut = (lzo_bytep) malloc(imageLen);

    //DECOMPRESS
    //new_len = imageLen;
    int r = lzo1x_decompress(input_Compress, inputLen_Compress, tempOut, &lenAfterDecompress, NULL);
    if (r == LZO_E_OK && lenAfterDecompress == imageLen)
        qDebug() << QString("decompressed %1 bytes back into %2 bytes\n").arg(inputLen_Compress).arg(imageLen);
    else {
        qDebug() << QString("decompress failed");
    }

    //UPDATE IMAGE
    *image = QImage(DEFAULT_WIDTH, DEFAULT_HIGHT, image->format());
    memcpy((char*)image->bits(),tempOut,lenAfterDecompress);

    update();
    free(tempOut);

#ifdef QT_DEBUG
    end = clock();
    QString text;
    text.sprintf("--TIMER-- s_readFrom_undoStackForwar --> %0.6f sec", ((float) end-start)/CLOCKS_PER_SEC);
    qDebug() << text;
#endif
}

bool Canvas::saveImage(const QString &fileName, const char *format)
{
    if(image->save(fileName,format))
        return true;
    else
        return false;
}
