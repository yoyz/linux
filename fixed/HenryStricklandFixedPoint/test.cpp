/*
Copyright (c) 2006 Henry Strickland & Ryan Seto

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

        (* http://www.opensource.org/licenses/mit-license.php *)
*/



#include <stdio.h>
#include <assert.h>

#include "fixed.h"

main()
{
	Fixed a1;
	Fixed a2= 3.1415;
	Fixed a3= a1+a2;

	assert( a3 < (float)3.1416 );
	assert( a3 > (float)3.1414 );
	assert( a3 <= (float)3.1416 );
	assert( a3 >= (float)3.1414 );
	assert( a3 == (float)3.1415 );
	assert( a3 != (float)3.1414 );

	assert( (float)3.1416 > a3 );
	assert( (float)3.1414 < a3 );
	assert( (float)3.1416 >= a3 );
	assert( (float)3.1414 <= a3 );
	assert( (float)3.1415 == a3 );
	assert( (float)3.1414 != a3 );


	assert( a3 < 3.1416 );
	assert( a3 > 3.1414 );
	assert( a3 <= 3.1416 );
	assert( a3 >= 3.1414 );
	assert( a3 == 3.1415 );
	assert( a3 != 3.1414 );

	assert( 3.1416 > a3 );
	assert( 3.1414 < a3 );
	assert( 3.1416 >= a3 );
	assert( 3.1414 <= a3 );
	assert( 3.1415 == a3 );
	assert( 3.1414 != a3 );

	Fixed a4= 3.1414;
	Fixed a6= 3.1416;

	assert( a3 < a6 );
	assert( a3 > a4 );
	assert( a3 <= a6 );
	assert( a3 >= a4 );
	assert( a3 == a2 );
	assert( a3 != a4 );

	assert( a6 > a3 );
	assert( a4 < a3 );
	assert( a6 >= a3 );
	assert( a4 <= a3 );
	assert( a2 == a3 );
	assert( a4 != a3 );


	Fixed one= 1.0;
	Fixed two= 2.0;
	Fixed minus10= -10.0;

	assert( one + two == 3.0 );
	assert( two + one == 3.0 );
	assert( two - one == 1.0 );
	assert( two * one == 2.0 );
	assert( two / one == 2.0 );

	assert( one + minus10 == -9.0 );
	assert( minus10 + one == -9.0 );
	assert( minus10 - one == -11.0 );
	assert( minus10 * one == -10.0 );
	assert( minus10 / one == -10.0 );

	assert( two + minus10 == -8.0 );
	assert( minus10 + two == -8.0 );
	assert( minus10 - two == -12.0 );
	assert( minus10 * two == -20.0 );
	assert( minus10 / two == -5.0 );

	assert( 3.0 == (one += two) );
	assert( 8.0 == (one -= -5) );
	assert( 16.0 == (one *= two) );
	assert( 4.0 == (one /= 4) );

	Fixed SPone= (float)1.0;
	Fixed SPtwo= (float)2.0;
	Fixed SPminus10= (float)-10.0;

	assert( SPone + SPtwo == (float)3.0 );
	assert( SPtwo + SPone == (float)3.0 );
	assert( SPtwo - SPone == (float)1.0 );
	assert( SPtwo * SPone == (float)2.0 );
	assert( SPtwo / SPone == (float)2.0 );

	assert( SPone + SPminus10 == (float)-9.0 );
	assert( SPminus10 + SPone == (float)-9.0 );
	assert( SPminus10 - SPone == (float)-11.0 );
	assert( SPminus10 * SPone == (float)-10.0 );
	assert( SPminus10 / SPone == (float)-10.0 );

	assert( SPtwo + SPminus10 == (float)-8.0 );
	assert( SPminus10 + SPtwo == (float)-8.0 );
	assert( SPminus10 - SPtwo == (float)-12.0 );
	assert( SPminus10 * SPtwo == (float)-20.0 );
	assert( SPminus10 / SPtwo == (float)-5.0 );

	assert( 3.0 == (SPone += SPtwo) );
	assert( 8.0 == (SPone -= -5) );
	assert( 16.0 == (SPone *= SPtwo) );
	assert( 4.0 == (SPone /= 4) );

	return 0;
}
