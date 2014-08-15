#ifndef SHARED_H
#define SHARED_H

#include <QApplication>
#include <QStyledItemDelegate>


// Just to make the child height
class CustomComboBoxItem : public QStyledItemDelegate
{
public:
    CustomComboBoxItem(QObject* parent=0, int height=34) : QStyledItemDelegate(parent) { this->size = QSize(100, height); }
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const { return this->size; }
private:
    QSize size;
};


#endif // SHARED_H
