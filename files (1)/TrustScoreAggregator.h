#pragma once
// ============================================================
//  DeepfakeGuardian :: TrustScoreAggregator.h
//  Fuses CNN score, rPPG, EAR, and AV-sync into a single
//  trust score in [0, 1].  0 = deepfake, 1 = genuine.
// ============================================================
#include <optional>

namespace dfg {

struct TrustInput {
    float cnnFakeScore;          ///< From InferenceEngine (face CNN)
    float audioFakeScore;        ///< From AudioAnalyzer (RNN)
    std::optional<float> rppgAnomalyScore;   ///< May be absent if no face
    std::optional<float> earAnomalyScore;
    std::optional<float> avSyncAnomalyScore;
};

struct TrustOutput {
    float trustScore;            ///< [0,1] – 0 = fake, 1 = real
    bool  shouldTriggerKillSwitch;
    char  label[32];             ///< "GENUINE" / "SUSPICIOUS" / "DEEPFAKE"
};

/// -------------------------------------------------------
///  TrustScoreAggregator
///  Uses a configurable weighted sum, then applies a Kalman-
///  style temporal smoother to avoid flickering.
/// -------------------------------------------------------
class TrustScoreAggregator {
public:
    struct Weights {
        float cnn       = 0.40f;
        float audio     = 0.20f;
        float rppg      = 0.20f;
        float ear       = 0.10f;
        float avSync    = 0.10f;
    };

    struct Config {
        Weights weights;
        float   killSwitchThreshold = 0.25f;  ///< Trust below this → kill
        float   smoothingAlpha      = 0.35f;  ///< EMA smoothing factor
    };

    explicit TrustScoreAggregator(Config cfg = {});

    TrustOutput aggregate(const TrustInput& input);

    /// Reset EMA smoother (e.g., on new call session).
    void resetSmoothing();

private:
    Config m_cfg;
    float  m_smoothedScore = 1.0f;  ///< Starts "genuine" until proven fake
};

} // namespace dfg
