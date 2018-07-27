PROGRAM TEST

  IMPLICIT NONE

  INTEGER, PARAMETER :: FILE_SIZE_MB = 145000
!!  INTEGER, PARAMETER :: FILE_SIZE_MB = 520
  INTEGER, PARAMETER :: NZ = 520
  INTEGER, PARAMETER :: UNIT = 77
  INTEGER, PARAMETER :: SIZE_OF_FLOAT = 4
  INTEGER, PARAMETER :: NUMREC = (FILE_SIZE_MB/NZ)*(1024*1024/SIZE_OF_FLOAT)
  CHARACTER(LEN=*), PARAMETER :: FNAME = 'TEST.BIN'
  CHARACTER(LEN=256) :: argv

  REAL :: seismicTrace(NZ)

  INTEGER :: irec, iol

  INQUIRE(IOLENGTH=iol) seismicTrace

  IF (iargc() .NE. 1) STOP "USE fortran_IO [read or write]"

  CALL GETARG(1,argv)

  IF ((argv(1:1) .NE. 'r').AND.(argv(1:1) .NE. 'R'))THEN

    WRITE(*,"(A)") 'WRITE TEST.'
    OPEN(UNIT, FILE=FNAME, ACCESS='DIRECT', STATUS="REPLACE", ACTION='WRITE', blocksize=1048576, buffered='yes', buffercount=1, RECL=iol)
    DO irec=1, NUMREC
      seismicTrace = irec
      WRITE(UNIT, REC=irec)seismicTrace
    ENDDO

  ELSE

    WRITE(*,"(A)") 'READ TEST.'
    OPEN(UNIT, FILE=FNAME, ACCESS='DIRECT', ACTION='READ', blocksize=1048576, buffered='yes', buffercount=1, RECL=iol)
    DO irec=1, NUMREC
      READ(UNIT, REC=irec)seismicTrace
    ENDDO

  ENDIF

  CLOSE(UNIT)
  WRITE(*,"(A)") 'DONE.'

END PROGRAM
