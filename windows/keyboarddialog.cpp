#include "keyboarddialog.h"
#include "ui_keyboarddialog.h"
#include <QPushButton>
#include <QCloseEvent>
#include <data/settings.h>
#include "app.h"

KeyboardDialog* KeyboardDialog::Instance;

KeyboardDialog::KeyboardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyboardDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    this->buttons.push_back(this->ui->pushButton_Q);
    this->buttons.push_back(this->ui->pushButton_W);
    this->buttons.push_back(this->ui->pushButton_E);
    this->buttons.push_back(this->ui->pushButton_R);
    this->buttons.push_back(this->ui->pushButton_T);
    this->buttons.push_back(this->ui->pushButton_Y);
    this->buttons.push_back(this->ui->pushButton_U);
    this->buttons.push_back(this->ui->pushButton_I);
    this->buttons.push_back(this->ui->pushButton_O);
    this->buttons.push_back(this->ui->pushButton_P);
    this->buttons.push_back(this->ui->pushButton_A);
    this->buttons.push_back(this->ui->pushButton_S);
    this->buttons.push_back(this->ui->pushButton_D);
    this->buttons.push_back(this->ui->pushButton_F);
    this->buttons.push_back(this->ui->pushButton_G);
    this->buttons.push_back(this->ui->pushButton_H);
    this->buttons.push_back(this->ui->pushButton_J);
    this->buttons.push_back(this->ui->pushButton_K);
    this->buttons.push_back(this->ui->pushButton_L);
    this->buttons.push_back(this->ui->pushButton_Z);
    this->buttons.push_back(this->ui->pushButton_X);
    this->buttons.push_back(this->ui->pushButton_C);
    this->buttons.push_back(this->ui->pushButton_V);
    this->buttons.push_back(this->ui->pushButton_B);
    this->buttons.push_back(this->ui->pushButton_N);
    this->buttons.push_back(this->ui->pushButton_M);

    this->keyboardKeys.push_back("q");
    this->keyboardKeys.push_back("w");
    this->keyboardKeys.push_back("e");
    this->keyboardKeys.push_back("r");
    this->keyboardKeys.push_back("t");
    this->keyboardKeys.push_back("y");
    this->keyboardKeys.push_back("u");
    this->keyboardKeys.push_back("i");
    this->keyboardKeys.push_back("o");
    this->keyboardKeys.push_back("p");
    this->keyboardKeys.push_back("a");
    this->keyboardKeys.push_back("s");
    this->keyboardKeys.push_back("d");
    this->keyboardKeys.push_back("f");
    this->keyboardKeys.push_back("g");
    this->keyboardKeys.push_back("h");
    this->keyboardKeys.push_back("j");
    this->keyboardKeys.push_back("k");
    this->keyboardKeys.push_back("l");
    this->keyboardKeys.push_back("z");
    this->keyboardKeys.push_back("x");
    this->keyboardKeys.push_back("c");
    this->keyboardKeys.push_back("v");
    this->keyboardKeys.push_back("b");
    this->keyboardKeys.push_back("n");
    this->keyboardKeys.push_back("m");

    this->keyboardCapsKeys.push_back("Q");
    this->keyboardCapsKeys.push_back("W");
    this->keyboardCapsKeys.push_back("E");
    this->keyboardCapsKeys.push_back("R");
    this->keyboardCapsKeys.push_back("T");
    this->keyboardCapsKeys.push_back("Y");
    this->keyboardCapsKeys.push_back("U");
    this->keyboardCapsKeys.push_back("I");
    this->keyboardCapsKeys.push_back("O");
    this->keyboardCapsKeys.push_back("P");
    this->keyboardCapsKeys.push_back("A");
    this->keyboardCapsKeys.push_back("S");
    this->keyboardCapsKeys.push_back("D");
    this->keyboardCapsKeys.push_back("F");
    this->keyboardCapsKeys.push_back("G");
    this->keyboardCapsKeys.push_back("H");
    this->keyboardCapsKeys.push_back("J");
    this->keyboardCapsKeys.push_back("K");
    this->keyboardCapsKeys.push_back("L");
    this->keyboardCapsKeys.push_back("Z");
    this->keyboardCapsKeys.push_back("X");
    this->keyboardCapsKeys.push_back("C");
    this->keyboardCapsKeys.push_back("V");
    this->keyboardCapsKeys.push_back("B");
    this->keyboardCapsKeys.push_back("N");
    this->keyboardCapsKeys.push_back("M");

    this->numberKeys.push_back("1");
    this->numberKeys.push_back("2");
    this->numberKeys.push_back("3");
    this->numberKeys.push_back("4");
    this->numberKeys.push_back("5");
    this->numberKeys.push_back("6");
    this->numberKeys.push_back("7");
    this->numberKeys.push_back("8");
    this->numberKeys.push_back("9");
    this->numberKeys.push_back("0");

    this->numberDecimalKeys.push_back("1");
    this->numberDecimalKeys.push_back("2");
    this->numberDecimalKeys.push_back("3");
    this->numberDecimalKeys.push_back("4");
    this->numberDecimalKeys.push_back("5");
    this->numberDecimalKeys.push_back("6");
    this->numberDecimalKeys.push_back("7");
    this->numberDecimalKeys.push_back("8");
    this->numberDecimalKeys.push_back("9");
    this->numberDecimalKeys.push_back("0");
    this->numberDecimalKeys.push_back(".");

    this->numberDatetimeKeys.push_back("1");
    this->numberDatetimeKeys.push_back("2");
    this->numberDatetimeKeys.push_back("3");
    this->numberDatetimeKeys.push_back("4");
    this->numberDatetimeKeys.push_back("5");
    this->numberDatetimeKeys.push_back("6");
    this->numberDatetimeKeys.push_back("7");
    this->numberDatetimeKeys.push_back("8");
    this->numberDatetimeKeys.push_back("9");
    this->numberDatetimeKeys.push_back("0");
    this->numberDatetimeKeys.push_back("/");
    this->numberDatetimeKeys.push_back(":");

    this->symbolKeys.push_back(".");
    this->symbolKeys.push_back(",");
    this->symbolKeys.push_back("!");
    this->symbolKeys.push_back("@");
    this->symbolKeys.push_back("#");
    this->symbolKeys.push_back("$");
    this->symbolKeys.push_back("%");
    this->symbolKeys.push_back("^");
    this->symbolKeys.push_back("&");
    this->symbolKeys.push_back("*");
    this->symbolKeys.push_back("(");
    this->symbolKeys.push_back(")");
    this->symbolKeys.push_back("-");
    this->symbolKeys.push_back("_");
    this->symbolKeys.push_back("=");
    this->symbolKeys.push_back("+");
    this->symbolKeys.push_back("/");
    this->symbolKeys.push_back("?");
    this->symbolKeys.push_back("<");
    this->symbolKeys.push_back(">");
    this->symbolKeys.push_back("|");
    this->symbolKeys.push_back(":");
    this->symbolKeys.push_back(";");
    this->symbolKeys.push_back("[");
    this->symbolKeys.push_back("]");
    this->symbolKeys.push_back("~");

    // Init keys
    for(int i=0; i<buttons.size(); i++)
    {
        QPushButton* button = buttons[i];
        connect(button, SIGNAL(clicked()), this, SLOT(onKeyClicked()));
    }
}

