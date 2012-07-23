#ifndef DJCONTEXT_H
#define DJCONTEXT_H

#include <QWidget>

#include "effectstab.h"
#include "sampletab.h"
#include "tracktab.h"

namespace Ui {
class Ui_DJContext;
}

class LiveBar;

class djcontext : public QWidget
{
    Q_OBJECT

    effectstab* s_et;
    sampletab* s_st;
    tracktab* s_tt;
    LiveBar* s_lb;

public:
    explicit djcontext (QWidget *parent = 0);
    ~djcontext();

public slots:
    void showSnarky();
    void showEffects();
    void showSample();
    void showTrack();

private:
    Ui::Ui_DJContext *ui;
};

#endif // DJCONTEXT_H
