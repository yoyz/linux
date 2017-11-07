program hello
   integer(kind=8) :: a
   integer(kind=8) :: b
   integer(kind=8) :: c
   integer(kind=8) :: n
   integer,dimension(1:10,1:10)::tableau
   n=10
   do a = 1,n
     do b = 1, n
       tableau(a,b)=b+a*10
     enddo
   enddo
   open (unit = 8, file = "divisors", access="stream",form="unformatted")

   c=0
   do a = 1,n
     do b = 1, n
       c=c+1
       print *,tableau(a,b)
       write(8)tableau(a,b)
     enddo
   enddo 
   close (1)

end program Hello
