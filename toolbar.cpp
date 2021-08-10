#include "toolbar.h"
#include "QGridLayout"

class TSL_QTPaint;

static Toolbar *_myToolbar = nullptr;

Toolbar::Toolbar(QWidget *parent) : QWidget(parent)
{
    setFixedSize(110,170);
    setWindowTitle(QString("Tool"));

    colorBlue = new QPushButton(this);
    colorRed = new QPushButton(this);
    colorGreen = new QPushButton(this);
    colorAny = new QPushButton(this);
    clearImage = new QPushButton(this);

    colorBlue->setStyleSheet("background-color: blue");
    colorRed->setStyleSheet("background-color: red");
    colorGreen->setStyleSheet("background-color: green");
    colorAny->setText("...");
    clearImage->setText("Zmaž obr.");

    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(colorBlue, 0, 0);
    mainLayout->addWidget(colorRed, 0, 1);
    mainLayout->addWidget(colorGreen, 1, 0);
    mainLayout->addWidget(colorAny, 1, 1);
    mainLayout->addWidget(clearImage,2,0,2,2);

}

QPushButton *Toolbar::getClearImage() const
{
    return clearImage;
}

QPushButton *Toolbar::getColorAny() const
{
    return colorAny;
}

QPushButton *Toolbar::getColorBlue() const
{
    return colorBlue;
}

QPushButton *Toolbar::getColorGreen() const
{
    return colorGreen;
}

QPushButton *Toolbar::getColorRed() const
{
    return colorRed;
}

Toolbar *Toolbar::createToolbarInstance(QWidget *parent)
{
    if(_myToolbar == nullptr)
        _myToolbar = new Toolbar(parent);

    return _myToolbar;
}

void Toolbar::closeEvent(QCloseEvent *event)
{

}
