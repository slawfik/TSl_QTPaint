#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>
#include <QPushButton>
#include "QGridLayout"

class Toolbar : public QWidget
{
    Q_OBJECT
public:
    static Toolbar* createToolbarInstance(QWidget *parent = nullptr);

    QPushButton *getColorRed() const;
    QPushButton *getColorGreen() const;
    QPushButton *getColorBlue() const;
    QPushButton *getColorAny() const;

    QPushButton *getClearImage() const;

protected:
    void closeEvent(QCloseEvent *event) override;
    Toolbar(QWidget *parent = nullptr);

    QPushButton *colorRed;
    QPushButton *colorBlue;
    QPushButton *colorGreen;
    QPushButton *colorAny;
    QPushButton *clearImage;

    QGridLayout *mainLayout;

private slots:

signals:

};

#endif // TOOLBAR_H
