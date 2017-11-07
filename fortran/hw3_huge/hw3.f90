program testingInt
implicit none

   !two byte integer
   integer(kind=2) :: shortval
   
   !four byte integer
   integer(kind=4) :: longval
   
   !eight byte integer
   integer(kind=8) :: verylongval
   
   !sixteen byte integer
   integer(kind=16) :: veryverylongval
   
   !default integer 
   integer :: defval
        
   print *, huge(shortval)
   print *, huge(longval)
   print *, huge(verylongval)
   print *, huge(veryverylongval)
   print *, huge(defval)
   
end program testingInt
