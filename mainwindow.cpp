
#include <QWidget>
#include <QContextMenuEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

    QWidget *canvasContainer = ui_->canvasContainer;

    globeCanv_ = new Canvas(canvasContainer, session_);

    QHBoxLayout* layout = new QHBoxLayout(canvasContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(globeCanv_);

    createActions();
    createMenus();

    setWindowTitle("Globe");
    resize(800, 600);

    markerModel_ = new QStringListModel(this);
    ui_->markerListView->setModel(markerModel_);
    connect(session_.get(), &Session::markerListUpdated, this, &MainWindow::markerListUpdated);

    markerSourceModel_ = new QStringListModel(this);
    ui_->markerSourceListView->setModel(markerSourceModel_);
    connect(session_.get(), &Session::markerListUpdated, this, &MainWindow::markerSourceListUpdated);//  FIXME implement proper signals for sources(maybe)

    connect(ui_->addMarkerButton, &QPushButton::clicked, this, &MainWindow::onAddMarkerButtonClicked);
    connect(ui_->removeMarkerButton, &QPushButton::clicked, this, &MainWindow::onRemoveMarkerButtonClicked);

    connect(session_.get(), &Session::globeRotated, this, &MainWindow::onGlobeRotated);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::showEvent(QShowEvent* event){
    QMainWindow::showEvent(event);

    session_->updateGlobeRotation(QVector3D(0,0,0));

    session_->addMarker(Marker(0.0f, 0.0f));
    session_->addMarker(Marker(90.0f, 0.0f));
}

void MainWindow::createActions()
{
    newSessionAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen),
                                tr("&New..."), this);
    newSessionAct->setShortcuts(QKeySequence::New);
    newSessionAct->setStatusTip(tr("Open an existing file"));
    connect(newSessionAct, &QAction::triggered, this, &MainWindow::newSession);

    openSessionAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen),
                                tr("&Open..."), this);
    openSessionAct->setShortcuts(QKeySequence::Open);
    openSessionAct->setStatusTip(tr("Open an existing file"));
    connect(openSessionAct, &QAction::triggered, this, &MainWindow::openSession);

    saveSessionAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave),
                                 tr("&Save"), this);
    saveSessionAct->setShortcuts(QKeySequence::Save);
    saveSessionAct->setStatusTip(tr("Save the document to disk"));
    connect(saveSessionAct, &QAction::triggered, this, &MainWindow::saveSession);

    captureCanvasAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentPrint),
                                   tr("&Capture canvas..."), this);
    captureCanvasAct->setShortcuts(QKeySequence::Print);
    captureCanvasAct->setStatusTip(tr("Print the document"));
    connect(captureCanvasAct, &QAction::triggered, this, &MainWindow::captureCanvas);

    exitAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit),
                          tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    undoAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditUndo),
                          tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last operation"));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    redoAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditRedo),
                          tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last operation"));
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    aboutAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout),
                           tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(aboutQtAct, &QAction::triggered, this, &MainWindow::aboutQt);

    //context menu
    cutAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCut),
                         tr("&Cut"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);

    copyAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy),
                          tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);

    pasteAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste),
                           tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);

    exportMarkersAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste),
                                   tr("&Export"), this);
    connect(exportMarkersAct, &QAction::triggered, this, &MainWindow::exportMarkers);


    importMarkersFromPythonScriptAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste),
                                                   tr("&Import from Python..."), this);
    connect(importMarkersFromPythonScriptAct, &QAction::triggered, this, &MainWindow::importMarkersFromPythonScript);


    importMarkersFromCsvFileAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste),
                                              tr("&Import from Csv..."), this);
    connect(importMarkersFromCsvFileAct, &QAction::triggered, this, &MainWindow::importMarkersFromCsvFile);
}


void MainWindow::createMenus()
{
    sessionMenu = menuBar()->addMenu(tr("&Session"));
    sessionMenu->addAction(newSessionAct);
    sessionMenu->addAction(openSessionAct);
    sessionMenu->addAction(saveSessionAct);
    sessionMenu->addSeparator();
    sessionMenu->addAction(exitAct);

    markersMenu = menuBar()->addMenu(tr("&Markers"));
    importMarkersMenu = markersMenu->addMenu(tr("&Import"));
    importMarkersMenu->addAction(importMarkersFromPythonScriptAct);
    importMarkersMenu->addAction(importMarkersFromCsvFileAct);
    markersMenu->addAction(exportMarkersAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    canvasMenu = menuBar()->addMenu(tr("&Canvas"));
    canvasMenu->addAction(captureCanvasAct);
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(cutAct);
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU


void MainWindow::onAddMarkerButtonClicked(){
    if (!addMarkerDialog_.isNull()) addMarkerDialog_->deleteLater();

    addMarkerDialog_ = QPointer(new AddMarkerDialog(this));
    addMarkerDialog_ ->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);

    connect(addMarkerDialog_, &AddMarkerDialog::add_marker_accept, this, &MainWindow::add_marker_accept);

    addMarkerDialog_->show();
}

void MainWindow::add_marker_accept(Marker m){
    QList<MarkerSource> msl = session_->getMarkerSources();

    QString name("user");
    auto it = std::find_if(msl.begin(), msl.end(),
                           [name](const MarkerSource& ms){ return ms.name == name; });
    if(it == msl.end()){
        MarkerSource ms;
        ms.name = "user";
        ms.markers.append(m);
        session_->addMarkerSource(ms);
    } else {
        session_->addMarker(m); // TODO implement proper source logic
    }
}

void MainWindow::onRemoveMarkerButtonClicked(){
    QModelIndexList selected = ui_->markerListView->selectionModel()->selectedIndexes();
    for(const QModelIndex& index : selected){
        QString name = markerModel_->data(index).toString();
        session_->removeMarker(name);
    }
}

//TODO optimize this
void MainWindow::markerListUpdated(){
    QList<Marker> markers = session_->getMarkers();

    markerModel_->removeRows(0, markerModel_->rowCount());
    QStringList l;
    for(int i = 0; i < markers.size(); i++){
        l.append(markers.at(i).uuid.toString());
    }
    markerModel_->setStringList(l);

    //draw new markers on canvas
    globeCanv_->update();
}

void MainWindow::markerSourceListUpdated(){
    QList<MarkerSource> markerSources = session_->getMarkerSources();

    markerSourceModel_->removeRows(0, markerSourceModel_->rowCount());
    QStringList l;
    for(int i = 0; i < markerSources.size(); i++){
        l.append(markerSources.at(i).name);
    }
    markerSourceModel_->setStringList(l);

    //draw new markers on canvas
    globeCanv_->update();
}

void MainWindow::openSession(){

}

void MainWindow::saveSession(){

}

void MainWindow::newSession(){

}

void MainWindow::captureCanvas(){

}

void MainWindow::undo(){

}

void MainWindow::redo(){

}

void MainWindow::about(){

}

void MainWindow::aboutQt(){

}

void MainWindow::copy(){

}

void MainWindow::paste(){

}

void MainWindow::cut(){

}

void MainWindow::importMarkersFromPythonScript(){

}

void MainWindow::importMarkersFromCsvFile(){

}

void MainWindow::exportMarkers(){

}

void MainWindow::onGlobeRotated(){
    QVector3D eulers = session_->getGlobeEulers();
    ui_->xGlobeRot->setText(QString::number(eulers.x()));
    ui_->yGlobeRot->setText(QString::number(eulers.y()));
    ui_->zGlobeRot->setText(QString::number(eulers.z()));
}
