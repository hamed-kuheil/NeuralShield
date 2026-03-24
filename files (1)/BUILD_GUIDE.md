# DeepfakeGuardian – Environment Setup & Build Guide

## Prerequisites

| Tool | Version | Download |
|------|---------|----------|
| Visual Studio | 2022 (MSVC v143) | https://visualstudio.microsoft.com |
| CMake | ≥ 3.25 | https://cmake.org/download |
| Qt | 6.6+ (MSVC 2019 64-bit) | https://www.qt.io/download |
| OpenCV | 4.9+ (CUDA build recommended) | https://opencv.org/releases |
| ONNX Runtime | 1.17+ | https://github.com/microsoft/onnxruntime/releases |
| PortAudio | 19.7+ | http://www.portaudio.com/download.html |

---

## 1 · Install Qt 6

Run the Qt Online Installer. Select:
- **Qt 6.6.x → MSVC 2019 64-bit**
- **Qt Quick / QML** component
- Add `C:\Qt\6.6.x\msvc2019_64\bin` to your `PATH`.

---

## 2 · Build OpenCV with CUDA (optional but recommended)

```bat
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git

cmake -S opencv -B opencv_build ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DWITH_CUDA=ON ^
  -DOPENCV_EXTRA_MODULES_PATH=opencv_contrib/modules ^
  -DBUILD_opencv_python3=OFF ^
  -DCMAKE_INSTALL_PREFIX=C:/opencv

cmake --build opencv_build --config Release --parallel
cmake --install opencv_build
```

---

## 3 · Download ONNX Runtime GPU

```bat
# Download the ONNX Runtime GPU package from GitHub Releases:
# onnxruntime-win-x64-gpu-<version>.zip
# Extract to: C:\onnxruntime
```

---

## 4 · Configure & Build DeepfakeGuardian

```bat
cd DeepfakeGuardian

cmake -S . -B build ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DQt6_DIR="C:/Qt/6.6.3/msvc2019_64/lib/cmake/Qt6" ^
  -DOPENCV_DIR="C:/opencv/build" ^
  -DONNXRUNTIME_DIR="C:/onnxruntime" ^
  -DPORTAUDIO_DIR="C:/portaudio" ^
  -DDFG_USE_DIRECTML=ON ^
  -DDFG_USE_AVX2=ON

cmake --build build --config Release --parallel
```

Binary lands at: `build/bin/DeepfakeGuardian.exe`

---

## 5 · Directory Structure (after build)

```
DeepfakeGuardian/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── capture/
│   │   ├── CaptureModule.h/.cpp
│   │   ├── CameraCapture.h/.cpp
│   │   └── AudioCapture.h/.cpp
│   ├── inference/
│   │   ├── InferenceEngine.h/.cpp
│   │   └── DeepfakeClassifier.h/.cpp
│   ├── analysis/
│   │   ├── EARAnalyzer.h/.cpp
│   │   ├── RPPGAnalyzer.h/.cpp
│   │   └── AVSyncAnalyzer.h/.cpp
│   ├── pipeline/
│   │   ├── Pipeline.h/.cpp
│   │   ├── FrameBuffer.h
│   │   └── TrustScoreAggregator.h/.cpp
│   └── ui/
│       ├── OverlayController.h/.cpp
│       └── ShieldViewModel.h/.cpp
├── resources/
│   └── qml/
│       ├── ShieldOverlay.qml
│       ├── TrustMeter.qml
│       └── ControlPanel.qml
├── models/            ← encrypted .onnx files go here
├── configs/
│   └── default.json
└── build/
    └── bin/
        └── DeepfakeGuardian.exe
```

---

## 6 · Hardware Acceleration Flags Summary

| Flag | Effect |
|------|--------|
| `-DDFG_USE_DIRECTML=ON` | Uses DirectML EP in ONNX Runtime (Intel/AMD/NVIDIA) |
| `-DDFG_USE_CUDA=ON` | Uses TensorRT/CUDA EP (NVIDIA only, fastest) |
| `-DDFG_USE_AVX2=ON` | Enables AVX2 SIMD for image pre-processing |
| `-DDFG_USE_OPENCL=ON` | Activates OpenCV CUDA/OpenCL UMat paths |

---

## 7 · Models Needed

Place these ONNX models in `/models/`:

| File | Purpose | Suggested Source |
|------|---------|-----------------|
| `face_deepfake_cnn.onnx` | Frame-level fake score | FaceForensics++ trained model |
| `audio_deepfake_rnn.onnx` | Mel-spectrogram classifier | ASVspoof2019 trained model |

Encrypt them at first-run using `ModelEncryption.cpp` (AES-256 key derived from machine ID).

---

## 8 · Packaging (Inno Setup)

After building, run `windeployqt` (done automatically by CMake post-build step), then use the provided `installer.iss` script with Inno Setup to produce a one-click installer that:
- Adds the app to Windows Startup
- Creates a System Tray icon
- Installs to `%PROGRAMFILES%\DeepfakeGuardian`
