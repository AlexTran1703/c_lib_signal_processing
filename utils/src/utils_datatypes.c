#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "utils_datatypes.h"
#include <string.h>

/* 
    Begin
    NArray1D structure
*/

bool narray1d_datatype_itemsize(DataType type, size_t *out_item_size) {
    if (out_item_size == NULL) {
        return false;
    }

    else if (type == INT_TYPE) {
        *out_item_size = sizeof(int);
        return true;
    }

    else if (type == FLOAT_TYPE) {
        *out_item_size = sizeof(float);
        return true;
    }

    else if (type == DOUBLE_TYPE) {
        *out_item_size = sizeof(double);
        return true;
    }

    else if (type == UINT8_TYPE) {
        *out_item_size = sizeof(uint8_t);
        return true;
    }

    else {
        fprintf(stderr, "Invalid data type\n");
        return false;
    }
            
}

bool narray1d_create(NArray1D *array, DataType type, size_t size) {
    if(array == NULL) {
        fprintf(stderr, "Array is not initialized\n");
        return false;
    }

    size_t item_size;
    if (!narray1d_datatype_itemsize(type, &item_size)) {
        return false;
    }

    if (size == 0 && item_size > SIZE_MAX / item_size) {
        return false; // Prevent overflow
    }

    array->data = malloc(size * item_size);
    if (array->data == NULL && size != 0) {
        fprintf(stderr, "Invalid input parameters\n");
        return false; // Memory allocation failed
    }

    array->size = size;
    array->item_size = item_size;
    array->type = type;
    array->own_data = true; // The structure owns the data

    return true;
}

bool narray1d_free(NArray1D *array) {
    if (array == NULL) {
        fprintf(stderr, "Invalid input parameters\n");
        return false;
    }

    if (array->own_data && array->data != NULL) {
        free(array->data);
    }

    array->data = NULL;
    array->size = 0;
    array->item_size = 0;
    array->type = -1;
    array->own_data = false;

    return true;
}

void *narray1d_at(NArray1D *array, size_t index) {
    if (array == NULL || array->data == NULL) {
        fprintf(stderr, "Invalid input parameters\n");
        return NULL;
    }

    if (index >= array->size) {
        fprintf(stderr, "Index %zu out of bounds for array of size %zu\n", index, array->size);
        return NULL;
    }

    char *base = (char *)array->data;
    return base + index * array->item_size;
}

const void *narray1d_at_const(const NArray1D *array, size_t index) {
    if (array == NULL || array->data == NULL) {
        fprintf(stderr, "Invalid input parameters\n");
        return NULL;
    }

    if (index >= array->size) {
        fprintf(stderr, "Index %zu out of bounds for array of size %zu\n", index, array->size);
        return NULL;
    }

    const char *base = (const char *)array->data;
    return base + index * array->item_size;
}

bool narray1d_get_item(const NArray1D *array, size_t index, void *out_value) {
    if (array == NULL || array->data == NULL || out_value == NULL) {
        fprintf(stderr, "Invalid input parameters\n");
        return false;
    }

    if (index >= array->size) {
        fprintf(stderr, "Index %zu out of bounds for array of size %zu\n", index, array->size);
        return false;
    }

    const void *src = narray1d_at_const(array, index);
    if (src == NULL) {
        fprintf(stderr, "Failed to get array element at index %zu\n", index);
        return false;
    }

    memcpy(out_value, src, array->item_size);

    return true;
}

bool narray1d_set_item(NArray1D *array, size_t index, const void *value) {
    if (array == NULL || array->data == NULL || value == NULL) {
        fprintf(stderr, "Invalid input parameters\n");
        return false;
    }

    if (index >= array->size) {
        fprintf(stderr, "Index %zu out of bounds for array of size %zu\n", index, array->size);
        return false;
    }

    void *dst = narray1d_at(array, index);
    if (dst == NULL) {
        fprintf(stderr, "Failed to get array element at index %zu\n", index);
        return false;
    }

    memcpy(dst, value, array->item_size);

    return true;
}


