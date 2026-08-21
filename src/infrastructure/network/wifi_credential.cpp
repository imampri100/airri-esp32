#include "infrastructure/network/wifi_credential.h"

namespace irrigation
{

    WifiCredential::WifiCredential(
        const String &apSsid,
        const String &apPassword,
        const String &staSsid,
        const String &staPassword,
        bool apEnabled,
        bool staEnabled)
        : m_apSsid(apSsid),
          m_apPassword(apPassword),
          m_staSsid(staSsid),
          m_staPassword(staPassword),
          m_apEnabled(apEnabled),
          m_staEnabled(staEnabled)
    {
    }

    const String &WifiCredential::apSsid() const
    {
        return m_apSsid;
    }

    const String &WifiCredential::apPassword() const
    {
        return m_apPassword;
    }

    const String &WifiCredential::staSsid() const
    {
        return m_staSsid;
    }

    const String &WifiCredential::staPassword() const
    {
        return m_staPassword;
    }

    bool WifiCredential::apEnabled() const
    {
        return m_apEnabled;
    }

    bool WifiCredential::staEnabled() const
    {
        return m_staEnabled;
    }

    bool WifiCredential::hasSta() const
    {
        return m_staEnabled && m_staSsid.length() > 0;
    }

}
