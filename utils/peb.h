
#ifndef _PEB_TEB_H_
#define _PEB_TEB_H_

#include "win_um_internal.h"



PEB *GetCurrentPEB();
#define GetCurrentLdr()   GetCurrentPEB()->Ldr

void PrintCurrentTEB (void);


void PrintCurrentPEB (void);
void PrintModulesInCurrentPEB (void);

#endif  // _PEB_TEB_H_
