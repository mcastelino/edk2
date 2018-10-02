/** @file
  Various register numbers and value bits for the x86 virt platform
  Copyright (C) 2018, Intel Corporation. All rights reserved

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.   The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef __X86_VIRT_H__
#define __X86_VIRT_H__

//
// Device ID for emulated PCI host bridge used on virt platform
//
#define VIRT_QEMU_DEVICE_ID 0x8

//
// SLP_TYP and SLP_EN values for HW-reduced ACPI used on virt platform
//
#define ACPI_REDUCED_SLEEP_EN   (1<<5)
#define ACPI_REDUCED_SLEEP_TYPE 5 

//
// RESET_VALUE for HW-reduced ACPI used on virt platform
//
#define ACPI_REDUCED_RESET_VALUE 4

//
// Values for sleep control ioport address used on virt platform
//
#define VIRT_SLEEP_CONTROL_ADDRESS 0x3B0

//
// Values for reset ioport address used on virt platform
//
#define VIRT_RESET_ADDRESS 0x3C0

#endif
