#include "staticscrollarea.h"
#include <iostream>
#include <fstream>
#include <data/settings.h>
#include <QDebug>


StaticScrollArea::StaticScrollArea(QWidget *parent) :
    QFrame(parent)
{
    currentOffset = 0;
    selectedIndex = -1;

    string cssFilename = Settings::GetString("cssFile");
    ifstream file(cssFilename.c_str());
    if (!file)
    {
        qDebug("Failed to load css file! %s", cssFilename.c_str());
        return;
    }

    string str;
    while(getline(file, str))
        style.append(str.c_str());

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    this->setLayout(layout);

    QWidget* buttonHolder = new QWidget();
    buttonLayout = new QVBoxLayout();
    buttonLayout->setMargin(0);
    buttonLayout->setSpacing(0);
    buttonLayout->setAlignment(Qt::AlignTop);
    buttonHolder->setLayout(buttonLayout);

    this->scrollbar = new QScrollBar();
    this->scrollbar->setStyleSheet(style);
    connect(this->scrollbar, SIGNAL(valueChanged(int)), this, SLOT(scrolled(int)));

    layout->addWidget(buttonHolder, Qt::AlignLeft);
    layout->addWidget(scrollbar, Qt::AlignRight);

    SetNumButtons(6);
}

void StaticScrollArea::SetSelectedIndex(int index)
{
    selectedIndex = index;
    emit SelectionChanged(selectedIndex);
    updateButtons();
}

void StaticScrollArea::SetNumButtons(int buttonCount)
{
    this->scrollbar->setRange(0, options.size()-buttonCount);
    int h = height();

    // Create buttons
    for (unsigned int i=buttons.size();i<buttonCount; i++)
    {
        QPushButton* button = new QPushButton();
        button->setText(QString("Button %1").arg(i));
        button->setMinimumHeight(h/buttonCount);
        button->setFocusPolicy(Qt::NoFocus);
        button->setFlat(true);
        button->setFont(QFont("Arial", 12));
        button->setStyleSheet(style);
        button->setCheckable(true);
        buttonLayout->addWidget(button);
        buttons.push_back(button);
        connect(button, SIGNAL(clicked()), this, SLOT(itemClicked()));
    }
}

void StaticScrollArea::SetOptions(vector<string> &opts)
{
    // I dont know why this didn't work in the constructor, but it works here ha
    this->setFrameShape(QFrame::NoFrame);

    this->options = opts;
    this->scrollbar->setRange(0, options.size()-buttons.size());

    this->updateButtons();
}

void StaticScrollArea::scrolled(int index)
{
    this->currentOffset = index;
    updateButtons();
}

void StaticScrollArea::updateButtons()
{
    for(int i=0; i<buttons.size(); i++)
    {
        int index = currentOffset + i;
        QPushButton* button = buttons[i];
        button->setVisible(index < options.size());
        button->setChecked(index == selectedIndex);
        if (index < options.size())
            button->setText(options[index].c_str());
    }
}

void StaticScrollArea::itemClicked()
{
    QPushButton* button = (QPushButton*)sender();
    int index = std::find(buttons.begin(), buttons.end(), button) - buttons.begin();
    SetSelectedIndex(index + currentOffset);
}
