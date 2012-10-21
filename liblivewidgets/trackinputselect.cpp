/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/trackinputselect.h>
#include <live/audio>
#include "ui_trackinputselect.h"
#include <cmath>
#include <math.h>
#include <QPropertyAnimation>
#include <QTimer>

#include <live_widgets/pianokey.h>

using namespace live_widgets;
using namespace live;

int TrackInputSelect::ss_inputId = 0;

inline int octave(const int&mPitch)
{
    int a = mPitch;
    int octave = -1;
    while (a > 11)
    {
        a -= 12;
        octave++;
    }
    return octave;
}

inline char letterName(int a)
{
    // This function, as of now, only takes into account sharps.
    a%=12;
    switch (a)
    {
    case 0:
    case 1:
        return 'C';
        break;
    case 2:
    case 3:
        return 'D';
        break;
    case 4:
        return 'E';
        break;
    case 5:
    case 6:
        return 'F';
        break;
    case 7:
    case 8:
        return 'G';
        break;
    case 9:
    case 10:
        return 'A';
        break;
    case 11:
        return 'B';
        break;
    }
    return '?';
}

inline int whiteKeys(const int&a,const int&b)
{
    return qAbs(8*(octave(a) - octave(b)) + (letterName(a) - letterName(b))-1);
}

TrackInputSelect::TrackInputSelect(QWidget*parent, bool popout, bool allowMidi, bool allowAudio)
  : live_widgets::ObjectChooser(parent)
  , s_selectedFilter(0)
  , s_selectedMode(0)
  , s_ui(new Ui_Frame)
  , s_allowMidi(allowMidi)
  , s_allowAudio(allowAudio)
  , s_inputId(++ss_inputId)
  , all()
  , b_trackName("No Input")
  , b_audio(allowAudio)
  , ui_instScene(new QGraphicsScene)
  , ui_instView(new QGraphicsView(ui_instScene))
  { s_ui->setupUi(this);

    QVBoxLayout* ui_topLayout=new QVBoxLayout();

    setMinimizedButton(s_ui->inputType);
    setTopFrame(s_ui->inputType);
    setBottomFrame(s_ui->missile);
    setAlignedLeft(true);

    connect(s_ui->input_combo,SIGNAL(clicked(QModelIndex)),s_ui->input_combo,SLOT(setCurrentIndex(QModelIndex)));
    connect(s_ui->input_combo,SIGNAL(clicked(QModelIndex)),this,SLOT(activateSelected()));
    connect(s_ui->input_combo,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(activateSelected()));

    connect(s_ui->inputNew, SIGNAL(clicked()), this, SIGNAL(newOutputRequested()));

    all.push_back("0");
    updateObjects();

    binding::addWidget(this);

    connect(s_ui->inputType, SIGNAL(toggled(bool)), this, SLOT(setMaximized(bool)));

    s_ui->inputName->setText(b_trackName);
    connect(&b_trackName, SIGNAL(changeObserved(QString,QString)), s_ui->inputName, SLOT(setText(QString)));
    connect(&b_audio, SIGNAL(changeObserved(bool,bool)), this, SLOT(onSetAudio(bool)));
    connect(&b_audio, SIGNAL(changeObserved(bool,bool)), s_ui->inputPaint, SLOT(setHidden(bool)));

    setFixedWidth(55);

    setPopup(popout);

    QButtonGroup* bg=new QButtonGroup;

    for (int i=0;i<5;i++)
    {
        ui_colourSelect[i]=new ToolButton(this);
        ui_colourSelect[i]->setObjectName("colourSelect"+QString::number(i));
        ui_colourSelect[i]->setText("");
        ui_colourSelect[i]->setFixedSize(25,25);
        ui_colourSelect[i]->setCheckable(1);
        ui_colourSelect[i]->setProperty("filter_id",i);
        ui_colourSelect[i]->setStyleSheet("*{padding:0px;}");
        QPixmap solidPixmap(8,8);
        solidPixmap.fill(song::current()->colours[i]); //FIXME
        QIcon solidIcon(solidPixmap);
        ui_colourSelect[i]->setIcon(solidIcon);
        ui_topLayout->addWidget(ui_colourSelect[i]);
        connect(ui_colourSelect[i],SIGNAL(toggled(bool)),this,SLOT(changeActiveFilter(bool)));
        bg->addButton(ui_colourSelect[i]);

        ui_colourSelect[i]->show();
    }

    ui_topLayout->addSpacerItem(new QSpacerItem(0,20,QSizePolicy::Fixed,QSizePolicy::Fixed));
    bg=new QButtonGroup;

    for (int i=0;i<5;i++)
    {
        ui_modeSelect[i]=new ToolButton(this);
        ui_modeSelect[i]->setObjectName("modeSelect"+QString::number(i));
        ui_modeSelect[i]->setText("");
        ui_modeSelect[i]->setFixedSize(25,25);
        ui_modeSelect[i]->setCheckable(1);
        ui_modeSelect[i]->setProperty("mode_id",i);
        ui_modeSelect[i]->setText(QString::number(i));
        ui_modeSelect[i]->setStyleSheet("*{padding:0px;}");

        ui_modeSelect[i]->show();
        ui_topLayout->addWidget(ui_modeSelect[i]);
        connect(ui_modeSelect[i],SIGNAL(toggled(bool)),this,SLOT(changeActiveMode(bool)));
        bg->addButton(ui_modeSelect[i]);
    }

    ui_topLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Fixed,QSizePolicy::Expanding));

    ui_instView->setAlignment(Qt::AlignLeft | Qt::AlignTop );
    ui_instView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui_instView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    ui_instView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    ui_instView->setFrameStyle( QFrame::NoFrame );
    ui_instView->setMinimumWidth(70);
    ui_instView->setMaximumWidth(70);
    ui_instView->setMinimumHeight(300);  // these are just sensible(?) defaults
    ui_instView->setMaximumHeight(300);  // and should be changed by the vertical key size and range.

    for (int i=0;i<5;i++) {
        s_midiFilters[i]=new MidiFilter;
    }
    song::current()->midiMaps.insert(s_inputId,s_midiFilters[0]);

    ui_instView->setObjectName("ui_instView");
    ui_instScene->setObjectName("ui_instScene");

    for (int i=0;i<5;i++)
    {
        ui_colourSelect[i]->setObjectName("ui_colourSelect"+QString::number(i));
    }

    connect(s_ui->inputPaint, SIGNAL(toggled(bool)), this, SLOT(showKeyboard(bool)));

    ui_modeSelect[0]->setChecked(1);
    ui_colourSelect[0]->setChecked(1);

    QHBoxLayout* hl = new QHBoxLayout(s_ui->piano);
    hl->addLayout(ui_topLayout);
    hl->addWidget(ui_instView);
    ui_instView->show();
    drawKeyboard();
}

