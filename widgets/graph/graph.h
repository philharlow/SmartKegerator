#ifndef GRAPH_H
#define GRAPH_H

#include <QMutex>
#include <QPainter>
#include <time.h>
#include <rollingqueue.h>

using namespace std;

#define arraySize(ar) ((sizeof(ar))/(sizeof(ar[0])))

enum GraphType { GRAPH_TYPE_LINE, GRAPH_TYPE_AREA };

class GraphRange
{
public:
    double Min;
    double Max;
    double Delta;
    GraphRange(double min, double max)
    {
        Min = min;
        Max = max;
        Delta = max - min;
    }
};

// Point
struct GraphPoint
{
public:
    double Time;
    double MinValue;
    double MaxValue;
    GraphPoint(double time, double minValue, double maxValue)
    {
        Time = time;
        MinValue = minValue;
        MaxValue = maxValue;
    }
};

class GraphDataSet
{
public:
    GraphDataSet(int timeStep, int maxCount);
    RollingQueue<GraphPoint> Points;
    int TimeStep;
    double LastTime;

    double MinValue, MaxValue, DeltaValue;
};

// Definitions

// Base Graph
class Graph
{
public:
    static int TimeSteps[];
    static int TimeStepCount;

    Graph(QString label, int maxCount, QColor color);

    void Lock();
    void Unlock();
    void AddReading(double value);

    vector<GraphDataSet*> DataSets;
    QString Label;
    bool Enabled;
    QColor Color;
    QBrush Brush;
    QPen Pen;
    int MinThickness;

protected:
    QMutex dataLock;
};

#endif // GRAPH_H
