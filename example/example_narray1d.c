#include <stdio.h>
#include <stdlib.h>
#include "utils_env.h"
#include "utils_datatypes.h"

int main(int argc, char *argv[]) {
    // Data arrays
    size_t size = 4;
    NArray1D *array = malloc(sizeof(NArray1D));
    narray1d_create(array, DOUBLE_TYPE, size);
    narray1d_set_item(array, 0, &(double){1.0});
    narray1d_set_item(array, 1, &(double){5.0});
    narray1d_set_item(array, 2, &(double){3.0});
    narray1d_set_item(array, 3, &(double){-1.0});

    // Open a persistent pipeline to Gnuplot
    FILE *gnuplotPipe = POPEN("gnuplot -persistent", "w");
    
    if (gnuplotPipe) {
        // Send styling and configuration commands
        fprintf(gnuplotPipe, "set title 'My Pure C Plot'\n");
        fprintf(gnuplotPipe, "set xlabel 'X Axis'\n");
        fprintf(gnuplotPipe, "set ylabel 'Y Axis'\n");
        
        // Instruct gnuplot to read data inline ('-') using linespoints
        fprintf(gnuplotPipe, "plot '-' with linespoints title 'Data'\n");
        
        // Stream the coordinates to the pipe
        for (int i = 0; i < size; i++) {
            double value;
            narray1d_get_item(array, (size_t) i, &value);
            fprintf(gnuplotPipe, "%ld %lf\n", i, (double) value);
        }
        
        // Send the termination character 'e' to signify end of data
        fprintf(gnuplotPipe, "e\n");
        
        // Flush and close the stream
        fflush(gnuplotPipe);
        PCLOSE(gnuplotPipe);
    } else {
        printf("Gnuplot not found. Please install it on your system.\n");
    }

    return 0;
}