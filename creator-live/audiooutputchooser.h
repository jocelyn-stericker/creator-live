/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef AUDIOOUTPUTCHOOSER_H
#define AUDIOOUTPUTCHOOSER_H

#include <QWidget>
#include <live/audio>
#include <live/object>
#include <live/variantbinding>
#include <live_widgets/objectchooser.h>

namespace Ui {
class AudioOutputChooser;
}

class AudioOutputChooser : public live_widgets::ObjectChooser
{
    Q_OBJECT
    QStringList all;
    
public:
    explicit AudioOutputChooser(QWidget *parent = 0);
    ~AudioOutputChooser();
    
public slots:
    void activateSelected();
    void updateObjects();

private:
    Ui::AudioOutputChooser *s_ui;

private:
    Q_DISABLE_COPY(AudioOutputChooser)
};

#endif // AUDIOOUTPUTCHOOSER_H
