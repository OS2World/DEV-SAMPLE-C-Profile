/* profile.c */

#define INCL_DOSINFOSEG
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSSIGNALS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "profile.h"

SEL        selMessage;
volatile PROMESSAGE  *pProMessage;
ULONG     aulHits[PRO_MAX];
BOOL fStarted;
CHAR szWork[80];
time_t timeStart, timeEnd;

USHORT fnusSetPriority(VOID)
{
  SEL selGlobalSeg, selLocalSeg;
  LINFOSEG *plis;
  USHORT usRetCode;

  DosGetInfoSeg(&selGlobalSeg, &selLocalSeg);
  plis = MAKEPLINFOSEG(selLocalSeg);
  usRetCode = DosSetPrty(PRTYS_THREAD, PRTYC_TIMECRITICAL, -15,
                         plis->tidCurrent);
  if(usRetCode) {
    printf("Profiler: change priority failed x:%x",usRetCode);
    return TRUE;
  }

  return FALSE;
}

VOID fnvCloseUp(VOID)
{
  DosFreeSeg(selMessage);
}

VOID fnvResults(VOID)
{
  USHORT usRow, usCol, usIndex;
  ULONG ulTotal;

  ulTotal = 0;
  for(usIndex = 0; usIndex < PRO_MAX; usIndex++)
    ulTotal += aulHits[usIndex];
  time(&timeEnd);
  for(usIndex = 0; usIndex < PRO_MAX; usIndex++) {
    //usRow = usIndex / 4;
    //usCol = usIndex % 4;
    //if(!usCol)
      printf("\n");
    printf("%03u:%5.4lf    ", usIndex,aulHits[usIndex]/(double)ulTotal);
  }
  printf("\nTotal time %lu\n",timeEnd-timeStart);
}

VOID PASCAL FAR fnvInterrupt(USHORT usSigArg, USHORT usSigNum)
{
  _strtime(szWork);
  printf("CTRL-C at %s\n",szWork);
  if(fStarted)
    fnvResults();
  exit (0);
}

int cdecl main(void)
{
  ULONG  ulPrevSigH;
  USHORT usPrevSigA;

  fStarted = FALSE;

  if(fnusSetPriority()) {
    fnvCloseUp();
    return TRUE;
  }

  DosAllocShrSeg(sizeof(PROMESSAGE), PRO_MEM, &selMessage);
  pProMessage = MAKEP(selMessage,0);
  memset((VOID * ) pProMessage, 0, sizeof(PROMESSAGE));
  pProMessage->sCode = PRO_WAITING;
  DosSetSigHandler(fnvInterrupt,
                   (PFNSIGHANDLER FAR *) &ulPrevSigH,
                   &usPrevSigA,
                   SIGA_ACCEPT,
                   SIG_CTRLC);

  _strtime(szWork);
  printf("Profiler started at %s\n",szWork);

  while(pProMessage->sCode != PRO_EXIT) {
    if(!fStarted && pProMessage->sCode == PRO_START) {
      fStarted = TRUE;
      _strtime(szWork);
      time(&timeStart);
      printf("PRO_START at %s\n",szWork);
      continue;
    }
    if(fStarted)
      if(pProMessage->sCode > 0 && pProMessage->sCode < PRO_MAX)
        aulHits[pProMessage->sCode]++;
    DosSleep(10L);
  }
  _strtime(szWork);
  printf("PRO_EXIT at %s\n",szWork);

  if(fStarted)
    fnvResults();

  return FALSE;
}
