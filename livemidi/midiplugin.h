#ifndef MIDIPLUGIN_H
#define MIDIPLUGIN_H

#include <QtGui/QStylePlugin>


class MidiPlugin : public QStylePlugin
{
    Q_OBJECT
public:
    MidiPlugin(QObject *parent = 0);
};

#endif // MIDIPLUGIN_H
