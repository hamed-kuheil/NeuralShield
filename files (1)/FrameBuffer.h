#pragma once
// ============================================================
//  DeepfakeGuardian :: FrameBuffer.h
//  Lock-free SPSC ring buffer – frames live in VRAM (cv::UMat)
//  Never calls new/malloc inside the hot path.
// ============================================================
#include <array>
#include <atomic>
#include <optional>
#include <cstdint>
#include <opencv2/core/umat.hpp>

namespace dfg {

/// A single slot in the pool.
/// UMat keeps pixel data in OpenCL/CUDA memory – zero CPU copy.
struct VideoFrame {
    cv::UMat   image;        ///< GPU-resident frame (OpenCL UMat)
    double     timestampMs;  ///< Wall-clock capture time
    uint64_t   frameIndex;   ///< Monotonic counter
};

/// -------------------------------------------------------
///  FrameBuffer<N>
///  Single-Producer / Single-Consumer wait-free ring buffer.
///  N must be a power-of-two (compile-time checked).
/// -------------------------------------------------------
template<std::size_t N = 8>
    requires (N > 1 && (N & (N - 1)) == 0)  // C++20 concept constraint
class FrameBuffer {
public:
    FrameBuffer() : m_head(0), m_tail(0) {
        // Pre-allocate UMat slots so the capture loop never allocates.
        for (auto& slot : m_slots) {
            slot.image.create(1080, 1920, CV_8UC3, cv::USAGE_ALLOCATE_DEVICE_MEMORY);
        }
    }

    /// Producer: try to write a frame (non-blocking).
    /// Returns false if the buffer is full (drop frame).
    bool tryPush(VideoFrame&& frame) noexcept {
        const auto head = m_head.load(std::memory_order_relaxed);
        const auto next = (head + 1) & MASK;
        if (next == m_tail.load(std::memory_order_acquire))
            return false;  // full

        m_slots[head] = std::move(frame);
        m_head.store(next, std::memory_order_release);
        return true;
    }

    /// Consumer: try to read a frame (non-blocking).
    std::optional<VideoFrame> tryPop() noexcept {
        const auto tail = m_tail.load(std::memory_order_relaxed);
        if (tail == m_head.load(std::memory_order_acquire))
            return std::nullopt;  // empty

        VideoFrame frame = std::move(m_slots[tail]);
        m_tail.store((tail + 1) & MASK, std::memory_order_release);
        return frame;
    }

    bool empty() const noexcept {
        return m_head.load(std::memory_order_acquire)
            == m_tail.load(std::memory_order_acquire);
    }

private:
    static constexpr std::size_t MASK = N - 1;

    alignas(64) std::atomic<std::size_t> m_head;  // cache-line isolated
    alignas(64) std::atomic<std::size_t> m_tail;

    std::array<VideoFrame, N> m_slots;
};

} // namespace dfg
