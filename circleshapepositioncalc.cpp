#include "circleshapepositioncalc.h"
#include "nodeitem.h"
#include <qmath.h>
#include <QDebug>

// Define some constants for ease of use when tweaking and debugging
static const int CIRCLE_SHAPE_MIN_SIZE = 200;
static const int CIRCLE_SHAPE_INC_PER_ITEM = 20;

/*
 *  Constructor
 */
CircleShapePositionCalc::CircleShapePositionCalc(QList<NodeItem*>& nodelist)
    : AbstractNodeItemPositionCalc(nodelist), knownNumNodes(0)
{
    _currentSize = calculateRelativeSize();
}

/*
 *  Calculates the initial positions of all nodes, centered around the origin.
 *
 *  At the current stage, it just positions them in a circle and spreads the nodes
 *  with many children in a roughly even manner.
 *
 *  Call this:
 *  1) In the beginning when all nodes have been added from parsing the file(s)
 *  2) If a new node is added to an already created model. (Editable model not implemented yet!)
 *     Also make sure to call moveTo([new position]) afterwards.
 */
void CircleShapePositionCalc::calculate()
{
    circleShape();
}


/*
 *  Comparator function for sorting of the node list
 */
bool nodeItemSortChildCount(const NodeItem* node1, const NodeItem* node2)
{
    return node1->childCount() > node2->childCount();
}

/*
 *  Creates the node map by determining the positions of all the nodes, based on
 *  placing the node with the most number of children in the center and the rest
 *  of the nodes in a circle around it.
 */
void CircleShapePositionCalc::circleShape()
{
    // If the number of nodes has changed since last time the size was calculated
    // As of 2013-05-17, this is not an issue, as the list's size is constant
    if (knownNumNodes != _nodelist.size()) {
        knownNumNodes = _nodelist.size();
        _currentSize = calculateRelativeSize();
    }

    int maxWidth = _currentSize.width();

    qSort(_nodelist.begin(), _nodelist.end(), nodeItemSortChildCount);  // Sorts the nodes on childCount

    spreadNodelistOnChildCount();

    int radius = (static_cast<qreal>(maxWidth) / 2.0) * 0.9;    // Make the radius from the center less than half to
                                                                // avoid that nodes are drawn partially outside the view.

    qreal radiansBetweenNodes = (M_PI / 180.0) * (360.0 / _nodelist.size());
    int xpos, ypos;

    // Set the coords for all nodes, circle wise around (0,0)
    // Note that the positions will need to be translated later during painting
    for (int i = 0; i < _nodelist.size(); ++i) {
        xpos = radius * qCos(radiansBetweenNodes * i);      // cos v = x / r <=> x = r * cos v
        ypos = radius * qSin(radiansBetweenNodes * i);      // sin v = y / r <=> y = r * sin v
        _nodelist[i]->setPosition(QPoint(xpos, ypos));
    }
}


/*
 *  This function uses a simple spreading algorithm of the values in the nodelist.
 *  The point is to even out the position of nodes with many children among the entire range of nodes,
 *  making the graphical representation easier to interpret.
 *  Unless the list is sorted before calling the function, the spread is rather useless.
 *  The list is also meant to be displayed in a circle to have the best effect, but could also be used otherwise.
 *
 *  The algorithm can be described as:
 *  Do the following [list size / 3] times:
 *      Find the opposite position of the current index in a circle, and subtract 1
 *      Swap the current node with that on the position just determined
 *      Let the next current node be the one found one step backwards from the opposite one used above
 */
void CircleShapePositionCalc::spreadNodelistOnChildCount()
{
    // Prepare the variables
    int swapIndexFirst = 1;                 // Note index start at 1, as we don't want to spread index 0 == the node with most children
    int swapIndexSecond;
    int totalSwaps = _nodelist.size() / 3;  // Seems like a good number of swaps, at least for small number of nodes
    qreal listSize = _nodelist.size();      // Store the size as a real number (avoid implicit integer assumptions/conversion)

    // Make the swaps and perform the next swap calculations
    for (int i = 0; i < totalSwaps; ++i) {
        swapIndexSecond = (qCeil(swapIndexFirst + (listSize / 2.0)) - 1) % static_cast<int>(listSize);   // List seen as a circle, the opposite side (index), rounded up, minus 1

        if (swapIndexSecond <= -1)
            swapIndexSecond = listSize - 1;

        swapNodes(_nodelist[swapIndexFirst], _nodelist[swapIndexSecond]);
        swapIndexFirst = swapIndexSecond - 1;                           // Move one step backwards, using the last second index as firs next time

        if (swapIndexFirst <= -1)
            swapIndexFirst = listSize - 1;
    }
}


void CircleShapePositionCalc::swapNodes(NodeItem*& node1, NodeItem*& node2)
{
    NodeItem* temp = node1;
    node1 = node2;
    node2 = temp;
}

QSize CircleShapePositionCalc::calculateRelativeSize() const
{
    int width = CIRCLE_SHAPE_MIN_SIZE + _nodelist.size() * CIRCLE_SHAPE_INC_PER_ITEM;
    int height = width;                                                 // For now, use a square..

    QSize size(width + 40, height + 40);

    return size;
}
