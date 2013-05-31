#include "nodeview.h"
#include "nodeitemmodel.h"
#include <QEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QPainter>
#include <QModelIndex>
#include <QDebug>
#include <qmath.h>


static const int MIN_INITIAL_WIDTH = 800;
static const int MIN_INITIAL_HEIGHT = 600;


NodeView::NodeView(const QSize& modelTargetSize, QWidget *parent)
    : QAbstractItemView(parent),
      modelTargetWidth(modelTargetSize.width()),
      modelTargetHeight(modelTargetSize.height()),
      mouseLBDown(false)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    resize(qMin(modelTargetWidth + verticalScrollBar()->width(), MIN_INITIAL_WIDTH),
           qMin(modelTargetHeight + horizontalScrollBar()->height(), MIN_INITIAL_HEIGHT));
}

/*
 *  Necessary implementation from QAbstractItemView
 *  Calculates the rectangle of the node in viewport coordinates
 */
QRect NodeView::visualRect(const QModelIndex &index) const
{
    if (!index.isValid())
        return QRect();

    return viewportRectForRow(index.row());
}


/*
 *  Necessary implementation from QAbstractItemView
 *  Determines where the item is compared to the viewport, and moves the viewport there
 */
void NodeView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    Q_UNUSED(hint);

    QRect viewRect = viewport()->rect();
    QRect itemRect = visualRect(index);

    // Item is left of viewport
    if (itemRect.left() < viewRect.left()) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + itemRect.left() - viewRect.left());
    }
    // Item is right of viewport
    else if (itemRect.right() > viewRect.right()) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() +
                                        qMin(itemRect.right() - viewRect.right(), itemRect.left() - viewRect.left()));
    }

    // Item is above the viewport
    if (itemRect.top() < viewRect.top()) {
        verticalScrollBar()->setValue(verticalScrollBar()->value() + itemRect.top() - viewRect.top());
    }
    // Item is below the viewport
    else if (itemRect.bottom() > viewRect.bottom()) {
        verticalScrollBar()->setValue(verticalScrollBar()->value() +
                                      qMin(itemRect.bottom() - viewRect.bottom(), itemRect.top() - viewRect.top()));
    }

    // Schedule a redraw of the viewport to match the changes just made
    viewport()->update();
}


/*
 *  Necessary implementation from QAbstractItemView
 *  Return the index of the item at the point argument
 */
QModelIndex NodeView::indexAt(const QPoint &point) const
{
    // Create a (mutable) point and give it view coordinates, not just the viewport's
    QPoint mutPoint(point);
    mutPoint.rx() += horizontalScrollBar()->value();
    mutPoint.ry() += verticalScrollBar()->value();

    QRect itemRect;

    for (int row = 0; row < model()->rowCount(); ++row) {
        itemRect = rectForRow(row);

        // If an item contains a point, return an index to it
        if (itemRect.contains(mutPoint)) {
            return model()->index(row, 0, rootIndex());
        }
    }

    // Return invalid index if no item was found at that point
    return QModelIndex();
}


/*
 *  Necessary implementation from QAbstractItemView
 *  Return the index of the next item when the user is pressing the keyboard arrow keys
 */
QModelIndex NodeView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);

    // Store the current index
    QModelIndex current = currentIndex();

    // Check which key was pressed
    switch (cursorAction) {
        case MoveRight:
        case MoveUp:
            // Increase the index in a circular manner
            if (current.row() < (model()->rowCount() - 1))
                current = model()->index(current.row() + 1, current.column(), rootIndex());
            else
                current = model()->index(0, current.column(), rootIndex());
            break;
        case MoveLeft:
        case MoveDown:
            // Decrease the index in a circular manner
            if (current.row() > 0)
                current = model()->index(current.row() - 1, current.column(), rootIndex());
            else
                current = model()->index(model()->rowCount() - 1, current.column(), rootIndex());
            break;
        default:
            break;
    }

    // Schedule an update of the viewport and return the index of the currently selected item
    viewport()->update();
    return current;
}


/*
 *  Necessary implementation from QAbstractItemView
 *  Returns the left horizontal distance the viewport is placed on the view underneath
 */
int NodeView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}


