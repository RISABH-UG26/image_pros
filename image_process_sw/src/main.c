#include <stdio.h>
#include "xparameters.h"
#include "xil_io.h"
#include "xil_printf.h"

// === Update these from xparameters.h ===
#define IMG_PROC_BASE   XPAR_IMAGE_PROS_0_S_AXI_CONTROL_BASEADDR
#define GPIO_BASE       XPAR_AXI_GPIO_0_BASEADDR      // optional if you have GPIO
#define BRAM_BASE       XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR // optional if you use BRAM

// Image Processing IP register map (from HLS IP)
#define REG_CTRL        0x00   // bit0: ap_start, bit1: ap_done, bit2: ap_idle, bit3: ap_ready
#define REG_GIE         0x04
#define REG_IER         0x08
#define REG_ISR         0x0C
#define REG_FILTER      0x10   // filter_select
#define REG_THRESH      0x18   // threshold_val
#define REG_WIDTH       0x20
#define REG_HEIGHT      0x28

// Filter modes
#define FILTER_BYPASS     0
#define FILTER_GRAYSCALE  1
#define FILTER_SOBEL      2
#define FILTER_THRESHOLD  3
#define FILTER_GAUSSIAN   4
#define FILTER_NEGATIVE   5
#define FILTER_SHARPEN    6

static void ip_config(uint8_t filter, uint8_t thresh, uint16_t w, uint16_t h) {
    Xil_Out32(IMG_PROC_BASE + REG_FILTER,  filter);
    Xil_Out32(IMG_PROC_BASE + REG_THRESH, thresh);
    Xil_Out32(IMG_PROC_BASE + REG_WIDTH,  w);
    Xil_Out32(IMG_PROC_BASE + REG_HEIGHT, h);
}

static void ip_start(void) {
    // Write ap_start=1 (bit0)
    Xil_Out32(IMG_PROC_BASE + REG_CTRL, 0x01);
}

static int ip_wait_done(void) {
    // Poll ap_done (bit1) or ap_ready (bit3)
    while (1) {
        uint32_t st = Xil_In32(IMG_PROC_BASE + REG_CTRL);
        if (st & 0x02) return 0;   // ap_done
        if (st & 0x08) return 0;   // ap_ready
    }
    return -1;
}

int main() {
    xil_printf(" Image Processing Accelerator\r\n");
    xil_printf(" Vitis Bare-Metal Demo\r\n");

    // Example config: Sobel on a 64x64 frame
    uint16_t width  = 64;
    uint16_t height = 64;
    uint8_t  thresh = 128;
    uint8_t  filter = FILTER_SOBEL;

    ip_config(filter, thresh, width, height);
    xil_printf("Configured: filter=%d, thresh=%d, size=%dx%d\r\n",
               filter, thresh, width, height);

    // If you have BRAM as frame buffer, you could preload data at BRAM_BASE here.

    // Start processing
    ip_start();
    ip_wait_done();
    xil_printf("Processing done.\r\n");

    // If you wrote output to BRAM, you can read it back here.
    // Example read first pixel (optional):
    // uint8_t px0 = Xil_In8(BRAM_BASE);
    // xil_printf("First output pixel: %u\r\n", px0);

    xil_printf("Demo complete.\r\n");
    return 0;
}
