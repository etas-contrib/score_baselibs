#include "score/mw/log/logger.h"

int main()
{
    auto logger = score::mw::log::CreateLogger("CTX1", "Launch Manager logging context");
    auto asd = score::mw::log::CreateLogger("CTX2", "Launch Manager logging context");
    score::mw::log::LogSlog2Message sloggers { 21, "jeff"};
    score::mw::log::LogBin32 bin32 {333333323UL};
    score::mw::log::LogBin64 bin64 {319203810238129033ULL};

    logger.LogFatal() << 32 << true << "asd";
    logger.LogFatal() << 32;
    asd.LogWarn() << "Hello";
    logger.LogDebug() << 32;
    logger.LogFatal() << 32;
    logger.LogFatal() << 32;
    logger.LogFatal() << bin32;
    logger.LogFatal() << bin64;
    asd.LogError() << sloggers;
}
