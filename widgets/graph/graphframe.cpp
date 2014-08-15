#include <widgets/graph/graphframe.h>
#include <QPainter>
#include <deque>
#include <QMutex>
#include <time.h>
#include <cmath>


using namespace std;

int pixelsPerTimeStep = 5;
int bottomHeight = 12;
double steps[] = {0.125, 0.25, 0.5, 1, 5, 10, 20};

GraphFrame::GraphFrame(QWidget *parent) :
    QFrame(parent)
{
    setFrameShape(QFrame::NoFrame);

    PaintEnabled = true;

    TimeStepIndex = 1;
    paddingY = 0.1666;
    lastUpdate = 0;

    textPen = QPen(QColor(170,170,170, 150), 1, Qt::SolidLine);
    gridPen = QPen(QColor(150,150,150,70), 1, Qt::SolidLine);
}

void GraphFrame::AddGraph(Graph* graph)
{
    Graphs.push_back(graph);
    update();
}

bool dragging = false;
int dragStepThreshold = 75;
int clickThreshold = 25;
int startingIndex;

int graphIndex = 0;

void GraphFrame::ShowGraph(Graph *graph)
{
    for (int i=0; i<Graphs.size(); i++)
    {
        Graphs[i]->Enabled = Graphs[i] == graph;
        if (Graphs[i]->Enabled)
            graphIndex = i+1;
    }
    update();
}

void GraphFrame::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() != Qt::LeftButton)
        return;
    dragStart = ev->pos();
    dragging = false;
    ev->accept();
}

void GraphFrame::mouseMoveEvent(QMouseEvent *ev)
{
    //qDebug("moved %d, %d", ev->pos().x() - dragStart.x(), ev->pos().y() - dragStart.y());
    ev->accept();
    QPoint delta = QPoint(ev->pos().x() - dragStart.x(), dragStart.y() - ev->pos().y());
    if (dragging == false && abs(delta.x()) > dragStepThreshold)
    {
        dragging = true;
        startingIndex = TimeStepIndex;
    }
    if (dragging == false)
        return;
    int lastIndex = TimeStepIndex;
    int index = startingIndex + floor((delta.x()) / dragStepThreshold);
    TimeStepIndex = max(0, min(index, Graph::TimeStepCount-1));
    if (TimeStepIndex != lastIndex)
        update();
}

void GraphFrame::mouseReleaseEvent(QMouseEvent *ev)
{
    ev->accept();
    if (dragging == false)
    {
        int graphs = Graphs.size();
        graphIndex = (graphIndex + 1) % (graphs + 1);
        for (int i=0; i<graphs; i++)
            Graphs[i]->Enabled = graphIndex == (i+1) || graphIndex == 0;
        update();
    }
    dragging = false;
}

void GraphFrame::paintEvent(QPaintEvent *event)
{
    if (this->PaintEnabled == false)
        return;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setFont(QFont("Sans Serif", 8));

    frameWidth = this->width();
    frameHeight = this->height() - bottomHeight;

    updateMinMax();
    drawGrid(painter);

    drawGraphs(painter);
    drawGraphDetails(painter);

    painter.setPen(textPen);
    painter.drawText(5,frameHeight-5,QString("%1s").arg(Graph::TimeSteps[TimeStepIndex]));
}


int GraphFrame::getPixelX(double val) { return (int)(frameWidth - pixelsPerTimeStep * ((double)time(NULL) - val)/(double)Graph::TimeSteps[TimeStepIndex]); }
int GraphFrame::getPixelY(double val) { return (int)(frameHeight * (val - frameMinValue) / frameDeltaValue); }

void GraphFrame::drawGraphs(QPainter &painter)
{
    for(int i=0; i<Graphs.size(); i++)
    {
        Graph* graph = Graphs[i];
        if (graph->Enabled == false)
            continue;

        graph->Lock();

        GraphDataSet* dataSet = graph->DataSets[TimeStepIndex];
        vector<QPoint> topPoints;
        vector<QPoint> bottomPoints;
        QPolygon polygon;
        int pixelMinY, pixelMaxY;

        painter.setPen(graph->Pen);
        painter.setBrush(graph->Brush);

        for(int i=0; i < dataSet->Points.Count; i++)
        {
            GraphPoint* point = dataSet->Points.Get(i);
            int pixelX = getPixelX(point->Time);

            pixelMinY = getPixelY(point->MinValue);
            pixelMaxY = getPixelY(point->MaxValue);

            if (pixelMaxY - pixelMinY < graph->MinThickness)
            {
                pixelMaxY += 0.5 * graph->MinThickness;
                pixelMinY -= 0.5 * graph->MinThickness;
            }

            // Draws only one point where pixelX is less than 0 (so lines continue off screen nicely)
            if (pixelX < 0 && topPoints.size() > 0)
            {
                topPoints[0] = QPoint(pixelX, frameHeight - pixelMaxY);
                bottomPoints[0] = QPoint(pixelX, frameHeight - pixelMinY);
            }
            else
            {
                topPoints.push_back(QPoint(pixelX, frameHeight - pixelMaxY));
                bottomPoints.push_back(QPoint(pixelX, frameHeight - pixelMinY));
            }
        }
        // Draw it to edge
        topPoints.push_back(QPoint(frameWidth, frameHeight - pixelMaxY));
        bottomPoints.push_back(QPoint(frameWidth, frameHeight - pixelMinY));

        for (int i=0; i<topPoints.size(); i++)
            polygon << topPoints[i];
        for (int i=bottomPoints.size()-1; i>=0; i--)
            polygon << bottomPoints[i];
        painter.drawPolygon(polygon);

        graph->Unlock();
    }
}


