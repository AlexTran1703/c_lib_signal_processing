#include <stdio.h>
#include <stdlib.h>
#include "utils_env.h"

int main(int argc, char *argv[]) {
    // Data arrays
    double x[] = {1, 2, 3, 4};
    double y[] = {1, 4, 9, 16};
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
            fprintf(gnuplotPipe, "%lf %lf\n", x[i], y[i]);
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