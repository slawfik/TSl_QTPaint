#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QMouseEvent>

#define defaultHight 800
#define defaultWidth 1200
#define undoStackMax 10

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);
    ~Canvas();
    bool saveImage(const QString &fileName,const char *format);
    int getPenWidth() const;

public slots:
    void s_changeRedColor();
    void s_changeBlueColor();
    void s_changeGreenColor();
    void s_setPenColor(); //anyButton
    void s_clearImage();
    void s_readFrom_undoStackBack();
    void s_readFrom_undoStackForwar();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QImage *image;
    bool drowing;
    unsigned int penWidth;
    QColor penColor;

    QPoint pointLast;
    QCursor defBrus;
    QBrush *drowingBrush;
    QPixmap brushPixmap;
    QByteArray tempByteArray;

    //new undoStack
    QList<QByteArray> uStack;
    int uStackPosition = -1;
    //old unduStack
    /*QList<QImage> undoStack;
    int undoStack_insertPosition = 1;
    int undoStack_readPosition = 1;
    int stepBack = 0;
    int stepForward = 0;*/

    void paintLine(const QPoint &pointEnd);
    void paintPoint(const QPoint &point);
    void swapBrushColor(const QColor &from, QColor to);
    void insertTo_undoStack();

signals:

};

#endif // CANVAS_H
