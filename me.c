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
#include <string.h>

#include <sys/utsname.h> // uname();

#include <sys/types.h> // for stat();
#include <sys/stat.h>
#include <unistd.h>

#include <pwd.h> // for password file stuff
#include <grp.h> // for groups
#include <time.h> // for time

void process_permissions(struct stat, char*);
char getlastchar(char*);
void printMatchingUsers(char);

int main()
{
  struct passwd me;       // user info
  struct group gr;    // user group name
  struct stat sb;         // permission bits (unsigned long)
  char permissions[11];   // "drwxrwxrwx\0"
  char host[1024];        // dns of the host for this computer
  struct utsname system;  // to hold the system info struct

  struct tm birthdate;
    birthdate.tm_year = 1962;
    birthdate.tm_mon = 9;
    birthdate.tm_mday = 1;


  int days, months, years;

  me = *getpwnam(getenv("USER"));       // get the pw entry for this user
  stat(me.pw_dir, &sb);                 // get the permission bits for this user
  process_permissions(sb, permissions); // set permissions string
  gr = *getgrgid(me.pw_gid); // get the group name struct

  gethostname(host, 1024);    // get the name of this computer
  uname(&system);             // get OS information

  days = 0;
  months = 0;
  years = 0;

  printf ("\nAbout me\n");
  printf ("========\n");
  printf ("Unix User\t\t: %s (%d)\n", me.pw_name, (int) me.pw_uid);
  printf ("Name\t\t\t: %s\n", me.pw_gecos);
  printf ("Unix Group\t\t: %s (%d)\n", gr.gr_name, gr.gr_gid);
  printf ("Unix Home\t\t: %s\n", me.pw_dir);
  printf ("Home Permission\t\t: %s (%lo)\n",
          permissions, (unsigned long) sb.st_mode);
  printf ("Login Shell\t\t: %s\n", me.pw_shell);

  printf ("On 2011-Jan-16, I am %d years %d months and %d days old.\n",
          years, months, days);

  printf ("Other userids that end with '%c':\n", getlastchar(me.pw_name));

  printMatchingUsers(getlastchar(me.pw_name));

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

char getlastchar(char* str)
{
  // return the last character in a string
  return (str[strlen(str)-1]);
}

void printMatchingUsers(char lastchar)
{
  struct passwd* entry;
  char* name;
  printf("\t");
  setpwent(); // start at the beginning

  // step through each entry
  for (entry = getpwent(); entry != NULL; entry = getpwent()) {
    name = entry->pw_name;

    // print it if last letter matches user lastchar
    if (lastchar == getlastchar(name)) {
      printf("%s ", name);
    }
  }
  endpwent(); // close the stream
  printf("\n");
}
