#pragma once
// ============================================================
//  DeepfakeGuardian :: Pipeline.h
//  Producer-Consumer orchestrator (C++20 concepts + jthread)
// ============================================================
#include <atomic>
#include <functional>
#include <memory>
#include <thread>

#include "FrameBuffer.h"
#include "../capture/CaptureModule.h"
#include "../inference/InferenceEngine.h"
#include "../analysis/EARAnalyzer.h"
#include "../analysis/RPPGAnalyzer.h"
#include "../analysis/AVSyncAnalyzer.h"
#include "TrustScoreAggregator.h"

namespace dfg {

/// Callback fired on the UI thread after every analysed frame.
using TrustCallback = std::function<void(float trustScore, bool killSwitchTriggered)>;

/// -------------------------------------------------------
///  Pipeline
///  Wires Capture → FrameBuffer → Inference → Analysis
///  All heavy work stays off the Qt main thread.
/// -------------------------------------------------------
class Pipeline {
public:
    struct Config {
        int  targetFPS        = 60;
        bool lowResourceMode  = false;   ///< Halves inference rate on slow hardware
        bool privacyKillSwitch = false;  ///< Instantly blanks feed on detection
    };

    explicit Pipeline(Config cfg = {});
    ~Pipeline();

    // Non-copyable, non-movable (owns threads)
    Pipeline(const Pipeline&)            = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void setTrustCallback(TrustCallback cb);

    /// Starts all worker threads.
    void start();
    /// Graceful shutdown – joins all jthreads.
    void stop();

    bool isRunning() const noexcept { return m_running.load(); }

    /// UI-facing kill switch (can be toggled at runtime).
    void setPrivacyMode(bool enabled) noexcept;

private:
    // Worker entry points (run on jthreads)
    void captureWorker(std::stop_token st);
    void inferenceWorker(std::stop_token st);

    Config                            m_cfg;
    TrustCallback                     m_trustCb;

    std::unique_ptr<CaptureModule>    m_capture;
    std::unique_ptr<InferenceEngine>  m_inference;
    std::unique_ptr<EARAnalyzer>      m_ear;
    std::unique_ptr<RPPGAnalyzer>     m_rppg;
    std::unique_ptr<AVSyncAnalyzer>   m_avSync;
    std::unique_ptr<TrustScoreAggregator> m_aggregator;

    // Zero-copy GPU frame pool (shared between producer & consumer)
    std::shared_ptr<FrameBuffer>      m_frameBuffer;

    std::atomic<bool>                 m_running{false};
    std::atomic<bool>                 m_privacyMode{false};

    std::jthread                      m_captureThread;
    std::jthread                      m_inferenceThread;
};

} // namespace dfg
