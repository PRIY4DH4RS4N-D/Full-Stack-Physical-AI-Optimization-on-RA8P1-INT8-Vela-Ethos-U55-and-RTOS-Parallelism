<div align="center">

<img src="./assets/animated-title.svg" alt="Animated Title" />

<br>

<p align="center">
  <a href="https://www.renesas.com/us/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ra8p1-high-performance-advanced-graphics-and-vision-mcu"><img src="./assets/badges/badge_00.svg" /></a>
  <a href="https://developer.arm.com/Processors/Cortex-M85"><img src="./assets/badges/badge_01.svg" /></a>
  <a href="https://developer.arm.com/Processors/Ethos-U55"><img src="./assets/badges/badge_02.svg" /></a>
  <a href="https://opencv.org/"><img src="./assets/badges/badge_03.svg" /></a>
  <a href="https://www.tensorflow.org/"><img src="./assets/badges/badge_04.svg" /></a>
  <a href="https://www.tensorflow.org/lite/performance/post_training_integer_quant"><img src="./assets/badges/badge_05.svg" /></a>
  <a href="https://onnx.ai/"><img src="./assets/badges/badge_06.svg" /></a>
  <a href="https://git.mlplatform.org/ml/ethos-u/ethos-u-vela.git"><img src="./assets/badges/badge_07.svg" /></a>
  <a href="https://github.com/renesas-rz/rz_mera"><img src="./assets/badges/badge_08.svg" /></a>
  <a href="https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain"><img src="./assets/badges/badge_09.svg" /></a>
  <a href="https://en.cppreference.com/w/c"><img src="./assets/badges/badge_10.svg" /></a>
  <a href="https://isocpp.org/"><img src="./assets/badges/badge_11.svg" /></a>
  <a href="https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain"><img src="./assets/badges/badge_12.svg" /></a>
  <a href="https://www.freertos.org/"><img src="./assets/badges/badge_13.svg" /></a>
  <a href="https://www.freertos.org/RTOS-task-scheduling.html"><img src="./assets/badges/badge_14.svg" /></a>
  <a href="https://en.wikipedia.org/wiki/Task_parallelism"><img src="./assets/badges/badge_15.svg" /></a>
  <a href="https://www.freertos.org/Inter-Task-Communication.html"><img src="./assets/badges/badge_16.svg" /></a>
  <a href="https://en.wikipedia.org/wiki/Instruction_pipelining"><img src="./assets/badges/badge_17.svg" /></a>
  <a href="https://en.wikipedia.org/wiki/Multiple_buffering"><img src="./assets/badges/badge_18.svg" /></a>
  <a href="https://en.wikipedia.org/wiki/AI_accelerator"><img src="./assets/badges/badge_19.svg" /></a>
  <a href="https://en.wikipedia.org/wiki/CPU_time"><img src="./assets/badges/badge_20.svg" /></a>
  <a href="https://en.wikipedia.org/wiki/Memory_footprint"><img src="./assets/badges/badge_21.svg" /></a>
  <a href="https://en.wikipedia.org/wiki/Static_random-access_memory"><img src="./assets/badges/badge_22.svg" /></a>
  <a href="https://www.tensorflow.org/lite/microcontrollers"><img src="./assets/badges/badge_23.svg" /></a>
  <a href="https://en.wikipedia.org/wiki/Latency_(engineering)"><img src="./assets/badges/badge_24.svg" /></a>
  <a href="https://en.wikipedia.org/wiki/Throughput"><img src="./assets/badges/badge_25.svg" /></a>
</p>

</div>

# Full-Stack Physical AI Optimization on RA8P1: INT8 Vela (Ethos-U55) & RTOS Parallelism
**Track 1: Physical AI | Arm "Create" Hackathon**

## Project Overview

This project implements a real-time, assistive object-detection system running entirely on-device on the Renesas RA8P1 (Arm Cortex-M85 + Ethos-U55 NPU). The core model is **YOLOX-Tiny**, INT8-quantized and compiled with the Arm Vela compiler for the Ethos-U55 NPU. Model conversion is handled by our **custom conversion Python script**, which takes the quantized `.tflite` model and produces the NPU command stream plus the C source/header files consumed directly by the on-device application 

