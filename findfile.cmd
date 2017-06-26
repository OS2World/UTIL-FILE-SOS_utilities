/* ------------
 * findfile.cmd (C) Tommi Nieminen 1993.
 * ------------
 * WhereIs written in REXX.
 *
 * 24.02.1993   v1.0: first version ready.
 * 26.02.1993   v1.0a: bug corrections.
 * 26.02.1993   v1.1: multiple file parameters allowed.
 * 04.07.1993   v1.2: national date and time formats supported.
 */

Parse Arg find

prgname = "findfile.cmd v1.2"

If find == "" Then Call Error "no parameters (use /? to get help)"
If find == "/?" Then Call Help

  /* Load SysFileTree() and SysIni() functions */
Call RxFuncAdd "SysFileTree", "RexxUtil", "SysFileTree"
Call RxFuncAdd "SysIni", "RexxUtil", "SysFileTree"

  /* Read national language settings from "os2.ini" */
iDate = SysIni("user", "PM_National", "iDate")
iTime = SysIni("user", "PM_National", "iTime")
sDate = SysIni("user", "PM_National", "sDate")
sTime = SysIni("user", "PM_National", "sTime")

  /* SysIni() return strings seem to cause a lot of trouble: when I tried
   * to call all these functions together like
   *    datefmt = Abs(Left(SysIni( ...
   * REXX always disliked the syntax. There are also strange extra char-
   * acters in the Ini entries -- eg. sDate and sTime contain (in my own
   * os2.ini) a space character after the actual separator. That's why
   * Left() and Abs() functions must be called.
   *
   * Bug warning: in certain situations strange results may be returned
   * by SysIni(): I once got '0' as the date and time separators when I
   * ran Windows programs in a window.
   */
datefmt = Abs(Left(iDate, 1))
time24 = Abs(Left(iTime, 1))
datesep = Left(sDate, 1)
timesep = Left(sTime, 1)

  /* Go through all parameters */
Do i = 1 To Words(find)

      /* Find files. Files with D(irectory), H(idden) and (S)ystem attribute
       * are discarded. Attribute string (the last SysFileTree() parameter)
       * is of the format "ADHRS"; "*" means don't care, "-" attribute not
       * set, "+" attribute is set.
       */
    ok = SysFileTree(Word(find, i), files, "fst", "*--*-")

      /* Display results in national format */
    Do j = 1 To files.0

          /* Separate different parts of date-time string */
        datetime = Translate(Word(files.j, 1), " ", "/")
        year = Format(Word(datetime, 1), 2)
        mon = Format(Word(datetime, 2), 2)
        day = Format(Word(datetime, 3), 2)
        hour = Format(Word(datetime, 4), 2)
        min = Word(datetime, 5)

        Select
            When datefmt == 0 Then
                date = mon || datesep || day || datesep || year
            When datefmt == 1 Then
                date = day || datesep || mon || datesep || year
            When datefmt == 2 Then
                date = year || datesep || mon || datesep || day
            Otherwise
                Call Error "Date format code ==" datefmt"!!??"
        End

        Select
            When time24 == 0 Then Do
                ampm = "am"
                If hour > 12 Then Do
                    ampm = "pm"
                    hour = Format(hour - 12, 2)
                End
                time = hour || timesep || min || ampm
            End
            When time24 == 1 Then
                time = hour || timesep || min
            Otherwise
                Call Error "Time format code ==" timefmt"!!??"
        End

          /* Other information */
        size = Format(Word(files.j, 2), 8)
        attr = Word(files.j, 3)
        fname = Word(files.j, 4)

          /* Display information */
        Say date time size attr fname
    End
End

Exit 0

Error: Procedure Expose prgname
    Parse Arg msg

    Say prgname":" msg
Exit 1

Help: Procedure Expose prgname
    Say prgname" (C) SuperOscar Softwares, Tommi Nieminen 1993."
    Say
    Say "findfile.cmd is a file find program written in REXX. It finds files"
    Say "matching given mask(s) under a given path.  Hidden and system files"
    Say "and directories are not found."
    Say
    Say "   Usage: [D:\] whereis { [PATH\]FILE ... | /? }"
    Say
    Say "If PATH is not given, its default value is '.' (the current direct-"
    Say "ory)."
Exit 0
