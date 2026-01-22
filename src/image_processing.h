/*
 * Image Processing Accelerator - Header File
 * FPGA Implementation of Image Processing Functions
 * Target: Zynq-7020 (xc7z020clg400-1)
 */

#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

// ============================================
// Image Parameters
// ============================================
#define MAX_WIDTH   640
#define MAX_HEIGHT  480
#define KERNEL_SIZE 3

// ============================================
// Pixel Types
// ============================================
typedef ap_uint<8>  pixel_t;        // 8-bit grayscale pixel
typedef ap_int<16>  pixel_s16_t;    // Signed 16-bit for convolution
typedef ap_uint<24> pixel_rgb_t;    // 24-bit RGB pixel

// ============================================
// AXI4-Stream Types
// ============================================
typedef ap_axiu<8, 1, 1, 1>  axis_pixel_t;      // 8-bit stream with TLAST
typedef ap_axiu<24, 1, 1, 1> axis_rgb_t;        // 24-bit RGB stream

typedef hls::stream<axis_pixel_t> stream_t;
typedef hls::stream<axis_rgb_t>   stream_rgb_t;

// ============================================
// Filter Selection Modes
// ============================================
typedef enum {
    FILTER_BYPASS     = 0,  // Pass-through (no processing)
    FILTER_GRAYSCALE  = 1,  // RGB to Grayscale
    FILTER_SOBEL      = 2,  // Sobel Edge Detection
    FILTER_THRESHOLD  = 3,  // Binary Thresholding
    FILTER_GAUSSIAN   = 4,  // Gaussian Blur (3x3)
    FILTER_NEGATIVE   = 5,  // Image Negative/Inversion
    FILTER_SHARPEN    = 6   // Image Sharpening
} filter_mode_t;

// ============================================
// Control Register Structure
// ============================================
typedef struct {
    ap_uint<3>  filter_select;   // Filter mode (0-6)
    ap_uint<8>  threshold_val;   // Threshold value (0-255)
    ap_uint<16> img_width;       // Image width
    ap_uint<16> img_height;      // Image height
} control_t;

// ============================================
// Sobel Kernels (3x3)
// ============================================
const int SOBEL_X[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

const int SOBEL_Y[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

// ============================================
// Gaussian Kernel (3x3) - Normalized by 16
// ============================================
const int GAUSSIAN[3][3] = {
    {1, 2, 1},
    {2, 4, 2},
    {1, 2, 1}
};

// ============================================
// Sharpen Kernel (3x3)
// ============================================
const int SHARPEN[3][3] = {
    { 0, -1,  0},
    {-1,  5, -1},
    { 0, -1,  0}
};

// ============================================
// Function Prototypes
// ============================================

// Top-level function (to be exported as IP)
void image_pros(
    stream_t &src,
    stream_t &dst,
    ap_uint<3>  filter_select,
    ap_uint<8>  threshold_val,
    ap_uint<16> width,
    ap_uint<16> height
);

// Individual filter functions
void apply_sobel(
    pixel_t window[KERNEL_SIZE][KERNEL_SIZE],
    pixel_t &result
);

void apply_gaussian(
    pixel_t window[KERNEL_SIZE][KERNEL_SIZE],
    pixel_t &result
);

void apply_sharpen(
    pixel_t window[KERNEL_SIZE][KERNEL_SIZE],
    pixel_t &result
);

#endif // IMAGE_PROCESSING_H
