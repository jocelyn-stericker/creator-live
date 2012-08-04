/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/rotatedlabel.h>

#include <QMouseEvent>

live_widgets::RotatedLabel::RotatedLabel( QWidget *parent ) :
    QLabel(parent) {

}

void live_widgets::RotatedLabel::paintEvent( QPaintEvent * ) {
    QPainter painter( this );
    painter.save();
    painter.rotate( 90 );

    QString a= this->text();
    a.remove("\n");

    painter.setFont( this->font() );
    painter.drawText( 0, 0, a );
    painter.restore();
}

void live_widgets::RotatedLabel::mousePressEvent(QMouseEvent *ev) {
    if (ev->button()==Qt::LeftButton) {
        emit clicked();
    }
}
