#include "optionselectdialog.h"
#include "ui_optionselectdialog.h"
#include "app.h"
#include <QCloseEvent>

OptionSelectDialog* OptionSelectDialog::Instance = NULL;

OptionSelectDialog::OptionSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionSelectDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    selectedStylesheet = QString("border-image: url(:/images/ListItemButtonSelected.png) 8;\nborder: 8;");
    normalStylesheet = QString("border-image: url(:/images/ListItemButton.png) 8;\nborder: 8;");

    layout = new QVBoxLayout(this->ui->scrollArea);
    layout->setAlignment(Qt::AlignTop);
    layout->setMargin(0);
    layout->setSpacing(0);
    this->ui->scrollAreaWidgetContents->setLayout(layout);

    selectedIndex = -1;
}

bool OptionSelectDialog::TryGetChoice(int &val, string label, vector<string> choices, int selectedIndex)
{
    val = GetChoice(label, choices, selectedIndex);
    return val > -1;
}

int OptionSelectDialog::GetChoice(string title, vector<string> choices, int index)
{
    if (OptionSelectDialog::Instance == NULL)
        OptionSelectDialog::Instance = new OptionSelectDialog();

    OptionSelectDialog::Instance->setWindowTitle(QString(title.c_str()));
    OptionSelectDialog::Instance->ui->label->setText(QString(title.c_str()));

    int size = choices.size();

    for (unsigned int i=size; i<OptionSelectDialog::Instance->buttons.size(); i++)
    {
        QPushButton* button = OptionSelectDialog::Instance->buttons[i];
        button->setVisible(false);
    }

    for (unsigned int i=OptionSelectDialog::Instance->buttons.size();i<size; i++)
    {
        QPushButton* button = new QPushButton(OptionSelectDialog::Instance->ui->scrollAreaWidgetContents);
        button->setMinimumHeight(70);
        button->setFocusPolicy(Qt::NoFocus);
        button->setFlat(true);
        button->setFont(QFont("Arial", 12));
        OptionSelectDialog::Instance->layout->addWidget(button);
        OptionSelectDialog::Instance->buttons.push_back(button);
        connect(button, SIGNAL(clicked()), OptionSelectDialog::Instance, SLOT(itemClicked()));
    }

    for (unsigned int i=0;i<size; i++)
    {
        QPushButton* button = OptionSelectDialog::Instance->buttons[i];
        button->setVisible(true);
        button->setStyleSheet(i == index ? OptionSelectDialog::Instance->selectedStylesheet : OptionSelectDialog::Instance->normalStylesheet);
        button->setText(QString(choices[i].c_str()));
    }

    OptionSelectDialog::Instance->selectedIndex = index;

    OptionSelectDialog::Instance->ui->pushButton_Enter->setEnabled(index > -1);

    if (App::Fullscreen)
        OptionSelectDialog::Instance->move(40, 40);
    else
        OptionSelectDialog::Instance->move(App::WindowX + 40, App::WindowY + 40);
    OptionSelectDialog::Instance->show();


    OptionSelectDialog::Instance->exec();

    return OptionSelectDialog::Instance->selectedIndex;
}

void OptionSelectDialog::itemClicked()
{
    QPushButton* button = (QPushButton*)sender();
    int index = std::find(buttons.begin(), buttons.end(), button) - buttons.begin();

    if (selectedIndex != index)
    {
        if (selectedIndex > -1)
            buttons[selectedIndex]->setStyleSheet(normalStylesheet);
        if (index > -1)
            buttons[index]->setStyleSheet(selectedStylesheet);
    }
    selectedIndex = index;

    OptionSelectDialog::Instance->ui->pushButton_Enter->setEnabled(true);
}

void OptionSelectDialog::on_pushButton_Enter_clicked()
{
    this->hide();
}

void OptionSelectDialog::on_pushButton_Cancel_clicked()
{
    selectedIndex = -1;
    this->hide();
}

void OptionSelectDialog::closeEvent(QCloseEvent *ev)
{
    selectedIndex = -1;
    ev->ignore();
}


OptionSelectDialog::~OptionSelectDialog()
{
    delete ui;
}
