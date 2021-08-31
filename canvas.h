#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QMouseEvent>

#define DEFAULT_HIGHT 1000
#define DEFAULT_WIDTH 1800
#define undoStackMax 10

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);
    ~Canvas();
    bool saveImage(const QString &fileName,const char *format);
    int getPenWidth() const;
    void setHoldShiftKey(bool newHoldShiftKey);

public slots:
    void s_changeRedColor();
    void s_changeBlueColor();
    void s_changeGreenColor();
    void s_setPenColor(); //anyButton
    void s_clearImage();
    void s_setNewBrush();
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
    bool holdShiftKey =false;
    unsigned int penWidth;
    QColor penColor;

    QPoint pointLast;
    QCursor cursorBrus;
    QBrush *drowingBrush;
    QPixmap brushPixmap;
    QByteArray tempByteArray;

    //new undoStack
    QList<QByteArray> undoStack;
    int undoStackPosition = -1;

    void paintLine(const QPoint &pointEnd);
    void paintPoint(const QPoint &point);
    void swapBrushColor(const QColor &from, QColor to);
    void insertTo_undoStack();

signals:

};

#endif // CANVAS_H
