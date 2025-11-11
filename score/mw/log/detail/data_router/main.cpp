#include "score/mw/log/logger.h"

int main()
{
    auto logger = score::mw::log::CreateLogger("CTX1", "Launch Manager logging context");
    auto asd = score::mw::log::CreateLogger("CTX2", "Launch Manager logging context");

    logger.LogFatal() << 32 << true << "asd";
    logger.LogFatal() << 32;
    asd.LogWarn() << "Hello";
    logger.LogFatal() << 32;
    logger.LogFatal() << 32;
    logger.LogFatal() << 32;
}
