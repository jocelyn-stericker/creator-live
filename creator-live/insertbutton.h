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
    IBLabel s_label;
public:
    InsertButton(QWidget* parent = 0);
    void resizeEvent(QResizeEvent* t) {
        if (width() > 100) {
            s_label.setText(tr("<center>Click to Insert"));
        } else {
            s_label.setText("<center>+");
        }
        s_label.setGeometry(0, 0, width(), height());
        QFrame::resizeEvent(t);
    }
signals:
    void pressed();
};

#endif // INSERTBUTTON_H
