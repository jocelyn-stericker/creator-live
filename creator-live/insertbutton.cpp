#include "insertbutton.h"

InsertButton::InsertButton(QWidget *parent)
  : QFrame(parent)
  , m_label(this)
{
    m_label.setText("<center>+");
    QFont f = m_label.font();
    f.setPointSize(32);
    f.setFamily("handelgotdlig");
    m_label.setFont(f);
    m_label.setGeometry(0, 0, width(), height());
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Plain);
    setLineWidth(2);
    connect(&m_label, SIGNAL(pressed()), this, SIGNAL(pressed()));
}
