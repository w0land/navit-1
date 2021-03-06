#include "appsettings.h"
#include "alog.h"
#include "locationproxy.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/range/iterator_range.hpp>

namespace bfs = boost::filesystem;

namespace {
std::pair<std::string, std::string> getConfigPath()
{
    const bfs::path homePath{ getenv("HOME") };
    if (!bfs::exists(homePath)) {
        throw std::runtime_error("HOME does not exists");
    }

    const bfs::path defPath = homePath / bfs::path{ ".NavIt" };
    const bfs::path favPaths = defPath / bfs::path{ "favorites" };
    const bfs::path defFilePath = defPath / "user.conf";

    if (!bfs::exists(defPath)) {
        if (!bfs::create_directories(defPath) || !bfs::create_directories(favPaths)) {
            throw std::runtime_error("Unable to create ~/.NavIt directory");
        }
    }
    if (!bfs::exists(favPaths)) {
        if (!bfs::create_directories(favPaths)) {
            throw std::runtime_error("Unable to create ~/.NavIt/favorites directory");
        }
    }
    return std::make_pair(defFilePath.string(), favPaths.string());
}
}

AppSettings::AppSettings()
    : m_configPath(getConfigPath().first)
    , m_favoritesPath(getConfigPath().second)
{
    if (bfs::exists(m_configPath)) {
        try {
            boost::property_tree::read_json(m_configPath, m_tree);
        } catch(const std::exception&) {
            // user.conf is not valid
            bfs::remove(m_configPath);
            createDefaults();
            boost::property_tree::read_json(m_configPath, m_tree);
        }
    }
    else {
        // create default config file
        createDefaults();
    }
}

AppSettings::~AppSettings()
{
    save();
}

void AppSettings::save()
{
    boost::property_tree::write_json(m_configPath, m_tree);
}

void AppSettings::addToFavorites(LocationProxy* proxy)
{
    using boost::property_tree::ptree;
    using boost::property_tree::write_json;
    const std::string name = "fav_" + std::string{ proxy->id().toByteArray().data() };
    bfs::path favPath{ bfs::path{ m_favoritesPath } / name };

    aInfo() << "Adding " << name << " to from favorites"
            << " under " << favPath.string();
    ptree favTree;
    favTree.add("id", proxy->id().toByteArray().data());
    favTree.add("itemText", proxy->itemText().toStdString());
    favTree.add("description", proxy->description().toStdString());
    favTree.add("longitude", proxy->longitude());
    favTree.add("latitude", proxy->latitude());
    write_json(favPath.string(), favTree);
}

bool AppSettings::isFavorite(const std::string &name, const NXE::Position &p)
{
    // read all favorites
    bfs::directory_iterator dirIt{ m_favoritesPath };

    bool found {false};

    for (auto& entry : boost::make_iterator_range(dirIt, {})) {
        using boost::property_tree::ptree;
        using boost::property_tree::read_json;
        ptree entryTree;
        aInfo() << "Reading " << entry;
        read_json(entry.path().string(), entryTree);

        aInfo() << "ID = " << entryTree.get<std::string>("id");

        if (entryTree.get<std::string>("itemText") == name) {
            found = true;
            break;
        }
    }
    return found;
}

void AppSettings::removeFromFavorites(const std::string& id)
{
    aInfo() << "Remove " << id << " from favorites";
    const std::string name = "fav_" + id;
    bfs::path favPath{ bfs::path{ m_favoritesPath } / name };

    if(!bfs::remove(favPath)) {
        aError() << "Unable to remove " << favPath.string();
    }
}

void AppSettings::remove()
{
    using boost::property_tree::read_json;
    // remove ~/.NavIt/user.conf
    bfs::remove(m_configPath);
    bfs::remove_all(m_favoritesPath);

    getConfigPath();
    createDefaults();

    read_json(m_configPath, m_tree);
}

QList<LocationProxy*> AppSettings::favorites()
{
    aInfo() << "Reading favorites";
    bfs::directory_iterator dirIt{ m_favoritesPath };
    QList<LocationProxy*> favs;

    for (auto& entry : boost::make_iterator_range(dirIt, {})) {
        using boost::property_tree::ptree;
        using boost::property_tree::read_json;
        ptree entryTree;
        aInfo() << "Reading " << entry;
        read_json(entry.path().string(), entryTree);

        aInfo() << "ID = " << entryTree.get<std::string>("id");

        auto loc = new LocationProxy{
            QString::fromStdString(entryTree.get<std::string>("itemText")),
            true,
            QString::fromStdString(entryTree.get<std::string>("description")),
            false,
            -1,
            -1,
            QString::fromStdString(entryTree.get<std::string>("id"))
        };
        loc->setPosition(NXE::Position{ entryTree.get<double>("longitude"), entryTree.get<double>("latitude") });
        favs.append(loc);
    }

    return favs;
}

void AppSettings::createDefaults()
{
    aDebug() << "Creating default app settings under= " << m_configPath;
    set<Tags::EnablePoi>(false);
    set<Tags::Orientation>(-1);
    set<Tags::Ftu>(true);
    set<Tags::Voice>(true);
    set<Tags::MapView>("2D");
    save();
}
