<div align="center">

<!-- CINEMATIC HERO SVG -->
<svg width="800" height="340" viewBox="0 0 800 340" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <radialGradient id="glow" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="rgba(56, 189, 248, 0.3)" />
      <stop offset="100%" stop-color="rgba(0, 0, 0, 0)" />
    </radialGradient>
    <radialGradient id="glow-npu" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="rgba(52, 211, 153, 0.5)" />
      <stop offset="100%" stop-color="rgba(0, 0, 0, 0)" />
    </radialGradient>
    <filter id="blur" x="-50%" y="-50%" width="200%" height="200%">
      <feGaussianBlur stdDeviation="4" result="blur" />
      <feComposite in="SourceGraphic" in2="blur" operator="over" />
    </filter>
  </defs>

  <style>
    .bg { fill: #070B14; }
    .title-main { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; font-size: 42px; font-weight: 800; fill: #ffffff; text-anchor: middle; letter-spacing: 2px; }
    .title-hl { fill: #38bdf8; }
    .title-sub { font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace; font-size: 16px; font-weight: 500; fill: #94a3b8; text-anchor: middle; letter-spacing: 4px; }
    .title-tags { font-family: ui-monospace, SFMono-Regular, monospace; font-size: 13px; font-weight: 600; fill: #34d399; text-anchor: middle; letter-spacing: 3px; }
    .grid { stroke: rgba(56, 189, 248, 0.08); stroke-width: 1; }
    
    @keyframes pulse { 0%, 100% { opacity: 0.3; transform: scale(0.98); } 50% { opacity: 1; transform: scale(1.02); } }
    @keyframes dash { to { stroke-dashoffset: -100; } }
    @keyframes flow { 0% { stroke-dashoffset: 200; } 100% { stroke-dashoffset: 0; } }
    
    .node-pulse { animation: pulse 3s infinite ease-in-out; transform-origin: center; }
    .conn { fill: none; stroke: #1e293b; stroke-width: 2; }
    .conn-active { fill: none; stroke: #38bdf8; stroke-width: 2; stroke-dasharray: 4 4; animation: dash 2s linear infinite; opacity: 0.5; }
    .flow { fill: none; stroke: #34d399; stroke-width: 2; stroke-dasharray: 10 100; animation: flow 2s linear infinite; }
    
    .node-rect { fill: #0f172a; stroke: #38bdf8; stroke-width: 1.5; rx: 4; }
    .node-text { font-family: ui-monospace, SFMono-Regular, monospace; font-size: 10px; font-weight: 600; fill: #e2e8f0; text-anchor: middle; dominant-baseline: middle; }
    .node-text-hl { fill: #34d399; }
  </style>

  <rect width="100%" height="100%" class="bg" rx="12" />

  <!-- Background Grid -->
  <g class="grid">
    <path d="M0,40 L800,40 M0,80 L800,80 M0,120 L800,120 M0,160 L800,160 M0,200 L800,200 M0,240 L800,240 M0,280 L800,280 M0,320 L800,320" />
    <path d="M100,0 L100,340 M200,0 L200,340 M300,0 L300,340 M400,0 L400,340 M500,0 L500,340 M600,0 L600,340 M700,0 L700,340" />
  </g>

  <!-- Glowing Nodes Background -->
  <circle cx="400" cy="110" r="160" fill="url(#glow)" class="node-pulse" />
  
  <!-- Neural Paths -->
  <path d="M-50,50 Q200,150 400,150 T850,50" class="conn-active" />
  <path d="M-50,250 Q200,150 400,150 T850,250" class="conn-active" />
  
  <!-- Title Typography -->
  <text x="400" y="80" class="title-main" filter="url(#blur)">FULL-STACK</text>
  <text x="400" y="80" class="title-main">FULL-STACK</text>
  
  <text x="400" y="130" class="title-main title-hl" filter="url(#blur)">PHYSICAL AI</text>
  <text x="400" y="130" class="title-main title-hl">PHYSICAL AI</text>
  
  <text x="400" y="180" class="title-main" filter="url(#blur)">OPTIMIZATION</text>
  <text x="400" y="180" class="title-main">OPTIMIZATION</text>

  <text x="400" y="225" class="title-sub">ON RENESAS RA8P1</text>
  
  <line x1="250" y1="245" x2="550" y2="245" stroke="#1e293b" stroke-width="2" />
  <line x1="380" y1="245" x2="420" y2="245" stroke="#38bdf8" stroke-width="2" class="node-pulse" />

  <text x="400" y="268" class="title-tags">INT8 • VELA • ETHOS-U55 • RTOS PARALLELISM</text>

  <!-- Physical AI Pipeline -->
  <g transform="translate(0, 310)">
    <line x1="50" y1="0" x2="750" y2="0" class="conn" />
    <line x1="50" y1="0" x2="750" y2="0" class="flow" />
    
    <!-- MODEL -->
    <rect x="50" y="-12" width="60" height="24" class="node-rect" />
    <text x="80" y="1" class="node-text">MODEL</text>
    
    <!-- INT8 -->
    <rect x="170" y="-12" width="60" height="24" class="node-rect" />
    <text x="200" y="1" class="node-text">INT8</text>
    
    <!-- COMPILER -->
    <rect x="290" y="-12" width="80" height="24" class="node-rect" />
    <text x="330" y="1" class="node-text">COMPILER</text>
    
    <!-- NPU -->
    <circle cx="450" cy="0" r="25" fill="url(#glow-npu)" class="node-pulse" />
    <rect x="420" y="-12" width="60" height="24" class="node-rect" stroke="#34d399" />
    <text x="450" y="1" class="node-text node-text-hl">NPU</text>
    
    <!-- MEMORY -->
    <rect x="540" y="-12" width="70" height="24" class="node-rect" />
    <text x="575" y="1" class="node-text">MEMORY</text>
    
    <!-- RTOS -->
    <rect x="670" y="-12" width="60" height="24" class="node-rect" />
    <text x="700" y="1" class="node-text">RTOS</text>
  </g>
</svg>

<!-- STATIC FALLBACK TITLES (STAYS VISIBLE IF GITHUB STRIPS SVG NATIVELY) -->
<br>
<h1 align="center">FULL-STACK <br> PHYSICAL AI <br> OPTIMIZATION</h1>
<h3 align="center">ON RENESAS RA8P1</h3>
<p align="center"><b>INT8 • VELA • ETHOS-U55 • RTOS PARALLELISM</b></p>
<br>

<!-- ONE-LINE TAGLINE -->
<p align="center"><i>"From neural-network quantization to deterministic real-time edge execution."</i></p>

<br><br>

<!-- TECHNOLOGY STACK -->
<h2 align="center">⚙️ Technology Stack</h2>

<table style="border-collapse: collapse; border: none; width: 100%; max-width: 900px;">
  <tr style="border: none;">
    <!-- HARDWARE -->
    <td align="center" width="33%" style="border: 1px solid #30363d; padding: 25px; vertical-align: top;">
      <h4>🛠️ HARDWARE</h4>
      <img src="https://img.shields.io/badge/MCU-Renesas_RA8P1-005C97?style=for-the-badge&logo=renesas&logoColor=white" alt="RA8P1" />
      <br><br>
      <img src="https://img.shields.io/badge/CPU-Arm_Cortex--M85-0091BD?style=for-the-badge&logo=arm&logoColor=white" alt="Cortex-M85" />
      <br><br>
      <img src="https://img.shields.io/badge/NPU-Arm_Ethos--U55-0091BD?style=for-the-badge&logo=arm&logoColor=white" alt="Ethos-U55" />
    </td>
    <!-- AI OPTIMIZATION -->
    <td align="center" width="33%" style="border: 1px solid #30363d; padding: 25px; vertical-align: top;">
      <h4>🧠 AI OPTIMIZATION</h4>
      <img src="https://img.shields.io/badge/Model-Computer_Vision-FF6F00?style=for-the-badge" alt="Computer Vision" />
      <br><br>
      <img src="https://img.shields.io/badge/Inference-Neural_Networks-FF6F00?style=for-the-badge" alt="NN Inference" />
      <br><br>
      <img src="https://img.shields.io/badge/Quantization-INT8-8E24AA?style=for-the-badge" alt="INT8" />
      <br><br>
      <img src="https://img.shields.io/badge/Format-ONNX-005CDB?style=for-the-badge&logo=onnx&logoColor=white" alt="ONNX" />
    </td>
    <!-- COMPILATION -->
    <td align="center" width="33%" style="border: 1px solid #30363d; padding: 25px; vertical-align: top;">
      <h4>⚙️ AI COMPILATION</h4>
      <img src="https://img.shields.io/badge/Compiler-Arm_Vela-0091BD?style=for-the-badge&logo=arm&logoColor=white" alt="Arm Vela" />
      <br><br>
      <img src="https://img.shields.io/badge/Toolchain-MERA-4CAF50?style=for-the-badge" alt="MERA" />
      <br><br>
      <img src="https://img.shields.io/badge/Toolchain-Ethos--U55-0091BD?style=for-the-badge&logo=arm&logoColor=white" alt="Ethos-U55 Toolchain" />
    </td>
  </tr>
  <tr style="border: none;">
    <!-- EMBEDDED / FIRMWARE -->
    <td align="center" width="33%" style="border: 1px solid #30363d; padding: 25px; vertical-align: top;">
      <h4>💻 EMBEDDED / FIRMWARE</h4>
      <img src="https://img.shields.io/badge/Language-C-A8B9CC?style=for-the-badge&logo=c&logoColor=white" alt="C" />
      <br><br>
      <img src="https://img.shields.io/badge/Language-C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++" />
      <br><br>
      <img src="https://img.shields.io/badge/Toolchain-Arm_GNU-0091BD?style=for-the-badge&logo=gnu&logoColor=white" alt="Arm GNU Toolchain" />
    </td>
    <!-- REAL-TIME SYSTEM -->
    <td align="center" width="33%" style="border: 1px solid #30363d; padding: 25px; vertical-align: top;">
      <h4>⏱️ REAL-TIME SYSTEM</h4>
      <img src="https://img.shields.io/badge/OS-RTOS-E53935?style=for-the-badge" alt="RTOS" />
      <br><br>
      <img src="https://img.shields.io/badge/Execution-Task_Scheduling-E53935?style=for-the-badge" alt="Task Scheduling" />
      <br><br>
      <img src="https://img.shields.io/badge/Execution-Task_Parallelism-E53935?style=for-the-badge" alt="Task Parallelism" />
      <br><br>
      <img src="https://img.shields.io/badge/Execution-Inter--task_Sync-E53935?style=for-the-badge" alt="Synchronization" />
      <br><br>
      <img src="https://img.shields.io/badge/Execution-Pipeline-E53935?style=for-the-badge" alt="Pipeline" />
      <br><br>
      <img src="https://img.shields.io/badge/Memory-Double_Buffering-E53935?style=for-the-badge" alt="Double Buffering" />
    </td>
    <!-- SYSTEM ANALYSIS -->
    <td align="center" width="33%" style="border: 1px solid #30363d; padding: 25px; vertical-align: top;">
      <h4>📊 SYSTEM ANALYSIS</h4>
      <img src="https://img.shields.io/badge/Analysis-NPU_Utilization-607D8B?style=for-the-badge" alt="NPU Utilization" />
      <br><br>
      <img src="https://img.shields.io/badge/Analysis-CPU_Utilization-607D8B?style=for-the-badge" alt="CPU Utilization" />
      <br><br>
      <img src="https://img.shields.io/badge/Analysis-Memory_Footprint-607D8B?style=for-the-badge" alt="Memory Footprint" />
      <br><br>
      <img src="https://img.shields.io/badge/Optimization-SRAM-607D8B?style=for-the-badge" alt="SRAM" />
      <br><br>
      <img src="https://img.shields.io/badge/Memory-Tensor_Management-607D8B?style=for-the-badge" alt="Tensor Memory Management" />
      <br><br>
      <img src="https://img.shields.io/badge/Metrics-End--to--End_Latency-607D8B?style=for-the-badge" alt="Latency" />
      <br><br>
      <img src="https://img.shields.io/badge/Metrics-Real--Time_Throughput-607D8B?style=for-the-badge" alt="Throughput" />
    </td>
  </tr>
</table>

</div>
