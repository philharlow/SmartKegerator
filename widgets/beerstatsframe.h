#ifndef BEERSTATSFRAME_H
#define BEERSTATSFRAME_H

#include <QFrame>
#include <QPainter>
#include <data/keg.h>

using namespace std;

class QStringDoublePair
{
public:
    QString Label;
    double Value;
    QStringDoublePair(QString str, double val)
    {
        Label = str;
        Value = val;
    }
};

class BeerStatsFrame : public QFrame
{
    Q_OBJECT
public:
    explicit BeerStatsFrame(QWidget *parent = 0);
\
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void SetKeg(Keg* keg);

    bool PaintEnabled;

protected:
    void paintEvent(QPaintEvent *event);

private:
    Keg* CurrentKeg;
    vector<QString> dowNames;

    int frameWidth, frameHeight, footerHeight, headerHeight;
    QPoint dragStart;
    bool dragging;
    double xOffset;
    int currentPanel;

    QBrush tileBrush;
    QBrush barBrush;
    QPen textPen;

    void drawNone(QPainter &painter, QString str, int xOffset = 0);
    void drawHeader(QPainter &painter, QString text, int xOffset);
    void drawContent(QPainter &painter, int panel, int x);
    void drawBars(QPainter &painter, vector<QStringDoublePair> vals, int xOffset);
    void drawTiles(QPainter &painter, vector<QStringDoublePair> vals, int xOffset);
    void drawFooter(QPainter &painter);

    int panels;
    int padding;
    int extraPadding;
};

#endif // BEERSTATSFRAME_H