/*
 *  Necessary implementation from QAbstractItemView
 *  Returns the top vertictal distance the viewport is placed on the view underneath
 */
int NodeView::verticalOffset() const
{
    return verticalScrollBar()->value();
}


/*
 *  Necessary implementation from QAbstractItemView
 *  Returns false as we don't use any hiding in this view
 */
bool NodeView::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index);
    // TODO Perhaps implement this for the root item in an XML model? Or it's prob easier just to delete it..
    return false;
}


/*
 *  Necessary implementation from QAbstractItemView
 *  NO PURPOSE AT THIS STAGE OF DEVELOPMENT (?)
 */
void NodeView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    Q_UNUSED(rect);
    Q_UNUSED(command);
}


/*
 *  Necessary implementation from QAbstractItemView
 *  NO PURPOSE AT THIS STAGE OF DEVELOPMENT (?)
 */
QRegion NodeView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_UNUSED(selection);
    return QRegion();
}


/*
 *  Necessary implementation from QAbstractItemView
 *  Paints all items a their current position
 */
void NodeView::paintEvent(QPaintEvent *)
{
    QPainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

    QRect itemRect;
    QModelIndex itemIndex;
    QStyleOptionViewItem itemOption;

    // Loop through all items and paint them if they are inside the viewport
    for (int row = 0; row < model()->rowCount(); ++row) {
        itemRect = viewportRectForRow(row);

        // If the itemRect is outside the viewport, don't paint it. Continue to the next item.
        // Doesn't work very well with large models, commented out for the time being
//        if (itemRect.bottom() < 0 || itemRect.top() > viewport()->height() ||
//            itemRect.left() < 0 || itemRect.right() > viewport()->width())
//            continue;

        itemIndex = model()->index(row, 0, rootIndex());

        itemOption = viewOptions();
        itemOption.rect = itemRect;

        // Check the state of the model and send this along to the delegate to process accordingly
        if (selectionModel()->isSelected(itemIndex))
            itemOption.state |= QStyle::State_Selected;
        if (currentIndex() == itemIndex)
            itemOption.state |= QStyle::State_HasFocus;

        // Paint all of the nodes connection lines here, as the delegate won't know the viewport's location
        paintConnections(itemIndex, &painter);

        // Ask the delegate to paint the item!
        itemDelegate()->paint(&painter, itemOption, itemIndex);
    }
}


/*
 *  Calculate the rectangle for the row (i.e. the node), in view coordinates
 */
QRect NodeView::rectForRow(int row) const
{
    QModelIndex itemIndex = model()->index(row, 0, rootIndex());
    QPoint itemCenter = model()->data(itemIndex, NodeItem::PositionRole).toPoint();
    QSize itemSize = itemDelegate()->sizeHint(viewOptions(), itemIndex);
    QRect itemRect(itemCenter.x() - itemSize.width() / 2,
                   itemCenter.y() - itemSize.height() / 2,
                   itemSize.width(),
                   itemSize.height());

    return itemRect;
}


/*
 *  Calculate the rectangle for the row (i.e. the node), in viewport coordinates
 */
QRect NodeView::viewportRectForRow(int row) const
{
    QRect itemRect = rectForRow(row);

    QPoint itemTopLeft = itemRect.topLeft();
    itemTopLeft.rx() -= horizontalScrollBar()->value();
    itemTopLeft.ry() -= verticalScrollBar()->value();
    itemRect.setTopLeft(itemTopLeft);

    QPoint itemBottomRight = itemRect.bottomRight();
    itemBottomRight.rx() -= horizontalScrollBar()->value();
    itemBottomRight.ry() -= verticalScrollBar()->value();
    itemRect.setBottomRight(itemBottomRight);

    return itemRect;
}


/*
 *  Draws the connections between nodes.
 *  This is essentially code that should be placed the delegate's paint(), but
 *  as the delegate doesn't know where the viewport is located, it had to be
 *  moved to the view instead.
 */
