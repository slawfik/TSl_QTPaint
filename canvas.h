#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QMouseEvent>

#define defaultHight 800
#define defaultWidth 1200

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

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QImage *image;
    bool drowing;
    int penWidth;
    QColor penColor;
    QPoint pointLast;
    QCursor defBrus;
    QPixmap brushPixmap;

    void paintLine(const QPoint &pointEnd);
    void paintPoint(const QPoint &point);
    void swapBrushColor(const QColor &from, QColor to);

signals:

};

#endif // CANVAS_H
