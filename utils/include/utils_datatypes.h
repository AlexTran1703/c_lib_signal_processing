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

// typedef struct {
//     DataType type;
//     DataValue data;
//     size_t size;
// } NArray1D;

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
