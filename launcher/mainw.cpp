#include "mainw.h"
#include "ui_mainw.h"
#include <QPropertyAnimation>
#include <QNetworkReply>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#ifdef _WIN32
#include <Windows.h>
#include <ShellAPI.h>
#endif

static QProcess jackProcess;

MainW::MainW(QWidget *parent)
  : QWidget(parent)
  , m_channels()
  , m_serverQueue()
  , m_localQueue()
  , m_newVerQueue()
  , m_prog(0)
  , m_total(1)
  , ui(new Ui::MainW)
{
    qApp->setApplicationName("Live");
    qApp->setOrganizationName("Nettek");
    QPropertyAnimation* a =new QPropertyAnimation(this,"windowOpacity");
    a->setStartValue(0.0);
    a->setEndValue(0.95);
    a->setDuration(1000);
    a->start(QAbstractAnimation::DeleteWhenStopped);
    ui->setupUi(this);
    connect(ui->start,SIGNAL(clicked()),this,SLOT(update()));
    setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint);
    connect(ui->close,SIGNAL(linkActivated(QString)),qApp,SLOT(quit()));
    ui->start->hide();
    ui->progressBar->hide();
    ui->secondary_label->hide();
    ui->primary_label->setText("Searching for the latest version of Creator Live...");

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(gotChannel(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl("http://nettek.ca/live-alpha/channel.txt")));

    ui->asettings->hide();
}

void MainW::closeEvent(QCloseEvent *)
{
    qApp->quit();
}

void MainW::gotChannel(QNetworkReply *r)
{
    QSettings sett;
    QString homePath=QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QDir dir;
    dir.mkpath(homePath);
    QVariantMap vm=sett.value("vmap").toMap();
    if(r->error()!=QNetworkReply::NoError) {
        if(!sett.value("isInstalled",false).toBool()) {
            ui->primary_label->setText("Could not find the latest version of Creator Live! Try again later.");
        } else {
            start();
        }
        return;
    }
    m_channels=r->readAll();
    QStringList a=m_channels.split("\n");
    bool begun=0;
    bool ok=1;
    do {
        ok=1;
        for(int i=0;i<a.size();i++) {
            while(a[i].contains('\r')) a[i].remove('\r');
            if(a[i].startsWith("CNL")) {
                if(a[i].endsWith("JACK32")) {
                    begun=1;
                } else if(begun) {
                    break;
                }
                continue;
            }
            if(!begun) continue;

            if(a[i].contains('#')) a[i].truncate(a[i].indexOf('#'));
            if(a[i].isEmpty()) continue;

            if(a[i].count("| ")==2) {
                QList<QString> v=a[i].split("| ");
                int ver=v[0].toInt();
                QString webPath=v[1];
                QString localPath=v[2];
                localPath.replace("HOME",homePath);
                if(!webPath.contains("http")) webPath="http://nettek.ca/live-alpha/"+webPath;
                webPath.remove('\t');
                localPath.remove('\t');
                while(webPath.endsWith(' ')) webPath.chop(1);
                while(localPath.endsWith(' ')) localPath.chop(1);
                if(vm[localPath].toInt()<ver||!QFile::exists(localPath)) {
                    m_serverQueue.push_back(webPath);
                    m_localQueue.push_back(localPath);
                    m_newVerQueue.push_back(ver);
                }
            }
        }

        if(!m_localQueue.size()) {
            if(!sett.value("isInstalled",false).toBool()) {
                ui->primary_label->setText("Something got messed up either with your configuration or on the Live site. Trying to reinstall...\nIf that doesn't work, contact joshua@nettek.ca");
                sett.setValue("vmap",QVariantMap());
                ok=0;
            } else {
                start();
                return;
            }
        }


        r->deleteLater();
        if(!sett.value("isInstalled",false).toBool()) {
            initialWelcome();
        } else {
            upgradeWelcome();
        }
        break;
    } while(!ok);
}

void MainW::initialWelcome()
{
    ui->widget_2->hide();
    ui->start->show();
    ui->secondary_label->show();
}

void MainW::upgradeWelcome()
{
    ui->widget_2->hide();
    ui->start->setText("Update now");
    ui->start->show();
    ui->secondary_label->setText("or, if you don't like awesome stuff, <A href=\"start\">upgrade later");
    ui->secondary_label->show();
    connect(ui->secondary_label,SIGNAL(linkActivated(QString)),this,SLOT(start()));
}

