/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "trackgroupmidi.h"
#include "live_widgets/pianokey.h"
#include "live_widgets/pushbutton.h"
#include <QGraphicsView>
#include <QButtonGroup>

using namespace live;
using namespace live_widgets;

TrackGroupMidi::TrackGroupMidi(ObjectPtr c_input, QWidget *c_parent, bool empty) :
    TrackGroup(c_input,c_parent), s_selectedFilter(0), s_selectedMode(0)
{
    // init GUI
    ui_mainLayout = new QHBoxLayout;
    s_hathorView = new VScrollContainer(0);
    s_hathorView->setObjectName("s_hathorView");

    QVBoxLayout* ui_topLayout=new QVBoxLayout();

    ui_instLabel = new RotatedLabel;
    ui_instLabel->setText(c_input->name());
    ui_instLabel->setMinimumHeight(ui_instLabel->sizeHint().width()+15);
    ui_instLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui_instLabel->setMaximumWidth(15);
    ui_topLayout->addWidget(ui_instLabel,0, Qt::AlignTop | Qt::AlignLeft);

    QButtonGroup* bg=new QButtonGroup;
    ui_mainLayout->addLayout(ui_topLayout);
    for(int i=0;i<5;i++)
    {
        ui_colourSelect[i]=new ToolButton(this);
        ui_colourSelect[i]->setObjectName("colourSelect"+QString::number(i));
        ui_colourSelect[i]->setText("");
        ui_colourSelect[i]->setMaximumSize(10,10);
        ui_colourSelect[i]->setCheckable(1);
        ui_colourSelect[i]->setProperty("filter_id",i);
        ui_colourSelect[i]->setStyleSheet("*{padding:0px;}");
        QPixmap solidPixmap(8,8);
        solidPixmap.fill(song::current->colours[i]); //FIXME
        QIcon solidIcon(solidPixmap);
        ui_colourSelect[i]->setIcon(solidIcon);
        ui_topLayout->addWidget(ui_colourSelect[i]);
        connect(ui_colourSelect[i],SIGNAL(toggled(bool)),this,SLOT(changeActiveFilter(bool)));
        bg->addButton(ui_colourSelect[i]);
    }

    for(int i=0;i<5;i++)
    {
        ui_modeSelect[i]=new ToolButton(this);
        ui_modeSelect[i]->setObjectName("modeSelect"+QString::number(i));
        ui_modeSelect[i]->setText("");
        ui_modeSelect[i]->setMaximumSize(10,10);
        ui_modeSelect[i]->setCheckable(1);
        ui_modeSelect[i]->setProperty("mode_id",i);
        ui_modeSelect[i]->setText(QString::number(i));
        ui_modeSelect[i]->setStyleSheet("*{padding:0px;}");
        ui_topLayout->addWidget(ui_modeSelect[i]);
        connect(ui_modeSelect[i],SIGNAL(toggled(bool)),this,SLOT(changeActiveMode(bool)));
        bg->addButton(ui_modeSelect[i]);
    }

    ui_colourSelect[0]->setChecked(1);
    ui_topLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Fixed,QSizePolicy::Expanding));

    s_hathorView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    if(!empty)
    {
        QList<ObjectPtr> list=object::get(OutputOnly|MidiOnly);
        s_hathorView->push_back(new Track(c_input,list.first()));
    }
    PushButton* xpush=new PushButton("Insert new Output");
    xpush->setObjectName("xpush");
    s_hathorView->push_back(xpush);
    connect(xpush,SIGNAL(clicked()),this,SLOT(newHathorAuto()));
    s_hathorView->back()->setMinimumWidth(400);
    s_hathorView->compact=1;
    s_hathorView->updateItems();
    //mainLayout->setSpacing(0);

    ui_instScene = new QGraphicsScene;
    ui_instView = new QGraphicsView(ui_instScene);
    ui_instView->setAlignment(Qt::AlignLeft | Qt::AlignTop );
    ui_instView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui_instView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    ui_instView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    ui_instView->setFrameStyle( QFrame::NoFrame );
    ui_instView->setMinimumWidth(70);
    ui_instView->setMaximumWidth(70);
    ui_instView->setMinimumHeight(300);  // these are just sensible(?) defaults
    ui_instView->setMaximumHeight(300);  // and should be changed by the vertical key size and range.
    ui_mainLayout->addWidget( ui_instView,0, Qt::AlignTop | Qt::AlignLeft);
    ui_mainLayout->addWidget(s_hathorView);
    drawKeyboard();

    this->setLayout( ui_mainLayout );
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    MidiBindingQtSys::addWidget(this);

    for(int i=0;i<5;i++) {
        s_midiFilters[i]=new MidiFilter;
    }
    song::current->midiMaps.insert(c_input,s_midiFilters[0]);

    ui_instLabel->setObjectName("ui_instLabel");
    ui_instView->setObjectName("ui_instView");
    ui_instScene->setObjectName("ui_instScene");
    for(int i=0;i<5;i++)
    {
        ui_colourSelect[i]->setObjectName("ui_colourSelect"+QString::number(i));
    }
