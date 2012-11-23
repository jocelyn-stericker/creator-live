#include "insertbutton.h"

InsertButton::InsertButton(QWidget *parent)
  : QFrame(parent)
  , s_label(this)
{
    s_label.setText("<center>+");
    QFont f = s_label.font();
    f.setPointSize(32);
    f.setFamily("handelgotdlig");
    s_label.setFont(f);
    s_label.setGeometry(0, 0, width(), height());
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Plain);
    setLineWidth(2);
    connect(&s_label, SIGNAL(pressed()), this, SIGNAL(pressed()));
}
