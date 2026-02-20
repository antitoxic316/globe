#ifndef ADDMARKERDIALOG_H
#define ADDMARKERDIALOG_H

#include <QDialog>

#include "session.h"

namespace Ui {
class AddMarkerDialog;
}

class AddMarkerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddMarkerDialog(QWidget *parent = nullptr);
    ~AddMarkerDialog();

private:
    Ui::AddMarkerDialog *ui;

public slots:
    void accept();
signals:
    void add_marker_accept(Marker m);
};

#endif // ADDMARKERDIALOG_H