/* 
    End
    NArray1D structure
*/

/*
    Begin
    NArray structure
*/
#define narray_datatype_itemsize narray1d_datatype_itemsize

bool narray_compute_strides(NArray *array) {
    if (array == NULL || array->ndim == 0 || array->ndim > NARRAY_MAX_DIMS) {
        fprintf(stderr, "invalid input parameters\n");
        return false;
    }

    array->strides[array->ndim - 1] = array->item_size;

    for (size_t d = array->ndim - 1; d > 0; --d) {
        array->strides[d - 1] = array->strides[d] * array->shape[d];
    }

    return true;
}

bool narray_create(
    NArray *array, 
    DataType type,
    size_t ndim,
    const size_t *shape    
) {
    if (array == NULL || shape == NULL || ndim == 0 || ndim > NARRAY_MAX_DIMS) {
        fprintf(stderr, "invalid input parameters\n");
        return false;
    }

    size_t item_size = 0;
    if(!narray_datatype_itemsize(type, &item_size)) {
        return false;
    }

    size_t total_item = 1;
    for (size_t d = 0; d < ndim; ++d) {
        if (shape[d] == 0 || total_item > SIZE_MAX / shape[d]) {
            fprintf(stderr, "invalid shape or overflow\n");
            return false;
        }
        total_item *= shape[d];
    }

    if (total_item > SIZE_MAX / item_size) {
        fprintf(stderr, "overflow in total size calculation\n");
        return false;
    }

    array->data = malloc(total_item * item_size);
    if (array->data == NULL && total_item != 0) {
        fprintf(stderr, "memory allocation failed\n");
        return false;
    }
    memset(array->data, 0, total_item * item_size);

    array->type = type;
    array->ndim = ndim;
    array->item_size = item_size;
    array->total_items = total_item;
    memcpy(array->shape, shape, ndim * sizeof(size_t));

    array->own_data = true;

    bool strides_computed = narray_compute_strides(array);
    if (!strides_computed) {
        fprintf(stderr, "failed to compute strides\n");
        free(array->data);
        array->data = NULL;
        return false;
    }

    return true;
}

void narray_free(NArray *array) {
    if (array == NULL) {
        return;
    }

    if (array->own_data) {
        free(array->data);
    }

    array->data = NULL;
    array->ndim = 0;
    array->item_size = 0;
    array->total_items = 0;
    array->own_data = false;
}

bool narray_offset
(
    const NArray *array,
    const size_t *indices,
    size_t *out_offset
) {
    if (array == NULL || indices == NULL || out_offset == NULL) {
        fprintf(stderr, "[narray_offset] Invalid input parameters\n");
        return false;
    }
    if (array->ndim == 0 || array->ndim > NARRAY_MAX_DIMS || array->strides == NULL || array->shape == NULL) {
        fprintf(stderr, "[narray_offset] Array properties are invalid, Initialize the array first\n");
        return false;
    }

    for(size_t d = 0; d < array->ndim; ++d) {
        if(indices[d] >= array->shape[d]) {
            fprintf(stderr, "[narray_offset] Index %zu out of bounds for dimension %zu with size %zu\n", indices[d], d, array->shape[d]);
            return false;
        }
        *out_offset += indices[d] * array->strides[d];
    }
    return true;
}

bool narray_set_item
(
    const NArray *array,
    const size_t *indices,
    const void *value
) {
    if (array == NULL || indices == NULL || value == NULL) {
        fprintf(stderr, "[narray_set_item] Invalid input parameters\n");
        return false;
    }

    size_t offset = 0;

    if(!narray_offset(array, indices, &offset)) {
        fprintf(stderr, "[narray_set_item] Failed to compute offset\n");
        return false;
    }

    char *base = (char *) array->data;
    memcpy(base + offset, value, array->item_size);
    return true;
}

