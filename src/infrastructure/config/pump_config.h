#ifndef IRRIGATION_PUMP_CONFIG_H
#define IRRIGATION_PUMP_CONFIG_H

namespace irrigation
{

    namespace PumpConfig
    {
        // gak pake flow sensor (mahal + gak akurat di selang sekecil ini),
        // jadi volume disiram dihitung dari durasi nyala x angka ini.
        // ganti kalau ganti pompa
        constexpr float FLOW_RATE_ML_PER_MINUTE = 200.0f;
    }

}

#endif
