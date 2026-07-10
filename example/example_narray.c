#include <stdio.h>
#include <stdlib.h>
#include "utils_env.h"
#include "utils_datatypes.h"
#include "utils_gnuplot_wrapper.h"

int main(int argc, char *argv[]) {
    // Data arrays
    size_t shape[] = {6};
    long time[] = {0, 1, 2, 3, 4, 5};
    NArray *array = malloc(sizeof(NArray));
    narray_create(array, DOUBLE_TYPE, _NARRAY_GET_SHAPE_DIMENSION(shape), shape);
    narray_set_item(array, (size_t[]){1}, &(double){1.0});
    narray_set_item(array, (size_t[]){2}, &(double){5.0});
    narray_set_item(array, (size_t[]){3}, &(double){3.0});
    narray_set_item(array, (size_t[]){5}, &(double){-1.0});
    narray_print_info(array);
    // Open a persistent pipeline to Gnuplot
    FILE *gnuplotPipe = utils_gnuplot_open_pipe();
    utils_gnuplot_set_title(gnuplotPipe, "NArray Plot");
    utils_gnuplot_set_axis(gnuplotPipe, "X-axis", "Y-value");

    // Plot the data
    utils_gnuplot_plot_timeseries(gnuplotPipe, time, array->data, shape[0], "NArray Data");
    
    utils_gnuplot_close_pipe(gnuplotPipe);
    return 0;
}