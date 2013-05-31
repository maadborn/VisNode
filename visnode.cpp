#include "visnode.h"
#include <QFile>
#include <QFileDialog>
#include <QStringRef>
#include <QStringList>
#include <QListView>
#include <QTableView>
#include <QDebug>

// Initialize the literals for the different file types
const QString VisNode::FILEEXT_XML = "xml";
const QStringList VisNode::FILEEXT_CPP = QStringList() << "cpp" << "cc" << "c" << "cxx" << "h" << "hpp";


/*
 *  Constructor
 *  Note that _fileNames contains the full arguments list initially, which is modified
 *  in createParser() by removing the program name if it exists. This is to allow the
 *  program to be used from both command line and desktop
 */
VisNode::VisNode(QStringList& arguments)
    : _fileNames(arguments)
{
    _model = new NodeItemModel(_nodelist);

    if (!createParser()) {
        std::cerr << "Unknown or non-matching filetype(s) selected!" << std::endl;
        exit(1);
    }
}


/*
 *  Destructor
 */
VisNode::~VisNode()
{
    foreach (NodeItem* node, _nodelist) {
        delete node;
    }

    delete _view;
    delete _model;
    delete _parser;
}


/*
 *  Main class function, starts the show!
 *
 *  Has some instanciations and calls during early development,
 *  which should be moved into the constructor later
 */
void VisNode::run()
{
    std::cout << "Parsing... ";

    // Set the model as the parent of the nodes
    _parser->nodeCreator()->setNodeObjectParent(_model);

    // Parse the chosen files (effectively creates the nodes)
    if (!_parser->parseFiles(_fileNames)) {
        std::cout << "VisNode failed during parsing of files" << std::endl;
        exit(EXIT_FAILURE);
    }

    // When all nodes have been found and created, create the visual map of the node set
    _model->recalculateNodePositions();

    // Get the size of the visual map and translate the nodes' coordinates to work the view's coordinate system
    QSize modelSize = _model->modelGeometricSize();
    _model->moveNodePositions(QPoint(modelSize.width()/2, modelSize.height()/2));

    // Create the view and set the model and delegate to be used in the view
    _view = new NodeView(modelSize);
    _view->setModel(_model);
    _view->setItemDelegate(new NodeItemDelegate);

    std::cout << "done." << std::endl;

    //printNodelist();

    _view->show();

//    printNodelist();
//    qDebug() << "Closing program as debug mode!";
//    exit(EXIT_SUCCESS);
}


/*
 *  Debug function which prints the current node list with parents and their position and children
 */
void VisNode::printNodelist()
{
    std::cout << "--------------------------------------------------" << std::endl;

    for (int i = 0; i < _nodelist.size(); ++i) {
        std::cout << i << ": " << qPrintable(_nodelist.at(i)->name());

        std::cout << " (" << _nodelist.at(i)->position().x() << ", " << _nodelist.at(i)->position().y() << "): ";

        for (int j = 0; j < _nodelist.at(i)->childCount(); ++j) {
            std::cout << qPrintable(_nodelist.at(i)->child(j)->name()) << ", ";
        }

        std::cout << std::endl;
    }

    std::cout << "--------------------------------------------------" << std::endl;
}


/*
 *  Creates a parser based on the files provided
 *  (by command line arguments or file dialog).
 *  If a parser can be created, true is returned. Otherwise, false.
 */
bool VisNode::createParser()
{
    // If additional files was added through command line arguments
    if (_fileNames.size() > 1) {
        // If all files in the current directory should be parsed
        if (_fileNames.at(1) == ".") {
            // TODO Implement add all files in directory
        }

        // Remove the program name from the arguments list
        _fileNames.removeFirst();
    }
    // Else, show a file selection dialog
    else {
        _fileNames.clear();
        _fileNames = QFileDialog::getOpenFileNames(NULL,
                                                   QObject::tr("Open file"),
                                                   ".",
                                                   QObject::tr("XML files (*.xml);;C++ files (*.cpp *.cc *.c *.cxx *.h *.hpp)"));
    }

    if (filesOK() && !_fileNames.isEmpty()) {
        // As the file types are already validated, get an arbitrary file's extension
        QString filetype = getFileExtension(_fileNames.at(0));

        // Determine the type of parser to create
        if (FILEEXT_XML.contains(filetype, Qt::CaseInsensitive)) {
            qDebug() << "VisNode::createParser() constructed an XMLNodeParser";
            _parser = new XMLNodeParser(_nodelist);
        }
        else if (FILEEXT_CPP.contains(filetype, Qt::CaseInsensitive)) {
            qDebug() << "VisNode::createParser() constructed a CPPNodeParser";
            _parser = new CPPNodeParser(_nodelist);
        }
        else {
            std::cerr << "VisNode::createParser() found a non-supported file type" << std::endl;
            return false;
        }

        return true;
    }

    // If nothing worked, return false
    std::cerr << "VisNode::createParser(): Failed to create data parser." << std::endl;
    return false;
}


/*
 *  Checks if all files are okey and then returns true.
 *  If anything isn't all right, return false.
 */
bool VisNode::filesOK() const
{
    QFile tempFile;

    for (int i = 0; i < _fileNames.size(); i++) {
        tempFile.setFileName(_fileNames.at(i));

        if (!tempFile.exists()) {
            std::cerr << "filesOK() found a file that doesn't exist" << std::endl;
            return false;
        }
    }

    if (!fileExtensionsOK())
        return false;

    return true;
}


/*
 *  Makes sure the file extensions are valid, based on the program's ability of
 *  parsing different filetypes
 *  Returns true if everything looks good, else false.
 */
bool VisNode::fileExtensionsOK() const
{
    QString fileExt, firstMatch;

    // Loop through all file names and determine the file types
    for (int i = 0; i < _fileNames.size(); i++) {
        fileExt = getFileExtension(_fileNames.at(i));

        // Save the first extension found to match later extensions against
        if (firstMatch.isEmpty())
            firstMatch = fileExt;

        // Verify that the extension found is supported by the program
        if (FILEEXT_XML.contains(fileExt, Qt::CaseInsensitive)) {

            // Verify that this file and the first one found match types
            // Neither the model nor common sense accepts that different types should be combined
            if (fileExt != firstMatch) {
                std::cerr << "fileExtensionsOK() found files of non-matching types" << std::endl;
                return false;
            }
        }
        else if (FILEEXT_CPP.contains(fileExt, Qt::CaseInsensitive)) {

            if (!FILEEXT_CPP.contains(firstMatch, Qt::CaseInsensitive)) {
                std::cerr << "fileExtensionsOK() found files of non-matching types" << std::endl;
                return false;
            }
        }
        // TODO Add additional file type support
        else {
            std::cerr << "fileExtensionsOK() found a non-supported file type" << std::endl;
            return false;
        }
    }

    return true;
}


/*
 *  Returns the file extension of the supplied file name
 *  If the no extension could be found, an empty QString is returned
 */
const QString VisNode::getFileExtension(const QString& fileName) const
{
    QStringRef fileExt;
    int dotPosition = 0;

    dotPosition = fileName.lastIndexOf('.');

    if (dotPosition == -1) {
        std::cerr << "getFileExtension() failed to parse filename: no extension found." << std::endl;
        return QString();
    }

    fileExt = QStringRef(&fileName, dotPosition + 1, (fileName.length() - dotPosition - 1));

    return fileExt.toString();
}
