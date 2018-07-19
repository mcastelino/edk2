/** @file
  Internal definitions for ACPI Timer Library

  Copyright (C) 2014, Gabriel L. Somlo <somlo@cmu.edu>
  Copyright (C) 2018, Intel Corporation

  This program and the accompanying materials are licensed and made
  available under the terms and conditions of the BSD License which
  accompanies this distribution.   The full text of the license may
  be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef _KVM_CLOCK_TIMER_LIB_H_
#define _KVM_CLOCK_TIMER_LIB_H_

UINT64
QueryKVMClockTscKhz(
  VOID
);

#endif // _KVM_CLOCK_TIMER_LIB_H_
