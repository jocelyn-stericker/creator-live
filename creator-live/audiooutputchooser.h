/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/


#ifndef AUDIOOUTPUTCHOOSER_H
#define AUDIOOUTPUTCHOOSER_H

#include <QWidget>
#include <live/object>
#include <live/variantbinding>

namespace Ui {
class AudioOutputChooser;
}

class OutputChooser : public QWidget
{
    Q_OBJECT
public:
    OutputChooser(QWidget* parent)
      : QWidget(parent)
      , b_trackName(0)
      {
    }

    live::Bound<QString> b_trackName;
};

class AudioOutputChooser : public OutputChooser
{
    Q_OBJECT
    QStringList all;
    
public:
    explicit AudioOutputChooser(QWidget *parent = 0);
    ~AudioOutputChooser();
    
public slots:
    void go();
    void refresh();
    void rowChangedEvent();
    void internalStockChangedEvent();

    void maximize(bool reverse = false) { minimize(!reverse); }
    void minimize(bool reverse = false);

    void roundCorners();
    void squareCorners();

private:
    void resizeEvent(QResizeEvent *);
    Ui::AudioOutputChooser *s_ui;

signals:
    void resized();
    void doneResizing();
    void outputChosen(live::ObjectPtr);

private:
    AudioOutputChooser(const AudioOutputChooser&);
    AudioOutputChooser& operator=(const AudioOutputChooser&) {
        TCRASH();
        return *this;
    }
};

#endif // AUDIOOUTPUTCHOOSER_H