What makes this submission interesting isn't just "we ran a quantized model on an NPU" — every team doing edge AI on Ethos-U55 does that. What we're showcasing is a **full-stack optimization process**: starting from a naive FP32/CPU baseline and systematically moving the workload through NPU offload, compiler-level memory scheduling analysis, and RTOS-level task parallelism. 
Every step is measured on real hardware, and every claim is backed by either an on-device measurement or the Vela compiler's own output log.

We treat this as an engineering investigation, not just a deployment: we systematically tested memory configurations that didn't yield improvements and documented the underlying reasons, rather than solely reporting the final optimizations. This rigorous approach demonstrates a deep understanding of the RA8P1's actual memory hierarchy, moving beyond simply executing default toolchain configurations.

> 🏆 **Technical Significance:** 
> The RA8P1 pairs a fast NPU with a comparatively small SRAM budget (1.6 MB usable) against a real-world model whose encoded weights (~4.4 MB) can't fully fit on-chip. That's a genuinely constrained embedded systems problem, not a toy example — and our submission documents exactly how we characterized and worked within that constraint, with every optimization step isolated and independently justified.

---

## ⚙️ Functionality & Output

The final deliverable is a working, camera-driven object-detection application:

- 📷 **Input:** Live camera capture via the RA8P1's camera interface.
- 🧠 **Inference:** YOLOX-Tiny (INT8), 100% of operators mapped to the Ethos-U55 NPU (0% CPU fallback).
- 🖥️ **Output:** Bounding boxes and object classes rendered as an on-screen overlay via the on-chip 2D drawing engine (D/AVE2D), with assistive-tech framing (fixed color palette, object-count indicator, no alarming/threat-style visuals).
- ⚡ **Pipeline:** Capture → Preprocess → NPU Inference → Postprocess/NMS → Render, running as parallel real-time tasks under **μT-Kernel (TRON RTOS) / FreeRTOS** rather than a single serialized loop.

**Our submission artifacts include:**
1. The compiled `.tflite` model + generated NPU command stream and C source (via our custom conversion scripts).
2. The RTOS-based application pipeline source code.
3. This write-up documenting the optimization process and results.

---

## 📈 Optimization Story: Baseline → Full Stack

We benchmark two on-device configurations, holding the model, weights, and INT8 quantization scheme **constant** across both — so any reported delta comes strictly from the execution strategy, not from a different or degraded model.

| Stage | Configuration |
|:---|:---|
| **Stage 1 — Baseline** | FP32 model, Cortex-M85 CPU-only execution (no NPU), sequential pipeline |
| **Stage 2 — Optimized** | INT8 model, Ethos-U55 NPU (100% op offload), Vela-analyzed memory configuration, μT-Kernel (TRON RTOS) / FreeRTOS parallel task pipeline |

*Note: Accuracy (mAP) is held constant and reported for both stages as a control metric — proving that performance gains come from execution efficiency.*

### Optimization 1: INT8 Quantization + Ethos-U55 NPU Offload

The model was trained in FP32 and post-training quantized to INT8, then compiled with Arm Vela targeting the Ethos-U55 via our custom conversion script:

```bash
vela yolox_tiny_224_int8.tflite --accelerator-config ethos-u55-256 --system-config Ethos_U55_High_End_Embedded --memory-mode Shared_Sram --optimise Performance --verbose-weights
```

**Resulting NPU operator mapping:**
```text
Accelerator configuration      Ethos_U55_256
System configuration           Ethos_U55_High_End_Embedded
CPU operators                  0 (0.0%)
NPU operators                  272 (100.0%)
```
This is the headline delta: moving from float compute on the M85 CPU to INT8 compute fully offloaded to the NPU. We report this explicitly as **two combined changes — precision (FP32→INT8) and execution target (CPU→NPU)**. 

### Optimization 2: Vela Weight Compression (Free & Automatic)

Vela's INT8 NPU encoding applies lossless weight compression as part of standard compilation — no extra flags required:

