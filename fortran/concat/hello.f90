program hello
   character(len=64) :: str1
   character(len=64) :: str2
   integer                a
   a=10
   write(str2,'(a,i0)') "machaine",a
   !str2=trim(str2)
   Print *, "Hello World! " 
   Print *,str2
end program Hello
