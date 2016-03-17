/*
htop - DDMeter.c

This meter written by Pierre Manceron.
*/

#include "DDMeter.h"
#include "Statsd.h"

#include "ProcessList.h"
#include "CRT.h"
#include "StringUtils.h"
#include "Platform.h"

#include <string.h>
#include <stdlib.h>

/*{
#include "Meter.h"

}*/

#define METRICS_NUMBER 5
#define METRIC_NAME_SIZE 256
char ddmetrics[METRICS_NUMBER][METRIC_NAME_SIZE] = {"postgres.inserts", "postgres.inserts", "postgres.inserts", "postgres.inserts", "postgres.inserts"};

int DDMeter_attributes[] = {
};

static void DDMeter_setValues(Meter* this, char* buffer, int size) {
   double val = Statsd_getMetric(ddmetrics[this->param]);
   snprintf(buffer, size, "val: %f", val);
}

MeterClass DDMeter_class = {
   .super = {
      .extends = Class(Meter),
      .delete = Meter_delete
   },
   .setValues = DDMeter_setValues,
   .defaultMode = TEXT_METERMODE,
   .maxItems = METRICS_NUMBER,
   .total = 100.0,
   .attributes = DDMeter_attributes,
   .name = "DD metric!!",
   .uiName = "DD metric!!",
   .caption = "DD metricc!!: "
};
