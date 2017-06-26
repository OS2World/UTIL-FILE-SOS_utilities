/* ---------
 * sostree.c (C) SuperOscar Softwares, Tommi Nieminen 1993.
 * ---------
 * SOS Tree directory tree drawer for OS/2.
 * Programming language: Borland C++ for OS/2 1.0.
 * Programmer: Tommi Nieminen.
 *
 * 04.07.1993   v1.0: first PD version ready.
 *
 */

#include <stdio.h>              /* Standard I/O */
#include <stdarg.h>             /* va_ functions */
#include <dir.h>                /* findfirst(), findnext(), chdir() */
#include <string.h>             /* String handling */
#include <alloc.h>              /* malloc(), free() */
#include <dos.h>                /* Symbolic names for file attributes */
#include <ctype.h>              /* toupper() */
#define INCL_DOSNLS
#include <os2.h>                /* Load NLS support */

  /* Official program name string for messages */
const char *program_name = "SOS Tree v1.0";
const char *copyright = "(C) SuperOscar Softwares, Tommi Nieminen 1993";

  /* Program flags */
#define F_SHOW_TREE         0x0001
#define F_SHOW_FILES        0x0002
#define F_SHOW_DATES        0x0004
#define F_SHOW_SIZES        0x0008
#define F_SHOW_ATTRS        0x0010
#define F_SHOW_HIDDEN       0x0020
#define F_QUIET             0x0040
#define F_SHOW_HELP         0x8000

  /* Error codes */
typedef enum {
    NO_ERROR,
    ERR_PATH_NOT_FOUND,
    ERR_UNKNOWN_SWITCH,
    ERR_CMDLINE_SYNTAX,
    ERR_INTERNAL
} ERROR;

  /* Error messages */
const char *errormsg[] = {
#if defined(LANG_SUOMI)
    "Polkua \"%s\" ei l”ydy",
    "Tuntematon valitsin \"-%c\"",
    "Virheellinen komentorivi (\"-?\" antaa apua)",
    "Sis„inen ohjelmavirhe"
#else
    "Path not found \"%s\"",
    "Unknown switch \"-%c\"",
    "Invalid command line syntax (use \"-?\" to get help)",
    "Program internal error"
#endif
};

#if defined(LANG_SUOMI)
const char *header_line = "Hakemistopuu \"%s\"\n";
#else
const char *header_line = "Directory tree of \"%s\"\n";
#endif

  /* Global variables */
ERROR status = NO_ERROR;                /* Global error status */
int datefmt, timefmt;                   /* Date and time formats */
char datesep, timesep;                  /* Date and time separators */
unsigned flags = F_SHOW_TREE;           /* Program flags */

  /* Macroes */
#define ISDIR(f)        (f.ff_attrib & FA_DIREC)
#define ISDOTDIR(f)     (!strcmp(f.ff_name, ".") || !strcmp(f.ff_name, ".."))
#define D_DAY(date)     ((date) & 31)
#define D_MON(date)     (((date) & 480) / 32)
#define D_YEAR(date)    (((date) & 65024) / 512 + 80)
#define T_MIN(time)     (((time) & 2016) / 32)
#define T_HOUR(time)    (((time) & 63488) / 2048)

  /* Function declarations */
char *cmdline_parse(int argc, char *argv[]);
int dir_exists(char *pathname);
void country_get(void);
void tree_draw(const char *mother);
char *path_cat(const char *part1, const char *part2);
char *date_format(unsigned short filedate);
char *time_format(unsigned short filetime);
void error_raise(ERROR err, ...);
void help(void);

int main(int argc, char *argv[])
{
    char *base;

    country_get();
    base = cmdline_parse(argc, argv);

    if (flags & F_SHOW_HELP)
        help();
    else if (!status)
        if (!dir_exists(base))
            error_raise(ERR_PATH_NOT_FOUND, base);
        else {
            if (!(flags & F_QUIET))
                printf(header_line, base);
            tree_draw(base);
        }
    free(base);

    return(status);
}

