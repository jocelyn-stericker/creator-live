#ifndef LIVEBAR_H
#define LIVEBAR_H

#include <QWidget>

namespace Ui {
class LiveBar;
}

class LiveBar : public QWidget
{
    Q_OBJECT
    
public:
    explicit LiveBar(QWidget *parent = 0);
    ~LiveBar();
    
public slots:
    void modeChosen(int z);

signals:
    void djMode();
    void trackMode();
    void sampleMode();

private:
    Ui::LiveBar *ui;
};

#endif // LIVEBAR_H
