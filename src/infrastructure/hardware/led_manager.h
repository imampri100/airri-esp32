#ifndef IRRIGATION_LED_MANAGER_H
#define IRRIGATION_LED_MANAGER_H

namespace irrigation
{

    enum class LedState
    {
        Idle,     // hijau menyala
        Watering, // kuning menyala
        Syncing,  // kuning berkedip
        Error     // merah berkedip
    };

    class LedManager
    {
    public:
        bool begin();

        void setState(LedState state);

        // Panggil terus di loop() supaya efek blink (Syncing/Error) jalan.
        void tick();

    private:
        LedState currentState = LedState::Idle;

        unsigned long lastBlinkAt = 0;
        bool blinkOn = false;

        void applyState(bool ledOn);
    };

}

#endif
