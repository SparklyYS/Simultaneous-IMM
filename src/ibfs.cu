#include "ibfs.h"
#include <cstdio>
#include <cstring>


__global__ void hello()
{
    printf("hello world\n");
}


void ibfs()
{
    hello<<<1, 10>>>();
}

