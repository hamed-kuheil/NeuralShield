#pragma once
// ============================================================
//  DeepfakeGuardian :: InferenceEngine.h
//  ONNX Runtime session manager.
//  Automatically selects: TensorRT → DirectML → CPU fallback.
// ============================================================
#include <memory>
#include <string>
#include <vector>
#include <span>

#include <onnxruntime_cxx_api.h>
#include <opencv2/core/umat.hpp>

namespace dfg {

struct InferenceResult {
    float     fakeScore;        ///< 0.0 = genuine, 1.0 = deepfake
    float     confidence;
    uint64_t  inferenceTimeUs;  ///< Latency in microseconds
};

/// -------------------------------------------------------
///  InferenceEngine
///  Wraps two ONNX sessions: face CNN + audio RNN.
///  Uses zero-copy IOBinding to keep tensors in VRAM.
/// -------------------------------------------------------
class InferenceEngine {
public:
    struct Config {
        std::string faceModelPath;     ///< Encrypted .onnx – decrypted in memory
        std::string audioModelPath;
        bool        useCUDA      = false;
        bool        useDirectML  = true;
        int         gpuDeviceId  = 0;
        int         intraThreads = 4;  ///< ORT intra-op parallelism
    };

    explicit InferenceEngine(const Config& cfg);
    ~InferenceEngine();

    /// Run face deepfake model on a GPU frame (UMat stays in VRAM).
    InferenceResult runFaceInference(const cv::UMat& frame);

    /// Run audio classifier on a mel-spectrogram buffer.
    InferenceResult runAudioInference(std::span<const float> melSpectrogram);

private:
    void buildSession(const std::string& modelPath,
                      std::unique_ptr<Ort::Session>& session);

    Ort::Env                        m_env;
    Ort::SessionOptions             m_sessionOptions;
    std::unique_ptr<Ort::Session>   m_faceSession;
    std::unique_ptr<Ort::Session>   m_audioSession;
    Ort::MemoryInfo                 m_memInfo;
    Config                          m_cfg;

    // IOBinding for zero-copy GPU tensor transfer
    std::unique_ptr<Ort::IoBinding> m_faceBinding;
    std::unique_ptr<Ort::IoBinding> m_audioBinding;
};

} // namespace dfg
