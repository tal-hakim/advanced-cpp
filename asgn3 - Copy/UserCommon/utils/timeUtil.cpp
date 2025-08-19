#include "timeUtil.h"

namespace UserCommon_322213836_212054837 {
    std::string getCurrentTimeString() {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::ostringstream oss; oss << std::put_time(std::localtime(&now), "%Y%m%d_%H%M%S");
        return oss.str();
    }


//    std::string getCurrentTimeString() {
//        auto now = std::chrono::system_clock::now();
//        auto now_time = std::chrono::system_clock::to_time_t(now);
//        std::tm tm;
//        localtime_r(&now_time, &tm);
//        std::ostringstream oss;
//        oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
//        return oss.str();
//    }
}
