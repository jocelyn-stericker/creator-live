#ifndef AUDIOSETTINGSWIDGET_H
#define AUDIOSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class AudioSettingsWidget;
}

class AudioSettingsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit AudioSettingsWidget(QWidget *parent = 0);
    ~AudioSettingsWidget();

public slots:
    void setASIOs(QStringList);
    void setFP(int fp);
    void setFR(int fr);
    void setMode(int mode);
    void launch();
signals:
    void launchRequested(QString asio,int a,int b,int c, bool dontAskAgain);
    
private:
    Ui::AudioSettingsWidget *ui;
};

#endif // AUDIOSETTINGSWIDGET_H
