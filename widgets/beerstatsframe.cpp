#include "widgets/beerstatsframe.h"
#include <QFont>
#include <QMouseEvent>
#include <data/user.h>
#include <data/pour.h>
#include <data/constants.h>

BeerStatsFrame::BeerStatsFrame(QWidget *parent) :
    QFrame(parent)
{
    setFrameShape(QFrame::NoFrame);

    panels = 3;
    padding = 10;
    extraPadding = 2;
    xOffset = 0;
    currentPanel = 0;

    dragging = false;

    dowNames.push_back(QString("sun"));
    dowNames.push_back(QString("mon"));
    dowNames.push_back(QString("tue"));
    dowNames.push_back(QString("wed"));
    dowNames.push_back(QString("thur"));
    dowNames.push_back(QString("fri"));
    dowNames.push_back(QString("sat"));

    headerHeight = 20;
    footerHeight = 10;

    CurrentKeg = NULL;

    tileBrush = QBrush(QColor(150,150,150,150));
    barBrush = QBrush(QColor(0, 183, 205, 150));
    textPen = QPen(QColor(200,200,200,150));

}

void BeerStatsFrame::SetKeg(Keg *keg)
{
    CurrentKeg = keg;
    update();
}

int dragThreshold = 10;
double velocity = 0;

void BeerStatsFrame::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() != Qt::LeftButton)
        return;
    dragStart = ev->pos();
    dragStart.setX(dragStart.x() - xOffset);
    dragging = false;
    velocity = 0;
    ev->accept();
}

void BeerStatsFrame::mouseMoveEvent(QMouseEvent *ev)
{
    ev->accept();
    int delta = dragStart.x() - ev->pos().x();
    if (abs(delta) > dragThreshold)
        dragging = true;
    if (dragging == false)
        return;
    velocity = (4.0*velocity + ((ev->pos().x() - dragStart.x()) - xOffset))/5.0; // this doesnt actually work since move events only happen on moves
    xOffset = ev->pos().x() - dragStart.x();
    update();
}

void BeerStatsFrame::mouseReleaseEvent(QMouseEvent *ev)
{
    ev->accept();
    if (dragging == false)
    {
        currentPanel = (currentPanel + 1) % panels;
    }
    else
    {
        velocity = (9.0*velocity + ((ev->pos().x() - dragStart.x()) - xOffset))/10.0; // this doesnt actually work since move events only happen on moves
        xOffset = ev->pos().x() - dragStart.x();
        int dest = xOffset + 10*velocity - 0.5*frameWidth;
        currentPanel = max(0, min(panels-1, -dest/frameWidth));
    }

    dragging = false;
    update();
}

void BeerStatsFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    frameWidth = width();
    frameHeight = height();

    if (CurrentKeg == NULL)
    {
        drawNone(painter, QString("empty"));
        return;
    }

    drawFooter(painter);

    int panel = -xOffset / frameWidth;
    int offset = (int)xOffset % frameWidth;

    drawContent(painter, panel, offset);
    if (offset != 0)
        drawContent(painter, panel + 1, offset + frameWidth);

    int panelX = currentPanel * -frameWidth;

    if (dragging == false && xOffset != panelX)
    {
        xOffset = (9*xOffset + panelX)/10.0;
        if ((int)(xOffset-0.5) == panelX)
            xOffset = panelX;
        update();
    }
}

void BeerStatsFrame::drawContent(QPainter &painter, int panel, int offset)
{
    vector<QStringDoublePair> pairs;

    switch(panel)
    {
        case 0: // today's pours
        {
            drawHeader(painter, QString("today's pours"), offset);

            map<User*, int> userPours;
            vector<Pour*> pours = Pour::PoursByKegId[CurrentKeg->Id];
            double now = time(NULL);
            for (int i=0; i<pours.size(); i++)
            {
                Pour* pour = pours[i];
                if (now - pour->Time >= 0.5 * Constants::Day)
                    continue;
                User* user = User::UsersById[pour->UserId];
                userPours[user]++;
            }

            map<User*, int>::iterator iter;
            for (iter = userPours.begin(); iter != userPours.end(); iter++)
                pairs.push_back(QStringDoublePair(QString(iter->first->Name.c_str()),iter->second));


            if (pairs.size() == 0)
                drawNone(painter, QString("none"), offset);
            else
                drawTiles(painter, pairs, offset);
        }
        break;
        case 1: //pours by day
        {
            drawHeader(painter, QString("pours by day"), offset);

            // DOW of week pours
            vector<int> dowPours = Pour::GetPoursByDow(CurrentKeg);
            //int dowPours[] = {22, 5, 12, 13, 11, 24, 20};
            int maxDowPours = 0;
            for (int i=0; i<7; i++)
                maxDowPours = max(dowPours[i], maxDowPours);

            for (int i=0; i<7; i++)
                pairs.push_back(QStringDoublePair(dowNames[i],dowPours[i]/(double)maxDowPours));

            if (maxDowPours == 0)
                drawNone(painter, QString("none"), offset);

            drawBars(painter, pairs, offset);
        }
        break;
        case 2: // pours by user
        {
            drawHeader(painter, QString("pours by user"), offset);

            if (CurrentKeg == NULL)
            {
                drawNone(painter, QString("none"), offset);
                return;
            }

            map<User*, int> userPours = Pour::GetPoursByUser(CurrentKeg);
            int maxPours = 0;
            map<User*, int>::iterator iter;
            for (iter = userPours.begin(); iter != userPours.end(); iter++)
                maxPours = max(maxPours, iter->second);

            for (iter = userPours.begin(); iter != userPours.end(); iter++)
                pairs.push_back(QStringDoublePair(QString(iter->first->Name.c_str()),iter->second/(double)maxPours));

            if (maxPours == 0)
                drawNone(painter, QString("none"), offset);
            else
                drawBars(painter, pairs, offset);
        }
        break;
    }
}

