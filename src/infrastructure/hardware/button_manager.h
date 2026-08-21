#ifndef IRRIGATION_BUTTON_MANAGER_H
#define IRRIGATION_BUTTON_MANAGER_H

namespace irrigation
{

    class ButtonManager
    {
    public:
        bool begin();

        bool isPressed();
    };

}

#endif