void GraphFrame::drawGraphDetails(QPainter &painter)
{
    int w = 100;
    painter.setPen(QPen(Qt::transparent));
    for(int i=0; i<Graphs.size(); i++)
    {
        Graph* graph = Graphs[i];
        painter.setPen(QPen(Qt::transparent));
        painter.setBrush(QBrush(QColor(200,200,200,50)));
        int y = 5+(i*20);
        painter.drawRect(5,y, w, 18);
        if (graph->Enabled)
            painter.setBrush(QBrush(graph->Color));
        else
            painter.setBrush(QBrush(QColor(200,200,200,25)));
        painter.drawRect(9, y+4, 10, 10);
        painter.setPen(textPen);
        painter.drawText(22, y+3, w, 15, 0, graph->Label);
    }
}

double stepDevisions = 5.0;

void GraphFrame::updateMinMax()
{
    if (Graphs.size() > 0)
    {
        graphMinValue = 99999999;
        graphMaxValue = -99999999;

        for(int i=0; i<Graphs.size(); i++)
        {
            GraphDataSet* dataSet = Graphs[i]->DataSets[TimeStepIndex];
            if (Graphs[i]->Enabled == false)
                continue;
            if (dataSet->MinValue < graphMinValue) graphMinValue = dataSet->MinValue;
            if (dataSet->MaxValue > graphMaxValue) graphMaxValue = dataSet->MaxValue;
        }
        graphDeltaValue = graphMaxValue - graphMinValue;
        if (graphDeltaValue == 0)
        {
            double delta = 0.5;
            double center = graphMinValue + 0.5 * graphDeltaValue;
            graphMinValue = center - 0.5 * delta;
            graphMaxValue = center + 0.5 * delta;
            graphDeltaValue = graphMaxValue - graphMinValue;
        }

        frameMaxValue = graphMaxValue + graphDeltaValue * paddingY;
        frameMinValue = graphMinValue - graphDeltaValue * paddingY;
        frameDeltaValue = frameMaxValue - frameMinValue;

        stepY = frameDeltaValue / stepDevisions;
        for (int i=0; i<arraySize(steps); i++)
        {
            if (stepY < steps[i])
            {
                stepY = steps[i];
                break;
            }
        }
    }
}

void GraphFrame::drawVerticalGridLine(double time, QPen pen, bool drawText, QPainter &painter)
{
    int x = getPixelX(time);
    painter.setPen(pen);
    painter.drawLine(x,0,x,frameHeight + bottomHeight);

    if (drawText)
    {
        painter.setPen(textPen);

        time_t timeObj = time;
        //time(&timeObj);
        tm *pTime = localtime(&timeObj);

        QString mins = QString(pTime->tm_min < 10 ? "0%1" : "%1").arg(pTime->tm_min);
        QString secs = QString(pTime->tm_sec < 10 ? "0%1" : "%1").arg(pTime->tm_sec);
        QString amPm = QString(pTime->tm_hour < 12 ? "am" : "pm");
        QString label;//= QString("%1:%2:%3").arg((pTime->tm_hour%12) == 0 ? 12 : (pTime->tm_hour%12)).arg(mins).arg(secs);

        int step = Graph::TimeSteps[TimeStepIndex];
        if (step < 60)
            label = QString("%1:%2:%3").arg((pTime->tm_hour%12) == 0 ? 12 : (pTime->tm_hour%12)).arg(mins).arg(secs);
        else if (step < 30*60)
            label = QString("%1:%2%3").arg((pTime->tm_hour%12) == 0 ? 12 : (pTime->tm_hour%12)).arg(mins).arg(amPm);
        else// if (step < 60*60*24)
            label = QString("%1/%2 %3:%4%5").arg(pTime->tm_mon + 1).arg(pTime->tm_mday).arg((pTime->tm_hour%12) == 0 ? 12 : (pTime->tm_hour%12)).arg(mins).arg(amPm);

        //ui->uptimeField->setText(QString("%1/%2/%3 %4:%5")
        //                         .arg(pTime->tm_mon + 1).arg(pTime->tm_mday).arg(pTime->tm_year - 100)
        //                         .arg((pTime->tm_hour%12) == 0 ? 12 : (pTime->tm_hour%12)).arg(mins));

        painter.drawText(x, frameHeight + bottomHeight - 2, label);
    }
}

void GraphFrame::drawHorizontalGridLine(double val, QPen pen, int decimals, QPainter &painter)
{
    int y = frameHeight - getPixelY(val);
    painter.setPen(pen);
    painter.drawLine(0,y,frameWidth, y);

    if (decimals > -1)
    {
        painter.setPen(textPen);
        QString str = QString::number(val, 'f', decimals);
        int offsets[] = {20, 30, 40};
        painter.drawText(frameWidth - offsets[decimals], y-2, str);
    }
}

void GraphFrame::drawGrid(QPainter &painter)
{
    double now = time(NULL);
    int frameDeltaTime = (frameWidth / (double)pixelsPerTimeStep) * Graph::TimeSteps[TimeStepIndex];
    double minTime = now - frameDeltaTime;
    double stepTime = 20 * Graph::TimeSteps[TimeStepIndex];
    minTime = floor(minTime / stepTime) * stepTime;
    double minY = floor(frameMinValue / stepY) * stepY;
    int decimals = stepY < 0.5 ? 2 : (stepY < 1 ? 1 : 0);

    // Horizontal lines
    for (double y=minY; y<=frameMaxValue; y+=stepY)
        drawHorizontalGridLine(y, gridPen, decimals, painter);

    // Vertical
    painter.setBrush(QBrush(QColor(18,18,18)));
    painter.setPen(QPen(Qt::transparent));
    painter.drawRect(0, frameHeight, frameWidth, bottomHeight);
    for (double time=minTime; time<=now; time+=stepTime)
        drawVerticalGridLine(time, gridPen, true, painter);
}
