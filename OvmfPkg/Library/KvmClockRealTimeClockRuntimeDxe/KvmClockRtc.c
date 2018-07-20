/** @file
  Provides Set/Get time operations.

Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <time.h>

EFI_HANDLE             mHandle = NULL;

typedef struct  {
        UINT32   version;
        UINT32   pad0;
        UINT64   tsc_timestamp;
        UINT64   system_time;
        UINT32   tsc_to_system_mul;
        INT8    tsc_shift;
        UINT8    pad[3];
} __attribute__((__packed__)) PVCLOCK_VCPU_TIME_INFO; 

typedef struct {
    UINT32 version;
    UINT32 sec;
    UINT32 nsec;
} __attribute__((__packed__)) PVCLOCK_WALL_CLOCK;

STATIC PVCLOCK_VCPU_TIME_INFO mVcpuTimeInfo;
STATIC PVCLOCK_WALL_CLOCK mWallClock;

#define DAYSECONDS (60 * 60 * 24)
#define IS_LEAP(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

// Does not handle leap seconds!
STATIC VOID
UnixEpochToEfi(UINT64 seconds, EFI_TIME *Time)
{
  UINT64 year_seconds, month_seconds;
  INT16 year = 1970;
  UINT8 month = 1;
  UINT8 day, h, m, s;

  const int month_days [2][12] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
  };

  year_seconds = IS_LEAP(year) ? DAYSECONDS * 366 : DAYSECONDS * 365 ;

  // Obviously goes wrong when seconds gets close to MAX_UINT64 / 2...
  // If this code is still running then i'm really sorry. :-)
  while ((INT64)(seconds - year_seconds) > 0) {
    seconds -= year_seconds;
    year++;
    year_seconds = IS_LEAP(year) ? DAYSECONDS * 366 : DAYSECONDS * 365 ;
  }

  month_seconds = month_days[IS_LEAP(year)][month - 1] * DAYSECONDS;
  while ((INT64)(seconds - month_seconds) > 0) {
    seconds -= month_seconds;
    month++;
    month_seconds = month_days[IS_LEAP(year)][month - 1] * DAYSECONDS;
  }

  day = (seconds / DAYSECONDS) + 1;

  h = (seconds % DAYSECONDS) / 3600;
  m = ((seconds % DAYSECONDS) % 3600) / 60;
  s = ((seconds % DAYSECONDS) % 3600) % 60;

  Time->Year = year;
  Time->Month = month;
  Time->Day = day;
  Time->Hour = h;
  Time->Minute = m;
  Time->Second = s;

  return;
}

/**
  Returns the current time and date information, and the time-keeping capabilities
  of the hardware platform.

  @param  Time          A pointer to storage to receive a snapshot of the current time.
  @param  Capabilities  An optional pointer to a buffer to receive the real time
                        clock device's capabilities.

  @retval EFI_SUCCESS            The operation completed successfully.
  @retval EFI_INVALID_PARAMETER  Time is NULL.
  @retval EFI_DEVICE_ERROR       The time could not be retrieved due to hardware error.

**/
EFI_STATUS
EFIAPI
KvmClockRtcEfiGetTime (
  OUT EFI_TIME                *Time,
  OUT EFI_TIME_CAPABILITIES   *Capabilities  OPTIONAL
  )
{
  UINT64 delta;
  UINT64 delta_tsc;
  UINT64 system_time;
  UINT64 wall_clock;
  UINT32 orig_version;
  UINT32 orig_wall_clock_version;
  UINT64 system_time_seconds;
  
  do {
    orig_version = mVcpuTimeInfo.version;
    orig_wall_clock_version = mWallClock.version;
    MemoryFence();

    delta_tsc = AsmReadTsc() - mVcpuTimeInfo.tsc_timestamp;
    
    delta = delta_tsc;

    if (mVcpuTimeInfo.tsc_shift < 0) {
      delta <<= -mVcpuTimeInfo.tsc_shift;
    } else {
      delta >>= mVcpuTimeInfo.tsc_shift;
    }
    
    delta >>= 32;
    delta *= mVcpuTimeInfo.tsc_to_system_mul;

    // TODO: Handle nanosecond part?
    system_time = mVcpuTimeInfo.system_time + delta;
    system_time_seconds = system_time / 1000000000ULL;
    wall_clock = mWallClock.sec + system_time_seconds;

    MemoryFence();
  } while (mVcpuTimeInfo.version != orig_version || mVcpuTimeInfo.version % 2 != 0 ||
           mWallClock.version != orig_wall_clock_version || mWallClock.version % 2 != 0);

  UnixEpochToEfi(wall_clock, Time);

  return EFI_SUCCESS;
}

