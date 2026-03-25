🛡️ NeuralShield: Real-Time Deepfake Detection Engine
Technology should protect people — not deceive them.
📖 نبذة عن المشروع | Overview
NeuralShield هو محرك أمني متطور لسطح المكتب، مصمم لكشف الوجوه والأصوات المزيفة (Deepfakes) في الوقت الفعلي أثناء المكالمات الفيديوية. يعتمد المحرك على دمج خمسة مسارات تحليلية مستقلة لضمان أعلى مستويات الدقة والموثوقية.

NeuralShield is a high-performance desktop security tool that detects AI-generated deepfake faces and voices during live video calls (Zoom, Teams, Meet). It runs 100% locally, ensuring absolute privacy.

👨‍💻 مطور المشروع | About the Developer
Hamed Hazem Kuheil
Engineering Student | Software Architect | AI Safety Advocate

مهندس برمجيات طموح، أجمع بين دراسة هندسة الحاسوب (IUG) وهندسة البرمجيات (Riphah). شغفي يكمن في سد الفجوة بين الأمان السيبراني والذكاء الاصطناعي، مع التركيز على بناء أنظمة عالية الأداء باستخدام C++ و Low-level programming.

Focus: AI Ethics, Hardware-Software Integration, Performance Engineering.

Philosophy: Building "Safe-by-Design" technology.

🧠 آلية العمل | Core Detection Concept
يعتمد النظام على صهر (Fusion) خمس إشارات حيوية وتقنية للوصول إلى "درجة الثقة" (Trust Score):

Signal (الإشارة)	Weight	What It Detects (ماذا تكتشف)
Face CNN	40%	التشويه البصري في ملامح الوجه (Spatial Artefacts)
Audio RNN	20%	التزييف الصوتي والترددات غير الطبيعية (Voice Spoofing)
rPPG Pulse	20%	نبضات القلب من خلال تغير لون الجلد (No Heartbeat in AI)
EAR Rhythm	10%	نمط رمش العين البشري الطبيعي (Blink Anomalies)
AV Sync	10%	مطابقة حركة الشفاه مع الصوت (Lip-Phoneme Timing)
🚀 المميزات التقنية | Technical Highlights
⚡ Latency: أداء فائق بسرعة 60+ إطار في الثانية (sub-16ms) بفضل استهلاك قدرات الـ GPU.

🔒 Privacy First: معالجة محلية بالكامل؛ لا تخرج أي بيانات من جهازك (Local Inference).

🚨 Kill Switch: قاطع آلي يغلق الكاميرا فور اكتشاف تزييف عالي الثقة.

🎨 Zero-Copy Pipeline: استخدام cv::UMat لضمان بقاء البيانات داخل الـ VRAM وتجنب عنق الزجاجة.

🏗️ هندسة النظام | Architecture
تعتمد البنية على نمط Producer-Consumer باستخدام خيوط المعالجة الحديثة C++20 jthread لضمان واجهة مستخدم سلسة لا تتأثر بعمليات المعالجة الثقيلة.

Plaintext
[ Capture ] ➔ [ SPSC Ring Buffer ] ➔ [ Inference Engines ] ➔ [ Weighted EMA Fusion ] ➔ [ Qt Overlay ]
📂 هيكلية الملفات | File Structure
Bash
DeepfakeGuardian/
├── src/
│   ├── capture/      # التقاط الفيديو والصوت من النظام
│   ├── inference/    # محركات ONNX (CUDA / DirectML)
│   ├── analysis/     # خوارزميات rPPG و EAR و AV Sync
│   └── ui/           # واجهة Qt QML الشفافة
├── models/           # النماذج المشفرة (Encrypted .onnx)
└── resources/        # ملفات التصميم والقوالب
🔧 المتطلبات وبدء التشغيل | Build Quick Start
المكونات المطلوبة:
Compiler: MSVC 2022 (C++20 Support)

Libraries: Qt 6.6+, OpenCV 4.9+, ONNX Runtime GPU.

PowerShell
# Configure & Build
cmake -S . -B build -DDFG_USE_DIRECTML=ON
cmake --build build --config Release
📊 مستهدفات الأداء | Performance Targets
GPU Target: 60 FPS / < 8ms Latency.

VRAM: < 800 MB.

CPU: < 5% (Idle).

🛡️ ميثاق المشروع | Project Ethos
"نؤمن أن التكنولوجيا يجب أن تكون درعاً يحمي الحقيقة، لا أداة لتزييفها."

DeepfakeGuardian · Confidential & Proprietary · March 2026

هل تود مني إضافة أي روابط تواصل اجتماعي أو روابط لمشاريعك الأخرى (مثل SafeGuard) في قسم "نبذة عني"؟
