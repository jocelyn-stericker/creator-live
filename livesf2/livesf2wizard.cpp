/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( SF2 PLUGIN ) ) )

*******************************************************/

#include "livesf2wizard.h"
#include "ui_livesf2wizard.h"
#include "soundfont.h"

#include <live/object>

#include <QDesktopServices>
#include <QFile>

LiveSF2Wizard::LiveSF2Wizard()
  : QStackedWidget()
  , ui(new Ui::LiveSF2Wizard)
  { ui->setupUi(this);

    connect(ui->toolButton_next, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui->toolButton_next_2, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui->toolButton_back4, SIGNAL(clicked()), this, SLOT(prev()));
    connect(ui->toolButton_back4_2, SIGNAL(clicked()), this, SLOT(prev()));

    setCurrentIndex(0);

    QStringList insts;
    QFile f(":/sf2/programs");
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << f.errorString();
    }
    while (!f.atEnd()) {
        insts.push_back(f.readLine());
    }
    ui->listWidget_out->addItems(insts);
    ui->listWidget_out->setCurrentRow(0);

    connect(live::object::singleton(), SIGNAL(stockChanged()), this, SLOT(updateLoopbacks()));
    updateLoopbacks();

    connect(ui->listWidget_loop, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_out, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(next()));
}

void LiveSF2Wizard::updateLoopbacks() {
    s_loopbacks = live::object::get(live::AudioOnly | live::OutputOnly);
    ui->listWidget_loop->clear();
    for (int i = 0; i < s_loopbacks.size(); ++i) {
        ui->listWidget_loop->addItem(s_loopbacks[i]->name());
    }
}

void LiveSF2Wizard::next() {
    if (ui->listWidget_out->currentRow() == -1)
        return;

    if (currentIndex() + 1 == count()) {
        if (ui->listWidget_out->currentRow() == -1)
            return;

        emit instrumentUpdated(s_out, s_loopback =
                s_loopbacks[ui->listWidget_loop->currentRow()]);
        return;
    } else {
        int l = ui->listWidget_out->currentRow();

        if (s_out.valid())
            s_out.detach();

        QString dir =
#ifndef __QNX__
        qApp->applicationDirPath()+"/../plugins/";
#else
#error FIXME
#endif
        if (l < 3)
            s_out = live::ObjectPtr(new Soundfont(dir + "grand.sf2"));
        else
            s_out = live::ObjectPtr(new Soundfont(dir + "mustheory2.sf2"));

        live::cast<Soundfont*>(s_out)->setProgram(0, l);
    }

    setCurrentIndex(currentIndex() + 1);
}

void LiveSF2Wizard::prev() {
    if (currentIndex())
        setCurrentIndex(currentIndex() - 1);
}
