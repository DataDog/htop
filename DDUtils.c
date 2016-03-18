#include "DDUtils.h"
#include "Panel.h"
#include "Statsd.h"
#include "ListItem.h"

void addMetricToPanel(int key, void* metricCast, void* panelCast) {
   Metric* metric = (Metric *) metricCast;
   Panel* panel = (Panel*) panelCast;
   Panel_add(panel, (Object*) ListItem_new(metric->name, 1));
   printf("%s", metric->name);
}
