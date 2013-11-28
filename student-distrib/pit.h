#ifndef PIT__H
#define PIT__H

#include "types.h"

int32_t pit_init();

int32_t pit_set_frequency(uint32_t frequency);

void pit_process_interrupt();

void pit_enable();

void pit_disable();

#endif
