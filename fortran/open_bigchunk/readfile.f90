program hello
   integer(kind=8) :: a
   integer(kind=8) :: b
   integer(kind=8) :: c
   integer(kind=8) :: n
   integer,dimension(1:1024,1:1024)::tableau
   n=1024
   open (unit = 8, file = "divisors", access="direct",recl=n*n*4)
   read (8,rec=1) tableau
   close (8)

   do a = 1,n
     do b = 1, n
       print *,tableau(b,a)
     enddo
   enddo


end program Hello
