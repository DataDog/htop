/*
htop - BatteryMeter.c
(C) 2004-2011 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.

This meter written by Ian P. Hands (iphands@gmail.com, ihands@redhat.com).
*/

#include "DDMeter.h"

#include "ProcessList.h"
#include "CRT.h"
#include "StringUtils.h"
#include "Platform.h"

#include <string.h>
#include <stdlib.h>

/*{

#include "Meter.h"

}*/

int DDMeter_attributes[] = {
   BATTERY
};

static void DDMeter_setValues(Meter* this, char* buffer, int size) {
   snprintf(buffer, size, "metric");
}

MeterClass DDMeter_class = {
   .super = {
      .extends = Class(Meter),
      .delete = Meter_delete
   },
   .setValues = DDMeter_setValues,
   .defaultMode = TEXT_METERMODE,
   .maxItems = 5,
   .total = 100.0,
   .attributes = DDMeter_attributes,
   .name = "DD metric!!",
   .uiName = "DD metric!!",
   .caption = "DD metric!!: "
};
