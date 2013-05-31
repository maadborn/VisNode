/*
 * nodeitemdelegate.h
 *
 * The NodeItemDelegate is part of Qt's MVC pattern, representing the Delegate.
 * That means it paints the node graphically and calculates its geometric size.
 *
 * Mats Adborn, 2013-05-12
 */


#ifndef NODEITEMDELEGATE_H
#define NODEITEMDELEGATE_H

#include <QStyledItemDelegate>

class NodeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NodeItemDelegate(QObject* parent = 0);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;    
};

#endif // NODEITEMDELEGATE_H
