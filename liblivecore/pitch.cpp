/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/
#include <live/pitch.h>

live::Pitch::Pitch(char croot,Accidental caccidental,int coctave) :
    s_root(croot),
    s_accidental(caccidental),
    s_octave(coctave),
    valid(QChar(s_root).toLower().toLatin1()<=(char)'g'&&QChar(s_root).toLower().toLatin1()>=(char)'a')
{}

live::Pitch::Pitch(char croot,char caccidental,int coctave) : s_root(croot),
    s_accidental(caccidental=='#'?Pitch::Sharp:(caccidental=='b'?Pitch::Flat:(caccidental==' '?Pitch::Natural:(caccidental=='x'?Pitch::DoubleSharp:Pitch::DoubleFlat)))), s_octave(coctave),
    valid(QChar(s_root).toLower().toLatin1()<=(char)'g'&&QChar(s_root).toLower().toLatin1()>=(char)'a') {if (caccidental!='#'&&caccidental!='b'&&caccidental!='-'&&caccidental!=' '&&caccidental!='x')valid=0;}

live::Pitch::Pitch(QString cpitch,int coctave) : s_root('_'),
    s_accidental(cpitch.contains('#')?Pitch::Sharp:((cpitch.contains('b')&&cpitch.lastIndexOf('b'))?Pitch::Flat:(cpitch.contains('x')?Pitch::DoubleSharp:(cpitch.contains("-")?Pitch::DoubleFlat:Pitch::Natural)))),
    s_octave(coctave),valid(1)
{
    bool ok=0;
    bool failModePlus=1;
    for (int i=0;i<cpitch.size();i++)
    {
        if (s_root=='_'&&QChar(cpitch[i]).toLower().toLatin1()<=(char)'g'&&QChar(cpitch[i]).toLower().toLatin1()>=(char)'a')
        {
            s_root=cpitch[i].toLatin1();
            if (s_accidental!=Flat) {ok=1;continue;}
            else continue;
        }
        if (s_accidental==Flat&&!ok&&cpitch[i]=='b') {ok=1;continue;}
        if (cpitch[i]!=' '&&cpitch[i]!='#'&&cpitch[i]!='b'&&cpitch[i]!='-'&&cpitch[i]!=' '&&cpitch[i]!='x') {failModePlus=0; break; }
    }
    if (!failModePlus||!ok) { valid=0; }
}

int live::Pitch::midiNote()
{
    if (!valid) return INVALID;
    int realBase[]= {-3,-1,0,2,4,5,7};
    int octave = (s_octave==AUTOOCTAVE)?4:s_octave;
    int base = QChar(s_root).toLower().toLatin1()-(char)'a';
    base = realBase[base]<0?realBase[base]+12:realBase[base];
    return (octave+1)*12+base+s_accidental;
}
