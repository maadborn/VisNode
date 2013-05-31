#include "abstractnodeitempositioncalc.h"
#include "nodeitem.h"

/*
 *  Constructor
 */
AbstractNodeItemPositionCalc::AbstractNodeItemPositionCalc(QList<NodeItem*>& nodelist)
    : _nodelist(nodelist), _centerPoint(QPoint(0,0))
{

}

/*
 *  Moves all node positions to center around a new point.
 *
 *  Call this function to move the nodes into a desired space. If the size of the
 *  new area differs from the old, use scaleTo() too. The order of these calls
 *  is arbitrary.
 *
 *  This function is called inside scaleTo(), but all members it changes in there
 *  are turned back to their original state afterwards, making the move temporary.
 */
void AbstractNodeItemPositionCalc::moveInto(const QPoint &newCenterPoint)
{
    if (_centerPoint == newCenterPoint)         // Save some cpu cycles if nothing is to be changed
        return;
                                                // Create a point with the difference of the new and old center
    QPoint diffCenterPoint(newCenterPoint.x()-_centerPoint.x(), newCenterPoint.y()-_centerPoint.y());

    QPoint temppoint;

    for (int i = 0; i < _nodelist.size(); ++i) {    // Move all points to their new position
        temppoint = _nodelist[i]->position();

        temppoint.rx() += diffCenterPoint.x();
        temppoint.ry() += diffCenterPoint.y();

        _nodelist[i]->setPosition(temppoint);
    }

    _centerPoint = newCenterPoint;              // Save the new center point
}

/*
 *  Scales all current node positions to the QSize passed as argument.
 *  This is done by first moving the whole set to center around origin (0,0)
 *  to have a known, constant centerpoint to scale around. Here all coordinates
 *  are multiplied with the relative factor of the current and the new size.
 *  And finally the coordinates are moved back to center around the last centerpoint.
 */
void AbstractNodeItemPositionCalc::scaleTo(const QSize &sizeToFit)
{
    if (_currentSize == sizeToFit)              // Save some cpu cycles if nothing is to be changed
        return;

    QPoint originalCenterPoint = _centerPoint;

    moveInto(QPoint(0,0));                      // Move to origin

    qreal xRatio = sizeToFit.width() / static_cast<qreal>(_currentSize.width());
    qreal yRatio = sizeToFit.height() / static_cast<qreal>(_currentSize.height());

    QPoint temppoint;

    for (int i = 0; i < _nodelist.size(); ++i) {    // Scale all coords
        temppoint = _nodelist[i]->position();

        temppoint.rx() *= xRatio;
        temppoint.ry() *= yRatio;

        _nodelist[i]->setPosition(temppoint);
    }

    moveInto(originalCenterPoint);              // Move all coords back
    _currentSize = sizeToFit;                   // Update the current size
}

const QSize &AbstractNodeItemPositionCalc::modelGeometricSize() const
{
    return _currentSize;
}




