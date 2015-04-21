#include "nxe_extension.h"
#include "nxe_instance.h"
#include "inavitipc.h"
#include "dbuscontroller.h"

#include "navitprocessimpl.h"
#include "navitdbus.h"
#include "gpsdprovider.h"
#include "mapdownloaderdbus.h"
#include "log.h"
#include "settings.h"
#include "settingtags.h"
#include "speechimpl.h"

#include <fruit/fruit.h>

using namespace NXE;

void NXExtension::createLogger()
{
    Settings s;
    const std::string path = s.get<SettingsTags::LogPath>();
    const std::string level = s.get<SettingsTags::LogLevel>();
    std::cout << "logger path= " << path << " level = " << level;
    std::shared_ptr<spdlog::sinks::sink> rot{ new spdlog::sinks::simple_file_sink_mt(path + "/nxe.log", true) };
    std::shared_ptr<spdlog::sinks::sink> out{ new spdlog::sinks::stdout_sink_mt() };
    spdlog::create("nxe", { rot, out});

    if (level == "debug") {
        spdlog::set_level(spdlog::level::debug);
    }
    else if (level == "warn") {
        spdlog::set_level(spdlog::level::warn);
    }
    else if (level == "trace") {
        spdlog::set_level(spdlog::level::trace);
    }
    else {
        spdlog::set_level(spdlog::level::err);
    }

}
