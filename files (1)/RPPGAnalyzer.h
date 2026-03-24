#pragma once
// ============================================================
//  DeepfakeGuardian :: RPPGAnalyzer.h
//  Remote Photoplethysmography – detects heartbeat via subtle
//  RGB skin-color oscillations (~0.5-3 Hz band).
//  Real humans have a pulse; deepfakes typically don't.
// ============================================================
#include <deque>
#include <optional>
#include <opencv2/core.hpp>

namespace dfg {

struct RPPGResult {
    bool    pulseDetected;      ///< True if a valid BPM was found
    float   bpm;                ///< Beats per minute (0 if not detected)
    float   snr;                ///< Signal-to-noise ratio of the pulse
    float   anomalyScore;       ///< 0 = biological, 1 = synthetic
};

/// -------------------------------------------------------
///  RPPGAnalyzer
///  Algorithm: Plane-Orthogonal-to-Skin (POS) method.
///  Runs on the CPU using AVX2-accelerated OpenCV routines.
///  Window: 10 seconds of frames for reliable FFT.
/// -------------------------------------------------------
class RPPGAnalyzer {
public:
    struct Config {
        int   fps           = 30;     ///< Capture frame rate
        int   windowSecs    = 10;     ///< Sliding window length
        float lowHz         = 0.75f;  ///< ~45 BPM minimum
        float highHz        = 3.33f;  ///< ~200 BPM maximum
    };

    explicit RPPGAnalyzer(Config cfg = {});

    /// Feed a new frame. Returns a result every `windowSecs` seconds,
    /// otherwise returns nullopt (not enough data yet).
    std::optional<RPPGResult> update(const cv::Mat& frame,
                                     const cv::Rect& faceROI);

    void reset();

private:
    /// Extract mean RGB from forehead / cheek ROIs within the face bbox.
    cv::Vec3f extractSkinSignal(const cv::Mat& frame, const cv::Rect& face) const;

    /// POS algorithm: project RGB channels onto the pulse plane.
    std::vector<float> posFilter(const std::vector<cv::Vec3f>& rgbWindow) const;

    /// Welch's periodogram to estimate dominant frequency.
    float dominantFrequency(const std::vector<float>& signal) const;

    Config                     m_cfg;
    std::deque<cv::Vec3f>      m_rgbBuffer;   ///< Rolling raw RGB samples
    int                        m_maxSamples;
};

} // namespace dfg
