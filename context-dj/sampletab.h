#ifndef SAMPLETAB_H
#define SAMPLETAB_H

#include <QWidget>

namespace Ui {
class sampletab;
}

class sampletab : public QWidget
{
    Q_OBJECT
    
public:
    explicit sampletab(QWidget *parent = 0);
    ~sampletab();
    
private:
    Ui::sampletab *ui;
};

#endif // SAMPLETAB_H
