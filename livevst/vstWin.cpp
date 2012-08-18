/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#ifdef _WIN32

#include "src/Pipeline/Vst_p.h"
#include "src/Framework/System.h"
#include "VstSidekick.h"
//#undef UNICODE

//#include "windows.h"

#include "extern/aeffect.h"
#include "extern/aeffectx.h"
#include "extern/vstfxstore.h"

QStringList Vst::s_vstpaths_linux;
QStringList* Vst::s_vstCache=0;
QMultiMap<QString, QPair<QString, Vst*> > Vst::s_map;

SecretVst* SecretVst::singleton=0;
int Vst::lastVstId=-1;
QList<Vst*> Vst::s_vst;

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
Interface with VST library
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////
typedef AEffect *(*vstPluginFuncPtr) (audioMasterCallback host);
typedef VstIntPtr (*dispatcherFuncPtr)(AEffect *effect, VstInt32 opCode, VstInt32 index, VstInt32 value, void *ptr, float opt);
typedef float (*getParameterFuncPtr)(AEffect *effect, VstInt32 index);
typedef void (*setParameterFuncPtr)(AEffect *effect, VstInt32 index, float value);
typedef VstInt32 (*processEventsFuncPtr)(VstEvents *events);
typedef void (*processFuncPtr)(AEffect *effect, float **inputs, float **outputs, VstInt32 sampleFrames);
struct EW: DLGTEMPLATE
{
    WORD ext[3];
    EW ()
    {
        memset (this, 0, sizeof (*this));
    }
};

extern "C"
{
    // Main host callback
    VstIntPtr VSTCALLBACK hostCallback(
        AEffect *effect, VstInt32 opcode, VstInt32, VstInt32, void *, float )
    {
        qDebug()<<opcode;
        switch (opcode)
        {
        case audioMasterVersion:
            return 7;
        case audioMasterIdle:
            ( (dispatcherFuncPtr)(effect->dispatcher) )(effect, effEditIdle, 0, 0, 0, 0);
            return 1;
            break;
        case audioMasterGetTime:
            return 0; // see VstTimeInfo
            break; // FIX ME
        case audioMasterGetCurrentProcessLevel:
            //23. audioMasterGetCurrentProcessLevel   >= VST2.0
            //x[return]:
            //0 = not supported,
            //1 = currently in user thread (gui)
            //2 = currently in audio thread (where process is called)
            //3 = currently in 'sequencer' thread (midi, timer etc)
            //4 = currently offline processing and thus in user thread
            return 2;
            break;
        case audioMasterUpdateDisplay:
            return 1;
            break;
        default:
            return 1;
            //FIX ME :: "Plugin requested value of opcode " << opcode;
            break;
        }
        return 0;
    }
}

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
VstEditor
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

void VstEditor::init()
{
    connect( s_timer, SIGNAL(timeout()), this, SLOT(timeEvent()) );
    s_timer->start( 40 );

    s_fx->dispatcher( s_fx, effEditOpen, 0, 0, this->winId(), 0 );
    ERect* eRect = 0;
    s_fx->dispatcher( s_fx, effEditGetRect, 0, 0, &eRect, 0 );
    if ( eRect )
    {
        int width = eRect->right - eRect->left;
        int height = eRect->bottom - eRect->top;
        if ( width  < 100 ) width  = 100;
        if ( height < 100 ) height = 100;
        this->resize( width, height );
    }
    else
    {
        // Invalid editor
        Q_ASSERT(1);
    }
    show();
}

void VstEditor::unInit()
{
    Q_ASSERT(s_fx);
    s_fx->dispatcher( s_fx, effEditClose, 0, 0, 0, 0 );
}

void VstEditor::hideEvent(QHideEvent *)
{
    Q_ASSERT(s_parent);
    s_parent->vstEditor=0;
    deleteLater();
}

void VstEditor::timeEvent()
{
    Q_ASSERT( s_fx );
    s_fx->dispatcher( s_fx, effEditIdle , 0, 0, 0, 0 );
}

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
Vst
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

