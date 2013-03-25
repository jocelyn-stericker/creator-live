/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef COLLECTIONCONTEXT_H
#define COLLECTIONCONTEXT_H

#include <QWidget>
#include "djcontext.h"

namespace Ui {
class CollectionContext;
}

class CollectionContext : public QWidget
{
    Q_OBJECT

    DJContext* m_parent;
    
public:
    explicit CollectionContext(DJContext* parent = 0);
    ~CollectionContext();
    
    void connect_changeEvents();
    void disconnect_changeEvents();

public slots:
    void stockChanged_logic();

private:
    Ui::CollectionContext *ui;
};

#endif // COLLECTIONCONTEXT_H
