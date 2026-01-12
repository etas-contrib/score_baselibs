#ifndef SCORE_ANALYSIS_TRACING_COMMON_UTILITIES_TRY_INCREMENT_H_
#define SCORE_ANALYSIS_TRACING_COMMON_UTILITIES_TRY_INCREMENT_H_

#include <atomic>
#include <cstdint>
#include <limits>

namespace score
{
namespace analysis
{
namespace tracing
{

template <typename A>
struct atomic_value_type;

template <typename T>
struct atomic_value_type<std::atomic<T>>
{
    using type = T;
};

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
class IncrementWithSaturation
{

  public:
    static constexpr T FLAG_MASK = 1ULL << (std::numeric_limits<T>::digits - 1);
    static constexpr T COUNTER_MASK = FLAG_MASK - 1;

    IncrementWithSaturation() = delete;
    ~IncrementWithSaturation() = delete;
    // Increments the value atomically with saturation check.
    // If the value is saturated, it will not increment further, and pair::first will be true.
    // pair::second will be true if retries exceeded.
    // Value is successfully incremented only if both pair flags are false.
    static inline std::pair<bool, bool> IncrementWithSaturationCheck(std::atomic<T>& val)
    {
        T old = val.load(std::memory_order_relaxed);
        std::uint8_t max_retries = 100;
        while (max_retries-- > 0)
        {
            T flag = old & FLAG_MASK;
            // old > 0 is checked for signed values. If the old value is -ve, we can increment the value without
            // checking for flag.
            if (old >= 0 && flag)
            {
                // overflow already occurred
                return {true, false};
            }

            T counter = old & COUNTER_MASK;
            if (val.compare_exchange_weak(old, counter + 1, std::memory_order_relaxed, std::memory_order_relaxed))
            {
                return {false, false};
            }
        }
        return {false, true};
    }
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_UTILITIES_TRY_INCREMENT_H_
