/** @file
  Provide constructor and frequency for Base Rom instance of KVM Clock Timer

  Copyright (C) 2014, Gabriel L. Somlo <somlo@cmu.edu>
  Copyright (C) 2018, Intel Corporation

  This program and the accompanying materials are licensed and made
  available under the terms and conditions of the BSD License which
  accompanies this distribution.   The full text of the license may
  be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <OvmfPlatforms.h>

UINT64
QueryKVMClockTscKhz(
  VOID
) {
  return 800 * 1000;
}

RETURN_STATUS
EFIAPI
KVMClockTimerLibConstructor (
  VOID
  )
{
  return RETURN_SUCCESS;
}