// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "ximage_pros.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XImage_pros_CfgInitialize(XImage_pros *InstancePtr, XImage_pros_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XImage_pros_Start(XImage_pros *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_AP_CTRL) & 0x80;
    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XImage_pros_IsDone(XImage_pros *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XImage_pros_IsIdle(XImage_pros *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XImage_pros_IsReady(XImage_pros *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XImage_pros_EnableAutoRestart(XImage_pros *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XImage_pros_DisableAutoRestart(XImage_pros *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_AP_CTRL, 0);
}

void XImage_pros_Set_filter_select(XImage_pros *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_FILTER_SELECT_DATA, Data);
}

u32 XImage_pros_Get_filter_select(XImage_pros *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_FILTER_SELECT_DATA);
    return Data;
}

void XImage_pros_Set_threshold_val(XImage_pros *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_THRESHOLD_VAL_DATA, Data);
}

u32 XImage_pros_Get_threshold_val(XImage_pros *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_THRESHOLD_VAL_DATA);
    return Data;
}

void XImage_pros_Set_width(XImage_pros *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_WIDTH_DATA, Data);
}

u32 XImage_pros_Get_width(XImage_pros *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_WIDTH_DATA);
    return Data;
}

void XImage_pros_Set_height(XImage_pros *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_HEIGHT_DATA, Data);
}

u32 XImage_pros_Get_height(XImage_pros *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_HEIGHT_DATA);
    return Data;
}

void XImage_pros_InterruptGlobalEnable(XImage_pros *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_GIE, 1);
}

void XImage_pros_InterruptGlobalDisable(XImage_pros *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_GIE, 0);
}

void XImage_pros_InterruptEnable(XImage_pros *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_IER);
    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_IER, Register | Mask);
}

void XImage_pros_InterruptDisable(XImage_pros *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_IER);
    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_IER, Register & (~Mask));
}

void XImage_pros_InterruptClear(XImage_pros *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_pros_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_ISR, Mask);
}

u32 XImage_pros_InterruptGetEnabled(XImage_pros *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_IER);
}

u32 XImage_pros_InterruptGetStatus(XImage_pros *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XImage_pros_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_PROS_CONTROL_ADDR_ISR);
}

