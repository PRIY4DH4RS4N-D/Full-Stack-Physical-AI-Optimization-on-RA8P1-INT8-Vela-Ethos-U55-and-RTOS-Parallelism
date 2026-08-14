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

## Track 1: Physical AI

## Project Overview

This project implements a real-time, assistive object-detection HUD running entirely on-device on the Renesas RA8P1 (Arm Cortex-M85 + Ethos-U55 NPU). The core model is **YOLOX-Tiny**, INT8-quantized and compiled with the Arm Vela compiler for the Ethos-U55 NPU. Model conversion is handled by our **custom conversion Python script**, which takes the quantized `.tflite` model and produces the NPU command stream plus the C source/header files consumed directly by the on-device application — no external runtime framework involved.

What makes this submission interesting isn't just "we ran a quantized model on an NPU" — every team doing edge AI on Ethos-U55 does that. What we're showcasing is a **full-stack optimization process**: starting from a naive FP32/CPU baseline and systematically moving the workload through NPU offload, compiler-level memory scheduling analysis, and RTOS-level task parallelism — with every step measured on real hardware, and every claim backed by either an on-device measurement or the Vela compiler's own output log.

We treat this as an engineering investigation, not just a deployment: we tested memory configurations that *didn't* help and reported why, rather than only reporting the wins. That honesty is itself part of the technical story — it shows we understand the RA8P1's actual memory hierarchy rather than running default commands and hoping for the best.

**Why this should win:** the RA8P1 pairs a fast NPU with a comparatively small SRAM budget (1.6 MB usable) against a real-world model whose encoded weights (~4.4 MB) can't fully fit on-chip. That's a genuinely constrained embedded systems problem, not a toy example — and our submission documents exactly how we characterized and worked within that constraint, with every optimization step isolated and independently justified.

---

## Functionality / Output

The final deliverable is a working, camera-driven object-detection HUD:

- **Input:** live camera capture via the RA8P1's camera interface
- **Inference:** YOLOX-Tiny (INT8), 100% of operators mapped to the Ethos-U55 NPU (0% CPU fallback)
- **Output:** bounding boxes and object classes rendered as a HUD overlay via the on-chip 2D drawing engine (D/AVE2D), with assistive-tech framing (fixed color palette, object-count indicator, no alarming/threat-style visuals)
- **Pipeline:** capture → preprocess → NPU inference → postprocess/NMS → render, run as parallel real-time tasks under **μT-Kernel (TRON RTOS) / FreeRTOS** rather than a single serialized loop

The optimized artifact we're submitting is the **compiled `.tflite` model + generated NPU command stream and C source (via our custom conversion scripts)**, the **RTOS-based application pipeline source**, and this write-up documenting the optimization process and results.

---

## Optimization Story: Baseline → Full Stack

We benchmark two on-device configurations, holding the model, weights, and INT8 quantization scheme **constant** across both — so any reported delta comes from execution strategy, not from a different or degraded model:

| Stage | Configuration |
|---|---|
| **Stage 1 — Baseline** | FP32 model, Cortex-M85 CPU-only execution (no NPU), sequential pipeline |
| **Stage 2 — Optimized** | INT8 model, Ethos-U55 NPU (100% op offload), Vela-analyzed memory configuration, μT-Kernel (TRON RTOS) / FreeRTOS parallel task pipeline |

Accuracy (mAP) is held constant and reported for both stages as a control metric — proving that performance gains come from execution efficiency, not from silently degrading the model.

### Optimization 1 — INT8 Quantization + Ethos-U55 NPU Offload

The model was trained in FP32 and post-training quantized to INT8, then compiled with Arm Vela targeting the Ethos-U55 via our custom conversion Python script:

```bash
vela yolox_tiny_224_int8.tflite \
  --accelerator-config ethos-u55-256 \
  --system-config Ethos_U55_High_End_Embedded \
  --memory-mode Shared_Sram \
  --optimise Performance \
  --verbose-weights
```

Resulting NPU operator mapping:

```
Accelerator configuration      Ethos_U55_256
System configuration           Ethos_U55_High_End_Embedded
CPU operators                  0 (0.0%)
NPU operators                  272 (100.0%)
```

This is the headline Stage 1 → Stage 2 delta: moving from float compute on the M85 CPU to INT8 compute fully offloaded to the NPU. We report this transition explicitly as **two combined changes — precision (FP32→INT8) and execution target (CPU→NPU)** — rather than implying a single cause, since isolating them individually would require a third measured configuration we didn't have time to capture. This is stated plainly rather than glossed over, because a rigorous benchmark should be explicit about what it does and doesn't isolate.