void TrackInputSelect::setPopup(bool popout) {
    s_ui->inputPaint->setVisible(!popout);
    s_ui->inputNew->setVisible(!popout);
    setMaximized(popout);
}

void TrackInputSelect::showAudio(bool b) {
    s_allowAudio = b;
    updateObjects();
}

void TrackInputSelect::showMidi(bool b) {
    s_allowMidi = b;
    updateObjects();
}

void TrackInputSelect::updateObjects() {
    int flags = InputOnly|NoRefresh;

    if (s_allowAudio && !s_allowMidi)
        flags = flags|AudioOnly;
    if (!s_allowAudio && s_allowMidi)
        flags = flags|MidiOnly;

    QList<ObjectPtr> list = object::get(flags);
    QStringList v;
    v<<list;
    if (all!=v) {
        all.clear();
        all+=v;
        s_ui->input_combo->clear();
        s_ui->input_combo->insertItems(0,all);
    }
}

void TrackInputSelect::activateSelected() {
    b_trackName = object::get(InputOnly|NoRefresh)[s_ui->input_combo->currentRow()]->name();
    if (width() != 55) {
        setMinimized();
        QTimer::singleShot(205, this, SLOT(activateSelected()));
    } else {
        emit objectChosen(object::get(InputOnly|NoRefresh)[s_ui->input_combo->currentRow()]);
    }
}

void TrackInputSelect::onSetAudio(bool b) {
    if (b) {
        s_ui->inputPaint->hide();
        s_ui->inputType->setIcon(QIcon(":/icons/microphone.png"));
    } else {
        s_ui->inputPaint->show();
        s_ui->inputType->setIcon(QIcon(":/icons/midi.png"));
    }
}

void TrackInputSelect::enableAddTrackButton() {
    s_ui->inputNew->setEnabled(true);
}

void TrackInputSelect::showKeyboard(bool b) {
    s_maximizedWidth = b ? 165 : 400;
    s_maximizedHeight = b ? 600 : -1;
    setMaximized(b);
    s_ui->stackedWidget->setCurrentIndex(b ? 1 : 0);
}

