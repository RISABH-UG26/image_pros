# FPGA Image Processing Accelerator

## Project Overview

Hardware accelerator for image processing functions implemented on Xilinx Zynq-7020 FPGA.

**No Linux required** - Uses bare-metal MicroBlaze for control.

## Supported Filters

| Filter | Mode | Description |
|--------|------|-------------|
| Bypass | 0 | Pass-through (no processing) |
| Grayscale | 1 | RGB to grayscale conversion |
| **Sobel** | 2 | Edge detection (3x3 convolution) |
| Threshold | 3 | Binary thresholding |
| Gaussian | 4 | 3x3 Gaussian blur |
| Negative | 5 | Image inversion (255 - pixel) |
| Sharpen | 6 | Image sharpening |

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     FPGA (Zynq-7020)                        │
│                                                             │
│  ┌──────────────┐    ┌─────────────────────────────────┐   │
│  │  MicroBlaze  │    │   Image Processing IP (HLS)     │   │
│  │  (Control)   │    │                                 │   │
│  │              │    │  ┌─────────┐   ┌────────────┐  │   │
│  │  - Filter    │───▶│  │  Line   │──▶│ Convolution│  │   │
│  │    Select    │    │  │ Buffers │   │   Engine   │  │   │
│  │  - Start/    │    │  └─────────┘   └────────────┘  │   │
│  │    Stop      │    │       │              │         │   │
│  │  - Params    │    │       ▼              ▼         │   │
│  └──────────────┘    │  ┌─────────────────────────┐   │   │
│         │            │  │   Filter Selection      │   │   │
│         │            │  │   (Sobel/Gauss/etc)     │   │   │
│         ▼            │  └─────────────────────────┘   │   │
│  ┌──────────────┐    │              │                 │   │
│  │   AXI-Lite   │◀───│──────────────┘                 │   │
│  │  Interconnect│    └─────────────────────────────────┘   │
│  └──────────────┘                   │                      │
│         │                           │                      │
│         ▼                           ▼                      │
│  ┌──────────────┐           ┌──────────────┐              │
│  │     BRAM     │           │  AXI-Stream  │              │
│  │ (Image Data) │◀─────────▶│  Interface   │              │
│  └──────────────┘           └──────────────┘              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## Project Structure

```
image_pros/
├── src/
│   ├── image_processing.h      # Header with types and constants
│   ├── image_processing.cpp    # Main HLS implementation
│   └── testbench.cpp           # C simulation testbench
├── sw/
│   └── main.c                  # MicroBlaze bare-metal app
├── run_hls.tcl                 # Vitis HLS build script
├── vivado_block_design.tcl     # Vivado block design script
├── solution1/                  # HLS solution directory
└── README.md
```

## Build Instructions

### Step 1: HLS Synthesis (Vitis HLS)

```bash
cd /home/risabh/image_pros
vitis_hls -f run_hls.tcl
```

This will:
1. Run C simulation
2. Synthesize to RTL
3. Run co-simulation
4. Export IP for Vivado

### Step 2: Vivado Block Design

1. Open Vivado
2. Run in Tcl Console:
   ```tcl
   cd /home/risabh/image_pros
   source vivado_block_design.tcl
   ```
3. Run Synthesis → Implementation → Generate Bitstream
4. Export Hardware (.xsa file)

### Step 3: Bare-Metal Software (Vitis IDE)

1. Open Vitis IDE
2. Create Platform Project from .xsa
3. Create Application Project
4. Copy `sw/main.c` to your application
5. Build and run on hardware

## AXI4-Stream Interface

### Signal Description

| Signal | Direction | Width | Description |
|--------|-----------|-------|-------------|
| TDATA | In/Out | 8 bits | Pixel data |
| TVALID | In/Out | 1 bit | Data valid |
| TREADY | In/Out | 1 bit | Ready to receive |
| TLAST | In/Out | 1 bit | End of line |
| TUSER | In | 1 bit | Start of frame |

### Data Flow

```
Pixel[0,0] → Pixel[0,1] → ... → Pixel[0,639] (TLAST=1)
Pixel[1,0] → Pixel[1,1] → ... → Pixel[1,639] (TLAST=1)
...
Pixel[479,0] → ... → Pixel[479,639] (TLAST=1)
```

## Sobel Edge Detection Algorithm

### Kernels

**Gx (Horizontal):**
```
|-1  0  1|
|-2  0  2|
|-1  0  1|
```

**Gy (Vertical):**
```
|-1 -2 -1|
| 0  0  0|
| 1  2  1|
```

### Computation

```
Gradient = |Gx| + |Gy|  (approximation)
Output = saturate(Gradient, 0, 255)
```

## Resource Utilization (Estimated)

| Resource | Usage | Available | % Used |
|----------|-------|-----------|--------|
| LUT | ~2,500 | 53,200 | 4.7% |
| FF | ~1,800 | 106,400 | 1.7% |
| BRAM | 4 | 140 | 2.9% |
| DSP | 6 | 220 | 2.7% |

## Performance

- **Clock**: 100 MHz
- **Throughput**: 1 pixel/cycle (pipelined)
- **Latency**: ~3 line delays for 3x3 filters
- **Frame Rate**: 100M / (640×480) ≈ **325 FPS** (theoretical)

## Viva Q&A Preparation

### Q: Why use FPGA instead of CPU?

**A:** Parallel processing - FPGA processes all pixels simultaneously in pipeline, achieving >100x speedup over sequential CPU processing.

### Q: Explain the line buffer concept.

**A:** Line buffers store previous rows to build the 3x3 convolution window. For 640-width image, we need 2 line buffers (1280 bytes) to access the 3x3 neighborhood of each pixel.

### Q: Why AXI4-Stream?

**A:** Streaming interface is ideal for pixel-by-pixel processing:
- No address overhead
- Handshaking (TVALID/TREADY) prevents data loss
- TLAST marks row boundaries
- Industry standard for video IP

### Q: How does pipelining help?

**A:** With II=1 (Initiation Interval = 1), we can accept a new pixel every clock cycle, achieving maximum throughput of 100M pixels/second.

## License

Educational project for FPGA implementation course.
