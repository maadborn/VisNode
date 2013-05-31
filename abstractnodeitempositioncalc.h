/*
 * nodeitempositioncalculator.h
 *
 * This class is the abstract base class for calculating the position of the nodes
 * to be represented graphically in the view.
 * It is a part of the NodeItemModel.
 *
 * Mats Adborn, 2013-05-12
 */

#ifndef ABSTRACTNODEITEMPOSITIONCALC_H
#define ABSTRACTNODEITEMPOSITIONCALC_H

#include <QList>
#include <QPoint>
#include <QSize>

class NodeItem;

class AbstractNodeItemPositionCalc
{
public:
    AbstractNodeItemPositionCalc(QList<NodeItem*>& nodelist);
    virtual ~AbstractNodeItemPositionCalc() {}

    virtual void calculate() = 0;
    void moveInto(const QPoint& newCenterPoint);
    void scaleTo(const QSize& sizeToFit);
    const QSize& modelGeometricSize() const;

protected:
    QList<NodeItem*>& _nodelist;
    QPoint _centerPoint;
    QSize _currentSize;
};


#endif // ABSTRACTNODEITEMPOSITIONCALC_H
