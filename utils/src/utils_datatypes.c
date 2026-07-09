#include <stdio.h>
#include <stdlib.h>
#include "utils_datatypes.h"

NArray1D create_narray1d(DataType type, size_t size) {
    NArray1D array;
    array.type = type;
    array.size = size;

    switch (type) {
        case INT_TYPE:
            array.data.intValue = (int *)malloc(size * sizeof(int));
            break;
        case FLOAT_TYPE:
            array.data.floatValue = (float *)malloc(size * sizeof(float));
            break;
        case DOUBLE_TYPE:
            array.data.doubleValue = (double *)malloc(size * sizeof(double));
            break;
        default:
            fprintf(stderr, "Unsupported data type\n");
            exit(EXIT_FAILURE);
    }

    return array;
}

void free_narray1d(NArray1D *array) {
    if (array == NULL) return;

    switch (array->type) {
        case INT_TYPE:
            free(array->data.intValue);
            break;
        case FLOAT_TYPE:
            free(array->data.floatValue);
            break;
        case DOUBLE_TYPE:
            free(array->data.doubleValue);
            break;
        default:
            fprintf(stderr, "Unsupported data type\n");
            exit(EXIT_FAILURE);
    }

    array->size = 0;
}

void get_single_narray1d(NArray1D *array, size_t index, void *out_value) {
    if (array == NULL || out_value == NULL || index >= array->size) {
        fprintf(stderr, "Invalid parameters for array_get\n");
        exit(EXIT_FAILURE);
    }

    switch (array->type) {
        case INT_TYPE:
            *(int *)out_value = array->data.intValue[index];
            break;
        case FLOAT_TYPE:
            *(float *)out_value = array->data.floatValue[index];
            break;
        case DOUBLE_TYPE:
            *(double *)out_value = array->data.doubleValue[index];
            break;
        default:
            fprintf(stderr, "Unsupported data type\n");
            exit(EXIT_FAILURE);
    }
}

void set_single_narray1d(NArray1D *array, size_t index, void *in_value) {
    if (array == NULL || in_value == NULL || index >= array->size) {
        fprintf(stderr, "Invalid parameters for array_set\n");
        exit(EXIT_FAILURE);
    }

    switch (array->type) {
        case INT_TYPE:
            array->data.intValue[index] = *(int *)in_value ;
            break;
        case FLOAT_TYPE:
            array->data.floatValue[index] = *(float *)in_value ;
            break;
        case DOUBLE_TYPE:
            array->data.doubleValue[index] = *(double *)in_value;
            break;
        default:
            fprintf(stderr, "Unsupported data type\n");
            exit(EXIT_FAILURE);
    }
}




