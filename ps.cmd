extproc D:\usr\bin\UNIX\perl -x

#!perl
# ------
# ps.cmd (C) Tommi Nieminen 1993.
# ------
# Changes output coming from PStat to shorter and more readable format.
# Requires Perl (tested with Perl 4.0.1.7 patchlevel 35).
#
# 31.03.1993    v1.0: first usable AWK version.
# 02.06.1993    v2.0: translation to Perl.
# 03.07.1993    v3.0: whole process done from within Perl.

print "PID   PPID  SID   PROCESS\n";
open(PSTAT, "pstat /c |");
while (<PSTAT>) {
    @table = split(/ +/);
    if ($#table == 8) {
        $pid = hex($table[1]);
        $ppid = hex($table[2]);
        $sid = hex($table[3]);
        $name = $table[4];

          # Convert name to lower case
        $name =~ tr/A-Z/a-z/;

        printf("%04d  %04d  %04d  %s\n", $pid, $ppid, $sid, $name);
    }
}
