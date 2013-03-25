#ifndef MAINW_H
#define MAINW_H

#include <QWidget>

namespace Ui {
class MainW;
}
class QNetworkReply;

class MainW : public QWidget
{
    Q_OBJECT
    
    QString m_channels;
    QList<QString> m_serverQueue;
    QList<QString> m_localQueue;
    QList<int> m_newVerQueue;
    int m_prog;
    int m_total;

public:
    explicit MainW(QWidget *parent = 0);
    ~MainW();

    void closeEvent(QCloseEvent *);

public slots:
    void gotChannel(QNetworkReply*);
    void initialWelcome();
    void upgradeWelcome();
    void update();
    void incr(QNetworkReply*r=0);
    void start_asio(QString,int,int,int,bool);
    void installAsio4All(QNetworkReply* r);

    void start();

public:
    QStringList getASIODevices();
    QString exec(QString r);
    
private:
    Ui::MainW *ui;

    Q_DISABLE_COPY(MainW)
};

#endif // MAINW_H