void NodeView::paintConnections(const QModelIndex &index, QPainter *painter) const
{
    // Get the node's and all its children's center points
    QPoint nodePoint = viewportRectForRow(index.row()).center();
    QList<QVariant> children = index.data(NodeItem::ChildrenRole).toList();

    QPoint childPoint;

    // Loop through all children and draw the connections
    for (int child = 0; child < children.size(); ++child) {
        childPoint = children.at(child).toPoint();
        childPoint.rx() -= horizontalScrollBar()->value();      // Compensate for viewport position
        childPoint.ry() -= verticalScrollBar()->value();
        drawArrowToEdge(painter, nodePoint, childPoint);
    }
}


/*
 *  Calculates how the arrow and arrow head must be positioned to go from the center of the start node
 *  to the edge of the end node, and then draws the whole shebang!
 */
void NodeView::drawArrowToEdge(QPainter* painter, const QPoint &start, const QPoint &end) const
{
    qreal xLength = end.x() - start.x();
    qreal yLength = end.y() - start.y();

    // This gives the angle the line goes out of the start point
    qreal angleAtStart = atan2(yLength, xLength);

    // Decide on the relative positions of the two points
    const bool startAboveEnd = start.y() < end.y() ? true : false;
    const bool startRightOfEnd = start.x() > end.x() ? true : false;

    // Make sure the start angle is positive (it should be after last step!) [Debugging]
    while (angleAtStart < 0.0)
        angleAtStart += (2 * M_PI);

    // The angle the line comes in at the end point
    qreal angleAtEnd = angleAtStart - M_PI;

    // Make sure the end angle is positive, which is needed to easily compare it to constant angles (like pi radiand = 180 degrees)
    while (angleAtEnd < 0.0)
        angleAtEnd += (2 * M_PI);

    // Determine which node corresponds to the coordinates of the end point, and get its size
    QModelIndex endIndex = indexAt(end);
    QSize endSize = itemDelegate()->sizeHint(viewOptions(), endIndex);

    // Determine the angle from the end node's center to its upper right corner
    qreal angleFromCenterToCorner = qAtan(static_cast<qreal>(endSize.height()) / static_cast<qreal>(endSize.width()));

    // Save some variables with the values with which we will later modify the end point of the arrow
    qreal endXMod, endYMod;

    // If the line at the end point hits the upper or lower edge of the end node
    if ((angleAtEnd > angleFromCenterToCorner && angleAtEnd < (M_PI - angleFromCenterToCorner)) ||
            (angleAtEnd > (M_PI + angleFromCenterToCorner) && angleAtEnd < (2*M_PI - angleFromCenterToCorner))) {
        // Since the line came in from the above or below, the y modification will always be half of the node's height
        endYMod = endSize.height() / 2.0;           // Always positive
        endXMod = endYMod / qTan(angleAtEnd);       // tan(angleAtEnd) = endYMod / endXMod <=> ...
        endXMod = qAbs(endXMod);                    // Make sure it's positive

    }
    // Else, the line will hit the left or right edge of the end node
    else {
        // Since the line came in from the side, the x modification will always be half of the node's width
        endXMod = endSize.width() / 2.0;            // Always positive
        endYMod = endXMod * qTan(angleAtEnd);       // tan(angleAtEnd) = endYMod / endXMod <=> ...
        endYMod = qAbs(endYMod);                    // Make sure it's positive
    }

    // As the parameter is const, make a copy that isn't
    QPoint modifiedEnd = end;

    // Modifiy the end point with the values we just found, according to its relative position to the start point
    if (startAboveEnd && startRightOfEnd) {
        modifiedEnd.rx() += endXMod;
        modifiedEnd.ry() -= endYMod;
    }
    else if (startAboveEnd && !startRightOfEnd) {
        modifiedEnd.rx() -= endXMod;
        modifiedEnd.ry() -= endYMod;
    }
    else if (!startAboveEnd && !startRightOfEnd) {
        modifiedEnd.rx() -= endXMod;
        modifiedEnd.ry() += endYMod;
    }
    else {
        modifiedEnd.rx() += endXMod;
        modifiedEnd.ry() += endYMod;
    }

    // Create a QPointF array with the arrow tip polygon form
    // The head of the arrow tip is centered at the origin making it easy to rotate to fit the right angle
    // This polygon makes the arrow head point at an angle of (180-45=) 135 degrees, or 3/4 PI radians
    const int NUM_ARROW_POINTS = 4;
    static const QPointF arrowPoints[NUM_ARROW_POINTS] = {
        QPointF(0,0),
        QPointF(5, 15),
        QPointF(8, 8),
        QPointF(15, 5)
    };

    // Save the painter so we can restore the changes about to be made later
    painter->save();

    // Save a constant with the angle needed to rotate the arrow pointing at zero angle (3 o'clock)
    // Use this value and the angle at the start point to calculate how the arrow head needs to
    // be rotated to point in the same way as the line. Note that rotate() does this in a
    // _clockwise_ manner, so the start angle is converted to [2*PI - itself] first
    const qreal arrowZeroedRotationRads = (3 * M_PI_4);
    qreal rotateArrowheadDegrees = ((2 * M_PI) + angleAtStart + arrowZeroedRotationRads) * (180 / M_PI);

    // Make sure we use an angle of 0..360 degrees (just for convinience of debugging)
    //    while (rotateArrowheadDegrees > 360)
    //        rotateArrowheadDegrees -= 360;

    // Next few lines draws the line; and translates, rotates and draws the arrow head
    painter->drawLine(start, modifiedEnd);

    painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));

    painter->translate(modifiedEnd.x(), modifiedEnd.y());
    painter->rotate(rotateArrowheadDegrees);

    painter->drawPolygon(arrowPoints, NUM_ARROW_POINTS);

    painter->restore();
}


