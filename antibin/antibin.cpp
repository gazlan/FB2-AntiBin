/* ******************************************************************** **
** @@ AntiBin
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Dscr   : Remove FB2 files binary part
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>

#include "..\shared\file_find.h"
#include "..\shared\search_bmh.h"
#include "..\shared\mmf.h"

/* ******************************************************************** **
** @@                   internal #defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#endif 

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

extern DWORD      dwKeepError = 0;

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ Proceed()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Modify :
** @  Notes  :
** ******************************************************************** */

static void Proceed(const char* const pszFileName)
{
   char     pszBackName[_MAX_PATH];
   char     pszDrive   [_MAX_DRIVE];
   char     pszDir     [_MAX_DIR];
   char     pszFName   [_MAX_FNAME];
   char     pszExt     [_MAX_EXT];

   _splitpath(pszFileName,pszDrive,pszDir,pszFName,pszExt);
   _makepath( pszBackName,pszDrive,pszDir,pszFName,"bak");

   CopyFile(pszFileName,pszBackName,FALSE);

   MMF   Victim;

   if (!Victim.OpenReadOnly(pszBackName))
   {
      // Error !
      return;
   }

   BYTE*    pBuf   = Victim.Buffer();
   DWORD    dwSize = Victim.Size();

   DWORD    dwOutSize = dwSize;


   const char     pszPattern[] = "<binary";
   int            iPatSize     = strlen(pszPattern);

   int      iFoundAt = BMH_ISearch(pBuf,dwSize,(BYTE*)pszPattern,iPatSize);

   if (iFoundAt > 0)
   {
      dwOutSize = iFoundAt;
   }

   int   iFile = _open(pszFileName,_O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY | _S_IWRITE); 
   
   _write(iFile,pBuf,dwOutSize);
   _chmod(pszFileName,_S_IWRITE);
   _close(iFile);

   Victim.Close();
}

/* ******************************************************************** **
** @@ ShowHelp()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Modify :
** @  Notes  :
** ******************************************************************** */

static void ShowHelp()
{
   const char  pszCopyright[] = "-*-   AntiBin 1.0   *   Copyright  (c)  Gazlan, 2014   -*-";
   const char  pszDescript [] = "Remove FB2 files binary part";
   const char  pszE_Mail   [] = "complains_n_suggestions direct to gazlan@yandex.ru";

   printf("%s\n\n",pszCopyright);
   printf("%s\n\n",pszDescript);
   printf("Usage: antibin.com wildcards\n\n");
   printf("%s\n",pszE_Mail);
}

/* ******************************************************************** **
** @@ main()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int main(int argc,char** argv)
{
   if (argc == 2 && ((!strcmp(argv[1],"?")) || (!strcmp(argv[1],"/?")) || (!strcmp(argv[1],"-?")) || (!stricmp(argv[1],"/h")) || (!stricmp(argv[1],"-h"))))
   {
      ShowHelp();
      return 0;
   }

   FindFile   FF;

   if (argv[1] && *argv[1])
   {
      FF.SetMask(argv[1]);
   }
   else
   {
      FF.SetMask("*.*");
   }

   while (FF.Fetch())
   {
      if ((FF._w32fd.dwFileAttributes | FILE_ATTRIBUTE_NORMAL) && !(FF._w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {  
         Proceed(FF._w32fd.cFileName);
      }
   }

   return 0;
}

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
