/*
 * Image Processing Accelerator - Main Implementation
 * FPGA Implementation of Image Processing Functions
 * Target: Zynq-7020 (xc7z020clg400-1)
 * 
 * Supported Filters:
 *   0 - Bypass (no processing)
 *   1 - Grayscale (pass-through for grayscale input)
 *   2 - Sobel Edge Detection
 *   3 - Binary Thresholding
 *   4 - Gaussian Blur (3x3)
 *   5 - Negative/Inversion
 *   6 - Sharpening
 */

#include "image_processing.h"
#include <hls_math.h>

// ============================================
// Sobel Edge Detection Filter
// ============================================
void apply_sobel(
    pixel_t window[KERNEL_SIZE][KERNEL_SIZE],
    pixel_t &result
) {
#pragma HLS INLINE
    
    pixel_s16_t gx = 0;
    pixel_s16_t gy = 0;
    
    // Apply Sobel kernels
    SOBEL_X_LOOP:
    for (int i = 0; i < KERNEL_SIZE; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < KERNEL_SIZE; j++) {
#pragma HLS UNROLL
            gx += window[i][j] * SOBEL_X[i][j];
            gy += window[i][j] * SOBEL_Y[i][j];
        }
    }
    
    // Compute gradient magnitude (approximation)
    pixel_s16_t abs_gx = (gx < 0) ? (pixel_s16_t)(-gx) : gx;
    pixel_s16_t abs_gy = (gy < 0) ? (pixel_s16_t)(-gy) : gy;
    pixel_s16_t magnitude = abs_gx + abs_gy;
    
    // Saturate to 8-bit
    if (magnitude > 255) {
        result = 255;
    } else {
        result = (pixel_t)magnitude;
    }
}

// ============================================
// Gaussian Blur Filter (3x3)
// ============================================
void apply_gaussian(
    pixel_t window[KERNEL_SIZE][KERNEL_SIZE],
    pixel_t &result
) {
#pragma HLS INLINE
    
    pixel_s16_t sum = 0;
    
    GAUSSIAN_LOOP:
    for (int i = 0; i < KERNEL_SIZE; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < KERNEL_SIZE; j++) {
#pragma HLS UNROLL
            sum += window[i][j] * GAUSSIAN[i][j];
        }
    }
    
    // Normalize by 16 (sum of Gaussian kernel weights)
    result = (pixel_t)(sum >> 4);
}

// ============================================
// Sharpening Filter
// ============================================
void apply_sharpen(
    pixel_t window[KERNEL_SIZE][KERNEL_SIZE],
    pixel_t &result
) {
#pragma HLS INLINE
    
    pixel_s16_t sum = 0;
    
    SHARPEN_LOOP:
    for (int i = 0; i < KERNEL_SIZE; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < KERNEL_SIZE; j++) {
#pragma HLS UNROLL
            sum += window[i][j] * SHARPEN[i][j];
        }
    }
    
    // Saturate to 0-255
    if (sum < 0) {
        result = 0;
    } else if (sum > 255) {
        result = 255;
    } else {
        result = (pixel_t)sum;
    }
}

// ============================================
// Main Image Processing Function (Top-Level)
// ============================================
void image_pros(
    stream_t &src,
    stream_t &dst,
    ap_uint<3>  filter_select,
    ap_uint<8>  threshold_val,
    ap_uint<16> width,
    ap_uint<16> height
) {
    // ========================================
    // Interface Pragmas
    // ========================================
#pragma HLS INTERFACE axis port=src
#pragma HLS INTERFACE axis port=dst
#pragma HLS INTERFACE s_axilite port=filter_select bundle=control
#pragma HLS INTERFACE s_axilite port=threshold_val bundle=control
#pragma HLS INTERFACE s_axilite port=width bundle=control
#pragma HLS INTERFACE s_axilite port=height bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

    // ========================================
    // Line Buffers for 3x3 Window
    // ========================================
    static pixel_t line_buffer[2][MAX_WIDTH];
#pragma HLS ARRAY_PARTITION variable=line_buffer complete dim=1
    
    pixel_t window[KERNEL_SIZE][KERNEL_SIZE];
#pragma HLS ARRAY_PARTITION variable=window complete dim=0

    // ========================================
    // Process Image Row by Row
    // ========================================
    ROW_LOOP:
    for (int row = 0; row < height; row++) {
#pragma HLS LOOP_TRIPCOUNT min=480 max=480
        
        COL_LOOP:
        for (int col = 0; col < width; col++) {
#pragma HLS LOOP_TRIPCOUNT min=640 max=640
#pragma HLS PIPELINE II=1
            
            // Read input pixel from stream
            axis_pixel_t src_pixel = src.read();
            pixel_t current_pixel = src_pixel.data;
            
            // Shift window columns
            for (int i = 0; i < KERNEL_SIZE; i++) {
#pragma HLS UNROLL
                window[i][0] = window[i][1];
                window[i][1] = window[i][2];
            }
            
            // Load new column from line buffers
            window[0][2] = line_buffer[0][col];
            window[1][2] = line_buffer[1][col];
            window[2][2] = current_pixel;
            
            // Update line buffers
            line_buffer[0][col] = line_buffer[1][col];
            line_buffer[1][col] = current_pixel;
            
            // ====================================
            // Apply Selected Filter
            // ====================================
            pixel_t output_pixel;
            
            // Check if we have valid 3x3 window (not at border)
            bool valid_window = (row >= 2) && (col >= 2);
            
            switch (filter_select) {
                
                case FILTER_BYPASS:
                    // No processing - pass through
                    output_pixel = current_pixel;
                    break;
                
                case FILTER_GRAYSCALE:
                    // Already grayscale - pass through
                    output_pixel = current_pixel;
                    break;
                
                case FILTER_SOBEL:
                    if (valid_window) {
                        apply_sobel(window, output_pixel);
                    } else {
                        output_pixel = 0;  // Black border
                    }
                    break;
                
                case FILTER_THRESHOLD:
                    output_pixel = (current_pixel > threshold_val) ? 255 : 0;
                    break;
                
                case FILTER_GAUSSIAN:
                    if (valid_window) {
                        apply_gaussian(window, output_pixel);
                    } else {
                        output_pixel = current_pixel;
                    }
                    break;
                
                case FILTER_NEGATIVE:
                    output_pixel = 255 - current_pixel;
                    break;
                
                case FILTER_SHARPEN:
                    if (valid_window) {
                        apply_sharpen(window, output_pixel);
                    } else {
                        output_pixel = current_pixel;
                    }
                    break;
                
                default:
                    output_pixel = current_pixel;
                    break;
            }
            
            // Write output pixel to stream
            axis_pixel_t dst_pixel;
            dst_pixel.data = output_pixel;
            dst_pixel.keep = src_pixel.keep;
            dst_pixel.strb = src_pixel.strb;
            dst_pixel.user = src_pixel.user;
            dst_pixel.id   = src_pixel.id;
            dst_pixel.dest = src_pixel.dest;
            
            // Set TLAST at end of each row
            dst_pixel.last = (col == width - 1) ? 1 : 0;
            
            dst.write(dst_pixel);
        }
    }
}
