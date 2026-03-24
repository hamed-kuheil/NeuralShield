#pragma once
// ============================================================
//  DeepfakeGuardian :: EARAnalyzer.h
//  Eye Aspect Ratio (EAR) blink detector.
//  Real humans blink ~15-20x/min with micro-tremor;
//  deepfake models often produce static or irregular eyes.
// ============================================================
#include <array>
#include <deque>
#include <optional>
#include <opencv2/core.hpp>

namespace dfg {

/// 6 landmark points per eye (dlib / MediaPipe ordering).
using EyeLandmarks = std::array<cv::Point2f, 6>;

struct EARResult {
    float leftEAR;
    float rightEAR;
    float blinkRatePerMin;   ///< Measured over the last 60 s
    float microTremorScore;  ///< RMS of sub-frame eyelid jitter
    float anomalyScore;      ///< 0 = looks human, 1 = looks synthetic
};

/// -------------------------------------------------------
///  EARAnalyzer
///  Formula: EAR = (||p2-p6|| + ||p3-p5||) / (2 * ||p1-p4||)
///  Blink detected when EAR < threshold for N consecutive frames.
/// -------------------------------------------------------
class EARAnalyzer {
public:
    struct Config {
        float blinkThreshold    = 0.21f;  ///< EAR below this = closed
        int   consecutiveFrames = 3;       ///< Frames closed = one blink
        int   fps               = 30;
        // Human norms (±2σ)
        float minBlinkRate      = 8.0f;   ///< Blinks/min
        float maxBlinkRate      = 30.0f;
    };

    explicit EARAnalyzer(Config cfg = {});

    /// Update with fresh landmarks. Returns result once enough history exists.
    std::optional<EARResult> update(const EyeLandmarks& left,
                                    const EyeLandmarks& right);

    void reset();

private:
    static float computeEAR(const EyeLandmarks& eye);

    Config              m_cfg;
    std::deque<float>   m_earHistory;     ///< Rolling EAR values (60 s)
    int                 m_closedFrames = 0;
    int                 m_blinkCount   = 0;
    int                 m_frameCount   = 0;
};

} // namespace dfg