void Vst::init()
{
    Q_ASSERT(!rep);
    if (!SecretVst::singleton) new SecretVst;
    Q_ASSERT(SecretVst::singleton);

    Q_UNUSED(lock);

    rep=new VstR(this,SecretVst::singleton->s_loadPlugin(filename));
    const unsigned long& nframes=AudioSys::nFrames();
    channelData = new float*[ rep->_vst->numInputs ];
    outData = new float*[ rep->_vst->numOutputs ];
    for (int i=0;i<rep->_vst->numInputs;i++)
    {
        channelData[i]=new float[nframes];
    }
    for (int i=0;i<rep->_vst->numOutputs;i++)
    {
        outData[i]=new float[nframes];
    }
    chains=new ObjectChain[ rep->_vst->numOutputs ];
    for (int i=0;i<rep->_vst->numOutputs/2;i++)
    {
        VstSidekick* l=new VstSidekick(rep,i,this,this,i*2);
        ObjectStore::set(l); //reg to unload...
        rep->s_sidekicks.push_back(l);

    }
    if (rep)
    {
        show();
    }
}

void Vst::show()
{
    Q_ASSERT(rep);
    if (rep->vstEditor)
    {
        return;
    }
//    Q_ASSERT(!rep->vstEditor);
    rep->vstEditor=new VstEditor(rep->_vst,rep);
}

void Vst::hide()
{
    if (!rep->vstEditor)
    {
        return;
    }
    rep->vstEditor->hide();
}

void Vst::aIn(const float* x, int chan, ObjectChain*s)
{
    Q_UNUSED(lock);
    Q_UNUSED(s);

    //todo: should be mixer.
    const unsigned long& nframes = AudioSys::nFrames();
    for (long frame = 0; frame < nframes; ++frame)
    {
        channelData[chan][frame] = x[frame];
    }
//    chains[chan]=s;
} //////////////////////////////// GOES TO ////////////////////////////////
void Vst::PROC_VST()
{
    Q_UNUSED(lock);

    const unsigned long& nframes=AudioSys::nFrames();

    rep->silenceChannel( outData, rep->_vst->numOutputs, nframes );
    rep->processAudio( channelData, outData, nframes );

    rep->silenceChannel( channelData, rep->_vst->numInputs, nframes );
    for (int i=0;i<rep->_vst->numOutputs;i++)
    {
        chains[i].push_back(this);
        aOut(outData[i],i,chains[i]);
        chains[i].pop_back();
    }
}

void Vst::mIn(const Event *data, ObjectChain*p)
{
    p->push_back(this);
    mOut(data,p);
    p->pop_back();

    if (data->simpleStatus()==-1) return;
    Q_UNUSED(lock);

    Event* ev=new Event;
    *ev=*data;
    rep->midiQueue.push_back(ev);
    rep->sourceQueue.push_back(p->back());   //check this


    //a.events = b;
}

QStringList Vst::getVstPaths()
{
    return s_vstpaths_linux;
//    if (s_vstCache) return *s_vstCache;
//    QStringList vsts;
//    for (int i=0; i<s_paths; i++)
//    {
//        QStringList files = s_paths[i].entryList(QStringList("*.dll"));
//        for (int j=0; j<files; j++)
//        {
//            //verify it's a VST
//            AEffect *a = NULL;
//            void* modulePtr = LoadLibraryA( (char*) ((QString)(s_paths[i].absolutePath()+"\\"+files[j])).toStdString().c_str() );
//            vstPluginFuncPtr mainEntryPoint = (vstPluginFuncPtr) GetProcAddress(
//                                                  (HMODULE) modulePtr, "VSTPluginMain" );
//            if (mainEntryPoint)
//            {
//                FreeLibrary((HINSTANCE__*)modulePtr);
//                vsts.push_back(s_paths[i].absolutePath()+"\\"+files[j]);
//                qDebug()<<s_paths[i].absolutePath()+"\\"+files[j]<<": ADDED!";
//                continue;
//            }
//            if (!mainEntryPoint)
//                mainEntryPoint= (vstPluginFuncPtr)GetProcAddress ((HMODULE) modulePtr, "main");
//            if (!mainEntryPoint)
//            {
//                qDebug()<<s_paths[i].absolutePath()+"\\"+files[j]<<": NOT A VST";
//                FreeLibrary((HINSTANCE__*)modulePtr);
//                continue;
//            }
//            a = mainEntryPoint( hostCallback );
//            if (a->magic!=kEffectMagic)
//            {
//                qDebug()<<s_paths[i].absolutePath()+"\\"+files[j]<<": NOT A VST";
//                FreeLibrary((HINSTANCE__*)modulePtr);
//                continue;
//            }
//            FreeLibrary((HINSTANCE__*)modulePtr);
//            vsts.push_back(s_paths[i].absolutePath()+"\\"+files[j]);
//            qDebug()<<s_paths[i].absolutePath()+"\\"+files[j]<<": ADDED!";
//            continue;
//        }
//    }
//    s_vstCache=new QStringList;
//    return (*s_vstCache)=vsts;
}

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
VstR
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

