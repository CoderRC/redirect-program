#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <string.h>
#ifndef REDIRECT
#define REDIRECT "../rta"
#endif

int
main (int argc, char **argv, char *envp[])
{
  struct stat st;
  Dl_info dlInfo;
  size_t installDirNameSize;
  char *installDir;
  const char *redirect = REDIRECT;
  const size_t redirectSize = strlen (redirect);
  size_t i, j, k;
  pid_t spid;
  char **sargv;
  siginfo_t ssiginfo;
  char *dirStr;
  size_t dirStrSize;
  installDirNameSize = 0;
  dlInfo.dli_fname = 0;
  if (!dladdr (&main, &dlInfo) || !dlInfo.dli_fname)
    {
      dlInfo.dli_fname = *argv;
    }
  for (j = i = 0; dlInfo.dli_fname[i]; i++)
    {
      k = dlInfo.dli_fname[i] == '/' || dlInfo.dli_fname[i] == '\\';
      k--;
      installDirNameSize = (i & (0 - k - 1)) + (installDirNameSize & k);
    }
  if (!installDirNameSize)
    {
      installDirNameSize = strlen (dlInfo.dli_fname);
      dirStr = getenv ("PATH");
      dirStrSize = 0;
      if (dirStr)
	{
	  for (; dirStr[dirStrSize] && dirStr[dirStrSize] != ':';
	       dirStrSize++);
	  for (; dirStr[dirStrSize] && !dirStrSize;)
	    {
	      dirStr += *dirStr == ':';
	      for (dirStrSize = 0;
		   dirStr[dirStrSize] && dirStr[dirStrSize] != ':';
		   dirStrSize++);
	    }
	}
      while (dirStrSize)
	{
	  installDir = (char *) malloc (dirStrSize + installDirNameSize + 2);
	  if (installDir)
	    {
	      memcpy (installDir, dirStr, dirStrSize);
	      installDir[dirStrSize] = '/';
	      memcpy (installDir + dirStrSize + 1, dlInfo.dli_fname,
		      installDirNameSize);
	      installDir[dirStrSize + 1 + installDirNameSize] = 0;
	      if (!stat (installDir, &st) && S_ISREG (st.st_mode))
		{
		  free (installDir);
		  dlInfo.dli_fname = dirStr;
		  installDirNameSize = dirStrSize;
		  break;
		}
	      free (installDir);
	    }
	  dirStr += dirStrSize;
	  dirStr += *dirStr == ':';
	  for (dirStrSize = 0;
	       dirStr[dirStrSize] && dirStr[dirStrSize] != ':'; dirStrSize++);
	  for (; dirStr[dirStrSize] && !dirStrSize;)
	    {
	      dirStr += *dirStr == ':';
	      for (dirStrSize = 0;
		   dirStr[dirStrSize] && dirStr[dirStrSize] != ':';
		   dirStrSize++);
	    }
	}
      if (dlInfo.dli_fname != dirStr)
	{
	  installDirNameSize = 0;
	}
    }
  installDir = (char *) malloc (installDirNameSize + redirectSize + 2);
  if (!installDir)
    {
      return 0;
    }
  sargv = (char **) malloc ((argc + 1) * sizeof (*sargv));
  if (!sargv)
    {
      free (installDir);
      return 0;
    }
  for (i = 0; i != installDirNameSize; i++)
    {
      installDir[i] = dlInfo.dli_fname[i];
      j = installDir[i] == '\\';
      installDir[i] = (installDir[i] & (j - 1)) + ('/' & (0 - j));
    }
  installDir[installDirNameSize] = '/';
  installDirNameSize += installDirNameSize != 0;
  j = 1;
  for (i = 0; i != redirectSize; i++)
    {
      if (j && redirect[i] == '.' && redirectSize - i != 1
	  && redirect[i + 1] == '/')
	{
	  i++;
	  continue;
	}
      if (j && redirect[i] == '.' && redirectSize - i != 1
	  && redirect[i + 1] == '.' && redirectSize - i != 2
	  && redirect[i + 2] == '/')
	{
	  i++;
	  i += redirectSize - i != 1;
	  if (!installDirNameSize)
	    {
	      *installDir = '.';
	      installDir[1] = '.';
	      installDir[2] = '/';
	      installDirNameSize += 3;
	      continue;
	    }
	  installDir[installDirNameSize] = 0;
	  for (installDirNameSize--;
	       installDirNameSize && installDir[installDirNameSize] == '/';
	       installDirNameSize--);
	  for (; installDirNameSize && installDir[installDirNameSize] != '/';
	       installDirNameSize--);
	  installDirNameSize += installDir[installDirNameSize] == '/';
	  if (installDir[installDirNameSize] == '.'
	      && installDir[installDirNameSize + 1] == '.'
	      && installDir[installDirNameSize + 2] == '/')
	    {
	      installDirNameSize += 3;
	      installDir[installDirNameSize] = '.';
	      installDir[installDirNameSize + 1] = '.';
	      installDir[installDirNameSize + 2] = '/';
	      installDirNameSize += 3;
	    }
	  if (!installDirNameSize)
	    {
	      *installDir = '.';
	      installDir[1] = '/';
	      installDirNameSize += 2;
	    }
	  continue;
	}
      j = redirect[i] == '/';
      installDir[installDirNameSize] = redirect[i];
      installDirNameSize++;
    }
  installDir[installDirNameSize] = 0;
  *sargv = installDir;
  for (i = 1; i != argc; i++)
    {
      sargv[i] = argv[i];
    }
  sargv[argc] = NULL;
  if (installDir && posix_spawnp (&spid, installDir, NULL, NULL, sargv, envp))
    {
      free (installDir);
      installDir = 0;
    }
  free (sargv);
  if (installDir)
    {
      free (installDir);
      waitid (P_PID, spid, &ssiginfo, WEXITED);
    }
  return 0;
}
