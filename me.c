/*
 * Author: Joshua Kovach
 * Date: 23 January 2011
 * Program Description:
 *  http://www.cis.gvsu.edu/~dulimarh/CS452/Projects/ME/
 *
 *  Write a C program that displays information about the current user and the
 *  current machine as obtained from the system where the program runs.
 *
 *  Sample Output:
 *
 *  About me
    ========
    Unix User       : dulimarh (2032)
    Name            : Hans Dulimarta
    Unix Group      : users (500)
    Unix Home       : /home/dulimarh
    Home permission : drwx--xr-x (40715)
    Login Shell     : /bin/bash
    On 2011-Jan-16, I am __ years __ months __ days old
    Other userids that end with an 'h':
            saslauth dicksoch halljoh alsabbagh schmmich

    About my machine
    =================
    Host   : eos19.cis.gvsu.edu
    System : Linux 2.6.34.7-63.fc13.x86_64

 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/utsname.h> // uname();

#include <sys/types.h> // for stat();
#include <sys/stat.h>
#include <unistd.h>

#include <pwd.h> // for password file stuff
#include <grp.h> // for groups

void process_permissions(struct stat, char*);

int main()
{
  struct passwd me;         // user info
  struct stat sb;         // permission bits (unsigned long)
  char permissions[11];   // "drwxrwxrwx\0"
  char host[1024];             // dns of the host for this computer
  struct utsname system;

  int days, months, years;

  me = *getpwnam(getenv("USER"));
  stat(me.pw_dir, &sb);
  process_permissions(sb, permissions); // set permissions string

  host = gethostname(host, 1024);
  uname(&system);

  days = 0;
  months = 0;
  years = 0;

  printf ("\nAbout me\n");
  printf ("========\n");
  printf ("Unix User\t\t: %s (%d)\n", me.pw_name, (int) me.pw_uid);
  printf ("Name\t\t\t: %s\n", me.pw_gecos);
  //printf ("Unix Group\t\t: %s (%d)\n", group.gr_name, group.gr_gid);
  printf ("Unix Home\t\t: %s\n", me.pw_dir);
  printf ("Home Permission\t\t: %s (%lo)\n",
          permissions, (unsigned long) sb.st_mode);
  printf ("Login Shell\t\t: %s\n", me.pw_shell);

  printf ("On 2011-Jan-16, I am %d years %d months and %d days old.\n",
          years, months, days);

  printf ("Other userids that end with '':\n");


  printf ("\nAbout my machine\n");
  printf ("==================\n");
  printf ("Host\t: %s\n", host);
  printf ("System\t: %s %s.%s\n",
          system.sysname, system.release, system.machine);
  printf ("\n");

  return 0;
}

void process_permissions(struct stat sb, char *p)
{
  unsigned long fp = sb.st_mode;

  p[0] = (S_ISDIR(fp)) ? 'd' : '-';
  p[1] = (fp & S_IRUSR) ? 'r' : '-';
  p[2] = (fp & S_IWUSR) ? 'w' : '-';
  p[3] = (fp & S_IXUSR) ? 'x' : '-';
  p[4] = (fp & S_IRGRP) ? 'r' : '-';
  p[5] = (fp & S_IWGRP) ? 'w' : '-';
  p[6] = (fp & S_IXGRP) ? 'x' : '-';
  p[7] = (fp & S_IROTH) ? 'r' : '-';
  p[8] = (fp & S_IWOTH) ? 'w' : '-';
  p[9] = (fp & S_IXOTH) ? 'x' : '-';
  p[10] = '\0';
}