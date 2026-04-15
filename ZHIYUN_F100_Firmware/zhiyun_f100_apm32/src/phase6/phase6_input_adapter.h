#ifndef PHASE6_INPUT_ADAPTER_H
#define PHASE6_INPUT_ADAPTER_H

#include "phase6/phase6_light_pd_skeleton.h"

/*
 * Hardware-side input sampling adapter for Phase 6.
 * This layer reads minimal signals and converts them to Phase6_InputEvent.
 */
void Phase6_InputAdapter_Init(void);
unsigned char Phase6_InputAdapter_Poll(Phase6_InputEvent *out_event);
unsigned int Phase6_InputAdapter_ReadStableAdc(void);

#endif /* PHASE6_INPUT_ADAPTER_H */