void VstR::silenceChannel(float **channelData, int numChannels, long numFrames)
{
    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (long frame = 0; frame < numFrames; ++frame)
        {
            channelData[channel][frame] = 0.0f;
        }
    }
}

void VstR::processAudio(float **inputs, float **outputs, long numFrames)
{
    QList<Event*> playNow;
    Time x=MidiSys::getTime()+Time(20);
    for (int i=0; i<midiQueue; i++)
    {
        if (midiQueue[i]->time<x)
        {
            playNow.push_back(midiQueue.takeAt(i));
            i--;
        }
    }

    // fixme
    if (playNow.size())
    {
        midiEvents = (struct VstEvents*) malloc(
                         sizeof(struct VstEvents) + (playNow.size()>=2?playNow.size()+2:0)*sizeof(VstEvent*));
        midiEvents->numEvents = 0;
        midiEventCount=-1;

        for (int i=0; i<playNow; i++)
        {
            ++(midiEvents->numEvents);
            VstMidiEvent* event = new VstMidiEvent; // MEMORY LEAK
            event->type = kVstMidiType;
            event->byteSize = sizeof( VstMidiEvent );
            event->deltaFrames = 0;  // FIX ME:: should be like this _ONLY_ if direct
            event->flags = kVstMidiEventIsRealtime; // FIX ME:: should only be if direct
            event->noteLength = 0;
            event->noteOffset = 0;
            event->midiData[0] = playNow[i]->message;
            event->midiData[1] = playNow[i]->data1;
            event->midiData[2] = playNow[i]->data2;
            event->midiData[3] = 0;
            event->detune = 0;
            event->noteOffVelocity = 0;
            midiEvents->events[++midiEventCount] = (VstEvent*) event;
        }

        if ( midiEvents )
        {
            _vst->dispatcher( _vst, effProcessEvents, 0, 0, midiEvents, 0.0f );
            midiEvents = NULL; // MEMORY LEAK
            midiEventCount = -1;
        }
    }
    _vst->processReplacing( _vst, inputs, outputs, numFrames );
}

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
SecretVst
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

AEffect* SecretVst::s_loadPlugin(QString path)
{
    AEffect *a = NULL;

    void* modulePtr = LoadLibraryA( (char*) path.toStdString().c_str() );

    if (!modulePtr) return 0;

    vstPluginFuncPtr mainEntryPoint = (vstPluginFuncPtr) GetProcAddress(
                                          (HMODULE) modulePtr, "VSTPluginMain" );

    if (!mainEntryPoint)
        mainEntryPoint= (vstPluginFuncPtr)GetProcAddress ((HMODULE) modulePtr, "main");

    if (!mainEntryPoint) {
        FreeLibrary( (HMODULE) modulePtr);
        return 0;
    }

    // Instantiate the plugin
    a = mainEntryPoint( hostCallback );

    s_initPlugin( a );
    if (!a) {
        FreeLibrary( (HMODULE) modulePtr);
        return 0;
    }

    s_resumePlugin( a );
    s_getProperties( a );
    return a;
}