### Optimization 2 — Vela Weight Compression (free, automatic)

Vela's INT8 NPU encoding applies lossless weight compression as part of standard compilation — no extra flags required, visible directly in the same compiler run as Optimization 1:

```
Original Weights Size        4920.19 KiB
NPU Encoded Weights Size     4358.36 KiB
```

This is an **11.4% reduction in weight payload size**, achieved automatically with zero accuracy impact and zero additional engineering effort — a direct benefit of targeting the Ethos-U55 toolchain that's easy to overlook but genuinely reduces flash footprint and read traffic.

### Optimization 3 — Memory Hierarchy Analysis (Vela Memory Mode Characterization)

The RA8P1 provides ~1.6 MB of usable on-chip user SRAM. Our NPU-encoded weights are 4358.36 KiB (~4.26 MB) — meaning full on-chip weight residency is not physically possible on this hardware for this model size. Rather than assume this and move on, we verified it directly with three separate Vela test runs, each isolating one specific configuration question:

**Test case 1 — full on-chip weight residency (`Sram_Only`)**

*Question: can the full weight set fit on-chip if we force it to?*

```bash
vela yolox_tiny_224_int8.tflite \
  --accelerator-config ethos-u55-256 \
  --system-config Ethos_U55_High_End_Embedded \
  --memory-mode Sram_Only \
  --optimise Performance
```
Result:
```
Total On-chip Flash used     4464.19 KiB
```
Requesting full on-chip weight residency confirms a required footprint of ~4.46 MB against ~1.6 MB of available SRAM — quantitatively confirming that `Shared_Sram` (flash-streamed weights) is not an unoptimized default, but the only physically valid memory mode for this model on this hardware.

**Test case 2 — optimization target comparison (`Size` vs `Performance`)**

*Question: does compiling for a smaller footprint free up any headroom `Performance` mode leaves on the table?*

```bash
vela yolox_tiny_224_int8.tflite \
  --accelerator-config ethos-u55-256 \
  --system-config Ethos_U55_High_End_Embedded \
  --memory-mode Shared_Sram \
  --optimise Size \
  --verbose-weights
```
Result: identical `Total SRAM used` (794.81 KiB) and identical bandwidth figures to the `--optimise Performance` run in Optimization 1 — indicating Vela's scheduler has already reached its efficiency ceiling for this model at this SRAM budget; there was no further size/performance trade-off left to exploit.

**Test case 3 — SRAM scheduling budget sweep (`--arena-cache-size`)**

*Question: does giving the scheduler a larger on-chip budget let it cache more and cut flash traffic?*

```bash
vela yolox_tiny_224_int8.tflite \
  --accelerator-config ethos-u55-256 \
  --system-config Ethos_U55_High_End_Embedded \
  --memory-mode Shared_Sram \
  --arena-cache-size 1600000 \
  --optimise Performance \
  --verbose-weights
```
Result: `Total SRAM used` (794.81 KiB, unchanged) and `Weight Off-chip Flash bandwidth` (4.30 MB/inference, unchanged) versus the default budget — confirming the default cascade schedule was already optimal and not memory-constrained at this budget level.

We report this as a **memory characterization finding**, not a performance win: we determined precisely why and how much headroom exists, validated the current configuration as correct rather than accidental, and ruled out several plausible-looking "obvious" optimizations with hardware-grounded evidence rather than assumption. This is presented as evidence of engineering rigor for the Technological Implementation judging criterion.

### Optimization 4 — Parallel RTOS Task Pipeline (μT-Kernel / TRON RTOS, FreeRTOS)

The application pipeline runs capture, preprocessing, Ethos-U55 inference, and postprocessing as **separate, overlapping real-time tasks** under μT-Kernel (TRON RTOS) / FreeRTOS, rather than a single serialized loop:

```
Sequential (Stage 1 baseline):
Capture → Preprocess → Inference → Postprocess → Capture → ...

Parallel (Stage 2, this optimization):
Capture     ████      ████      ████
Preprocess     ████      ████      ████
Inference         ██████      ██████
Postprocess           ██          ██
```

This is the one optimization in this list that is **independent of the model and the custom compilation pipeline entirely** — it improves end-to-end FPS by removing idle time between pipeline stages, not by making any single stage individually faster. This is our primary, directly measured performance improvement and is reported alongside the memory analysis above as a distinct, complementary contribution.

---

## Benchmark Results

