program hello
   integer(kind=8) :: a
   integer(kind=8) :: b
   integer(kind=8) :: c
   integer(kind=8) :: n
   integer,dimension(1:10,1:10)::tableau
   n=10
   do a = 1,n
     do b = 1, n
       tableau(a,b)=a+b
     Enddo
   Enddo
   open (unit = 1, file = "divisors")

   write (1,*) "Here are the divisors of ", n, " :"
   do a = 1,n
     do b = 1, n
       write (1,*) " ",tableau(a,b)
     Enddo
   enddo 
   close (1)

end program Hello
