/*
 * Image Processing Accelerator - Testbench
 * Tests all filter modes with a sample image
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "image_processing.h"

using namespace std;

// Test image dimensions
#define TEST_WIDTH  64
#define TEST_HEIGHT 64

// ============================================
// Generate Test Pattern Image
// ============================================
void generate_test_image(pixel_t image[TEST_HEIGHT][TEST_WIDTH]) {
    for (int y = 0; y < TEST_HEIGHT; y++) {
        for (int x = 0; x < TEST_WIDTH; x++) {
            // Create a pattern with edges for Sobel testing
            if ((x >= 20 && x < 44) && (y >= 20 && y < 44)) {
                // White square in center
                image[y][x] = 200;
            } else if ((x >= 16 && x < 48) && (y >= 16 && y < 48)) {
                // Gray border
                image[y][x] = 128;
            } else {
                // Dark background with gradient
                image[y][x] = (x + y) % 64;
            }
        }
    }
}

// ============================================
// Save Image as PGM (Portable GrayMap)
// ============================================
void save_pgm(const char* filename, pixel_t image[TEST_HEIGHT][TEST_WIDTH]) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        return;
    }
    
    // PGM header
    file << "P2" << endl;
    file << TEST_WIDTH << " " << TEST_HEIGHT << endl;
    file << "255" << endl;
    
    // Pixel data
    for (int y = 0; y < TEST_HEIGHT; y++) {
        for (int x = 0; x < TEST_WIDTH; x++) {
            file << (int)image[y][x];
            if (x < TEST_WIDTH - 1) file << " ";
        }
        file << endl;
    }
    
    file.close();
    cout << "Saved: " << filename << endl;
}

// ============================================
// Run Single Filter Test
// ============================================
int test_filter(
    pixel_t input[TEST_HEIGHT][TEST_WIDTH],
    pixel_t output[TEST_HEIGHT][TEST_WIDTH],
    ap_uint<3> filter_mode,
    ap_uint<8> threshold,
    const char* filter_name
) {
    cout << "\n========================================" << endl;
    cout << "Testing: " << filter_name << endl;
    cout << "========================================" << endl;
    
    // Create streams
    stream_t src_stream;
    stream_t dst_stream;
    
    // Load input image into stream
    for (int y = 0; y < TEST_HEIGHT; y++) {
        for (int x = 0; x < TEST_WIDTH; x++) {
            axis_pixel_t pixel;
            pixel.data = input[y][x];
            pixel.keep = 1;
            pixel.strb = 1;
            pixel.user = (y == 0 && x == 0) ? 1 : 0;  // SOF
            pixel.last = (x == TEST_WIDTH - 1) ? 1 : 0; // EOL
            pixel.id = 0;
            pixel.dest = 0;
            src_stream.write(pixel);
        }
    }
    
    // Call DUT (Device Under Test)
    image_pros(
        src_stream,
        dst_stream,
        filter_mode,
        threshold,
        TEST_WIDTH,
        TEST_HEIGHT
    );
    
    // Read output from stream
    for (int y = 0; y < TEST_HEIGHT; y++) {
        for (int x = 0; x < TEST_WIDTH; x++) {
            axis_pixel_t pixel = dst_stream.read();
            output[y][x] = pixel.data;
        }
    }
    
    // Calculate statistics
    int min_val = 255, max_val = 0;
    long sum = 0;
    
    for (int y = 0; y < TEST_HEIGHT; y++) {
        for (int x = 0; x < TEST_WIDTH; x++) {
            int val = output[y][x];
            if (val < min_val) min_val = val;
            if (val > max_val) max_val = val;
            sum += val;
        }
    }
    
    cout << "Output Statistics:" << endl;
    cout << "  Min pixel value: " << min_val << endl;
    cout << "  Max pixel value: " << max_val << endl;
    cout << "  Avg pixel value: " << (sum / (TEST_WIDTH * TEST_HEIGHT)) << endl;
    
    return 0;
}

// ============================================
// Main Testbench
// ============================================
int main() {
    cout << "========================================" << endl;
    cout << " Image Processing Accelerator Testbench" << endl;
    cout << "========================================" << endl;
    cout << "Image size: " << TEST_WIDTH << " x " << TEST_HEIGHT << endl;
    
    // Allocate images
    static pixel_t input_image[TEST_HEIGHT][TEST_WIDTH];
    static pixel_t output_image[TEST_HEIGHT][TEST_WIDTH];
    
    // Generate test pattern
    generate_test_image(input_image);
    save_pgm("input.pgm", input_image);
    
    int errors = 0;
    
    // ========================================
    // Test 1: Bypass (No Processing)
    // ========================================
    errors += test_filter(input_image, output_image, 
                         FILTER_BYPASS, 128, "BYPASS");
    save_pgm("output_bypass.pgm", output_image);
    
    // Verify bypass outputs same as input
    for (int y = 0; y < TEST_HEIGHT; y++) {
        for (int x = 0; x < TEST_WIDTH; x++) {
            if (input_image[y][x] != output_image[y][x]) {
                cout << "ERROR: Bypass mismatch at (" << x << "," << y << ")" << endl;
                errors++;
            }
        }
    }
    
    // ========================================
    // Test 2: Sobel Edge Detection
    // ========================================
    errors += test_filter(input_image, output_image, 
                         FILTER_SOBEL, 128, "SOBEL EDGE DETECTION");
    save_pgm("output_sobel.pgm", output_image);
    
    // ========================================
    // Test 3: Threshold
    // ========================================
    errors += test_filter(input_image, output_image, 
                         FILTER_THRESHOLD, 100, "THRESHOLD (val=100)");
    save_pgm("output_threshold.pgm", output_image);
    
    // Verify threshold outputs are binary
    for (int y = 0; y < TEST_HEIGHT; y++) {
        for (int x = 0; x < TEST_WIDTH; x++) {
            if (output_image[y][x] != 0 && output_image[y][x] != 255) {
                cout << "ERROR: Threshold output not binary at (" << x << "," << y << ")" << endl;
                errors++;
            }
        }
    }
    
    // ========================================
    // Test 4: Gaussian Blur
    // ========================================
    errors += test_filter(input_image, output_image, 
                         FILTER_GAUSSIAN, 128, "GAUSSIAN BLUR");
    save_pgm("output_gaussian.pgm", output_image);
    
    // ========================================
    // Test 5: Negative
    // ========================================
    errors += test_filter(input_image, output_image, 
                         FILTER_NEGATIVE, 128, "NEGATIVE");
    save_pgm("output_negative.pgm", output_image);
    
    // Verify negative: output = 255 - input
    for (int y = 0; y < TEST_HEIGHT; y++) {
        for (int x = 0; x < TEST_WIDTH; x++) {
            pixel_t expected = 255 - input_image[y][x];
            if (output_image[y][x] != expected) {
                cout << "ERROR: Negative mismatch at (" << x << "," << y << ")" << endl;
                errors++;
            }
        }
    }
    
    // ========================================
    // Test 6: Sharpen
    // ========================================
    errors += test_filter(input_image, output_image, 
                         FILTER_SHARPEN, 128, "SHARPEN");
    save_pgm("output_sharpen.pgm", output_image);
    
    // ========================================
    // Summary
    // ========================================
    cout << "\n========================================" << endl;
    cout << " TEST SUMMARY" << endl;
    cout << "========================================" << endl;
    
    if (errors == 0) {
        cout << "ALL TESTS PASSED!" << endl;
        return 0;
    } else {
        cout << "FAILED with " << errors << " errors" << endl;
        return 1;
    }
}
