#include "qwtgraphframe.h"
#include <QFile>
#include <app.h>
#include <qwt_plot_curve.h>
#include <qwt_point_data.h>

QwtGraphFrame::QwtGraphFrame(QWidget *parent) :
    QwtPlot(parent)
{
    QFile stylesheet("/home/pi/qt/SmartKegerator/qwtStyle.css");
    stylesheet.open(QFile::ReadOnly);
    QString styleString(stylesheet.readAll());
    this->setStyleSheet(styleString);

    curve = new QwtPlotCurve();

    //data = new QwtPointArrayData(8igv `);
    curve->setSamples(data);

    curve->attach(this);
}

void QwtGraphFrame::AddValueNow(double val)
{
    dataLock.lock();

    time_t seconds = time(NULL);
    //data += QPoint(seconds - App::StartTime, val);

    dataLock.unlock();
}