void MainW::update()
{
    ui->close->setText("<A href=\"quit\">Abort installation...");
    ui->primary_label->setText("Downloading cool stuff...");
    ui->secondary_label->setText("...like a sequencer, and a looper, and a sampler...");
    {
        QPropertyAnimation* a =new QPropertyAnimation(ui->start,"geometry");
        a->setStartValue(QRect(232,280,180,51));
        a->setEndValue(QRect(232,280+51/2,180,0));
        a->setDuration(200);
        a->start(QAbstractAnimation::DeleteWhenStopped);
    }

    {
        QPropertyAnimation* a =new QPropertyAnimation(ui->widget_2,"geometry");
        a->setStartValue(QRect(0,312,680,0));
        a->setEndValue(QRect(0,292,680,40));
        a->setDuration(200);
        a->start(QAbstractAnimation::DeleteWhenStopped);
        ui->widget_2->show();
    }
    ui->progressBar->show();
    m_prog=0; m_total=m_serverQueue.size();
    ui->progressBar->setMaximum(m_total*2);
    incr(0);
}

void MainW::incr(QNetworkReply*r)
{
    Q_ASSERT(m_serverQueue.size()==m_localQueue.size());
    if(r) {
        Q_ASSERT(sender());
        if(r->error()!=QNetworkReply::NoError) {
            ui->progressBar->hide();
            qDebug()<<r->errorString();
            ui->close->setText("<A href=\"close\"><font color=red>X");
            ui->primary_label->setText("An error occured while downloading "+sender()->property("SERVER").toString()+". Try again later.");
            return;
        }
        QString path=sender()->property("LOCAL").toString();
        if(path.lastIndexOf("\\")>path.lastIndexOf("/")) path.truncate(path.lastIndexOf("\\"));
        else if(path.contains("/")) path.truncate(path.lastIndexOf("/"));

        QDir dir;
        dir.mkpath(path);

        QFile f(sender()->property("LOCAL").toString());
        if (!f.open(QFile::WriteOnly)) {
            ui->progressBar->hide();
            ui->close->setText("<A href=\"close\"><font color=red>X");
            ui->primary_label->setText("An error occured while saving content. Try again later.");
            return;
        }
        f.write(r->readAll());
        f.close();

        ui->progressBar->setValue(++m_prog);
        if(m_prog>0) {
            ui->secondary_label->setText("...and an arpeggiator...");
        }
        if(m_prog>1) {
            ui->secondary_label->setText("...and a metronome...");
        }
        if(m_prog>2) {
            ui->secondary_label->setText("...and a beat detector...");
        }
        if(m_prog>3) {
            ui->secondary_label->setText("...and a really cool MIDI splitter...");
        }
        if(m_prog>4) {
            ui->secondary_label->setText("...and a thingy for loading VSTs...");
        }
        if(m_prog>5) {
            ui->secondary_label->setText("...and lots more!");
        }

        QSettings sett;
        QVariantMap vm=sett.value("vmap").toMap();
        vm.insert(sender()->property("LOCAL").toString(),QVariant(sender()->property("VERSION").toInt()));
        sett.setValue("vmap",vm);

        QTimer::singleShot(221,this,SLOT(incr()));  //don't flood the server
        return;
    } else if(m_serverQueue.size()) {
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(incr(QNetworkReply*)));

        manager->setProperty("SERVER",m_serverQueue.first());

        manager->setProperty("LOCAL",m_localQueue.takeFirst());
        manager->setProperty("VERSION",m_newVerQueue.takeFirst());
        manager->get(QNetworkRequest(QUrl(m_serverQueue.takeFirst())));
    } else {
        start();
    }
}