void BeerStatsFrame::drawNone(QPainter &painter, QString str, int offset)
{
    painter.setPen(QPen(QColor(200,200,200,100)));
    painter.setFont(QFont("Sans Serif", 22));
    painter.drawText(QRect(offset + padding, padding, frameWidth - 2 * padding, frameHeight - 2 * padding), Qt::AlignCenter, str);
}

void BeerStatsFrame::drawBars(QPainter &painter, vector<QStringDoublePair> vals, int offset)
{
    double maxBarWidth = 50;
    int w = frameWidth - 2 * padding;
    double barSpacing = min(maxBarWidth, w/(double)vals.size());
    int barWidth = barSpacing - 2 * extraPadding;
    double x = 0.5 * (frameWidth - vals.size() * barSpacing) + extraPadding + offset;
    int barHeight = frameHeight - footerHeight - headerHeight - 2 * padding;

    painter.setPen(QPen(Qt::transparent));
    painter.setBrush(barBrush);
    painter.setFont(QFont("Sans Serif", 7));

    for (int i=0; i<vals.size(); i++)
    {
        QStringDoublePair pair = vals[i];
        int h = barHeight * pair.Value;
        painter.drawRect(x + i * barSpacing, headerHeight + barHeight + extraPadding, barWidth, -h);
    }

    painter.setPen(textPen);
    for (int i=0; i<vals.size(); i++)
    {
        QStringDoublePair pair = vals[i];
        painter.drawText(QRect(x + i * barSpacing, headerHeight + barHeight + extraPadding, barWidth, headerHeight), Qt::AlignCenter, pair.Label);
    }
}

void BeerStatsFrame::drawTiles(QPainter &painter, vector<QStringDoublePair> vals, int offset)
{
    int tileHeight = frameHeight - footerHeight - headerHeight - padding;
    int w = frameWidth - 2 * padding;
    double tileSpacing = min((double)tileHeight + extraPadding, w/(double)vals.size());
    int tileWidth = tileSpacing - 2 * extraPadding;
    double x = padding + extraPadding + offset;

    painter.setPen(QPen(Qt::transparent));
    painter.setBrush(tileBrush);

    for (int i=0; i<vals.size(); i++)
        painter.drawRect(x + i * tileSpacing, headerHeight + extraPadding, tileWidth, tileHeight);

    int nameHeight = 14;

    painter.setPen(textPen);
    for (int i=0; i<vals.size(); i++)
    {
        QStringDoublePair pair = vals[i];
        painter.setFont(QFont("Sans Serif", 32));
        painter.drawText(QRect(x + i * tileSpacing, headerHeight + extraPadding, tileWidth, tileHeight - nameHeight), Qt::AlignCenter, QString::number((int)pair.Value));
        painter.setFont(QFont("Sans Serif", 8));
        painter.drawText(QRect(x + i * tileSpacing, headerHeight + tileHeight - nameHeight, tileWidth, nameHeight), Qt::AlignCenter, pair.Label);
    }
}

void BeerStatsFrame::drawHeader(QPainter &painter, QString text, int offset)
{
    painter.setPen(textPen);
    painter.setFont(QFont("Sans Serif", 10));
    painter.drawText(QRect(offset, 0, frameWidth, headerHeight), Qt::AlignCenter, text);
}

void BeerStatsFrame::drawFooter(QPainter &painter)
{
    painter.setPen(QPen(Qt::transparent));

    int radius = 3;
    int spacing = 9;

    int x = 0.5 * frameWidth - (0.5 * panels) * spacing;
    int y = frameHeight - 0.5 * footerHeight - extraPadding;

    for (int i=0; i<panels; i++)
    {
        if (i == currentPanel)
            painter.setBrush(QBrush(QColor(200,200,200,200)));
        else
           painter.setBrush(QBrush(QColor(200,200,200,100)));
        painter.drawEllipse(QPoint(x + i*spacing, y), radius, radius);
    }
}
