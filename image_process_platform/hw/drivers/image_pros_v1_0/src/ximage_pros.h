// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XIMAGE_PROS_H
#define XIMAGE_PROS_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "ximage_pros_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u64 Control_BaseAddress;
} XImage_pros_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XImage_pros;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XImage_pros_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XImage_pros_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XImage_pros_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XImage_pros_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XImage_pros_Initialize(XImage_pros *InstancePtr, u16 DeviceId);
XImage_pros_Config* XImage_pros_LookupConfig(u16 DeviceId);
int XImage_pros_CfgInitialize(XImage_pros *InstancePtr, XImage_pros_Config *ConfigPtr);
#else
int XImage_pros_Initialize(XImage_pros *InstancePtr, const char* InstanceName);
int XImage_pros_Release(XImage_pros *InstancePtr);
#endif

void XImage_pros_Start(XImage_pros *InstancePtr);
u32 XImage_pros_IsDone(XImage_pros *InstancePtr);
u32 XImage_pros_IsIdle(XImage_pros *InstancePtr);
u32 XImage_pros_IsReady(XImage_pros *InstancePtr);
void XImage_pros_EnableAutoRestart(XImage_pros *InstancePtr);
void XImage_pros_DisableAutoRestart(XImage_pros *InstancePtr);

void XImage_pros_Set_filter_select(XImage_pros *InstancePtr, u32 Data);
u32 XImage_pros_Get_filter_select(XImage_pros *InstancePtr);
void XImage_pros_Set_threshold_val(XImage_pros *InstancePtr, u32 Data);
u32 XImage_pros_Get_threshold_val(XImage_pros *InstancePtr);
void XImage_pros_Set_width(XImage_pros *InstancePtr, u32 Data);
u32 XImage_pros_Get_width(XImage_pros *InstancePtr);
void XImage_pros_Set_height(XImage_pros *InstancePtr, u32 Data);
u32 XImage_pros_Get_height(XImage_pros *InstancePtr);

void XImage_pros_InterruptGlobalEnable(XImage_pros *InstancePtr);
void XImage_pros_InterruptGlobalDisable(XImage_pros *InstancePtr);
void XImage_pros_InterruptEnable(XImage_pros *InstancePtr, u32 Mask);
void XImage_pros_InterruptDisable(XImage_pros *InstancePtr, u32 Mask);
void XImage_pros_InterruptClear(XImage_pros *InstancePtr, u32 Mask);
u32 XImage_pros_InterruptGetEnabled(XImage_pros *InstancePtr);
u32 XImage_pros_InterruptGetStatus(XImage_pros *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