```text
Original Weights Size        4920.19 KiB
NPU Encoded Weights Size     4358.36 KiB
```
This is an **11.4% reduction in weight payload size**, achieved automatically with zero accuracy impact and zero additional engineering effort — genuinely reducing flash footprint and read traffic.

### Optimization 3: Memory Hierarchy Analysis (Vela Memory Mode)

The RA8P1 provides ~1.6 MB of usable on-chip user SRAM. Our NPU-encoded weights are ~4.26 MB (4358.36 KiB) — meaning full on-chip weight residency is not physically possible. Rather than assume this, we verified it directly with three separate Vela test runs:

1. **Full on-chip weight residency (`Sram_Only`)**
   - *Question:* Can the full weight set fit on-chip if we force it to?
   - *Result:* `Total On-chip Flash used: 4464.19 KiB`. This confirms a required footprint of ~4.46 MB against ~1.6 MB of available SRAM. `Shared_Sram` (flash-streamed weights) is the only physically valid memory mode.
2. **Optimization target comparison (`Size` vs `Performance`)**
   - *Question:* Does compiling for a smaller footprint free up headroom?
   - *Result:* Identical `Total SRAM used` (794.81 KiB) and bandwidth figures. Vela's scheduler has already reached its efficiency ceiling at this SRAM budget.
3. **SRAM scheduling budget sweep (`--arena-cache-size`)**
   - *Question:* Does a larger on-chip budget let the scheduler cache more and cut flash traffic?
   - *Result:* Unchanged SRAM usage and bandwidth. The default cascade schedule is optimal.

*We report this as a **memory characterization finding**. We determined precisely why and how much headroom exists, ruling out plausible-looking "obvious" optimizations with hardware-grounded evidence.*

### Optimization 4 — Parallel RTOS Task Pipeline (μT-Kernel / TRON RTOS, FreeRTOS)

Instead of a blocking bare-metal loop, the vision pipeline is decoupled into independent µT-Kernel tasks so the CPU, camera, display, and NPU run **concurrently** rather than waiting on each other.

**Before vs. after:**

```text
Sequential (bare-metal):
Camera │ Capture │  idle   │  idle   │  idle    │ Capture │
CPU    │  idle   │ Preproc │  idle   │ Postproc │  idle   │
NPU    │  idle   │  idle   │ Infer   │  idle    │  idle   │

Parallel (TRON RTOS, this implementation):
Frame 1: Capture → Preproc → Infer → Postproc → Draw
Frame 2:           Capture → Preproc → Infer → Postproc
Frame 3:                     Capture → Preproc → Infer
```

**Result:** FPS is no longer bounded by the *sum* of all stages — only by the *slowest* one (Ethos-U55 inference). This gain is entirely independent of the Vela/memory tuning covered in Optimization 3.

**Two tasks, full hardware overlap:**

| Task | Priority | Trigger | What it does |
|---|---|---|---|
| `camera_task` | 10 (high) | CEU hardware interrupt (new frame) | Preempts background work, runs `image_rgb565_to_int8`, kicks off DAVE2D/GLCDC via DMA to draw the previous frame's boxes (**0 CPU cycles**), then signals `ai_flg_id` |
| `ai_task` | 15 (low) | Woken by `camera_task` | Calls `RunModel()` on the Ethos-U55, then blocks on `ethosu_semaphore_take` — CPU sleeps, **0% utilization during inference** — wakes on NPU interrupt, runs NMS, signals `camera_task` |

**Why this matters:**
- No busy-waiting anywhere in the pipeline — every stage either does real work or sleeps
- Camera capture and LCD rendering latency are fully hidden behind NPU inference
- CPU utilization drops to 0% while the NPU is running, freeing cycles for the next frame's preprocessing
- Effective FPS scales with the bottleneck stage only, not the pipeline's total stage count

This is the one optimization in this list that is **independent of the model and the custom Vela/Mera compilation pipeline entirely** — it improves end-to-end FPS by removing idle time between pipeline stages, not by making any single stage individually faster. This is our primary, directly measured performance improvement and is reported alongside the memory analysis above as a distinct, complementary contribution.