void KeyboardDialog::onKeyClicked()
{
    QPushButton* button = (QPushButton*)(QObject::sender());

    for(int i=0; i<(int)buttons.size(); i++)
    {
        if (this->buttons[i] == button)
        {
            insertString(currentKeys->at(i));
            return;
        }
    }
}

string KeyboardDialog::GetString(string label, string startingInput, KeyboardInputMode mode, bool lockMode, bool password)
{
    if (KeyboardDialog::Instance == NULL)
        KeyboardDialog::Instance = new KeyboardDialog();

    if (App::Fullscreen)
        KeyboardDialog::Instance->move(40, 40);
    else
        KeyboardDialog::Instance->move(App::WindowX + 40, App::WindowY + 40);
    KeyboardDialog::Instance->show();

    KeyboardDialog::Instance->setWindowTitle(QString(label.c_str()));
    KeyboardDialog::Instance->ui->label->setText(label.c_str());
    KeyboardDialog::Instance->ui->label->setSelection(0, label.length());
    KeyboardDialog::Instance->original = startingInput;
    KeyboardDialog::Instance->ui->inputField->setText(startingInput.c_str());
    KeyboardDialog::Instance->ui->pushButton_ChangeKeyboard->setVisible(lockMode == false);
    KeyboardDialog::Instance->ui->inputField->setEchoMode(password ? QLineEdit::Password : QLineEdit::Normal);
    KeyboardDialog::Instance->shift = true;
    KeyboardDialog::Instance->cancelled = true;
    KeyboardDialog::Instance->setMode(mode);

    KeyboardDialog::Instance->exec();

    return KeyboardDialog::Instance->ui->inputField->text().toStdString();
}