char *cmdline_parse(int argc, char *argv[])
  /* Parse command line. Return base directory name. */
{
    char *dirname;
    int ind = 1;

    dirname = (char *) malloc(sizeof(char) * (MAXPATH + 1));
    strcpy(dirname, ".");

    if (-- argc) {
        char *p;

        if (argv[1][0] == '-') {
            ind = 2;
            for (p = &argv[1][1]; *p; p ++)
                switch (*p) {
                    case 'a':   flags |= F_SHOW_ATTRS;
                                break;
                    case 'd':   flags |= F_SHOW_DATES;
                                break;
                    case 'f':   flags |= F_SHOW_FILES;
                                break;
                    case 'h':   flags |= F_SHOW_HIDDEN;
                                break;
                    case 'n':   flags &= ~F_SHOW_TREE;
                                break;
                    case 'q':   flags |= F_QUIET;
                                break;
                    case 'z':   flags |= F_SHOW_SIZES;
                                break;
                    case '?':   flags |= F_SHOW_HELP;
                                break;
                    default:    error_raise(ERR_UNKNOWN_SWITCH, *p);
                }
        }
        if (argc == ind) {
            strncpy(dirname, argv[ind], MAXPATH);
            dirname[MAXPATH] = '\0';
        }
        else if (argc > ind)
            error_raise(ERR_CMDLINE_SYNTAX);
     }

    return(dirname);
}

int dir_exists(char *pathname)
  /* Check if 'fname' is a directory. Return NULL if it isn't, and
   * absolute path name if it is.
   */
{
    char current_path[MAXDIR + 1],
        drivestr[MAXDRIVE + 1];
    int drive = 0,
        current_drive,
        exists = 0;

    /* Note: 0 means A: drive in getdisk() and setdisk(), but `default'
     * drive in getcurdir() (where A: is 1).
     */

      /* Does 'fname' contain a drive letter? */
    fnsplit(pathname, drivestr, NULL, NULL, NULL);
    drive = (*drivestr) ? toupper(*drivestr) - '@' : 0;

      /* Store current drive, and current directory on drive 'drive'. */
    current_drive = getdisk();
    getcurdir(drive, current_path);

      /* Try to cd to 'fname' on drive 'drive'. */
    if (drive)
        setdisk(drive - 1);
    if (!chdir(pathname)) {
        char tmp[MAXDIR + 1];

        getcurdir(drive, tmp);
        strcpy(pathname, " :\\");
        pathname[0] = (char) ((drive) ? drive + '@' : current_drive + 'A');
        strcat(pathname, tmp);
        exists = 1;
    }

      /* Restore directory if changed. */
    chdir(current_path);
    if (drive)
        setdisk(current_drive);

    return(exists);
}

void country_get(void)
  /* Query current system date and time formats and separators. */
{
    ULONG len;
    COUNTRYCODE query;
    COUNTRYINFO info;
    APIRET rc;

    query.country = 0;
    query.codepage = 850;
    rc = DosQueryCtryInfo(sizeof(info), &query, &info, &len);
    if (!rc) {
        datesep = *(info.szDateSeparator);
        timesep = *(info.szTimeSeparator);
        datefmt = info.fsDateFmt;
        timefmt = info.fsTimeFmt;
    }
    else
        error_raise(ERR_INTERNAL);
}

void tree_draw(const char *mother)
  /* Recurse to all subdirectories of 'mother'. */
{
    static char sequel_buffer[MAXPATH] = "";
    char *search, *fullname;
    struct ffblk this, newest;
    int ok;

    search = path_cat(mother, "*");

    ok = findfirst(search, &newest, (flags & F_SHOW_HIDDEN) ?
        FA_DIREC | FA_HIDDEN | FA_SYSTEM : FA_DIREC);
    while (!ok) {
        this = newest;
        fullname = path_cat(mother, this.ff_name);

          /* First check whether this directory is the last on its
           * level or not.
           */
        if (flags & F_SHOW_FILES)
            ok = findnext(&newest);
        else {
              /* When not displaying ordinary files, more work is needed
               * because findnext() always finds files too: we have to
               * loop until we find a directory or run out of files.
               */
            ok = findnext(&newest);
            while (!ISDIR(newest) && !ok)
                ok = findnext(&newest);
        }

          /* Discard "." and ".." */
        if (!ISDOTDIR(this)) {
              /* If 'this' is not a directory and F_SHOW_FILES flag isn't
               * set, start next cycle
               */
            if (!ISDIR(this) && !(flags & F_SHOW_FILES))
                continue;

            if (flags & F_SHOW_DATES)
                printf("%s  %s  ", date_format(this.ff_fdate),
                    time_format(this.ff_ftime));

            if (flags & F_SHOW_SIZES)
                printf("%8lu  ", this.ff_fsize);

            if (flags & F_SHOW_ATTRS)
                printf("%c%c%c%c%c  ",
                    (this.ff_attrib & FA_ARCH) ? 'A' : '-',
                    (this.ff_attrib & FA_HIDDEN) ? 'H' : '-',
                    (this.ff_attrib & FA_RDONLY) ? 'R' : '-',
                    (this.ff_attrib & FA_SYSTEM) ? 'S' : '-',
                    (this.ff_attrib & FA_DIREC) ? 'D' : '-');

            if (flags & F_SHOW_TREE)
                printf("%s%cÄÄ%s\n", sequel_buffer, (ok) ? 'À' : 'Ã',
                    this.ff_name);
            else
                printf("%s\n", fullname);

              /* If 'this' is a directory, try to recurse to sub-
               * directories.
               */
            if (ISDIR(this)) {
                  /* Update upper-level sequel buffer. */
                strcat(sequel_buffer, (ok) ? "   " : "³  ");

                tree_draw(fullname);

                  /* Truncate sequel buffer to its previous value. */
                sequel_buffer[strlen(sequel_buffer) - 3] = '\0';
            }
        }
        free(fullname);
    }
    free(search);
}

