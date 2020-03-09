#include <QSettings>

#include "settings.h"

Settings::~Settings()
{
    save();
}

void Settings::initialize()
{
    load();
}

void Settings::addChinaDNSServer(const QString &server)
{
    if (!m_chinaDNSServerList.contains(server))
        m_chinaDNSServerList.append(server);
}

void Settings::removeChinaDNSServer(const QString &server)
{
    m_chinaDNSServerList.removeAll(server);
}

void Settings::addAbroadDNSServer(const QString &server)
{
    if (!m_abroadDNSServerList.contains(server))
        m_abroadDNSServerList.append(server);
}

void Settings::removeAbroadDNSServer(const QString &server)
{
    m_abroadDNSServerList.removeAll(server);
}

const QString &Settings::listenAddress() const
{
    return m_listenAddress;
}

void Settings::setListenAddress(const QString &listenAddress)
{
    m_listenAddress = listenAddress;
}

const QString &Settings::redis() const
{
    return m_redis;
}

void Settings::setRedis(const QString &redis)
{
    m_redis = redis;
}

const QString &Settings::customChinaDNSServerList() const
{
    return m_customChinaDNSServers;
}

void Settings::setCustomChinaDNSServerList(const QString &customChinaDNSServerList)
{
    m_customChinaDNSServers = customChinaDNSServerList;
}

const QString &Settings::customAbroadDNSServerList() const
{
    return m_customAbroadDNSServers;
}

void Settings::setCustomAbroadDNSServerList(const QString &customAbroadDNSServerList)
{
    m_customAbroadDNSServers = customAbroadDNSServerList;
}

const QString &Settings::customAdsBlackList() const
{
    return m_customAdsBlackList;
}

void Settings::setCustomAdsBlackList(const QString &customAdsBlackList)
{
    m_customAdsBlackList = customAdsBlackList;
}

const QString &Settings::customAdsWhiteList() const
{
    return m_customAdsWhiteList;
}

void Settings::setCustomAdsWhiteList(const QString &customAdsWhiteList)
{
    m_customAdsWhiteList = customAdsWhiteList;
}

const QStringList &Settings::chinaDNSServerList() const
{
    return m_chinaDNSServerList;
}

void Settings::setChinaDNSServerList(const QStringList &chinaDNSServerList)
{
    m_chinaDNSServerList = chinaDNSServerList;
}

const QStringList &Settings::abroadDNSServerList() const
{
    return m_abroadDNSServerList;
}

void Settings::setAbroadDNSServerList(const QStringList &abroadDNSServerList)
{
    m_abroadDNSServerList = abroadDNSServerList;
}

bool Settings::adsEnabled() const
{
    return m_adsEnabled;
}

void Settings::setAdsEnabled(bool adsEnabled)
{
    m_adsEnabled = adsEnabled;
}

bool Settings::adsCacheStore() const
{
    return m_adsCacheStore;
}

void Settings::setAdsCacheStore(bool adsCacheStore)
{
    m_adsCacheStore = adsCacheStore;
}

bool Settings::defaultAdsPolicy() const
{
    return m_defaultAdsPolicy;
}

void Settings::setDefaultAdsPolicy(bool defaultAdsPolicy)
{
    m_defaultAdsPolicy = defaultAdsPolicy;
}

const QString &Settings::adsAutoUpdateInterval() const
{
    return m_adsAutoUpdateInterval;
}

void Settings::setAdsAutoUpdateInterval(const QString &adsAutoUpdateInterval)
{
    m_adsAutoUpdateInterval = adsAutoUpdateInterval;
}

bool Settings::healthCheckEnabled() const
{
    return m_healthCheckEnabled;
}

void Settings::setHealthCheckEnabled(bool healthCheckEnabled)
{
    m_healthCheckEnabled = healthCheckEnabled;
}

bool Settings::hotReloadEnabled() const
{
    return m_hotReloadEnabled;
}

void Settings::setHotReloadEnabled(bool hotReloadEnabled)
{
    m_hotReloadEnabled = hotReloadEnabled;
}

bool Settings::logsEnabled() const
{
    return m_logsEnabled;
}

void Settings::setLogsEnabled(bool logsEnabled)
{
    m_logsEnabled = logsEnabled;
}

bool Settings::hostEnabled() const
{
    return m_hostEnabled;
}

void Settings::setHostEnabled(bool hostEnabled)
{
    m_hostEnabled = hostEnabled;
}

bool Settings::ttlCacheEnabled() const
{
    return m_ttlCacheEnabled;
}

