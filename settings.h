#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QStringList>

class Settings
{
public:
    Settings()                 = default;
    Settings(const Settings &) = delete;
    void operator=(const Settings &) = delete;
    Settings(Settings &&)            = delete;
    void operator=(Settings &&) = delete;
    ~Settings();

    void save();
    void load();
    void initialize();

    void changeChinaDNSServerCheckState(const QString &server, bool checked);
    void changeAbroadDNSServerCheckState(const QString &server, bool checked);

    [[nodiscard]] const QString &listenAddress() const;
    void                         setListenAddress(const QString &listenAddress);

    [[nodiscard]] const QString &redis() const;
    void                         setRedis(const QString &redis);

    [[nodiscard]] const QString &customChinaDNSServerList() const;
    void                         setCustomChinaDNSServerList(const QString &customChinaDNSServerList);

    [[nodiscard]] const QString &customAbroadDNSServerList() const;
    void                         setCustomAbroadDNSServerList(const QString &customAbroadDNSServerList);

    [[nodiscard]] const QString &customAdsBlackList() const;
    void                         setCustomAdsBlackList(const QString &customAdsBlackList);

    [[nodiscard]] const QString &customAdsWhiteList() const;
    void                         setCustomAdsWhiteList(const QString &customAdsWhiteList);

    [[nodiscard]] const QStringList &ChinaDNSServerList() const;
    void                             setChinaDNSServerList(const QStringList &ChinaDNSServerList);

    [[nodiscard]] const QStringList &AbroadDNSServerList() const;
    void                             setAbroadDNSServerList(const QStringList &AbroadDNSServerList);

    [[nodiscard]] bool adsEnabled() const;
    void               setAdsEnabled(bool adsEnabled);

    [[nodiscard]] bool adsCacheStore() const;
    void               setAdsCacheStore(bool adsCacheStore);

    [[nodiscard]] bool defaultAdsPolicy() const;
    void               setDefaultAdsPolicy(bool defaultAdsPolicy);

    [[nodiscard]] const QString &adsAutoUpdateInterval() const;
    void                         setAdsAutoUpdateInterval(const QString &adsAutoUpdateInterval);

    [[nodiscard]] bool healthCheckEnabled() const;
    void               setHealthCheckEnabled(bool healthCheckEnabled);

    [[nodiscard]] bool hotReloadEnabled() const;
    void               setHotReloadEnabled(bool hotReloadEnabled);

    [[nodiscard]] bool logsEnabled() const;
    void               setLogsEnabled(bool logsEnabled);

    [[nodiscard]] bool hostEnabled() const;
    void               setHostEnabled(bool hostEnabled);

    [[nodiscard]] bool ttlCacheEnabled() const;
    void               setTtlCacheEnabled(bool ttlCacheEnabled);

    [[nodiscard]] bool bogusEnabled() const;
    void               setBogusEnabled(bool bogusEnabled);

private:
    QString     m_listenAddress {":53"};
    QString     m_redis;
    QString     m_customChinaDNSServers;
    QString     m_customAbroadDNSServers;
    QString     m_customAdsBlackList;
    QString     m_customAdsWhiteList;
    QString     m_adsAutoUpdateInterval {"24h"};
    QStringList m_chinaDNSServerList;
    QStringList m_abroadDNSServerList;
    bool        m_adsEnabled {true};
    bool        m_adsCacheStore {true};
    bool        m_defaultAdsPolicy {true};
    bool        m_healthCheckEnabled {true};
    bool        m_hotReloadEnabled {true};
    bool        m_logsEnabled {true};
    bool        m_hostEnabled {true};
    bool        m_ttlCacheEnabled {true};
    bool        m_bogusEnabled {false};
};

inline Settings *g_settings = nullptr;

#endif // SETTINGS_H
