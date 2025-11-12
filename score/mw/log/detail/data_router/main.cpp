#include "score/mw/log/logger.h"
#include <array>

int main()
{
    auto logger = score::mw::log::CreateLogger("CTX1", "Launch Manager logging context");

    std::array<char, 21> data { 'b', 21};
    score::mw::log::LogRawBuffer raw_buff { data };
    score::mw::log::LogBin64 bin { 0x12313123 };
    std::string some_text {"asd"};

    logger.LogFatal() << raw_buff;
    logger.LogFatal() << bin;
    logger.LogFatal() << some_text;
}
