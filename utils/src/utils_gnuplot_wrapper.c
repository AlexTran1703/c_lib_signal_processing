#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils_gnuplot_wrapper.h"
#include "utils_env.h"

FILE* utils_gnuplot_open_pipe(void) {
    FILE* gnuplotPipe = POPEN("gnuplot -persistent", "w");
    if (gnuplotPipe == NULL) {
        fprintf(stderr, "Failed to create gnuplot pipe. Ensure that gnuplot is installed and available in your PATH.\n");
        return NULL;
    }
    return gnuplotPipe;
}

bool utils_gnuplot_set_title(FILE *gnuplotPipe, const char *title) {
    if (gnuplotPipe == NULL || title == NULL) {
        fprintf(stderr, "Invalid input parameters for setting title\n");
        return false;
    }
    fprintf(gnuplotPipe, "set title '%s'\n", title);
    return true;
}

bool utils_gnuplot_set_axis(FILE *gnuplotPipe, const char *xaxis, const char *yaxis) {
    if (gnuplotPipe == NULL || xaxis == NULL || yaxis == NULL) {
        fprintf(stderr, "Invalid input parameters for setting axis\n");
        return false;
    }
    fprintf(gnuplotPipe, "set xlabel '%s'\n", xaxis);
    fprintf(gnuplotPipe, "set ylabel '%s'\n", yaxis);
    return true;
}

bool utils_gnuplot_plot_timeseries(FILE *gnuplotPipe, const long *time, const double *data, size_t size, const char *title) {
    if (gnuplotPipe == NULL || data == NULL || size == 0 || title == NULL) {
        fprintf(stderr, "Invalid input parameters for plotting time series\n");
        return false;
    }

    fprintf(gnuplotPipe, "plot '-' with linespoints title '%s'\n", title);
    for (size_t i = 0; i < size; i++) {
        fprintf(gnuplotPipe, "%ld %lf\n", (long)time[i], data[i]);
    }
    fflush(gnuplotPipe);
    return true;
}

bool utils_gnuplot_close_pipe(FILE *gnuplotPipe) {
    if (gnuplotPipe == NULL) {
        fprintf(stderr, "Invalid gnuplot pipe. Cannot close a NULL pipe.\n");
        return false;
    }
    fprintf(gnuplotPipe, "e\n");
        fflush(gnuplotPipe);
    int result = PCLOSE(gnuplotPipe);
    if (result == -1) {
        fprintf(stderr, "Failed to close gnuplot pipe.\n");
        return false;
    }
    return true;
}


