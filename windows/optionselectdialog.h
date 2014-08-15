#ifndef OPTIONSELECTDIALOG_H
#define OPTIONSELECTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>

using namespace std;

namespace Ui {
class OptionSelectDialog;
}

class OptionSelectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptionSelectDialog(QWidget *parent = 0);
    ~OptionSelectDialog();

    static OptionSelectDialog* Instance;


    static int GetChoice(string title, vector<string> choices, int selectedIndex = -1);
    static bool TryGetChoice(int &val, string label, vector<string> choices, int selectedIndex = -1);

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void itemClicked();
    
    void on_pushButton_Enter_clicked();

    void on_pushButton_Cancel_clicked();

private:
    Ui::OptionSelectDialog *ui;
    int selectedIndex;
    QVBoxLayout* layout;
    vector<QPushButton*> buttons;

    QString selectedStylesheet;
    QString normalStylesheet;
};

#endif // OPTIONSELECTDIALOG_H
