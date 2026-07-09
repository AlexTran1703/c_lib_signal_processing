#include <stdio.h>

typedef enum {
    INT_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
} DataType;

typedef union {
    int *intValue;
    float *floatValue;
    double *doubleValue;
} DataValue;

typedef struct {
    DataType type;
    DataValue data;
    size_t size;
} NArray1D;

NArray1D create_narray1d(DataType type, size_t size);
void free_narray1d(NArray1D *array);
void get_single_narray1d(NArray1D *array, size_t index, void *out_value);
void set_single_narray1d(NArray1D *array, size_t index, void *in_value);
