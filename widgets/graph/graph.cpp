#include "graph.h"

int Graph::TimeSteps[] = {1, 5, 15, 30, 60, 15*60, 60*60, 12*60*60, 24*60*60};
int Graph::TimeStepCount = sizeof(TimeSteps)/sizeof(TimeSteps[0]);

// Data set
GraphDataSet::GraphDataSet(int timeStep, int maxCount)
{
    TimeStep = timeStep;
    Points = RollingQueue<GraphPoint>(maxCount);
    LastTime = 0;
}



// Area Graph
Graph::Graph(QString label, int maxCount, QColor color)
{
    Label = label;
    Color = color;
    Brush = QBrush(color);
    Pen = QPen(color);

    Enabled = true;
    MinThickness = 4;

    for (int i=0; i<TimeStepCount; i++)
        DataSets.push_back(new GraphDataSet(TimeSteps[i], maxCount));
}

void Graph::AddReading(double value)
{
    Lock();

    double now = time(NULL);

    for(int s=0; s<DataSets.size(); s++)
    {
        GraphDataSet* dataSet = DataSets[s];

        if (dataSet->Points.Count == 0 || now >= dataSet->LastTime + dataSet->TimeStep)
        {
            dataSet->Points.Add(GraphPoint(now, value, value));
            dataSet->LastTime = now;
        }
        else
        {
            GraphPoint* point = dataSet->Points.PeekNewest();
            if (value < point->MinValue) point->MinValue = value;
            if (value > point->MaxValue) point->MaxValue = value;
        }

        dataSet->MinValue = dataSet->MaxValue = value;
        for(int i=0; i<dataSet->Points.Count; i++)
        {
            GraphPoint* point = dataSet->Points.Get(i);
            if (point->MinValue < dataSet->MinValue) dataSet->MinValue = point->MinValue;
            if (point->MaxValue > dataSet->MaxValue) dataSet->MaxValue = point->MaxValue;
        }
        dataSet->DeltaValue = dataSet->MaxValue - dataSet->MinValue;
    }

    Unlock();
}

void Graph::Lock() { dataLock.lock(); }
void Graph::Unlock() { dataLock.unlock(); }
