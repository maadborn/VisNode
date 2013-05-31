/*
 * nodecreator.h
 *
 * NodeCreator class creates nodes, and is a part of the AbstractParser class
 *
 * Mats Adborn, 2013-05-01
 */

#ifndef NODECREATOR_H
#define NODECREATOR_H

#include "nodeitem.h"
#include <QObject>
#include <QList>
#include <QColor>


class NodeCreator
{
public:
    NodeCreator(QList<NodeItem*>& nodelist, QObject* nodeObjectParent = 0);

    void createNode(const QString& nodeName, const QString& parentName,
                    const QColor& color = QColor(), const QColor &parentColor = QColor());
    void createStandAloneNode(const QString& nodeName, const QColor& color = QColor());
    void setNodeObjectParent(QObject* parent);
    NodeItem* getNode(const QString& nodeName) const;

private:
    QList<NodeItem*>& _nodelist;
    QObject* _nodeObjectParent;

    NodeItem* addNode(const QString &name, const QColor &color);
};

#endif // NODECREATOR_H
