#ifndef MIDIWIZARD_H
#define MIDIWIZARD_H

#include <QWizard>

namespace Ui {
class MidiWizard;
}

class MidiWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit MidiWizard(QWidget *parent = 0);
    ~MidiWizard();
    
private:
    Ui::MidiWizard *ui;
};

#endif // MIDIWIZARD_H