/*
 *  Override of QAbstractScrollArea::resizeEvent() to update the scrollbars as well
 */
void NodeView::resizeEvent(QResizeEvent *event)
{
    QAbstractScrollArea::resizeEvent(event);
    updateGeometries();
}


/*
 *  Override of QAbstractScrollArea::updateGeometries() that updates the scrollbars
 */
void NodeView::updateGeometries()
{
    QFontMetrics fm(font());
    int rowHeight = fm.height();

    horizontalScrollBar()->setSingleStep(fm.width("n"));
    horizontalScrollBar()->setPageStep(viewport()->width());
    horizontalScrollBar()->setRange(0, qMax(0, modelTargetWidth - viewport()->width()));

    verticalScrollBar()->setSingleStep(rowHeight);
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(0, qMax(0, modelTargetHeight - viewport()->height()));
}

/*
 *  mousePressEvent
 */
void NodeView::mousePressEvent(QMouseEvent *event)
{
    // If the left mouse button is pressed down
    if (event->button() == Qt::LeftButton) {
        mouseLBDown = true;
        mouseLBDownOrigin = event->pos();

        // Get the index of the item that is pressed
        dragIndex = indexAt(mouseLBDownOrigin);

        if (dragIndex.isValid())
            // Nothing here, yet
            ;
        else {
            // If the LMB is pressed down outside a node, get the scrollbar values to update the scroll drag correctly
            mouseLBDownHScrollOrigin = horizontalScrollBar()->value();
            mouseLBDownVScrollOrigin = verticalScrollBar()->value();
        }
    }
}


/*
 *  mouseMoveEvent
 */
void NodeView::mouseMoveEvent(QMouseEvent *event)
{
    // Make sure the left mouse button is pressed down
    if (mouseLBDown) {
        int xDist = event->pos().x() - mouseLBDownOrigin.x();
        int yDist = event->pos().y() - mouseLBDownOrigin.y();

        // If the dragIndex is valid, it's an item drag that's occuring
        if (dragIndex.isValid()) {
            // Modify the event's position with the viewport's coords
            QPoint eventPos = event->pos();
            eventPos.rx() += horizontalScrollBar()->value();
            eventPos.ry() += verticalScrollBar()->value();

            // Update the item and schedule a graphical update
            model()->setData(dragIndex, eventPos, NodeItem::PositionRole);
            viewport()->update();
        }
        else {
            // Move viewport
            horizontalScrollBar()->setValue(mouseLBDownHScrollOrigin - xDist);
            verticalScrollBar()->setValue(mouseLBDownVScrollOrigin - yDist);
        }
    }
}

/*
 *  mousePressEvent
 */
void NodeView::mouseReleaseEvent(QMouseEvent *event)
{
    // Make sure it's the left button that's released, and then unset the data members involved
    if (event->button() == Qt::LeftButton) {
        mouseLBDown = false;
        dragIndex = QModelIndex();
    }
}
