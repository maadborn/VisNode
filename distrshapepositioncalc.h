/*
 * distrshapepositioncals.h
 *
 * This class calculates the position of the nodes to be represented graphically in the view.
 * The node with the most number of children will be placed first, surrounded with
 * it's children or other nodes that have the center one as a child. All other children or
 * parents will then be placed in arc shapes spreading out recursively.
 *
 * Mats Adborn, 2013-05-17
 */

#ifndef DISTRSHAPEPOSITIONCALC_H
#define DISTRSHAPEPOSITIONCALC_H

#include "abstractnodeitempositioncalc.h"
#include <QList>

class NodeItem;

class DistrShapePositionCalc : public AbstractNodeItemPositionCalc
{
public:
    DistrShapePositionCalc(QList<NodeItem*>& nodelist);

    virtual void calculate();

private:
    QList<NodeItem*> alreadyPlacedNodes;

    void distributedShape();

    NodeItem* getNode(const QString& nodeName);
    QList<NodeItem*> getAllConnectedNodes(const QString& nodeName);
    void placeConnNodesCircle(const NodeItem *centerNode, QList<NodeItem *> &connectedNodes);
    void placeConnNodesArc(const NodeItem *centerNode, const NodeItem* centerNodesParent);
    void removePlacedNodes(QList<NodeItem*>& list);
    int radiusCircle(int numOfConn) const;
    int radiusArc(int numOfConn) const;
    qreal getRadAngle(const QPoint& from, const QPoint& to);

    void calculateCurrentSize();
};

#endif // DISTRSHAPEPOSITIONCALC_H
