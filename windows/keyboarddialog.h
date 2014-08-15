#ifndef KEYBOARDDIALOG_H
#define KEYBOARDDIALOG_H

#include <QDialog>

enum KeyboardInputMode { KEYBOARD_INPUT, NUMBER_INPUT, NUMBER_DECIMAL_INPUT, NUMBER_DATETIME_INPUT, SYMBOL_INPUT };

using namespace std;

namespace Ui {
class KeyboardDialog;
}

class KeyboardDialog : public QDialog
{
    Q_OBJECT

public:
    typedef void (*FinishedCallback) (string);
    explicit KeyboardDialog(QWidget *parent = 0);
    ~KeyboardDialog();

    static KeyboardDialog* Instance;

    static string GetString(string label, string startingInput = "", KeyboardInputMode mode = KEYBOARD_INPUT, bool lockMode = false, bool password = false);
    static bool TryGetString(string &val, string label, string startingInput = "", KeyboardInputMode mode = KEYBOARD_INPUT, bool lockMode = false, bool password = false);
    static double GetDouble(string label, double startingInput = 0);
    static bool TryGetDouble(double &val, string label, double startingInput = 0);
    static int GetInt(string label, int startingInput = 0);
    static bool TryGetInt(int &val, string label, int startingInput = 0);
    static bool VerifyPassword();


protected:
    void closeEvent(QCloseEvent *);

private slots:
    void onKeyClicked();
    void on_pushButton_Delete_clicked();
    void on_pushButton_Shift_clicked();
    void on_pushButton_ChangeKeyboard_clicked();
    void on_pushButton_Space_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Enter_clicked();

private:
    Ui::KeyboardDialog *ui;

    bool shift;
    bool cancelled;
    string original;
    vector<string>* currentKeys;
    KeyboardInputMode currentMode;

    vector<QPushButton*> buttons;

    vector<string> keyboardKeys;
    vector<string> keyboardCapsKeys;
    vector<string> numberKeys;
    vector<string> numberDecimalKeys;
    vector<string> numberDatetimeKeys;
    vector<string> symbolKeys;


    void insertString(string);
    void updateKeys();
    void setMode(KeyboardInputMode);
};

#endif // KEYBOARDDIALOG_H
