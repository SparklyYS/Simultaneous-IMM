#ifndef COMM_H
#define COMM_H
#include <stdio.h>

static void HandleError(cudaError_t err, const char *file, int line)
{
    if (err != cudaSuccess)
    {
        printf("%s in %s at line %d\n", cudaGetErrorString(err), file, line);
        exit(EXIT_FAILURE);
    }
}

// Error Handler macro
#define H_ERR(err) (HandleError(err, __FILE__, __LINE__))


#define THDS_NUM 256
#define BLKS_NUM 256
#define OMP_THDS 24

// Error Handler macro
#define H_ERR(err) (HandleError(err, __FILE__, __LINE__))


#define THDS_NUM 256
#define BLKS_NUM 256
#define OMP_THDS 24

// Error Handler macro
#define H_ERR(err) (HandleError(err, __FILE__, __LINE__))


#define THDS_NUM 256
#define BLKS_NUM 256
#define OMP_THDS 24

enum ex_q_t
{
    SML_Q,
    MID_Q,
    LRG_Q,
    NONE
};


typedef int index_t;
typedef int vertex_t;
typedef unsigned char depth_t;
typedef uint4 comp_t;


#endif