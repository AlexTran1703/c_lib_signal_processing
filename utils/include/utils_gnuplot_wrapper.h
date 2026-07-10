#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils_env.h"

FILE* utils_gnuplot_open_pipe();

bool utils_gnuplot_set_title(FILE *gnuplotPipe, const char *title);

bool utils_gnuplot_set_axis(FILE *gnuplotPipe, const char *xaxis, const char *yaxis);

bool utils_gnuplot_plot_timeseries(FILE *gnuplotPipe, const long *time, const double *data, size_t size, const char *title);

bool utils_gnuplot_close_pipe(FILE *gnuplotPipe);