void Settings::setTtlCacheEnabled(bool ttlCacheEnabled)
{
    m_ttlCacheEnabled = ttlCacheEnabled;
}

bool Settings::bogusEnabled() const
{
    return m_bogusEnabled;
}

void Settings::setBogusEnabled(bool bogusEnabled)
{
    m_bogusEnabled = bogusEnabled;
}

const QString &Settings::chinaDomainList() const
{
    return m_chinaDomainList;
}

void Settings::setChinaDomainList(const QString &chinaDomainList)
{
    m_chinaDomainList = chinaDomainList;
}

const QString &Settings::googleDomainList() const
{
    return m_googleDomainList;
}

void Settings::setGoogleDomainList(const QString &googleDomainList)
{
    m_googleDomainList = googleDomainList;
}

const QString &Settings::appleDomainList() const
{
    return m_appleDomainList;
}

void Settings::setAppleDomainList(const QString &appleDomainList)
{
    m_appleDomainList = appleDomainList;
}

const QString &Settings::bogusIPList() const
{
    return m_bogusIPList;
}

void Settings::setBogusIPList(const QString &bogusIPList)
{
    m_bogusIPList = bogusIPList;
}

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "CoreDNS GUI");
    settings.setValue("listenAddress", m_listenAddress);
    settings.setValue("redis", m_redis);
    settings.setValue("customChinaDNSServerList", m_customChinaDNSServers);
    settings.setValue("customAbroadDNSServerList", m_customAbroadDNSServers);
    settings.setValue("customAdsBlackList", m_customAdsBlackList);
    settings.setValue("customAdsWhiteList", m_customAdsWhiteList);
    settings.setValue("adsAutoUpdateInterval", m_adsAutoUpdateInterval);
    settings.setValue("chinaDNSServerList", m_chinaDNSServerList);
    settings.setValue("abroadDNSServerList", m_abroadDNSServerList);
    settings.setValue("adsEnabled", m_adsEnabled);
    settings.setValue("adsCacheStore", m_adsCacheStore);
    settings.setValue("defaultAdsPolicy", m_defaultAdsPolicy);
    settings.setValue("healthCheckEnabled", m_healthCheckEnabled);
    settings.setValue("hotReloadEnabled", m_hotReloadEnabled);
    settings.setValue("logsEnabled", m_logsEnabled);
    settings.setValue("hostEnabled", m_hostEnabled);
    settings.setValue("ttlCacheEnabled", m_ttlCacheEnabled);
    settings.setValue("bogusEnabled", m_bogusEnabled);
    settings.setValue("chinaDomainList", m_chinaDomainList);
    settings.setValue("googleDomainList", m_googleDomainList);
    settings.setValue("appleDomainList", m_appleDomainList);
    settings.setValue("bogusIPList", m_bogusIPList);
    settings.sync();
}

void Settings::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "CoreDNS GUI");
    m_listenAddress          = settings.value("listenAddress", ":53").toString();
    m_redis                  = settings.value("redis").toString();
    m_customChinaDNSServers  = settings.value("customChinaDNSServerList").toString();
    m_customAbroadDNSServers = settings.value("customAbroadDNSServerList").toString();
    m_customAdsBlackList     = settings.value("customAdsBlackList").toString();
    m_adsAutoUpdateInterval  = settings.value("adsAutoUpdateInterval", "24h").toString();
    m_listenAddress          = settings.value("listenAddress").toString();
    m_chinaDNSServerList     = settings.value("chinaDNSServerList").toStringList();
    m_abroadDNSServerList    = settings.value("abroadDNSServerList").toStringList();
    m_adsEnabled             = settings.value("adsEnabled", true).toBool();
    m_adsCacheStore          = settings.value("adsCacheStore", true).toBool();
    m_defaultAdsPolicy       = settings.value("defaultAdsPolicy", true).toBool();
    m_healthCheckEnabled     = settings.value("healthCheckEnabled", true).toBool();
    m_hotReloadEnabled       = settings.value("hotReloadEnabled", true).toBool();
    m_logsEnabled            = settings.value("logsEnabled", true).toBool();
    m_hostEnabled            = settings.value("hostEnabled", true).toBool();
    m_ttlCacheEnabled        = settings.value("ttlCacheEnabled", true).toBool();
    m_bogusEnabled           = settings.value("bogusEnabled", false).toBool();
    m_chinaDomainList        = settings.value("chinaDomainList").toString();
    m_googleDomainList       = settings.value("googleDomainList").toString();
    m_appleDomainList        = settings.value("appleDomainList").toString();
    m_bogusIPList            = settings.value("bogusIPList").toString();
}
