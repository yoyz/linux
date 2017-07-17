
Overview
========

Track the IO done by each process (open,read,write,close ).
If it is a mpi process, the rank is catch and the log file contain the hostname.rank.pid.
If it is not a mpi process, only the hostname.pid is append to the log
filename.



Exemple
=======

$ g++ -fPIC -std=c++0x -ldl -shared inspectio11.cpp -o inspectio.so   	# compilation
$ export    INSPECTIO_DUMP=$PWD/LOG.${SLURM_JOBID}                   	# tell where is the DUMP directory
$ rm -f    $INSPECTIO_DUMP/*                                         	# remove this DUMP Directory
$ mkdir -p $INSPECTIO_DUMP                                           	# then recreate it to be clean
$ unset     INSPECTIO_ALL                                            	# don't want the "strace" like output
$ export LD_PRELOAD=$(pwd)/inspectio.so                              	# tell ld to link the library
$ time mpirun -n 4 dd if=/dev/zero of=/tmp/toto bs=1M count=1000	# launch a mpi job
$ unset LD_PRELOAD							# deactivate the library
$ grep ^ LOG.${SLURM_JOBID}/*|grep -v nompi				# show the log of inspectio
 

1000+0 records in
1000+0 records out
1048576000 bytes (1.0 GB) copied, 4.58632 s, 229 MB/s
1000+0 records in
1000+0 records out
1048576000 bytes (1.0 GB) copied, 1.58308 s, 662 MB/s
1000+0 records in
1000+0 records out
1048576000 bytes (1.0 GB) copied, 1.58339 s, 662 MB/s
1000+0 records in
1000+0 records out
1048576000 bytes (1.0 GB) copied, 5.20686 s, 201 MB/s

real    0m14.218s
user    0m8.051s
sys     0m6.542s

             
inspectiolog.sid4.bullx.0.22776:SUMARY[    CALL   MBYTE      SEC         CALL   MBYTE     SEC       FD OLDFD STATE NAME
inspectiolog.sid4.bullx.0.22776:WRITE [       0     0.0     0.0 ] READ[ 1000  1048.6     0.1 ]  [   -1     0     5 /dev/zero]
inspectiolog.sid4.bullx.0.22776:WRITE [    1000  1048.6     4.5 ] READ[ 0        0.0     0.0 ]  [   -1     1     5 /tmp/toto]
inspectiolog.sid4.bullx.0.22776:IOTIME[          1048.6     4.5 ] R/W [ 1048.6           0.1 ]BW[   458.6 MB/s ]
inspectiolog.sid4.bullx.1.22831:SUMARY[    CALL   MBYTE      SEC        CALL   MBYTE     SEC       FD OLDFD STATE NAME
inspectiolog.sid4.bullx.1.22831:WRITE [       0     0.0     0.0 ] READ[ 1000  1048.6     0.1 ]  [   -1     0     5 /dev/zero]
inspectiolog.sid4.bullx.1.22831:WRITE [    1000  1048.6     1.5 ] READ[ 0        0.0     0.0 ]  [   -1     1     5 /tmp/toto]
inspectiolog.sid4.bullx.1.22831:IOTIME[          1048.6     1.5 ] R/W [       1048.6     0.1]BW[  1335.5 MB/s ]
inspectiolog.sid4.bullx.2.22887:SUMARY[    CALL   MBYTE      SEC        CALL   MBYTE     SEC       FD OLDFD STATE NAME
inspectiolog.sid4.bullx.2.22887:WRITE [       0     0.0     0.0 ] READ[ 1000  1048.6     0.1 ]  [   -1     0     5 /dev/zero]
inspectiolog.sid4.bullx.2.22887:WRITE [    1000  1048.6     1.5 ] READ[ 0        0.0     0.0 ]  [   -1     1     5 /tmp/toto]
inspectiolog.sid4.bullx.2.22887:IOTIME[          1048.6     1.5 ] R/W [       1048.6     0.1    ]BW[  1335.4 MB/s ]
inspectiolog.sid4.bullx.3.22941:SUMARY[    CALL   MBYTE      SEC        CALL   MBYTE     SEC       FD OLDFD STATE NAME
inspectiolog.sid4.bullx.3.22941:WRITE [       0     0.0     0.0 ] READ[ 1000  1048.6     0.1 ]  [   -1     0     5 /dev/zero]
inspectiolog.sid4.bullx.3.22941:WRITE [    1000  1048.6     5.1 ] READ[ 0        0.0     0.0 ]  [   -1     1     5 /tmp/toto]
inspectiolog.sid4.bullx.3.22941:IOTIME[          1048.6     5.1 ] R/W [       1048.6     0.1 ]BW[   403.8 MB/s ]
inspectiolog.sid4.bullx.22771:SUMARY[    CALL   MBYTE      SEC           CALL MBYTE     SEC       FD OLDFD STATE NAME
inspectiolog.sid4.bullx.22771:IOTIME[             0.0     0.0 ] R/W [ 0.0     8.6    ]BW[     0.0 MB/s ]


