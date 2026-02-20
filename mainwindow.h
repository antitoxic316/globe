#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStringListModel>
#include <QPointer>

#include "canvas.h"
#include "session.h"
#include "addmarkerdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui_;

    std::shared_ptr<Session> session_ = std::make_shared<Session>();
    Canvas *globeCanv_;

    QStringListModel* markerModel_;
    QStringListModel* markerSourceModel_;
private slots:
    void add_marker_accept(Marker m);
    void markerListUpdated();
    void markerSourceListUpdated();
    void onGlobeRotated();

    void openSession();
    void saveSession();
    void newSession();
    void captureCanvas();
    void undo();
    void redo();
    void about();
    void aboutQt();
    void copy();
    void paste();
    void cut();
    void exportMarkers();
    void importMarkersFromPythonScript();
    void importMarkersFromCsvFile();
private:
    void onAddMarkerButtonClicked();
    void onRemoveMarkerButtonClicked();

    void createActions();
    void createMenus();

    QMenu *markersMenu;
    QMenu *importMarkersMenu;
    QMenu *sessionMenu;
    QMenu *canvasMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QActionGroup *alignmentGroup;
    QAction *newSessionAct;
    QAction *openSessionAct;
    QAction *saveSessionAct;
    QAction *captureCanvasAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *cutAct;

    QAction *exportMarkersAct;
    QAction *importMarkersFromPythonScriptAct;
    QAction *importMarkersFromCsvFileAct;

    QAction *aboutAct;
    QAction *aboutQtAct;
    QLabel *infoLabel;

    QPointer<AddMarkerDialog> addMarkerDialog_;
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

protected:
    void showEvent(QShowEvent* event) override;
};
#endif // MAINWINDOW_H
