/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/iodragwidget.h>
#include <live_widgets/toolbutton.h>
#include <live_widgets/dropframe.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLineEdit>
#include <QToolButton>
#include <QSettings>
#include <QTimer>
#include "ui_inputdragwidget.h"

void live_widgets::SpawnFrame::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasFormat("text/plain"))
    {
        e->acceptProposedAction();
    }
}

void live_widgets::SpawnFrame::dropEvent(QDropEvent *e)
{
    emit textDropped(e->mimeData()->text());
}

live_widgets::InputDragWidget::InputDragWidget(QWidget *parent) :
    QFrame(parent), s_count(-1), s_ui(new Ui::InputDragWidget)
{
    s_ui->setupUi(this);
    connect(s_ui->newMapping_frame,SIGNAL(textDropped(QString)),this,SLOT(newGroup(QString)));
    s_ui->chans_frame_out->hide();

    QSettings settings;
    QList<QVariant> inMap=settings.value("audio_inputMappings").toList();
    bool ok=0;
    for (int i=0; i<inMap; i++)
    {
        Q_ASSERT(inMap[i].toStringList().size());
        ok=1;
        newGroup( inMap[i].toStringList()[0],
                  (inMap[i].toStringList().size()<2)?"":(inMap[i].toStringList()[1]),
                  settings.value("audio_inputMappingNames").toStringList()[i]);
    }
    if (!ok)
    {
        newGroup("","","");
    }
}

void live_widgets::InputDragWidget::newGroup(QString x,QString y,QString z)
{
    QFrame*xf=new QFrame(this);
    xf->setLayout(new QHBoxLayout);
    QLineEdit*le=new QLineEdit;
    groupNames.push_back(le);
    xf->layout()->addWidget(le);
    le->setPlaceholderText("Input name");
    le->setText(z.size()?z:("Input Group "+QString::number(++s_count)));
    DropFrame* df1=new DropFrame(this);
    xf->layout()->addWidget(df1);
    df1->setText(x);
    df1->setFrameStyle(QFrame::Panel);
    df1->setFrameShadow(QFrame::Raised);
    df1->setLineWidth(1);
    df1->setFixedHeight(21);
    df1->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    dropFrameAs.push_back(df1);
    df1=new DropFrame(this);
    df1->setText(y);
    df1->setFrameStyle(QFrame::Panel);
    df1->setFrameShadow(QFrame::Raised);
    df1->setFixedHeight(21);
    df1->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    df1->setLineWidth(1);
    dropFrameBs.push_back(df1);
    xf->layout()->addWidget(df1);
    live_widgets::ToolButton* tb=new live_widgets::ToolButton;
    tb->setIcon(QIcon(":/icons/close.png"));
    xf->layout()->addWidget(tb);
    connect(tb,SIGNAL(clicked()),this,SLOT(destroyRow()));
//    QVBoxLayout::insertWidget()
    qobject_cast<QVBoxLayout*>(layout())->insertWidget(qobject_cast<QVBoxLayout*>(layout())->count()-2,xf);
    emit modified();
    connect(le,SIGNAL(textChanged(QString)),this,SIGNAL(modified()));
}

void live_widgets::InputDragWidget::destroyRow()
{
    for (int i=0;i<sender()->parent()->children().size();i++) {
        groupNames.removeAll(qobject_cast<QLineEdit*>(sender()->parent()->children()[i]));
        dropFrameAs.removeAll(qobject_cast<DropFrame*>(sender()->parent()->children()[i]));
        dropFrameBs.removeAll(qobject_cast<DropFrame*>(sender()->parent()->children()[i]));
    }
    sender()->parent()->deleteLater();
    QTimer::singleShot(0,this,SIGNAL(modified()));
}

live_widgets::InputDragWidget::~InputDragWidget()
{
}

