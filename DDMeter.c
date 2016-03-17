/*
htop - DDMeter.c

This meter written by Pierre Manceron.
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
