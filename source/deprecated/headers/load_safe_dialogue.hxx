#ifndef LOAD_SAFE_DIALOGUE_HXX
#define LOAD_SAFE_DIALOGUE_HXX

#include <QDialog>

namespace Ui {
    class LoadSafeDialogue;
}

class LoadSafeDialogue : public QDialog {
private:
    Ui::LoadSafeDialogue* ui;
    Q_OBJECT

public:
    explicit LoadSafeDialogue(QWidget* parent = nullptr);
    ~LoadSafeDialogue();
};

#endif // LOAD_SAFE_DIALOGUE_HXX
