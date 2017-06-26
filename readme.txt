readme for SOS utilities 1.0 =========================================

Author:         Tommi Nieminen 4-Jul-1993
E-mail to:      sktoni@kielo.uta.fi (Internet)

This package (sosutl10) should contain the following files:

    sostree.exe         Directory tree drawer
    sostree.c           Source code for sostree (Borland C++ 1.0)
    findfile.cmd        Find File utility written in REXX
    ps.cmd              Process Status utility written in Perl
    readme              This file

All these programs are public domain and intended to be free for every
user of OS/2.

The rest of this file contains manual pages for each of the enclosed
programs.

sostree --------------------------------------------------------------

Description

    SOS Tree - directory tree drawer for OS/2.

Usage

    [D:\] sostree [ -adfnqz ] PATH

    Options:

    -a      Display file attributes in the format "ADHRS"
    -d      Display dates and times too (in current national format).
    -f      Include plain files too in the listing.
    -n      List all directories (or all files and directories with -f
            option) with full path names; do not draw the tree graph.
    -q      Don't display the "Directory tree of ..." header.
    -z      Display file sizes too. This switch is meaningless if -f
            isn't used simultaneously.

Note

    This program is public domain. Copyrights are there only to show
    who first wrote this program.

Note of re-compiling

    SOS Tree should be distributed with full C source code included.
    If you compile the program with LANG_SUOMI symbolic constant de-
    fined, you get the Finnish version. It should also be easy to make
    other-language versions of the program: just search (with grep or
    such tool) for all instances of the string

        #if defined(LANG_SUOMI)

    and include your new language section after it. For example, to
    make an Esperanto version of SOS Tree:

        #if defined(LANG_SUOMI)
            /* Finnish messages */
        #elif defined(LANG_ESPERANTO)
            /* Esperanto messages */
        #elif defined(LANG_xxxx)
            /* ... */
        #else
            /* English messages */
        #endif

    Symbolic constants can be defined in the bcc command line like
    this:

        [D:\] bcc -DLANG_ESPERANTO sostree.c

Version

    SOS Tree v1.0 (C) SuperOscar Softwares, 1993.

findfile -------------------------------------------------------------

Description

    filefind - whereis-like find file utility written completely in
    REXX.

Usage

    [D:\] filefind { [PATH\]FNAME ... | /? }

    filefind finds files under PATH if that is specified, or the
    current directory (= '.') if it isn't. Multiple command line
    arguments are allowed.

Examples

    [D:\os2] filefind *.inf
        ; Finds all *.inf files under D:\os2
    [D:\] filefind \os2\*.inf
        ; Does the same as the preceding example
    [D:\] filefind c:\bin\*.exe
        ; Finds all *.exe files under C:\bin
    [D:\] filefind usr\*.inf os2\*.inf
        ; Finds all *.inf files under D:\usr and D:\os2

Note

    filefind displays file dates and times in the national format,
    which is found with a SysIni() call to "os2.ini". However, this
    seems to be causing troubles now and then--sometimes SysIni()
    returns insane results, like '0' as the date or time separator.
    IF YOU ARE HAVING PROBLEMS WITH THIS, remove all date and time
    formatting lines from the program, and change the following
    line:

        ok = SysFileTree(Word(find, i), files, "fst", "*--*-")

    to

        ok = SysFileTree(Word(find, i), files, "fs", "*--*-")

    filefind will then display dates and times in the American
    formats (MM/DD/YY and HH:MM{am|pm}, respectively) which are
    insane but work fine.

Version

    filefind.cmd v1.2
    (C) SuperOscar Softwares, Tommi Nieminen 1993

ps -------------------------------------------------------------------

Description

    ps - display processes with their process id's (PID), parent
    process id's (PPID), and session id's (SID).

Usage

    [D:\] ps

    ps output looks like this:

        PID   PPID  SID   PROCESS
        0002  0001  0001  d:\os2\pmshell.exe
        0025  0002  0022  d:\os2\viewdoc.exe
        0008  0002  0004  d:\usr\bin\4os2\4os2.exe
        0087  0008  0004  d:\usr\bin\unix\perl.exe
        0088  0087  0004  d:\usr\bin\4os2\4os2.exe
        0089  0088  0004  d:\os2\pstat.exe
        0007  0002  0019  d:\usr\bin\itsnow.exe
        0006  0002  0018  d:\usr\bin\pc2\pc2.exe
        0005  0002  0017  d:\usr\bin\xfeel.exe
        0004  0002  0016  d:\os2\pmshell.exe
        0003  0002  0000  d:\os2\system\harderr.exe

    All the numeric values are decimals. If you want hexadecimals,
    just remove all calls to hex() in ps.cmd.

System requirements

    ps is a Perl-utility and requires you to have Perl installed. Perl
    is a programming language originally written for UNIX (how else?),
    and it can be obtained in many FTP's and BBS's (I think, never
    tried the latter). For example, in hobbes.nmsu.edu Perl can be
    found in

        /os2/2_x/unix/program/perl*.zip

    (or something like that).

    Perl 4.0.1.7 patchlevel 35 was used in developing and testing this
    program.

    ps calls the OS/2 utility PStat to obtain PID, PPID and SID info,
    and re-formats the output to be more readable (or at least
    shorter). PStat is a part of OS/2 Diagnostic Tools installation
    option; if you don't have the file "pstat.exe" in your \OS2 dir-
    ectory, use the Selective Install object in your System folder to
    install it.

Notes

    (1) ps is a .cmd file that has "extproc" statement in the first
        line of the file. PLEASE EDIT the path to perl.exe to point to
        the correct directory. I have stored all UNIX-like utilities
        to \usr\bin\UNIX, but you very probably haven't.
    (2) Processes 87 ("perl.exe") and 88 ("4os2.exe") in the preceding
        list are created by ps.cmd. It would be fairly easy to make
        ps.cmd remove (a) the line that contains "perl.exe" (which is
        the running ps process), (b) the line whose PPID is the PID of
        "perl.exe" (this is a temporary shell created by the pipe
        ps.cmd uses to get output from PStat), but I didn't think it
        worth the trouble.

Version

    ps.cmd v3.0
    (C) SuperOscar Softwares, Tommi Nieminen 1993

---------------------------------------------------------------------

End of readme =======================================================
