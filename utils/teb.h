

#ifndef _TEB_H_
#define _TEB_H_


#include "win_um_internal.h"



IF_DEF_WUI_X86(TEB *GetCurrentTEB32 (void))
IF_DEF_WUI_X64(TEB *GetCurrentTEB64 (void))

#if defined(_WIN64)
#define GetCurrentTEB GetCurrentTEB64
#else
#define GetCurrentTEB GetCurrentTEB32
#endif


PEB *GetCurrentPEB();

void PrintTEBStructure (TEB *teb);
void PrintCurrentTEB (void);


#endif  // _TEB_H_
