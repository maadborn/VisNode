
#include "nodeitemdelegate.h"
#include "nodeitem.h"
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QFontMetrics>
#include <QDebug>
#include <QColor>


NodeItemDelegate::NodeItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

// Define some constants for the painting of the items here
static const int BORDER_PADDING = 0;
static const int BORDER_WIDTH = 4;
static const int BORDER_CORNER_ROUNDING = 10;

/*
 *  Reimplemented paint from QStyledItemDelegate
 *  Very much a work in progress
 */
void NodeItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // Save painter state (according to docs.. purpose in this case?)
    painter->save();

    QString itemTitleText = index.data(Qt::DisplayRole).toString();

//    QList<QVariant> children = index.data(NodeItem::ChildrenRole).toList();
//    QPoint nodePoint = qvariant_cast<QPoint>(index.data(NodeItem::PositionRole));

    QPen oriPen = painter->pen();

//    for (int child = 0; child < children.size(); ++child) {
//        painter->drawLine(nodePoint, children.at(child).toPoint());
//    }

    // Draw a rectangle around the item
    QRect borderRect = option.rect;
    borderRect.adjust(-BORDER_PADDING, -BORDER_PADDING, BORDER_PADDING, BORDER_PADDING);

    QColor bgColor = index.data(NodeItem::ColorRole).value<QColor>();

    if (!bgColor.isValid())
        bgColor.setRgb(255,255,255);

    QPen borderPen(Qt::darkGreen, BORDER_WIDTH, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush borderBrush(bgColor);
    painter->setPen(borderPen);
    painter->setBrush(borderBrush);
    painter->drawRoundedRect(borderRect, BORDER_CORNER_ROUNDING, BORDER_CORNER_ROUNDING);

    // Draw the title text
    painter->setPen(oriPen);
    painter->drawText(option.rect, itemTitleText, QTextOption(Qt::AlignCenter));

    // Restore painter state (according to docs.. purpose in this case?)
    painter->restore();

    // TODO Lines to other nodes - here or in the view?
}

/*
 *  Returns a *hint* to the view of how big the item should be
 */
QSize NodeItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QFontMetrics fontMetrics = option.fontMetrics;

    // Return an arbitrary standard size of a NodeItem if the index is invalid
    if (!index.isValid())
        return QSize(fontMetrics.width("abcdef") + BORDER_PADDING*2 + BORDER_WIDTH*2, fontMetrics.height() + BORDER_PADDING*2 + BORDER_WIDTH*2);

    QString text = index.model()->data(index, Qt::DisplayRole).toString();

    int textWidth = fontMetrics.width(text);
    int textHeight = fontMetrics.height();

    // Return the size of the item, border width and paddin included
    return QSize(textWidth + BORDER_PADDING*2 + BORDER_WIDTH*2, textHeight + BORDER_PADDING*2 + BORDER_WIDTH*2);
}

