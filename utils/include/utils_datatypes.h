#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    UINT8_TYPE,
    INT_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
} DataType;

typedef union {
    int *intValue;
    float *floatValue;
    double *doubleValue;
} DataValue;


/* 
    Begin
    NArray1D structure
*/
typedef struct {
    void *data;
    size_t size;
    size_t item_size;
    DataType type;
    bool own_data; // Flag to indicate if the structure owns the data
} NArray1D;

bool narray1d_datatype_itemsize(DataType type, size_t *out_item_size);
bool narray1d_create(NArray1D *array, DataType type, size_t size);
bool narray1d_free(NArray1D *array);
void *narray1d_at(NArray1D *array, size_t index);
const void *narray1d_at_const(const NArray1D *array, size_t index);
bool narray1d_get_item(const NArray1D *array, size_t index, void *out_value);
bool narray1d_set_item(NArray1D *array, size_t index, const void *value);

/*
    End
    NArray1D structure
*/


/* 
    Begin
    NArray structure
*/
#define NARRAY_MAX_DIMS 16
#define _NARRAY_GET_SHAPE_DIMENSION(x) (sizeof(x) / sizeof((x)[0])) // Macro to calculate the number of dimensions in a shape array

// narray
// np.a = [2.0, 4.0, 4.0;
//          2.0, 4.0, 4.0]
// np.a.shape = [2, 3], ndim = 2, item_size = sizeof(double), strides = [3 * sizeof(double), sizeof(double)]
typedef struct {
    void *data;
    DataType type;

    size_t ndim;
    size_t item_size;

    size_t shape[NARRAY_MAX_DIMS];
    size_t strides[NARRAY_MAX_DIMS];
    
    size_t total_items;
    bool own_data;
} NArray;


bool narray_compute_strides(NArray *array);

bool narray_create(NArray *array, DataType type,size_t ndim,const size_t *shape);

void narray_free(NArray *array);

bool narray_offset(const NArray *array,const size_t *indices, size_t *out_offset);

bool narray_set_item(const NArray *array, const size_t *indices, const void *value);

bool narray_get_item(const NArray *array, const size_t *indices, void *out_value);

bool narray_print_info(const NArray * array);
/* 
    End
    NArray structure
*/