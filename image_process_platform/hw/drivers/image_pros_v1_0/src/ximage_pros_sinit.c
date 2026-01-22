// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "ximage_pros.h"

extern XImage_pros_Config XImage_pros_ConfigTable[];

XImage_pros_Config *XImage_pros_LookupConfig(u16 DeviceId) {
	XImage_pros_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XIMAGE_PROS_NUM_INSTANCES; Index++) {
		if (XImage_pros_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XImage_pros_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XImage_pros_Initialize(XImage_pros *InstancePtr, u16 DeviceId) {
	XImage_pros_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XImage_pros_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XImage_pros_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

