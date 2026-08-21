#ifndef IRRIGATION_LANGUAGE_H
#define IRRIGATION_LANGUAGE_H

namespace irrigation
{

    // Bahasa tampilan untuk string yang dilihat end-user (API
    // decision.reason & layar TFT) - lihat Translator. Pesan error HTTP
    // & log Serial sengaja tidak ikut, tetap bahasa Indonesia (sifatnya
    // debug/development, bukan yang dilihat end-user awam).
    enum class Language
    {
        Indonesian,
        English
    };

}

#endif
