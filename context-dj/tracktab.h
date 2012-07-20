#ifndef TRACKTAB_H
#define TRACKTAB_H

#include <QWidget>

namespace Ui {
class tracktab;
}

class tracktab : public QWidget
{
    Q_OBJECT
    
public:
    explicit tracktab(QWidget *parent = 0);
    ~tracktab();
    
private:
    Ui::tracktab *ui;
};

#endif // TRACKTAB_H
