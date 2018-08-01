/** @file
  OVMF Platform definitions

  Copyright (C) 2015, Red Hat, Inc.
  Copyright (c) 2014, Gabriel L. Somlo <somlo@cmu.edu>

  This program and the accompanying materials are licensed and made
  available under the terms and conditions of the BSD License which
  accompanies this distribution.   The full text of the license may
  be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef __OVMF_PLATFORMS_H__
#define __OVMF_PLATFORMS_H__

#include <Library/PciLib.h>
#include <IndustryStandard/Pci22.h>
#include <IndustryStandard/Q35MchIch9.h>
#include <IndustryStandard/I440FxPiix4.h>

//
// OVMF Host Bridge DID Address
//
#define OVMF_HOSTBRIDGE_DID \
  PCI_LIB_ADDRESS (0, 0, 0, PCI_DEVICE_ID_OFFSET)

//
// Values we program into the PM base address registers
//
#define PIIX4_PMBA_VALUE  0xB000
#define ICH9_PMBASE_VALUE 0x0600

//
// Common bits in same-purpose registers
//
#define PMBA_RTE BIT0

//
// Common IO ports relative to the Power Management Base Address
//
#define ACPI_TIMER_OFFSET 0x8

//
// Device ID for emulated PCI host bridge used on virt platform
//
#define QEMU_GPEX_DEVICE_ID 0x8

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