//    setStyleSheet("background:black;");
}

void TrackGroupMidi::newHathorAuto()
{
    newHathor(object::get(OutputOnly|MidiOnly).first());
}

// HELPER FUNCTIONS --
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

void TrackGroupMidi::reactToPianoKeyUpdate()
{
    PianoKey* x = qobject_cast<PianoKey*>(sender());
    if(!x)
    {
        qCritical()<<"Expected sender() type PianoKey*!!!";
        return;
    }

    int xid = x->id();
    xid%=12;
    if(!x->isEnabled())
    {
        if(s_selectedFilter>=0)
        {
            x->enableKey(1);
            return;
        }
        else
        {
            x->setBrush( QBrush( (xid==1||xid==3||xid==6||xid==8||xid==10)?QColour("black"):QColour("white") ) ); //1,3... are black keys
            return;
        }
    }
    else
    {
        if(s_selectedFilter<0)
        {
            x->enableKey(0);
            return;
        }
        else
        {
            QColour col = song::current->colours[ s_selectedFilter ]; //FIXME
            x->setBrush( QBrush( (xid==1||xid==3||xid==6||xid==8||xid==10)?col.darker(250):col ) ); //1,3... are black keys
            song::current->midiMaps.value(s_input)->b_filterForNote[x->id()]=s_selectedFilter;
            return;
        }
    }
}

void TrackGroupMidi::drawKeyboard()
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

    int highest = 108,lowest = 21;
    /*s_instView->setSceneRect( 0, -( ( 1+( highest-lowest ) )*6 ),
                            60, vKeySize*(highest.whiteKeysTo(lowest))-( ( 1+( highest-lowest ) )*6 )+0.38*vKeySize );*/
    ui_instView->setMinimumHeight( whiteKeys(lowest,highest)*vKeySize - 65 );
    ui_instView->setMaximumHeight( whiteKeys(lowest,highest)*vKeySize - 65 );
    ui_instView->setStyleSheet( "background: transparent" );
    ui_instScene->clear(); // this delectes our pointers for us.


    int j = letterName(lowest)-'C'+6;
    int counter = -1;

    PianoKey *a = 0, *b = 0;
    for (int i = 0; i < 1+(highest-lowest); i++)
    {
        // i will be added to twice if a black key exists
        // and we're not at the end

        //White keys

        a = new PianoKey( 0,  vKeySize*++counter-( ( 1+( highest-lowest ) )*6 ),
                          60, 0.83*vKeySize, lowest+i, NULL);

        a->setBrush( QBrush(QColour("white")));
        a->setVirtual( isVirtual );

        connect( a, SIGNAL(updated()), this, SLOT(reactToPianoKeyUpdate()) );

        ui_instScene->addItem(a);
        a->setFlag(QGraphicsItem::ItemIsSelectable);

        //Black keys

        if ((++j < 2 || ( j > 2 && j < 6)) && (i++ < highest-lowest))
        {

            b = new PianoKey( 25, vKeySize*counter-( ( 1+( highest-lowest ) )*6 )+0.67*vKeySize,
                              35, 0.5*vKeySize, lowest+i, 0);

            b->setBrush( QBrush(QColour("black")));
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

void TrackGroupMidi::changeActiveFilter(bool really)
{
    qDebug()<<"CHANGE_ACTIVE_FILTER"<<really;
    if(!really)
    {
        return;
    }
    Q_ASSERT(sender()&&!sender()->property("filter_id").isNull());
    s_selectedFilter = sender()->property("filter_id").toInt();
}


void TrackGroupMidi::changeActiveMode(bool really)
{
    if(!really)
    {
        return;
    }
    Q_ASSERT(sender()&&!sender()->property("mode_id").isNull());
    s_selectedMode=sender()->property("mode_id").toInt();
    song::current->midiMaps.insert(s_input,s_midiFilters[s_selectedMode]);

    changeActiveMode_2(s_selectedMode);
}

void TrackGroupMidi::changeActiveMode_2(int mode)
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
