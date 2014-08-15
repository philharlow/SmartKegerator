#ifndef GRAPHFRAME_H
#define GRAPHFRAME_H

#include <QFrame>
#include <QMutex>
#include <QDragMoveEvent>
#include <widgets/graph/graph.h>

using namespace std;

class GraphFrame : public QFrame
{
    Q_OBJECT
public:
    explicit GraphFrame(QWidget *parent = 0);

    void AddGraph(Graph* graph);
    vector<Graph*> Graphs;
    void SetTimeStep(int timeStepIndex);
    int TimeStepIndex;
    void ShowGraph(Graph* graph);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    bool PaintEnabled;

protected:
    void paintEvent(QPaintEvent *event);

private:
    int frameWidth, frameHeight, graphHeight;
    double graphMinValue, graphMaxValue, graphDeltaValue;
    double frameMinValue, frameMaxValue, frameDeltaValue;
    double stepY, paddingY;
    double lastUpdate;

    QPen gridPen, textPen;
    QPoint dragStart;

    void updateMinMax();
    int getPixelY(double val);
    int getPixelX(double time);
    void drawGrid(QPainter &painter);
    void drawVerticalGridLine(double val, QPen pen, bool drawText, QPainter &painter);
    void drawHorizontalGridLine(double val, QPen pen, int decimals, QPainter &painter);
    void drawGraphs(QPainter &painter);
    void drawGraphDetails(QPainter &painter);

};

#endif // GRAPHFRAME_H
