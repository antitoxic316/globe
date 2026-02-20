#include "addmarkerdialog.h"
#include "ui_addmarkerdialog.h"

AddMarkerDialog::AddMarkerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddMarkerDialog)
{
    ui->setupUi(this);
}

AddMarkerDialog::~AddMarkerDialog()
{
    delete ui;
}

void AddMarkerDialog::accept() {
    emit add_marker_accept(Marker(ui->lonSpinBox->value(), ui->latSpinBox->value()));

    QDialog::accept();
}
