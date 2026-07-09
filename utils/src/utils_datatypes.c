#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "utils_datatypes.h"
#include <string.h>

bool narray1d_datatype_itemsize(DataType type, size_t *out_item_size) {
    if (out_item_size == NULL) {
        return false;
    }

    switch (type) {
        case INT_TYPE:
            *out_item_size = sizeof(int);
            return true;

        case FLOAT_TYPE:
            *out_item_size = sizeof(float);
            return true;

        case DOUBLE_TYPE:
            *out_item_size = sizeof(double);
            return true;

        case UINT8_TYPE:
            *out_item_size = sizeof(uint8_t);
            return true;

        default:
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



