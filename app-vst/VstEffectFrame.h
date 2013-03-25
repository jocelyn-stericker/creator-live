/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef VSTEFFECTFRAME_H
#define VSTEFFECTFRAME_H

#include "VstEffect.h"
#include "../App.h"
#include "../AppFrame.h"

namespace Ui {
    class VstEffectFrame;
}

class VstEffectFrame : public AppFrame
{
    Q_OBJECT
    VstEffectApp*m_app;

public:
    explicit VstEffectFrame(VstEffectApp* capp, QWidget *parent = 0);
    ~VstEffectFrame();

public slots:
    void init(QString);

private:
    Ui::VstEffectFrame *ui;
};

class VstEffectCreator : public AppCreator
{
public:
  VstEffectCreator()
  {
      AppSys::registerCreator(this);
  }
  QString name() { return "ca.nettek.live.vst"; }
  LObjectPtr newBackend() {return new VstEffectApp(); }
  AppFrame* newFrontend(LObjectPtr backend) { return new VstEffectFrame(live_cast_to_cptr<VstEffectApp*>(backend)); }
  QIcon icon() { return QIcon(":/icons/app_vst.png"); }
  static VstEffectCreator evil;    //you should(n't) really do this!!!
};


#endif // VSTEFFECTFRAME_H