bool KeyboardDialog::TryGetString(string &val, string label, string startingInput, KeyboardInputMode mode, bool lockMode, bool password)
{
    val = GetString(label, startingInput, mode, lockMode, password);
    return KeyboardDialog::Instance->cancelled == false;
}

double KeyboardDialog::GetDouble(string label, double startingValue)
{
    string startingValueStr = QString::number(startingValue, 'f', 2).toStdString();
    string str = GetString(label, startingValueStr, NUMBER_DECIMAL_INPUT, true);
    double val = QString(str.c_str()).toDouble();
    return val;
}

bool KeyboardDialog::TryGetDouble(double &val, string label, double startingInput)
{
    val = GetDouble(label, startingInput);
    return KeyboardDialog::Instance->cancelled == false;
}

int KeyboardDialog::GetInt(string label, int startingValue)
{
    string startingValueStr = QString::number(startingValue).toStdString();
    string str = GetString(label, startingValueStr, NUMBER_INPUT, true);
    int val = QString(str.c_str()).toInt();
    return val;
}

bool KeyboardDialog::TryGetInt(int &val, string label, int startingInput)
{
    val = GetInt(label, startingInput);
    return KeyboardDialog::Instance->cancelled == false;
}

bool KeyboardDialog::VerifyPassword()
{
    string input = KeyboardDialog::GetString("Enter Password", "", NUMBER_INPUT, true, true);
    return input == Settings::GetString("password");
}

void KeyboardDialog::setMode(KeyboardInputMode mode)
{
    this->currentMode = mode;
    updateKeys();
}

void KeyboardDialog::updateKeys()
{
    bool showShift = false;
    bool showSpace = false;

    switch(this->currentMode)
    {
        case KEYBOARD_INPUT:
            if (shift)
                currentKeys = &this->keyboardCapsKeys;
            else
                currentKeys = &this->keyboardKeys;
            showShift = showSpace = true;
        break;
        case NUMBER_DECIMAL_INPUT:
            currentKeys = &this->numberDecimalKeys;
        break;
        case NUMBER_DATETIME_INPUT:
            currentKeys = &this->numberDatetimeKeys;
            showSpace = true;
        break;
        case NUMBER_INPUT:
            currentKeys = &this->numberKeys;
        break;
        case SYMBOL_INPUT:
            currentKeys = &this->symbolKeys;
        break;
    }

    this->ui->pushButton_Shift->setVisible(showShift);
    this->ui->pushButton_Space->setVisible(showSpace);

    for(int i=0; i<(int)buttons.size(); i++)
    {
        if (i < (int)currentKeys->size())
        {
            buttons[i]->setVisible(true);
            string str = currentKeys->at(i);
            buttons[i]->setText(str.c_str());
        }
        else
            buttons[i]->setVisible(false);
    }
}

void KeyboardDialog::insertString(string in)
{
    this->ui->inputField->insert(QString(in.c_str()));

    if (shift)
    {
        shift = false;
        updateKeys();
    }
}

void KeyboardDialog::on_pushButton_Space_clicked()
{
    this->ui->inputField->insert(QString(" "));
}

void KeyboardDialog::on_pushButton_Delete_clicked()
{
    this->ui->inputField->backspace();
}

void KeyboardDialog::on_pushButton_Shift_clicked()
{
    shift = !shift;
    updateKeys();
}

void KeyboardDialog::on_pushButton_Cancel_clicked()
{
    this->ui->inputField->setText(QString(original.c_str()));
    this->hide();
}

void KeyboardDialog::on_pushButton_Enter_clicked()
{
    cancelled = false;
    this->hide();
}

void KeyboardDialog::on_pushButton_ChangeKeyboard_clicked()
{
    switch(currentMode)
    {
        case KEYBOARD_INPUT:
            setMode(NUMBER_INPUT);
        return;
        case NUMBER_INPUT:
        case NUMBER_DECIMAL_INPUT:
        case NUMBER_DATETIME_INPUT:
            setMode(SYMBOL_INPUT);
        return;
        case SYMBOL_INPUT:
            setMode(KEYBOARD_INPUT);
        return;
    }
}

void KeyboardDialog::closeEvent(QCloseEvent *event)
{
    this->ui->inputField->setText(QString(original.c_str()));
    event->ignore();
}

KeyboardDialog::~KeyboardDialog()
{
    delete ui;
}
