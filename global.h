
#include "button.h"
#include "core.h"


#ifndef __GLOBAL__
#define __GLOBAL__

   uint32_t LAST_UPSTREAM_REPORT = 0;
   uint32_t LAST_DIAG_RUN = 0;
   uint8_t lcdln = 0;

   Button __BTN_1(BTN_1, "BTN_1");
   Button __BTN_2(BTN_2, "BTN_2");

#endif
