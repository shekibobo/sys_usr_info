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

#include <sys/utsname.h>        // uname();

#include <sys/types.h>          // for stat();
#include <sys/stat.h>
#include <unistd.h>

#include <pwd.h>                // for password file stuff
#include <grp.h>                // for groups
#include <time.h>               // for time

void process_permissions(struct stat, char *);
char getlastchar(char *);
void printMatchingUsers(char, int);
void age(struct tm *, int *, int *, int *, char *);

int main()
{
    struct passwd me;           // user info
    struct group gr;            // user group name
    struct stat sb;             // permission bits (unsigned long)
    char permissions[11];       // "drwxrwxrwx\0"
    char host[1024];            // dns of the host for this computer
    struct utsname system;      // to hold the system info struct

    struct tm birthdate;
    birthdate.tm_year = 1962 - 1900;    //offset
    birthdate.tm_mon = 9 - 1;   //offset
    birthdate.tm_mday = 1;      //actual day
    int years, months, days;    // for age
    years = months = days = 0;
    char now_f[256];

    me = *getpwnam(getenv("USER"));     // get the pw entry for this user
    stat(me.pw_dir, &sb);       // get the permission bits for this user
    process_permissions(sb, permissions);       // set permissions string
    gr = *getgrgid(me.pw_gid);  // get the group name struct

    age(&birthdate, &years, &months, &days, now_f);

    gethostname(host, 1024);    // get the name of this computer
    uname(&system);             // get OS information

  /**
   *       PRINT OUTPUT TO SCREEN
   */
    printf("\nAbout me\n");
    printf("========\n");
    printf("Unix User\t\t: %s (%d)\n", me.pw_name, (int) me.pw_uid);
    printf("Name\t\t\t: %s\n", me.pw_gecos);
    printf("Unix Group\t\t: %s (%d)\n", gr.gr_name, gr.gr_gid);
    printf("Unix Home\t\t: %s\n", me.pw_dir);
    printf("Home Permission\t\t: %s (%lo)\n",
           permissions, (unsigned long) sb.st_mode);
    printf("Login Shell\t\t: %s\n", me.pw_shell);

    printf("On %s, I am %d years %d months and %d days old.\n",
           now_f, years, months, days);

    printf("Other userids that end with '%c':\n", getlastchar(me.pw_name));

    printMatchingUsers(getlastchar(me.pw_name), 5);

    printf("\nAbout my machine\n");
    printf("==================\n");
    printf("Host\t: %s\n", host);
    printf("System\t: %s %s.%s\n",
           system.sysname, system.release, system.machine);
    printf("\n");

  /**
   *             END OUTPUT
   */

    return 0;
}

/**
 * This method will take an argument of type struct stat and a string.
 * It pulls the permissions mode bit from the stat struct and puts the
 * ascii drwxrwxrwx interpretation into the passed string pointer.
 */
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

/**
 * This method will return the last character in the string passed to it.
 */
char getlastchar(char *str)
{
    // return the last character in a string
    return (str[strlen(str) - 1]);
}

/**
 * This method runs through each user entry of the passwd file and prints the
 * username from each entry where the username's last character is the same as
 * the character passed into the function. A limit must also be passed. If the
 * limit is set to 0, then it will print all matching usernames.
 *
 * Note: getpwent() aparrently has a massive memory leak when used on the
 * EOS and Arch machines.  If you comment out the section noted in the code,
 * it will compile with warnings of unused variables, but will also not
 * run with any memory leak warnings with valgrind.
 */
void printMatchingUsers(char lastchar, int limit)
{
    struct passwd *entry;       //passwd struct
    char *name;                 //user name
    int i;                      //counter
    i = (limit > 0 ? 0 : -1);

    printf("\t");
    /* -------------comment out from here-------------------- */
    setpwent();                 // start at the beginning

    // step through each entry
    while (((entry = getpwent()) != NULL) && (i < limit)) {

        name = entry->pw_name;

        // print it if last letter matches user lastchar
        if (lastchar == getlastchar(name)) {
            printf("%s ", name);
            limit > 0 ? i++ : i--;
        }
    }
    endpwent();                 // close the stream
    /* --------------to here with valgrind on eos/arch ------- */
    printf("\n");
}


/**
 * Takes a broken-time structure with the tm_year, tm_mon, and tm_mday set,
 * and gives a rough estimate (within a day or two) of your exact age, putting
 * the years, months, and days of the user's age into the y, m, d pointers
 * passed as arguments.Furhermore, it puts the current date into a readable
 * format in the buffer now_f.
 *
 * This assumes the current month always has 30 days.
 */
void age(struct tm *dob, int *y, int *m, int *d, char *now_f)
{
    int sy = dob->tm_year;
    int sm = dob->tm_mon;
    int sd = dob->tm_mday;

    time_t t = time(NULL);
    struct tm now = *localtime(&t);

    // pull the age apart by year, month, day, set the difference
    if (now.tm_mon < sm)
        *y = now.tm_year - (now.tm_mday < sd ? sy : sy + 1);
    else
        *y = now.tm_year - sy;
    *m = now.tm_mon < sm ? now.tm_mon + 12 - sm : now.tm_mon - sm;
    *d = now.tm_mday < sd ? now.tm_mday + 30 - sd : now.tm_mday - sd;

    // output as 2011-Jan-26
    strftime(now_f, 256, "%Y-%b-%d", &now);
}
