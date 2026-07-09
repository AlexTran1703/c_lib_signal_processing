#include <stdio.h>
#include <stdlib.h>
#include "utils_env.h"
#include "utils_datatypes.h"

int main(int argc, char *argv[]) {
    // Data arrays
    size_t size = 4;
    NArray1D array = create_narray1d( (DataType) DOUBLE_TYPE, (size_t) size);
    array.data.doubleValue[0] = 1.0;
    array.data.doubleValue[1] = 5.0;
    array.data.doubleValue[2] = 3.0;
    array.data.doubleValue[3] = -4.0;
    int n = 4;

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
        for (int i = 0; i < n; i++) {
            fprintf(gnuplotPipe, "%ld %lf\n", i, array.data.doubleValue[i]);
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