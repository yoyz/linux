program hello2
   integer(kind=8) :: a
   integer(kind=8) :: b
   integer(kind=8) :: c
   integer(kind=8) :: n
   integer,dimension(1:10,1:10)::tableau
   open (unit = 8, file = "divisors", access="stream",form="unformatted")
   !print *,"bla"
   c=0
   n=10
   do a = 1,n
     do b = 1, n
       c=c+1
       !print *,"val:",tableau(a,b)
       read(8) tableau(a,b)
     enddo
   enddo 
   close (8)

   do a = 1,n
      do b = 1, n
       
       print *,"val:",tableau(a,b)
      enddo
   enddo

end program hello2
