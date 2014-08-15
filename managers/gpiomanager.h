#ifndef GPIOMANAGER_H
#define GPIOMANAGER_H

#include <QApplication>

using namespace std;

class GPIOManager
{
public:

    static void Init();

    // Generics
    static void SetMode(int pin, string mode);
    static void Write(int pin, bool val);

    // Specialized
    static void EnableCameraLights(bool state);
};

#endif // GPIOMANAGER_H
