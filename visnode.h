/*
 * visnode.h
 *
 * VisNode is the central class in the VisNode application.
 * It controls:
 *  - file opening and access
 *  - creating and running the correct parser
 *  - creating the model and view, and showing the view
 *
 * Mats Adborn, 2013-05-12
 */

#ifndef VISNODE_H
#define VISNODE_H

#include "nodeitem.h"
#include "abstractnodeparser.h"
#include "xmlnodeparser.h"
#include "cppnodeparser.h"
#include "nodeitemmodel.h"
#include "nodeitemdelegate.h"
#include "nodeview.h"
#include <cstdlib>              // exit()
#include <iostream>             // cout, cerr, endl
#include <QApplication>         // qApp - needed?
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QList>
#include <QString>
#include <QStringList>


class VisNode
{
public:
    VisNode(QStringList& arguments);
    ~VisNode();

    void run();

    void printNodelist();

private:
    bool createParser();
    bool filesOK() const;
    bool fileExtensionsOK() const;
    const QString getFileExtension(const QString& fileName) const;

    QStringList _fileNames;
    QList<NodeItem*> _nodelist;

    AbstractNodeParser* _parser;
    NodeItemModel* _model;
    QAbstractItemView* _view;

    static const QString FILEEXT_XML;
    static const QStringList FILEEXT_CPP;

};

#endif // VISNODE_H
