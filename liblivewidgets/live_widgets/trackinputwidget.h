#ifndef TRACKINPUTWIDGET_H
#define TRACKINPUTWIDGET_H

#include <QFrame>
#include <live/variantbinding>

namespace Ui {
class TrackInputWidget;
}

namespace live_widgets {

class TrackInputWidget : public QFrame
{
    Q_OBJECT

public:
    explicit TrackInputWidget(QWidget *parent = 0);
    ~TrackInputWidget();

    live::Bound<QString> b_trackName;
    live::Bound<bool> b_audio;  // else midi

public slots:
    void onSetAudio(bool);

private:
    Ui::TrackInputWidget *ui;
};

}

#endif // TRACKINPUTWIDGET_H
