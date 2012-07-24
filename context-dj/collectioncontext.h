#ifndef COLLECTIONCONTEXT_H
#define COLLECTIONCONTEXT_H

#include <QWidget>

namespace Ui {
class CollectionContext;
}

class CollectionContext : public QWidget
{
    Q_OBJECT
    
public:
    explicit CollectionContext(QWidget *parent = 0);
    ~CollectionContext();
    
private:
    Ui::CollectionContext *ui;
};

#endif // COLLECTIONCONTEXT_H