/**
  Sets the current local time and date information.

  @param  Time                   A pointer to the current time.

  @retval EFI_SUCCESS            The operation completed successfully.
  @retval EFI_INVALID_PARAMETER  A time field is out of range.
  @retval EFI_DEVICE_ERROR       The time could not be set due due to hardware error.

**/
EFI_STATUS
EFIAPI
KvmClockRtcEfiSetTime (
  IN EFI_TIME                *Time
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Returns the current wakeup alarm clock setting.

  @param  Enabled  Indicates if the alarm is currently enabled or disabled.
  @param  Pending  Indicates if the alarm signal is pending and requires acknowledgement.
  @param  Time     The current alarm setting.

  @retval EFI_SUCCESS           The alarm settings were returned.
  @retval EFI_INVALID_PARAMETER Enabled is NULL.
  @retval EFI_INVALID_PARAMETER Pending is NULL.
  @retval EFI_INVALID_PARAMETER Time is NULL.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be retrieved due to a hardware error.
  @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this platform.

**/
EFI_STATUS
EFIAPI
KvmClockRtcEfiGetWakeupTime (
  OUT BOOLEAN     *Enabled,
  OUT BOOLEAN     *Pending,
  OUT EFI_TIME    *Time
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Sets the system wakeup alarm clock time.

  @param  Enabled  Enable or disable the wakeup alarm.
  @param  Time     If Enable is TRUE, the time to set the wakeup alarm for.
                   If Enable is FALSE, then this parameter is optional, and may be NULL.

  @retval EFI_SUCCESS            If Enable is TRUE, then the wakeup alarm was enabled.
                                 If Enable is FALSE, then the wakeup alarm was disabled.
  @retval EFI_INVALID_PARAMETER  A time field is out of range.
  @retval EFI_DEVICE_ERROR       The wakeup time could not be set due to a hardware error.
  @retval EFI_UNSUPPORTED        A wakeup timer is not supported on this platform.

**/
EFI_STATUS
EFIAPI
KvmClockRtcEfiSetWakeupTime (
  IN BOOLEAN      Enabled,
  IN EFI_TIME    *Time       OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}

/**
  The user Entry Point for KvmClockRtc module.

  This is the entrhy point for KvmClockRtc module. It installs the UEFI runtime service
  including GetTime(),SetTime(),GetWakeupTime(),and SetWakeupTime().

  @param  ImageHandle    The firmware allocated handle for the EFI image.
  @param  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval Others         Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeKvmClockRtc (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS  Status;

  UINT32 flags;

  AsmCpuid(0x40000001, &flags, NULL, NULL, NULL);

  if (!(flags & BIT3)) {
    return EFI_UNSUPPORTED;
  }

  AsmWriteMsr64(0x4b564d01, ((UINT64)&mVcpuTimeInfo) | 0x1);
  AsmWriteMsr64(0x4b564d00, (UINT64)&mWallClock);

  gRT->GetTime       = KvmClockRtcEfiGetTime;
  gRT->SetTime       = KvmClockRtcEfiSetTime;
  gRT->GetWakeupTime = KvmClockRtcEfiGetWakeupTime;
  gRT->SetWakeupTime = KvmClockRtcEfiSetWakeupTime;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gEfiRealTimeClockArchProtocolGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