bool SecretVst::isValid(QString path)
{
    if (!singleton) singleton=new SecretVst();
    AEffect* a=singleton->s_loadPlugin(path);
    return a;
}

void SecretVst::s_resumePlugin(AEffect *plugin)
{
    plugin->dispatcher(plugin, effMainsChanged, 0, 1, NULL, 0.0f);
}

void SecretVst::s_initPlugin(AEffect *&plugin)
{
    // Check plugin's magic number
    // If incorrect, then the file either was not loaded properly, is not a real
    // VST plugin, or is otherwise corrupt.
    if (plugin->magic!=kEffectMagic) {
        plugin=0;
        return;
    }

    if (!plugin) return;

    // Set up plugin callback functions
    plugin->getParameter = (getParameterFuncPtr)plugin->getParameter;
    plugin->processReplacing = (processFuncPtr)plugin->processReplacing;
    plugin->setParameter = (setParameterFuncPtr)plugin->setParameter;

    if (!plugin) return;

    plugin->dispatcher(plugin, effOpen, 0, 0, 0, 0);
    plugin->dispatcher(plugin, effSetSampleRate, 0, 0, 0, AudioSys::sampleRate());
    plugin->dispatcher(plugin, effSetBlockSize, 0, AudioSys::nFrames(), 0, 0);

    return;
}

void SecretVst::s_getProperties(AEffect *effect)
{
    char effectName[256] = {0};
    char vendorString[256] = {0};
    char productString[256] = {0};

    effect->dispatcher (effect, effGetEffectName, 0, 0, effectName, 0);
    effect->dispatcher (effect, effGetVendorString, 0, 0, vendorString, 0);
    effect->dispatcher (effect, effGetProductString, 0, 0, productString, 0);

    // Iterate programs...
    for (VstInt32 progIndex = 0; progIndex < effect->numPrograms; progIndex++)
    {
        char progName[256] = {0};
        if (!effect->dispatcher (effect, effGetProgramNameIndexed, progIndex, 0, progName, 0))
        {
            effect->dispatcher (effect, effSetProgram, 0, progIndex, 0, 0); // Note: old program not restored here!
            effect->dispatcher (effect, effGetProgramName, 0, 0, progName, 0);
        }
        //"Program %03d: %s\n" << progIndex << progName;
    }

    // Iterate parameters...
    for (VstInt32 paramIndex = 0; paramIndex < effect->numParams; paramIndex++)
    {
        char paramName[256] = {0};
        char paramLabel[256] = {0};
        char paramDisplay[256] = {0};

        effect->dispatcher (effect, effGetParamName, paramIndex, 0, paramName, 0);
        effect->dispatcher (effect, effGetParamLabel, paramIndex, 0, paramLabel, 0);
        effect->dispatcher (effect, effGetParamDisplay, paramIndex, 0, paramDisplay, 0);
        //float value = effect->getParameter (effect, paramIndex);

        //"Param %03d: %s [%s %s] (normalized = %f)\n" << paramIndex << paramName << paramDisplay << paramLabel << value;
    }

    // Can-do nonsense...
    /*static const char* canDos[] = {
            "receiveVstEvents",
            "receiveVstMidiEvent",
            "midiProgramNames"
    };*/

    /*for (uint canDoIndex = 0; canDoIndex < sizeof (canDos) / sizeof (canDos[0]); canDoIndex++) {
            VstInt32 result = (VstInt32)effect->dispatcher (effect, effCanDo, 0, 0, (void*)canDos[canDoIndex], 0);
            switch (result) {
                    case 0  : ("don't know"); break;
                    case 1  : ("yes"); break;
                    case -1 : ("definitely not!"); break;
                    default : ("?????");
            }
            ("\n");
    }*/
}

bool SecretVst::s_canPluginDo(AEffect *plugin, char *canDoString)
{
    return ((dispatcherFuncPtr)(plugin->dispatcher))( plugin, effCanDo, 0, 0, (void*) canDoString, 0.0f ) > 0;
}

#endif