char *path_cat(const char *part1, const char *part2)
  /* Combine two path name components into one. Add a `\' between
   * the parts if necessary.
   */
{
    char *newpath;

    newpath = (char *) malloc(sizeof(char) * (MAXPATH + 1));
    strcpy(newpath, part1);
    if (part1[strlen(part1) - 1] != '\\' && *part2 != '\\')
        strcat(newpath, "\\");
    strcat(newpath, part2);

    return(newpath);
}

char *date_format(unsigned short filedate)
  /* Format file date to a string of correct national format. */
{
    static char outstr[11];
    unsigned day, mon, year;

    day = D_DAY(filedate);
    mon = D_MON(filedate);
    year = D_YEAR(filedate);

    switch (datefmt) {
        case 0: case 1:
            sprintf(outstr, "%2d%c%2d%c%02d", (datefmt == 0) ? mon : day,
                datesep, (datefmt == 0) ? day : mon, datesep, year);
            break;
        case 2:
            sprintf(outstr, "%02d%c%02d%c%02d", year, datesep, mon, datesep,
                day);
            break;
    }

    return(outstr);
}

char *time_format(unsigned short filetime)
  /* Format file time to a string of correct national format. */
{
    static char outstr[8];
    unsigned hour, min;

    hour = T_HOUR(filetime);
    if (!hour)
        hour = 24;
    min = T_MIN(filetime);

    switch (timefmt) {
        case 0:
            sprintf(outstr, "%2d%c%02d%s", (hour <= 12) ? hour : hour - 12,
                timesep, min, (hour <= 12) ? "am" : "pm");
            break;
        case 1:
            sprintf(outstr, "%2d%c%02d", hour, timesep, min);
            break;
    }

    return(outstr);
}

void error_raise(ERROR err, ...)
{
    va_list argv;

    fprintf(stderr, "%s: ", program_name);
    va_start(argv, err);
    vfprintf(stderr, errormsg[err - 1], argv);
    va_end(argv);
    fprintf(stderr, "\n");

    status = err;
}

void help(void)
  /* Display help text. */
{
    printf("%s %s\n\n", program_name, copyright);
#if defined(LANG_SUOMI)
    puts("N„yt„ hakemistorakenne kaaviona tai luettelona. K„ytt”:");
    puts("\n    [D:\\] sostree [ -adfhnqz | -? ] [ HAK ]\n");
    puts("Valitsimien merkitykset ovat seuraavat:");
    puts("    -a  N„yt„ my”s tiedostom„„reet.");
    puts("    -d  N„yt„ my”s p„iv„ykset ja ajat.");
    puts("    -f  N„yt„ my”s tiedostot (lehtin„).");
    puts("    -h  N„yt„ my”s piilotetut ja j„rjestelm„tiedostot.");
    puts("    -n  N„yt„ t„ydet polkunimet puukaaviotta.");
    puts("    -q  Žl„ n„yt„ \"Hakemistopuu ...\" -tunnisterivi„.");
    puts("    -z  N„yt„ my”s tiedostojen koot.\n");
    puts("    -?  N„yt„ t„m„ avustusn„ytt”.");
#else
    puts("Display directory tree with or without graphics. Usage:");
    puts("\n    [D:\\] sostree [ -adfhnqz | -? ] [ DIR ]\n");
    puts("The meanings of the switches are the following:");
    puts("    -a  Show file attributes too.");
    puts("    -d  Show file dates and times too.");
    puts("    -f  Show files also (as leaves).");
    puts("    -h  Show hidden and system files too.");
    puts("    -n  Show full path names without graphics.");
    puts("    -q  Do not show \"Directory tree of ...\" header.");
    puts("    -z  Show file sizes too.\n");
    puts("    -?  Show this help screen.");
#endif
}
