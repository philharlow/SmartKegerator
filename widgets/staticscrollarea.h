#ifndef STATICSCROLLAREA_H
#define STATICSCROLLAREA_H

#include <QFrame>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QScrollBar>

using namespace std;

class StaticScrollArea : public QFrame
{
    Q_OBJECT
    
public:
    explicit StaticScrollArea(QWidget *parent = 0);

    void SetNumButtons(int buttonCount);
    void SetSelectedIndex(int index);
    void SetOptions(vector<string> &options);

signals:
    void SelectionChanged(int index);

private slots:
    void itemClicked();
    void scrolled(int);

private:
    QScrollBar* scrollbar;
    QVBoxLayout* buttonLayout;
    vector<QPushButton*> buttons;
    vector<string> options;
    QString style;
    int currentOffset;
    int selectedIndex;

    void updateButtons();

};

#endif // STATICSCROLLAREA_H