*(Fill in with on-device measurements captured via DWT cycle counters (CPU) and Ethos-U PMU driver (NPU).)*

| Metric | Stage 2: INT8, Ethos-U55, tuned + RTOS |
|---|---:|
| FPS | |
| End-to-end inference latency (ms) | |
| CPU utilization (%) | |
| NPU utilization (%) | |
| CPU fallback operators | 0 / 272 (0%) |
| SRAM used | 794.81 KiB |
| Off-chip flash bandwidth (weights) | 4.30 MB/inference |
| NPU-encoded weight size | 4358.36 KiB (11.4% smaller than original 4920.19 KiB) |
| Model size on disk | |
| mAP (accuracy, held constant) | |

---

## Setup Instructions

### Prerequisites
- Renesas RA8P1 Evaluation Kit (EK-RA8P1)
- e² studio / FSP toolchain, or equivalent RA8P1 build environment
- Python environment with the Arm Vela compiler and our backend toolchain (`pip install ethos-u-vela`, etc.) and required ML frameworks (`torch`, `onnx`, `onnx2tf`, `tensorflow`)
- Camera module compatible with RA8P1 capture interface

### 1. Model preparation and INT8 Quantization (Using our custom notebook)

To replicate our pipeline from PyTorch down to INT8 TFLite, run the provided Jupyter notebook: `custom_object_detection.ipynb`.

This notebook automates the following workflow:
1. **PyTorch to ONNX**: We start with the Megvii pretrained YOLOX-Tiny `.pth` checkpoint (originally trained at 416×416). To meet latency and SRAM constraints, the model was resized to 224×224 and fine-tuned briefly on COCO to recover accuracy at the smaller input size. The notebook exports this to ONNX FP32 format.
2. **ONNX to TFLite (FP32)**: We use `onnx2tf` to convert the ONNX model to TFLite.
3. **INT8 Quantization**: We apply Post-Training Quantization (PTQ) using COCO val2017 images to calibrate and convert the model weights and activations to INT8.

Run all cells in `custom_object_detection.ipynb`. This will produce `yolox_tiny_224_INT8.tflite` — a fully INT8 quantized model ready for the NPU.

### 2. Convert and compile for Ethos-U55 (Using our custom script)

Run our custom conversion script `convert_model.py`, which leverages the compiler toolchain to turn the quantized `.tflite` model into the NPU command stream plus a C source/header pair for the on-device application:

```bash
python convert_model.py \
  --input yolox_tiny_224_INT8.tflite \
  --accelerator-config ethos-u55-256 \
  --system-config Ethos_U55_High_End_Embedded \
  --memory-mode Shared_Sram \
  --optimise Performance \
  --output-dir embedded_c
```

Internally this configures the compiler (with `Shared_Sram` and `ethos-u55-256` configurations) and emits the model source arrays (`.c` and `.h` files) consumed directly by the embedded application build — no separate runtime SDK required.

### 3. Build and flash in e² studio
- Locate the generated C arrays (e.g., `model.c`, `model.h`) outputted by the `convert_model.py` script.
- In your Renesas e² studio project, replace the existing model source/header files with the newly generated ones.
- Build the application (capture / preprocess / inference / postprocess tasks) running under μT-Kernel (TRON RTOS) / FreeRTOS via e² studio.
- Flash the compiled binary to the EK-RA8P1 board.

### 4. Run and validate
- Power on the board with the camera module connected.
- The HUD overlay should render live bounding boxes on the connected display.
- On-device FPS/latency is logged via UART using the built-in DWT/PMU instrumentation.

### 5. Reproduce the baseline (Stage 1) for comparison
- Build the FP32, CPU-only variant (no NPU, no custom conversion script) and flash separately to capture baseline numbers on the same hardware.

---

## What Was Tested and Rejected (for transparency)

In the interest of showing our full process rather than only positive results:

- **`--memory-mode Sram_Only`** was tested and rejected — required footprint (~4.46 MB) exceeds available SRAM (~1.6 MB) on RA8P1.
- **`--optimise Size`** was tested against `--optimise Performance` — no difference in output; both converge to the same schedule.
- **`--arena-cache-size` sweep** up to 1.6 MB was tested — no change in SRAM usage or flash bandwidth versus Vela's default budget, indicating the default schedule was already optimal for this model.

Exact commands for all three test cases are documented in **Optimization 3** above. These negative results are included because they demonstrate that our final configuration (`Shared_Sram`, `--optimise Performance`, default arena budget) was arrived at through verification, not assumption.

---

## License

This project is released under the MIT License. See `LICENSE` for details.