live_widgets::OutputDragWidget::OutputDragWidget(QWidget *parent) :
    QFrame(parent), s_count(-1), s_ui(new Ui::InputDragWidget)
{
    s_ui->setupUi(this);
    connect(s_ui->newMapping_frame,SIGNAL(textDropped(QString)),this,SLOT(newGroup(QString)));
    s_ui->chans_frame_in->hide();
    s_ui->label->setText(s_ui->label->text().replace("In","Out"));
    s_ui->label->setText(s_ui->label->text().replace("in","out"));
    s_ui->label->setText(s_ui->label->text().replace("outto","into"));     //lol

    QSettings settings;
    QList<QVariant> outMap=settings.value("audio_outputMappings").toList();

    bool ok=0;
    for (int i=0; i<outMap; i++)
    {
        Q_ASSERT(outMap[i].toStringList().size());
        ok=1;
        newGroup( outMap[i].toStringList()[0],
                  (outMap[i].toStringList().size()<2)?"":(outMap[i].toStringList()[1]),
                  settings.value("audio_outputMappingNames").toStringList()[i]);
    }
    if (!ok)
    {
        newGroup("","","");
    }
}

void live_widgets::OutputDragWidget::newGroup(QString x,QString y,QString z)
{
    QFrame*xf=new QFrame(this);
    xf->setLayout(new QHBoxLayout);
    QLineEdit*le=new QLineEdit;
    groupNames.push_back(le);
    xf->layout()->addWidget(le);
    le->setPlaceholderText("Output name");
    le->setText(z.size()?z:("Output Group "+QString::number(++s_count)));
    DropFrame* df1=new DropFrame(this);
    xf->layout()->addWidget(df1);
    df1->setText(x);
    df1->setFrameStyle(QFrame::Panel);
    df1->setFrameShadow(QFrame::Raised);
    df1->setFixedHeight(21);
    df1->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    df1->setLineWidth(1);
    dropFrameAs.push_back(df1);
    df1=new DropFrame(this);
    df1->setText(y);
    df1->setFrameStyle(QFrame::Panel);
    df1->setFrameShadow(QFrame::Raised);
    df1->setFixedHeight(21);
    df1->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    df1->setLineWidth(1);
    dropFrameBs.push_back(df1);
    xf->layout()->addWidget(df1);
    live_widgets::ToolButton* tb=new live_widgets::ToolButton;
    tb->setIcon(QIcon(":/icons/close.png"));
    xf->layout()->addWidget(tb);
    connect(tb,SIGNAL(clicked()),this,SLOT(destroyRow()));
//    QVBoxLayout::insertWidget()
    qobject_cast<QVBoxLayout*>(layout())->insertWidget(qobject_cast<QVBoxLayout*>(layout())->count()-2,xf);
    emit modified();
    connect(le,SIGNAL(textChanged(QString)),this,SIGNAL(modified()));
}

void live_widgets::InputDragWidget::refresh()
{
    delete s_ui->chans_frame_in;
    s_ui->chans_frame_in=new InputDragListFrame(this);
    qobject_cast<QVBoxLayout*>(layout())->insertWidget(0,s_ui->chans_frame_in);
}

void live_widgets::OutputDragWidget::refresh()
{
    delete s_ui->chans_frame_out;
    s_ui->chans_frame_out=new OutputDragListFrame(this);
    qobject_cast<QVBoxLayout*>(layout())->insertWidget(0,s_ui->chans_frame_out);
}

void live_widgets::OutputDragWidget::destroyRow()
{
    for (int i=0;i<sender()->parent()->children().size();i++) {
        groupNames.removeAll(qobject_cast<QLineEdit*>(sender()->parent()->children()[i]));
        dropFrameAs.removeAll(qobject_cast<DropFrame*>(sender()->parent()->children()[i]));
        dropFrameBs.removeAll(qobject_cast<DropFrame*>(sender()->parent()->children()[i]));
    }
    sender()->parent()->deleteLater();
    QTimer::singleShot(0,this,SIGNAL(modified()));
}

live_widgets::OutputDragWidget::~OutputDragWidget()
{
}
