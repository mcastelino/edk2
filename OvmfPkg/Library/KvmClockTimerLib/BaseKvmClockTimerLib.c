/** @file
  Provide constructor and frequency for Base instance of KVM Clock Timer

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

typedef struct  {
        UINT32   version;
        UINT32   pad0;
        UINT64   tsc_timestamp;
        UINT64   system_time;
        UINT32   tsc_to_system_mul;
        INT8    tsc_shift;
        UINT8    pad[3];
} __attribute__((__packed__)) PVCLOCK_VCPU_TIME_INFO; 

STATIC PVCLOCK_VCPU_TIME_INFO mVcpuTimeInfo;

UINT64
QueryKVMClockTscKhz(
  VOID
) {
  UINT32 orig_version;
  UINT64 res;

  do {
    orig_version = mVcpuTimeInfo.version;    
    MemoryFence();
    res = 1000000ULL << 32;
    res = DivU64x32(res, mVcpuTimeInfo.tsc_to_system_mul);

    if (mVcpuTimeInfo.tsc_shift < 0) {
        res <<= -mVcpuTimeInfo.tsc_shift;
    } else {
        res >>= mVcpuTimeInfo.tsc_shift;
    }
    MemoryFence();
  } while (mVcpuTimeInfo.version != orig_version || mVcpuTimeInfo.version % 2 != 0);

  return res;
}

RETURN_STATUS
EFIAPI
KVMClockTimerLibConstructor (
  VOID
  )
{
  UINT32 flags;

  AsmCpuid(0x40000001, &flags, NULL, NULL, NULL);

  if (flags & BIT3) {
    DEBUG ((EFI_D_INFO, "KVM Clock available\n"));

    AsmWriteMsr64(0x4b564d01, ((UINT64)&mVcpuTimeInfo) | 0x1);
    
    DEBUG ((EFI_D_INFO, "KVM Clock system version=%u tsc_timestamp=%llu system_time=%llu tsc_to_system_mul=%u tsc_shift=%d\n",
      mVcpuTimeInfo.version,
      mVcpuTimeInfo.tsc_timestamp,
      mVcpuTimeInfo.system_time,
      mVcpuTimeInfo.tsc_to_system_mul,
      mVcpuTimeInfo.tsc_shift));
  }
 
  return RETURN_SUCCESS;
}