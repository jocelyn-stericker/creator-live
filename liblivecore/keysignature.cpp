/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/keysignature>

live::KeySignature::KeySignature(char croot,Accidental cacc,Tonality cton) : Pitch(croot,cacc), m_tonality(cton) {}
live::KeySignature::KeySignature(char croot,char cacc,Tonality cton) : Pitch(croot,cacc), m_tonality(cton) {}
live::KeySignature::KeySignature(QString cpitch,Tonality cton) : Pitch(cpitch), m_tonality(cton) {}
