#pragma once

#include <array>
#include <cstdint>

namespace igi::gameplay
{

class RetailRandomGenerator final
{
  public:
    // Original initializer 0x004B4720; draws 0x004B4740 and 0x004B4770.
    constexpr RetailRandomGenerator() noexcept = default;

    explicit constexpr RetailRandomGenerator(std::array<std::uint32_t, 3> words) noexcept
        : accumulator_(words[0]), feedback_word_(words[1]), shift_word_(words[2])
    {
    }

    [[nodiscard]] constexpr std::uint32_t next_word() noexcept
    {
        shift_word_ ^= accumulator_ >> 7;
        feedback_word_ ^= shift_word_;
        accumulator_ += feedback_word_;
        return accumulator_;
    }

    [[nodiscard]] float next_single() noexcept
    {
        return static_cast<float>(next_word() & 0xffffu) * 0.0000152587890625f;
    }

    [[nodiscard]] constexpr std::array<std::uint32_t, 3> state() const noexcept
    {
        return {accumulator_, feedback_word_, shift_word_};
    }

  private:
    std::uint32_t accumulator_ = 0xc8fb1a07u;
    std::uint32_t feedback_word_ = 0x39ea7756u;
    std::uint32_t shift_word_ = 0x7ff13ccdu;
};

} // namespace igi::gameplay
