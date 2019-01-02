/* prouser.c */

#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define PROFILE 1

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "profile.h"


PROMESSAGE   *pPM;


USHORT fnusIPro(VOID)
{
  SEL selMessage;

  if(DosGetShrSeg(PRO_MEM, &selMessage))
    return TRUE;
  pPM = MAKEP(selMessage,0);
  pPM->sCode = PRO_START;
  DosSleep(20L);
  pPM->asStack[0] =
  pPM->sCode = 1;
  return FALSE;
}

VOID fnvXPro(VOID)
{
  pPM->sCode = PRO_EXIT;
  DosSleep(20L);
  DosFreeSeg(SELECTOROF(pPM));
  return;
}

USHORT fnus2(VOID)
{
  ENTER(2);
  DosSleep(500L);
  LEAVE;
  return 0;
}

USHORT fnus3(VOID)
{
  ENTER(3);
  DosSleep(1500L);
  SET(4);
  DosSleep(500L);
  LEAVE;
  return 0;
}

int cdecl main(void)
{
  SETUP_PRO;

  DosSleep(500L);
  fnus2();
  DosSleep(500L);
  fnus3();
  DosSleep(500L);

  EXIT_PRO;
  return 0;
}
