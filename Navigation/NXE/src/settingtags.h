#ifndef SETTINGTAGS_H
#define SETTINGTAGS_H

#include <string>

namespace SettingsTags {
namespace Navit {

    struct Path {
        typedef std::string type;
        static std::string name() noexcept { return "navit.path"; }
    };

    struct AutoStart {
        typedef bool type;
        static std::string name() noexcept { return "navit.autostart"; }
    };
}
};

#endif // SETTINGTAGS_H