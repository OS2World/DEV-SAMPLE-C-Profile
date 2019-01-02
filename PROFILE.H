/* profile.h */

#define PRO_MEM "\\sharemem\\promsg"   //name of shared memory segment
#define PRO_WAITING -3  //profiler waiting for target to start
#define PRO_START   -2  //target tells profiler to start tracking
#define PRO_EXIT    -1  //target tells profiler to finish
#define PRO_MAX     40  //profiler will track values 0 - 39
#define PRO_STACK   20  //stack for up to 20 nested function calls

#ifdef PROFILE
#define ENTER(a) {pPM->sTop++; pPM->sCode = pPM->asStack[pPM->sTop] = a;}
#define SET(a)   {pPM->sCode = pPM->asStack[pPM->sTop] = a;}
#define LEAVE    {pPM->sTop--; pPM->sCode = pPM->asStack[pPM->sTop];}
#define SETUP_PRO if(fnusIPro()){printf("profile setup fail\n");exit(TRUE);}
#define EXIT_PRO  {fnvXPro();}
#else
#define ENTER       //start a function, pushes stack
#define SET         //changes code, does no push stack
#define LEAVE       //leave function, pops stack
#define SETUP_PRO   //set up memory segments in target program
#define EXIT_PRO    //clean up memory segments in target program
#endif

typedef struct _ProMessage {
  SHORT    sCode;
  SHORT    sTop;
  SHORT   asStack[PRO_STACK];
} PROMESSAGE, *PPROMESSATE;

extern PROMESSAGE   *pPM;

VOID fnvXPro(VOID);
USHORT fnusIPro(VOID);
