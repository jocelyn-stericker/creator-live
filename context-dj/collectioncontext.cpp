#include "collectioncontext.h"
#include "ui_collectioncontext.h"

CollectionContext::CollectionContext(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CollectionContext)
{
    ui->setupUi(this);
}

CollectionContext::~CollectionContext()
{
    delete ui;
}