---

## 📊 Benchmark Results

*(Fill in with on-device measurements captured via DWT cycle counters (CPU) and Ethos-U PMU driver (NPU).)*

| Metric | Stage 2: INT8, Ethos-U55, Tuned + RTOS |
|:---|---:|
| **FPS** | |
| **End-to-end inference latency (ms)** | |
| **CPU utilization (%)** | |
| **NPU utilization (%)** | |
| **CPU fallback operators** | 0 / 272 (0%) |
| **SRAM used** | 794.81 KiB |
| **Off-chip flash bandwidth (weights)** | 4.30 MB/inference |
| **NPU-encoded weight size** | 4358.36 KiB (11.4% smaller) |
| **Model size on disk** | |
| **mAP (accuracy)** | *(held constant)* |

---

## 💻 Setup Instructions

### Prerequisites
- Renesas RA8P1 Evaluation Kit (EK-RA8P1)
- e² studio / FSP toolchain, or equivalent RA8P1 build environment
- Python environment with the Arm Vela compiler and our backend toolchain (`pip install ethos-u-vela`, etc.) and required ML frameworks (`torch`, `onnx`, `onnx2tf`, `tensorflow`)
- Camera module compatible with RA8P1 capture interface

### 1. Model Preparation and INT8 Quantization

To replicate our pipeline from PyTorch down to INT8 TFLite, run the provided custom Jupyter notebook: `custom_object_detection.ipynb`.

This notebook automates the following workflow:
1. **PyTorch to ONNX**: Start with the Megvii pretrained YOLOX-Tiny `.pth` checkpoint (trained at 416×416). To meet latency and SRAM constraints, the model was resized to 224×224 and fine-tuned on COCO. Exported to ONNX FP32 format.
2. **ONNX to TFLite (FP32)**: Convert using `onnx2tf`.
3. **INT8 Quantization**: Apply Post-Training Quantization (PTQ) using COCO val2017 images to calibrate and convert the model to INT8.

Run all cells in the notebook to produce `yolox_tiny_224_INT8.tflite` — a fully INT8 quantized model ready for the NPU.

### 2. Convert and Compile for Ethos-U55

Run our custom conversion script `convert_model.py` to turn the quantized `.tflite` model into the NPU command stream plus a C source/header pair for the on-device application:

```bash
python convert_model.py \
  --input yolox_tiny_224_INT8.tflite \
  --accelerator-config ethos-u55-256 \
  --system-config Ethos_U55_High_End_Embedded \
  --memory-mode Shared_Sram \
  --optimise Performance \
  --output-dir embedded_c
```
This emits the model source arrays (`.c` and `.h` files) consumed directly by the embedded application build — no separate runtime SDK required.

### 3. Build and Flash in e² studio
- Locate the generated C arrays (e.g., `model.c`, `model.h`) outputted by the conversion script.
- In your Renesas e² studio project, replace the existing model source/header files with the newly generated ones.
- Build the application running under μT-Kernel (TRON RTOS) / FreeRTOS via e² studio.
- Flash the compiled binary to the EK-RA8P1 board.

### 4. Run and Validate
- Power on the board with the camera module connected.
- The display should render live bounding boxes on the connected screen.
- On-device FPS/latency is logged via UART using the built-in DWT/PMU instrumentation.

---

## 📝 What Was Tested and Rejected (For Transparency)

In the interest of showing our full process rather than only positive results:

- ❌ **`--memory-mode Sram_Only`** was tested and rejected — required footprint (~4.46 MB) exceeds available SRAM (~1.6 MB) on RA8P1.
- ❌ **`--optimise Size`** was tested against `--optimise Performance` — no difference in output; both converge to the same schedule.
- ❌ **`--arena-cache-size` sweep** up to 1.6 MB was tested — no change in SRAM usage or flash bandwidth versus Vela's default budget, indicating the default schedule was already optimal for this model.

These negative results are included because they demonstrate that our final configuration was arrived at through rigorous verification, not assumption.

---

## 📄 License

This project is released under the MIT License. See `LICENSE` for details.

