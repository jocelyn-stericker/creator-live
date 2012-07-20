#ifndef EFFECTSTAB_H
#define EFFECTSTAB_H

#include <QWidget>

namespace Ui {
class effectstab;
}

class effectstab : public QWidget
{
    Q_OBJECT
    
public:
    explicit effectstab(QWidget *parent = 0);
    ~effectstab();
    
private:
    Ui::effectstab *ui;
};

#endif // EFFECTSTAB_H
