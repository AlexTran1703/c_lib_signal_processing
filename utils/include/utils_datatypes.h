#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    UINT8_TYPE,
    INT_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
} DataType;

typedef enum {
    NARRAY_ORDER_C,
    NARRAY_ORDER_F,
    NARRAY_ORDER_KEEP,
    NARRAY_ORDER_ANY
} NArrayOrder;

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

    void *base;

    DataType type;

    size_t ndim;
    size_t item_size;

    size_t shape[NARRAY_MAX_DIMS];

     /*
        Byte strides.

        ptrdiff_t allows:
        - positive strides
        - zero strides for broadcasting
        - negative strides for reversed views
    */

    ptrdiff_t strides[NARRAY_MAX_DIMS]; 

    size_t total_items;
    size_t nbytes;

    bool own_data;
    bool is_view;

} NArray;


bool narray_compute_strides(NArray *array);

bool narray_compute_c_strides(NArray *array);

bool narray_compute_f_strides(NArray *array);

bool narray_is_c_contiguous(const NArray *array);

bool narray_is_f_contiguous(const NArray *array);

bool narray_is_contiguous(const NArray *array);

void *narray_at(NArray *array, const size_t *indices);

const void *narray_at_const(const NArray *array, const size_t *indices);

bool narray_create(NArray *array, DataType type,size_t ndim,const size_t *shape, NArrayOrder order);

void narray_free(NArray *array);

bool narray_set_item(NArray *array, const size_t *indices, const void *value);

bool narray_get_item(const NArray *array, const size_t *indices, void *out_value);

bool narray_print_info(const NArray * array);

bool narray_print_data(const NArray *array);

bool narray_reshape_view(const NArray *src, NArray *view, size_t new_ndim, const size_t *new_shape, NArrayOrder order);
/* 
    End
    NArray structure
*/