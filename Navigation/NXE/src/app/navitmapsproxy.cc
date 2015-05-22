#include "navitmapsproxy.h"
#include "nxe_instance.h"
#include "mapinfoproxy.h"
#include "alog.h"

#include <QtQml/QQmlContext>

NavitMapsProxy::NavitMapsProxy(const std::shared_ptr<NXE::NXEInstance> &nxe, QQmlContext *ctx, QObject *parent)
    : QObject(parent)
    , nxeInstance(nxe)
    , m_ctx(ctx)
{
    // mapDownloaderCallbacks!
    mapDownloaderListener.progressCb = [this](const std::string& mapName, std::uint64_t now, std::uint64_t total) {
        emit mapDownloadProgress(now, total, QString::fromStdString(mapName));
    };
    mapDownloaderListener.errorCb = [this](const std::string&, const std::string& strError) {
        emit mapDownloadError(QString::fromStdString(strError));
    };
    mapDownloaderListener.finishedCb = [this](const std::string& map) {
        reloadMaps();
        emit mapDownloadFinished(QString::fromStdString(map));
    };

    nxeInstance->setMapDownloaderListener(mapDownloaderListener);

    reloadMaps();
}

NavitMapsProxy::~NavitMapsProxy()
{
    qDeleteAll(m_maps);
    m_maps.clear();
    for(auto iter = m_mapsByContinent.begin(); iter != m_mapsByContinent.end(); ++iter)
        qDeleteAll(iter->second);
    m_mapsByContinent.clear();
}

void NavitMapsProxy::downloadMap(const QString& map)
{
    try {
        nxeInstance->mapDownloader()->download(map.toStdString());
    }
    catch (const std::exception& ex) {
        // this may throw if MapDownloader is not available
        emit mapDownloadError("MapDownloader DBus service is probably not running");
    }
}

bool NavitMapsProxy::isMapDownloaded(const QString &mapName)
{
    return std::find_if(m_nxeMaps.begin(), m_nxeMaps.end(), [&mapName](const NXE::MapInfo& mi) -> bool{
        return mi.name == mapName.toStdString() && mi.downloaded;
    }) != m_nxeMaps.end();
}

qreal NavitMapsProxy::mapSize(const QString &mapName)
{
    auto it = std::find_if(m_nxeMaps.begin(), m_nxeMaps.end(), [&mapName](const NXE::MapInfo& mi) -> bool{
        return mi.name == mapName.toStdString() && mi.downloaded;
    });

    if ( it != m_nxeMaps.end() ) {

        qreal ret = (it != m_nxeMaps.end() ? it->size/(1024 * 1024) : -1);
        aTrace() << "Map size for " << mapName.toStdString() << " is " << ret;
        return ret;
    } else {
        aError() << "Unable to find map size for " << mapName.toStdString();
        return 0;
    }
}

void NavitMapsProxy::cancelDownload(const QString &mapName)
{
    nxeInstance->mapDownloader()->cancel(mapName.toStdString());
}

void NavitMapsProxy::reloadMaps()
{
    qDeleteAll(m_maps);
    m_maps.clear();
    for(auto iter = m_mapsByContinent.begin(); iter != m_mapsByContinent.end(); ++iter)
        qDeleteAll(iter->second);
    m_mapsByContinent.clear();
    // Request for available maps
    m_nxeMaps = nxeInstance->mapDownloader()->maps();
    std::sort(m_nxeMaps.begin(), m_nxeMaps.end(), [] (const NXE::MapInfo& lhs, const NXE::MapInfo& rhs) ->bool {
        return lhs.name < rhs.name;
    });
    std::for_each(m_nxeMaps.begin(), m_nxeMaps.end(), [this](const NXE::MapInfo& mi) {
        if(mi.name != mi.continent) {
           m_mapsByContinent[mi.continent].append(new MapInfoProxy{mi});
        }
        else
           if(mi.continent != "Australia+Oceania" && mi.continent != "Whole Planet")
              m_maps.append(new MapInfoProxy{mi});
    });
    m_ctx->setContextProperty("africaModel", QVariant::fromValue(m_mapsByContinent["Africa"]));
    m_ctx->setContextProperty("asiaModel", QVariant::fromValue(m_mapsByContinent["Asia"]));
    m_ctx->setContextProperty("australiaModel", QVariant::fromValue(m_mapsByContinent["Australia"]));
    m_ctx->setContextProperty("europeModel", QVariant::fromValue(m_mapsByContinent["Europe"]));
    m_ctx->setContextProperty("northAmericaModel", QVariant::fromValue(m_mapsByContinent["North America"]));
    m_ctx->setContextProperty("southMiddleAmericaModel", QVariant::fromValue(m_mapsByContinent["South+Middle America"]));
    m_ctx->setContextProperty("allMapsModel", QVariant::fromValue(m_maps));
}
