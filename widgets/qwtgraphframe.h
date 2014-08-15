#ifndef QWTGRAPHFRAME_H
#define QWTGRAPHFRAME_H

#include <QFrame>
#include <QMutex>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_point_data.h>

class QwtGraphFrame : public QwtPlot
{
    Q_OBJECT
public:
    explicit QwtGraphFrame(QWidget *parent = 0);

    void AddValueNow(double val);


signals:
    
public slots:

private:
    QwtPointArrayData *data;
    QwtPlotCurve *curve;
    QMutex dataLock;
    
};

#endif // QWTGRAPHFRAME_H
