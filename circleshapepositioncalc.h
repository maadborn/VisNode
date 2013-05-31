/*
 * circleshapepositioncals.h
 *
 * This class calculates the position of the nodes to be represented graphically in the view.
 * The node with the most number of children will be placed in the center, surrounded with
 * it's children in a circle.
 *
 * Mats Adborn, 2013-05-17
 */

#ifndef CIRCLESHAPEPOSITIONCALC_H
#define CIRCLESHAPEPOSITIONCALC_H

#include "abstractnodeitempositioncalc.h"
#include <QList>

class NodeItem;

class CircleShapePositionCalc : public AbstractNodeItemPositionCalc
{
public:
    CircleShapePositionCalc(QList<NodeItem *> &nodelist);

    virtual void calculate();

private:
    int knownNumNodes;

    void circleShape();

    QSize calculateRelativeSize() const;
    void spreadNodelistOnChildCount();
    void swapNodes(NodeItem*& node1, NodeItem*& node2);
};

#endif // CIRCLESHAPEPOSITIONCALC_H
