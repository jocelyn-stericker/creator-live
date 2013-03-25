#ifndef INSERTBUTTON_H
#define INSERTBUTTON_H

#include <QFrame>
#include <QLabel>

class IBLabel : public QLabel {
    Q_OBJECT
public:
    IBLabel(QWidget* parent = 0) : QLabel(parent) {}
public slots:
    void mousePressEvent(QMouseEvent *ev) {
        emit pressed();
    }
signals:
    void pressed();
};

class InsertButton : public QFrame
{
    Q_OBJECT
    IBLabel m_label;
public:
    InsertButton(QWidget* parent = 0);
    void resizeEvent(QResizeEvent* t) {
        if (width() > 100) {
            m_label.setText(tr("<center>Click to Insert"));
        } else {
            m_label.setText("<center>+");
        }
        m_label.setGeometry(0, 0, width(), height());
        QFrame::resizeEvent(t);
    }
signals:
    void pressed();
};

#endif // INSERTBUTTON_H