bool narray_get_item
(
    const NArray *array,
    const size_t *indices,
    void *out_value
) {
    if (array == NULL || indices == NULL || out_value == NULL) {
        fprintf(stderr, "[narray_get_item] Invalid input parameters\n");
        return false;
    }

    size_t offset = 0;

    if(!narray_offset(array, indices, &offset)) {
        fprintf(stderr, "[narray_get_item] Failed to compute offset\n");
        return false;
    }

    const char *base = (const char *) array->data;
    memcpy(out_value, base + offset, array->item_size);
    return true;
}

const char *datatype_name(DataType type) {
    switch (type) {
        case INT_TYPE:
            return "int";

        case FLOAT_TYPE:
            return "float";

        case DOUBLE_TYPE:
            return "double";

        case UINT8_TYPE:
            return "uint8";

        default:
            return "unknown";
    }
}

bool narray_print_scalar(FILE *out, DataType type, const void *value) {
    if (out == NULL || value == NULL) {
        return false;
    }

    else if (type == UINT8_TYPE) {
        fprintf(out, "%u", (unsigned)*(const uint8_t *)value);
        return true;
    }
    else if (type == INT_TYPE) {
        fprintf(out, "%d", *(const int *)value);
        return true;
    }
    else if (type == FLOAT_TYPE) {
        fprintf(out, "%g", *(const float *)value);
        return true;
    }
    else if (type == DOUBLE_TYPE) {
        fprintf(out, "%g", *(const double *)value);
        return true;
    }

    else {
        fprintf(out, "<unsupported>");
        return false;
    }
}

static bool narray_print_recursive(
    FILE *out,
    const NArray *array,
    size_t dim,
    size_t byte_offset
) {
    if (out == NULL || array == NULL || array->data == NULL) {
        return false;
    }

    if (dim == array->ndim) {
        const char *base = (const char *)array->data;
        const void *value = base + byte_offset;

        return narray_print_scalar(out, array->type, value);
    }

    fprintf(out, "[");

    for (size_t i = 0; i < array->shape[dim]; i++) {
        size_t next_offset = byte_offset + i * array->strides[dim];

        if (!narray_print_recursive(out, array, dim + 1, next_offset)) {
            return false;
        }

        if (i + 1 < array->shape[dim]) {
            fprintf(out, ", ");
        }
    }

    fprintf(out, "]");

    return true;
}

bool narray_print_data(const NArray *array) {
    if (array == NULL) {
        fprintf(stderr, "[narray_print_data] array is NULL\n");
        return false;
    }

    if (array->data == NULL) {
        printf("Data: NULL\n");
        return true;
    }

    printf("Data: ");

    if (!narray_print_recursive(stdout, array, 0, 0)) {
        printf("\n");
        return false;
    }

    printf("\n");

    return true;
}


bool narray_print_info(const NArray *array) {
    if (array == NULL) {
        fprintf(stderr, "[narray_print_info] array is NULL\n");
        return false;
    }

    printf("NArray Info:\n");
    printf("  Data Type: %s (%d)\n", datatype_name(array->type), array->type);
    printf("  Number of Dimensions: %zu\n", array->ndim);
    printf("  Item Size: %zu bytes\n", array->item_size);
    printf("  Total Items: %zu\n", array->total_items);
    printf("  Owns Data: %s\n", array->own_data ? "Yes" : "No");
    printf("  Data Pointer: %p\n", array->data);

    printf("  Shape: [");
    for (size_t d = 0; d < array->ndim; d++) {
        printf("%zu", array->shape[d]);

        if (d + 1 < array->ndim) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("  Strides: [");
    for (size_t d = 0; d < array->ndim; d++) {
        printf("%zu", array->strides[d]);

        if (d + 1 < array->ndim) {
            printf(", ");
        }
    }
    printf("]\n");

    narray_print_data(array);

    return true;
}


// Narray array;
// size_t shape[2] = {2, 3};
// narray_create(&array, DOUBLE_TYPE, _NARRAY_GET_SHAPE_DIMENSION(shape), shape);
// narray_set_item(&array, (size_t[]) {0, 0}, &(double){2.0});

