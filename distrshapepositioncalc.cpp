#include "distrshapepositioncalc.h"
#include "nodeitem.h"
#include <qmath.h>
#include <QDebug>


// Define some constants for ease of use when tweaking and debugging
static const int DISTR_SHAPE_MIN_RADIUS = 90;
static const int DISTR_SHAPE_RADIUS_INC_PER_ITEM = 10;
static const int DISTR_SHAPE_ARC_DEGREES = 120;
static const int DISTR_SHAPE_WIDTH_MOD = 100;
static const int DISTR_SHAPE_HEIGHT_MOD = 100;

/*
 *  Constructor
 */
DistrShapePositionCalc::DistrShapePositionCalc(QList<NodeItem*>& nodelist)
    : AbstractNodeItemPositionCalc(nodelist)
{
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
void DistrShapePositionCalc::calculate()
{
    distributedShape();
}


/*
 *  Utility comparator function that helps sort the connections list in
 *  distributedShape() in falling order on the number of connection for
 *  each node name.
 */
static bool sortConnListLessThan(const QPair<QString, int>& one, const QPair<QString, int>& two) {
    return one.second > two.second;     // 'More than' achieves falling order sorting
}

/*
 *  This function creates the shape for the node map using the following algorithm:
 *
 *  1. Create a hashmap of all the items
 *  2. Count all links to each item, both to and from
 *  3. Create a list sorted by the number of links
 *  4. For the item with most links:
 *       - place it in the center
 *           - if there are more than one with the same number of node, choose one of them
 *       - place other items around it, determined by:
 *           - all the items that connect to the center one
 *  5. For all other items:
 *       - unless the child has already been placed, place it in:
 *           - a fan shape (~120 degr), facing away from the parent
 */
void DistrShapePositionCalc::distributedShape()
{
    // Create a hashmap of all the items (uses hash at this stage to ease access to items)
    QHash<QString, int> linkHash;

    foreach(NodeItem* item, _nodelist) {
        linkHash.insert(item->name(), 0);
    }

    // Count all links to each item, both to and from
    foreach (NodeItem* item, _nodelist) {
        int itemsChildrenCount = linkHash.value(item->name());
        linkHash.insert(item->name(), itemsChildrenCount + item->childCount());

        QList<NodeItem*> childlist = item->children();

        foreach (NodeItem* child, childlist) {
            int childsChildrenCount = linkHash.value(child->name());
            linkHash.insert(child->name(), childsChildrenCount + 1);
        }
    }

    // Create a list sorted by the number of links (use a list(pairs) at this stage to enable sorting)
    QList<QPair<QString, int> > connCountList;

    QHashIterator<QString, int> hashIter(linkHash);
    while (hashIter.hasNext()) {
        hashIter.next();
        connCountList.append(qMakePair(hashIter.key(), hashIter.value()));
    }

    qSort(connCountList.begin(), connCountList.end(), sortConnListLessThan);

    // For the item with most links:
    //  - place it in the center
    NodeItem* centralNode = getNode(connCountList.at(0).first);
    centralNode->setPosition(_centerPoint);
    alreadyPlacedNodes.append(centralNode);

    QList<NodeItem*> connectedNodes = getAllConnectedNodes(centralNode->name());

    // TODO Perhaps add some code to this section to allow for more than one centrally
    // placed node if more than one compete of that position

    //  - place other items around it, determined by:
    //      - all the items that connect to the center one
    placeConnNodesCircle(centralNode, connectedNodes);

    // For all other items:
    //  - unless the child has already been placed, place it in:
    //      - a fan shape (~120 degr), facing away from the parent
    foreach (NodeItem* node, connectedNodes) {
        placeConnNodesArc(node, centralNode);
    }

    // Calculate the current geometric size of the node map
    calculateCurrentSize();
}

/*
 *  Returns the node with the name passed along.
 *  If not found, NULL is returned.
 */
NodeItem* DistrShapePositionCalc::getNode(const QString &nodeName)
{
    for (int i = 0; i < _nodelist.size(); ++i) {
        if (_nodelist[i]->name() == nodeName)
            return _nodelist[i];
    }
    return NULL;
}

/*
 *  Returns a list with all the nodes that are connected to the node with the name [nodeName].
 *  This includes both its children and nodes who counts [nodeName] as its parent.
 */
QList<NodeItem*> DistrShapePositionCalc::getAllConnectedNodes(const QString& nodeName)
{
    QList<NodeItem*> connectedNodes;

    NodeItem* nodeToInvestigate = getNode(nodeName);

    // Add the node's children as connected nodes
    connectedNodes += nodeToInvestigate->children();

    // For each of the nodes in the _nodelist ... (except itself)
    foreach (NodeItem* node, _nodelist) {
        if (node == nodeToInvestigate)
            continue;
        // ... see if its children count [nodeToInvestigate] as its child
        foreach (NodeItem* child, node->children()) {
            if (child == nodeToInvestigate) {
                connectedNodes.append(node);
            }
        }
    }

    return connectedNodes;
}

/*
 *  Places the nodes connected to [centerNode] around it in a circle.
 *  Every node that gets its position is added to the exclusion list (alreadyPlacedNodes).
 */
void DistrShapePositionCalc::placeConnNodesCircle(const NodeItem* centerNode, QList<NodeItem*>& connectedNodes)
{
    removePlacedNodes(connectedNodes);

    int radius = radiusCircle(connectedNodes.size());

    qreal betweenNodesRad = (M_PI / 180.0) * (360.0 / connectedNodes.size());
    QPoint center = centerNode->position();
    int xpos, ypos;

    // Set the coords for all nodes, circle wise around (0,0)
    // Note that the positions will need to be translated later during painting
    for (int i = 0; i < connectedNodes.size(); ++i) {
        xpos = radius * qCos(betweenNodesRad * i);      // cos v = x / r <=> x = r * cos v
        ypos = radius * qSin(betweenNodesRad * i);      // sin v = y / r <=> y = r * sin v
        connectedNodes[i]->setPosition(QPoint(center.x() + xpos, center.y() + ypos));
        alreadyPlacedNodes.append(connectedNodes.at(i));
    }
}

/*
 *  Recursive function that calculates and sets the positions of all the nodes connected to the [centerNode]
 *  except for those nodes who are already placed. Every node that gets its position is added to the exclusion
 *  list (alreadyPlacedNodes), and then this function is called for that node as [centerNode].
 */
void DistrShapePositionCalc::placeConnNodesArc(const NodeItem* centerNode, const NodeItem* centerNodesParent)
{
    QList<NodeItem*> connectedNodes = getAllConnectedNodes(centerNode->name());

    // Remove all the nodes that have already been placed from the passed along list
    removePlacedNodes(connectedNodes);

    // If the list is empty, stop here
    if (connectedNodes.isEmpty()) {
        return;
    }

    // Calculate the radius of the arc with this number of nodes
    int radius = radiusArc(connectedNodes.size());

    // Get the angle from this node's parent to the current node itself
    qreal centerAngleRad = getRadAngle(centerNodesParent->position(), centerNode->position());

    // Modify the start point of the angle using half the arc angle and the center angle just retrieved
    qreal startAngelRad = centerAngleRad + (M_PI / 180.0) * (DISTR_SHAPE_ARC_DEGREES / 2);

    // Calculate the angle width between nodes, in radians
    // Divide the total arc angle with the number of nodes plus one (this centers the spread)
    qreal betweenNodesRad = (M_PI / 180.0) * (static_cast<qreal>(DISTR_SHAPE_ARC_DEGREES) / (connectedNodes.size() + 1));

    //qDebug() << centerAngleRad << "rad =" << centerAngleRad * (180.0 / M_PI) << "degr";
    //qDebug() << startAngelRad << "rad =" << startAngelRad * (180.0 / M_PI) << "degr";
    //qDebug() << radiansBetweenNodes << "rad =" << radiansBetweenNodes * (180.0 / M_PI) << "degr";

    QPoint center = centerNode->position();
    int xpos, ypos;
    qreal angle;

    // For each connected node, calculate it's position. Then run this function recursively on the that node.
    for (int i = 0; i < connectedNodes.size(); ++i) {                   // BUG #100: SIZE MIGHT DECREASE AS OTHER NODES ARE PLACED ON RECURSIE CALLS
        angle = startAngelRad - (betweenNodesRad * (i + 1));
        xpos = radius * qCos(angle);                    // cos v = x / r <=> x = r * cos v
        ypos = radius * qSin(angle);                    // sin v = y / r <=> y = r * sin v
        connectedNodes[i]->setPosition(QPoint(center.x() + xpos, center.y() + ypos));

        //qDebug() << angle << "rad =" << angle * (180.0 / M_PI) << "degr";

        // Add the node to the list of the already placed ones, to avoid it from
        alreadyPlacedNodes.append(connectedNodes.at(i));

        // Continue recursively on all connected nodes
        placeConnNodesArc(connectedNodes.at(i), centerNode);
    }

    // SOLUTION? TO BUG #100: Place all connected nodes in the for-loop above in a container
    // instead of calling placeConnNodesArc() on them in the loop. After this loop, create
    // another for-loop and in there call placeConnNodesArc() on the once stored in the container.
}

/*
 *  Removes all nodes from the list which have already been placed,
 *  to avoid duplicate nodes or "stolen" nodes
 */
void DistrShapePositionCalc::removePlacedNodes(QList<NodeItem*>& list)
{
    foreach (NodeItem* node, list) {
        if (alreadyPlacedNodes.contains(node)) {
            list.removeOne(node);
        }
    }
}

/*
 *  Calculates the radius of the circle shaped placement of nodes around the center node
 */
int DistrShapePositionCalc::radiusCircle(int numOfConn) const
{
    return (DISTR_SHAPE_MIN_RADIUS + numOfConn * DISTR_SHAPE_RADIUS_INC_PER_ITEM) * 0.75;
}

/*
 *  Calculates the radius of the fan, arc shaped placement of subsequent node placements
 */
int DistrShapePositionCalc::radiusArc(int numOfConn) const
{
    return DISTR_SHAPE_MIN_RADIUS + numOfConn * DISTR_SHAPE_RADIUS_INC_PER_ITEM;     // Let the radius be the same as for the circle for now, see how it works out
}

/*
 *  Calculates and returns the angle (in radians) of the [to] point, seen from the [from] point.
 *  Used to get the angle for placement of node arcs.
 */
qreal DistrShapePositionCalc::getRadAngle(const QPoint &from, const QPoint &to)
{
    // Get the distances
    qreal distX = (to.x() - from.x());
    qreal distY = (to.y() - from.y());

    qreal angleRad = atan2(distY, distX);

//    // If the x distancs is 0, the angle is known, depending on the y distance
//    // Also prevents division by zero in the arctangent calculation below
//    if (distX == 0) {
//        if (distY > 0)
//            return (M_PI / 2);
//        else
//            return -(M_PI / 2);
//    }

//    // If the y distancs is 0, the angle is known, depending on the x distance
//    if (distY == 0) {
//        if (distX > 0)
//            return 0;
//        else
//            return M_PI;
//    }

//    // Trigonometric arctangent calculation to get the angle
//    qreal angleRad = qAtan(distY / distX);

//    // If the angle is in the 2nd or 3rd quadrant, add PI radians (180 degrees)
//    if (distX < 0)
//        angleRad += M_PI;

//    // Just for debugging.. degrees are a bit quicker to understand
//    //    qreal angleDeg = angleRad * (180.0 / M_PI);
//    //    Q_UNUSED(angleDeg);

    return angleRad;
}

/*
 *  Calculates the width and height of the model by looking at
 *  the positions of all the nodes, finding the outermost ones.
 *  Adds a bit space around it determined by DISTR_SHAPE_WIDTH_MOD
 *  and DISTR_SHAPE_HEIGHT_MOD
 */
void DistrShapePositionCalc::calculateCurrentSize()
{
    // Store some default, always to be over-written values
    int maxX = INT_MIN, minX = INT_MAX,
        maxY = INT_MIN, minY = INT_MAX;

    // Look at each node's position and modify the max/min values if greater/lesser
    foreach (NodeItem* node, _nodelist) {
        QPoint nodepos = node->position();

        if (nodepos.x() > maxX)
            maxX = nodepos.x();
        if (nodepos.x() < minX)
            minX = nodepos.x();
        if (nodepos.y() > maxY)
            maxY = nodepos.y();
        if (nodepos.y() < minY)
            minY = nodepos.y();
    }

    // Add some extra space around the "node map"
    minX -= DISTR_SHAPE_WIDTH_MOD;
    maxX += DISTR_SHAPE_WIDTH_MOD;
    minY -= DISTR_SHAPE_HEIGHT_MOD;
    maxY += DISTR_SHAPE_HEIGHT_MOD;

    int width = (maxX - minX);
    int height = (maxY - minY);

    QSize newSize(width, height);

    // As the "node map" won't be perfectly centered at the origin, calculate a new center point
    QPoint newCenter(minX + (width / 2), minY + (height / 2));

    _currentSize = newSize;
    _centerPoint = newCenter;

    // Move the "node map" to center around the weighted center
    moveInto(_centerPoint);

//    _nodelist.append(new NodeItem(_nodelist.size(), "CENTER"));
//    _nodelist.at(_nodelist.size()-1)->setPosition(_centerPoint);
}
