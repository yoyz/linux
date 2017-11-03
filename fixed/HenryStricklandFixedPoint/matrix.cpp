#include "fixed.h"

TYPE m[M][M];


main()
{
  for (int x=0; x<M; x++) {
    for (int y=0; y<M; y++) {
	m[x][y]= TYPE(3.1415) * TYPE(x) + TYPE(y);
    }
  }

  for (int x=0; x<M; x++) {
    for (int y=0; y<M; y++) {
	m[x][y]= (m[x][y] +TYPE(y)) * (m[y][x] + TYPE(x));
    }
  }

  for (int x=0; x<M; x++) {
    for (int y=0; y<M; y++) {
	m[x][y]= (m[x][y]) + (m[y][x]);
    }
  }

  return 0;
}