void TrackInputSelect::drawKeyboard()
{
    // fix this
    int vKeySize=11;

    QGraphicsItem* tmp;
    PianoKey* v;
    bool isVirtual;

    if ( ui_instScene->items().isEmpty() )
    {
        isVirtual = 0;
    }

    foreach( tmp, ui_instScene->items() )
    {
        if ( tmp->data(-999)=="PianoKey" )
        {
            v = static_cast< PianoKey* >( tmp );
            isVirtual = v->isVirtual();
            break;
        }
    }

    qint16 highest = 108, lowest = 21;
    /*s_instView->setSceneRect( 0, -( ( 1+( highest-lowest ) )*6 ),
                            60, vKeySize*(highest.whiteKeysTo(lowest))-( ( 1+( highest-lowest ) )*6 )+0.38*vKeySize );*/
    ui_instView->setMinimumHeight( whiteKeys(lowest,highest)*vKeySize - 65 );
    ui_instView->setMaximumHeight( whiteKeys(lowest,highest)*vKeySize - 65 );
    ui_instView->setStyleSheet( "background: transparent" );
    ui_instScene->clear(); // this delectes our pointers for us.


    int j = letterName(lowest)-'C'+6;
    int counter = -1;

    PianoKey *a = 0, *b = 0;
    for (qint16 i = 0; i < 1+(highest-lowest); i++)
    {
        // i will be added to twice if a black key exists
        // and we're not at the end

        //White keys

        a = new PianoKey( 0,  vKeySize * ++counter - (( 1 + highest - lowest ) * 6),
                          60, 0.83*vKeySize, qint16(lowest + i), NULL);

        a->setBrush( QBrush(QColor("white")));
        a->setVirtual( isVirtual );

        connect( a, SIGNAL(updated()), this, SLOT(reactToPianoKeyUpdate()) );

        ui_instScene->addItem(a);
        a->setFlag(QGraphicsItem::ItemIsSelectable);

        //Black keys

        if ((++j < 2 || ( j > 2 && j < 6)) && (i++ < highest-lowest))
        {

            b = new PianoKey( 25, vKeySize*counter-( ( 1+( highest-lowest ) )*6 )+0.67*vKeySize,
                              35, 0.5*vKeySize, qint16(lowest + i), 0);

            b->setBrush( QBrush(QColor("black")));
            b->setVirtual( isVirtual );

            connect( b, SIGNAL(updated()), this, SLOT(reactToPianoKeyUpdate()) );

            ui_instScene->addItem(b);
            b->setFlag( QGraphicsItem::ItemIsSelectable );

            b->setZValue(1);    // keeps on top of white keys

        }
        else if (j == 6 )
        {
            j = -1;
        }
    }
}


void TrackInputSelect::reactToPianoKeyUpdate()
{
    PianoKey* x = qobject_cast<PianoKey*>(sender());
    if (!x)
    {
        qCritical() << "Expected sender() type PianoKey*!!!";
        return;
    }

    int xid = x->id();
    xid%=12;
    if (!x->isEnabled())
    {
        if (s_selectedFilter>=0)
        {
            x->enableKey(1);
            return;
        }
        else
        {
            x->setBrush( QBrush( (xid==1||xid==3||xid==6||xid==8||xid==10)?QColor("black"):QColor("white") ) ); //1,3... are black keys
            return;
        }
    }
    else
    {
        if (s_selectedFilter<0)
        {
            x->enableKey(0);
            return;
        }
        else
        {
            QColor col = song::current()->colours[ s_selectedFilter ]; //FIXME
            x->setBrush( QBrush( (xid==1||xid==3||xid==6||xid==8||xid==10)?col.darker(250):col ) ); //1,3... are black keys
            song::current()->midiMaps.value(s_inputId)->b_filterForNote[x->id()]=s_selectedFilter;
            return;
        }
    }
}

void TrackInputSelect::changeActiveFilter(bool really)
{
    if (!really)
    {
        return;
    }
    Q_ASSERT(sender()&&!sender()->property("filter_id").isNull());
    s_selectedFilter = sender()->property("filter_id").toInt();
}


void TrackInputSelect::changeActiveMode(bool really)
{
    if (!really)
    {
        return;
    }
    Q_ASSERT(sender()&&!sender()->property("mode_id").isNull());
    s_selectedMode=sender()->property("mode_id").toInt();
    song::current()->midiMaps.insert(s_inputId,s_midiFilters[s_selectedMode]);

    changeActiveMode_2(s_selectedMode);
}

void TrackInputSelect::changeActiveMode_2(int mode)
{
    s_selectedMode=mode;
    QGraphicsItem* tmp;
    int filterHold=s_selectedFilter;
    foreach( tmp, ui_instScene->items() )
    {
        PianoKey* v;
        if ( tmp->data(-999)=="PianoKey" )
        {
            v = static_cast< PianoKey* >( tmp );
            s_selectedFilter=s_midiFilters[s_selectedMode]->b_filterForNote[v->id()];
            v->enableKey(1);
        }
    }
    s_selectedFilter=filterHold;
}

void TrackInputSelect::incrHeight(int z) {
    if (s_oldHeight) {
        s_oldHeight += z;
        setMaximized(false);
    }
    else setFixedHeight(height() + z);
}