QString MainW::exec(QString cmd)
{
    qDebug()<<"EXEC"<<cmd;
    QStringList sl=cmd.split(' ');
    for(int i=0;i<sl.size();i++)
    {
        if(sl[i].startsWith("\""))
        {
            while(sl[i].startsWith("\"")&&(i+1<sl.size())&&!sl[i].endsWith("\""))
            {
                sl[i]+=" "+sl.takeAt(i+1);
            }
            sl[i].remove(0,1);
            sl[i].chop(1);
        }
    }
    if(!sl.size())
    {
        qDebug()<<"Invalid call to exec(...)";
        return "ERROR";
    }
    if(sl[0].startsWith("jack")) {
        qDebug()<<QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/jack32/";
        sl[0]="\""+QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/jack32/"+sl[0]+"\"";
    }
    QProcess* proc=(cmd.contains("-p"))?(&jackProcess):new QProcess;
    proc->setWorkingDirectory( QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/jack32/");
    proc->start(sl.takeFirst(),sl,QIODevice::ReadOnly);
    if(!proc->waitForStarted(3000))
    {
        QMessageBox::critical(this,"Can't talk to otherApp","Please ensure that Creator Live is installed correctly",QMessageBox::Close);
        return "ERROR";
    }
    QString result = "";
    if (!cmd.contains("-p"))
    {
        while(!(proc->atEnd()&&(proc->state()==QProcess::NotRunning)))
        {
            bool ok=1;
            while(proc->atEnd()) {
                if(proc->state()==QProcess::NotRunning)
                {
                    ok=0;
                    break;
                }
                proc->waitForReadyRead(300);
            }
            if(!ok)
            {
                break;
            }
            result+=proc->readLine();
            if(result.indexOf("End of list")!=-1)
            {
                break;
            }
        }
        delete proc;
    }
    //    if (!cmd.contains("-p"))  // hack, but works
    //    {
    //        while( !feof(pipe) )
    //        {
    //
    //    qApp->processEvents();
    //            if(fgets(buffer, 128, pipe) != NULL) result += buffer;
    //            if ( result.indexOf("End of list") != -1 ) break;
    //        }
    //    }
    //    pclose(pipe);
    return result;
}

QList<int> parseAutoType( QString a )
{
    QList<int> device;

    QTextStream ss(&a);
    QString l;
    bool good=0;
    while( l = ss.readLine(), l != "" )
    {
        QStringList ll = l.split(QRegExp("\\s+"));
        if ( ll.size() > 2 && ll[0] == "Name" )
        {
            if((good=l.contains("ASIO")))
            {
                device.push_back(0);
            }
        }

        if (!good)
        {
            continue;
        }

        if ( ll.size() > 2 && ll[0] == "Max" && ll[1]=="inputs" )
        {
            device.back()+=(ll[3].toInt()==0)?0:1;
        }
        else if ( ll.size() > 2 && ll[0] == "Max" && ll[1]=="outputs" )
        {
            device.back()+=(ll[3].toInt()==0)?0:2;
        }
    }

    return device;
}


QStringList parse( QString a )
{
    QStringList device;

    QTextStream ss(&a);
    QString l;
    while( l = ss.readLine(), l != "" )
    {
        QStringList ll = l.split(QRegExp("\\s+"));
        if ( ll.size() > 2 && ll[0] == "Name" )
        {
            QString ds;
            for ( int i = 2; i < ll.size(); i++ )
            {
                if ( i != 2 ) ds += ' ';
                ds += ll[i];
            }
            device.append( ds );
        }
    }

    return device;
}

int admin(QString command,QString param)
{
    Q_UNUSED(command);
    Q_UNUSED(param);
#ifdef _WIN32
    SHELLEXECUTEINFO shExecInfo;

    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

    shExecInfo.fMask = NULL;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = "runas";

    char* lol = new char[command.size()+1];
    strcpy(lol, command.toUtf8().data());

    char* lo2 = new char[param.size()+1];
    strcpy(lo2, param.toUtf8().data());

    lol[command.size()]='\0';
    lo2[param.size()]='\0';
    shExecInfo.lpFile = lol;
    shExecInfo.lpParameters = lo2;
    shExecInfo.lpDirectory = NULL;
    shExecInfo.nShow = SW_NORMAL;
    shExecInfo.hInstApp = NULL;

    ShellExecuteEx(&shExecInfo);

    delete[]lol;
    delete[]lo2;
#endif
    return 0;
}

QList<int> asioDeviceTypes;

QStringList MainW::getASIODevices()
{
#ifdef _WIN32
    QString data = exec( "jackd -d portaudio -l" );
    qDebug()<<"XX"<<data;
    for(int i=0; i<200; i++)
    {
        qApp->processEvents();
        Sleep(1);
    }
    exec( "taskkill /F /IM jackd.exe" );
    asioDeviceTypes = parseAutoType( data );
    QStringList p = parse( data );
    for(int i=0;i<p.size();i++) {
        if(!p[i].contains("ASIO::")) {
            p.removeAt(i--);
        }
    }
    return p;
#endif
    return QStringList();
}


void MainW::start()
{
#ifdef _WIN32
    exec( "taskkill /F /IM jackd.exe" );
    Sleep(200);
#endif

    ui->progressBar->hide();
    ui->widget_2->show();
    ui->start->hide();
    ui->secondary_label->setText("");
    ui->primary_label->setText("Searching for ASIO devices...");
    QSettings sett;
    sett.setValue("isInstalled",true);
    QStringList asio=getASIODevices();

    if(!asio.size())
    {
        ui->close->setText("<A href=\"close\"><font color=red>X");

        if(QMessageBox::information(this,"No ASIO driver found","You need an ASIO driver to use Creator Live.\n\n"
                                 "You currently don't have one installed, but Creator Live can download and install ASIO4ALL.\n\n"
                                 "Note that ASIO4ALL is a distinct software unrelated to Creator Live. It is not part of Creator Live. "
                                 "ASIO4ALL is a freeware software by Michael Tippach, and you don't need Creator Live to use ASIO4ALL. Learn more at http://asio4all.com.\n\n"
                                 "Would you like Creator Live to download and install ASIO4ALL?",QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes ||
            QMessageBox::information(this,"Terms and conditions",
                        "Do you agree to the following terms and conditions?\n"
                                     "--------------------------------------------------------------------------------------------------\n"
                        "ASIO4ALL - Universal ASIO driver for WDM sound cards\n"
                    "--------------------------------------------------------------------------------------------------\n"
                        "\n"
                    "THIS SOFTWARE IS PROVIDED \"AS IS\". IN NO EVENT SHALL I, THE AUTHOR, BE LIABLE FOR ANY KIND OF LOSS OR DAMAGE ARISING OUT OF THE USE, ABUSE OR THE INABILITY TO USE THIS SOFTWRAE, NEITHER SHALL CO-AUTHORS AND CONTRIBUTORS. USE IT AT YOUR OWN RISK!\n"
                        "\n"
                    "THIS SOFTWARE COMES WITHOUT ANY KIND OF WARRANTY, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.\n"
                        "\n"
                    "THIS SOFTWARE IS FREEWARE. NON- EXCLUSIVE PERMISSION TO USE THIS SOFTWARE IN ITS UNMODIFIED FORM HEREBY GRANTED. 'USE' SHALL BE DEFINED AS THE UTILISATION OF THIS SOFTWARE FOR ITS INTENDED END USER PURPOSE ONLY.\n"
                        "\n"
                    "YOU MAY FURTHER REDISTRIBUTE ONLY VERBATIM COPIES OF THIS SOFTWARE WITH THE FOLLOWING RESTRICTIONS:\n"
                        "\n"
                    "1.) THE SOLE SOURCE OF PROFIT TO BE MADE (IF ANY) - INCLUDING PROFIT INTENTED TO BE MADE - FROM THIS SOFTWARE SHALL BE THE SERVICE OF ITS REDISTRIBUTION ONLY. THIS SPECIFICALLY PROHIBITS PLACING SOFTWARE ON ANY MEDIA SOLD TOGETHER WITH AUDIO HARD- AND SOFTWARE, OR SIMILAR PRODUCTS TO WHICH THIS SOFTWARE WOULD ADD SUBSTANTIAL MARKET VALUE.\n"
                    "2.) EXCEPTIONS TO 1. REQUIRE WRITTEN CONSENT FROM THE AUTHOR.\n"
                        "\n"
                    "IF YOU DO NOT AGREE WITH THESE TERMS OR IF YOUR JURISTDICTION DOES NOT ALLOW THE EXCLUSION OF WARRANTY AND LIABILITY AS STATED ABOVE YOU ARE NOT ALLOWED TO USE THIS SOFTWARE AT ALL.\n"
                        "\n"
                    "THIS SOFTWARE IS COPYRIGHT(c)2003-2011, MICHAEL TIPPACH, ALL RIGHTS RESERVED.\n"
        , QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes) {
            ui->primary_label->setText("No ASIO driver found, and you did not give permission for Live to install ASIO4ALL.");
            return;
            }

        ui->primary_label->setText("Attempting to download generic ASIO driver...");

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(installAsio4All(QNetworkReply*)));
        manager->get(QNetworkRequest(QUrl("http://nettek.ca/live-alpha/asio4all.exe")));
        return;
    } else {
        ui->widget_2->hide();

        QSettings sett;
        bool dontAskAudioSettings=sett.value("dontAskAudioSettings",false).toBool();
        QString deviceName=sett.value("asioName").toString();
        int fp=sett.value("framesPerPeriod",256).toInt();
        int fr=sett.value("frameRate",44100).toInt();
        int mode=sett.value("mode",0).toInt();
        if(asio.contains(deviceName)&&dontAskAudioSettings) {
            start_asio(deviceName,fp,fr,mode,1);
        } else {
            ui->asettings->setASIOs(asio);
            ui->asettings->setFP(fp);
            ui->asettings->setFR(fr);
            ui->asettings->setMode(mode);
            ui->asettings->show();
            connect(ui->asettings,SIGNAL(launchRequested(QString,int,int,int,bool)),this,SLOT(start_asio(QString,int,int,int,bool)));
        }
    }
}

void MainW::start_asio(QString dev,int fp,int sr,int mode,bool again)
{
    ui->asettings->hide();
    ui->widget_2->show();
    ui->primary_label->setText("Starting Audio Server...");
    QSettings settings;
    if(!settings.value("dontAskAudioSettings",false).toBool()&&again) {
        QMessageBox::information(this,"Defaults","If you need to change the audio setup, you can do so by choosing \"Audio Setup\" under the Live menu.");
    }


    settings.setValue("asioName",dev);
    settings.setValue("framesPerPeriod",fp);
    settings.setValue("frameRate",sr);
    settings.setValue("mode",mode);
    settings.setValue("dontAskAudioSettings",again);



#ifdef _WIN32
    exec( "taskkill /F /IM jackd.exe" );
    Sleep(200);
#endif

    qDebug()<<QString::number(settings.value("framesPerPeriod",256).toInt());

    if(mode==0) { //duplex
        exec( QString("jackd -S -d portaudio -p " +
                      QString::number(settings.value("framesPerPeriod",256).toInt()) + QString(" -r ") +
                      QString::number(settings.value("frameRate",44100).toInt()) + QString(" -D -d") +
                      " \"" + settings.value("asioName",QString("")).toString() + "\" " //+
//                      "-P" + " \"" + settings.value("asioName",QString("")).toString() + "\""
                      ).toLatin1());
    } else {
        settings.setValue("dontAskAudioSettings",0);
        QMessageBox::information(this,"Error","Live only supports duplex mode. (To be fixed.)");
        qApp->quit();
    }
#ifdef _WIN32
    while(jackProcess.state()==QProcess::Starting) Sleep(100);
    Sleep(300);
#endif
    if(jackProcess.state()==QProcess::NotRunning) {
        ui->primary_label->setText("Could not start audio server. Try again.");
        settings.setValue("dontAskAudioSettings",0);
        return;
    }

    setWindowFlags(Qt::Window);
    show();
    setWindowState(Qt::WindowMinimized);
    ui->primary_label->setText("dev mode activated...");

//    int c=0;
//    while (!( SecretAudio::singleton->client = jack_client_open("Hathor", JackNullOption, NULL) ))
//    {
//        if(++c==20)
//        {
//            exit(1);
//        }
//        else
//        {
//            Sleep(500);
//        }
//    }
//    jack_set_procesm_callback( getJackClient(), SecretAudio::jackCallback, 0 );
//    jack_activate( getJackClient() );    // before refresh
//    SecretAudio::singleton->refresh();
}

void MainW::installAsio4All(QNetworkReply* r) {
    if(!r||(r->error()!=QNetworkReply::NoError)) {
        ui->primary_label->setText("Could not download generic ASIO driver...");
    }
    QString p=QStandardPaths::writableLocation(QStandardPaths::TempLocation)+"/asio4all.exe";
    if(QFile::exists(p)) {
        QDir d;
        qDebug()<<d.remove(p);
    }
    QFile f(p);
    if(!f.open(QFile::WriteOnly)) {
        ui->primary_label->setText("Try running Live again as an adminstrator.");
        return;
    }
    f.write(r->readAll());
    f.flush();
    f.close();

    hide();
    QString homePath=QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    qDebug()<<isActiveWindow();
    admin(p,"/S /D="+homePath+"");
    bool ok=0;
    show();
    ui->primary_label->setText("Waiting for installation to complete.");
    qApp->processEvents();
    for(int i=0;i<8;i++) {
#ifdef _WIN32
        Sleep(1000);
        if(getASIODevices().size()) { ok=1; break; }
#endif
    }
    if(ok) {
        start();
    } else {
        show();
        ui->primary_label->setText("Couldn't install generic ASIO driver... Try again later.");
    }
}

MainW::~MainW()
{
    delete ui;
}
