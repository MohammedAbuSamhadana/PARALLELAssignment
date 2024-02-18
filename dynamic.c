#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_ITERATIONS 1000


struct complex {
    double real;
    double imag;
};

int mandelbrot_set(struct complex c) {
    struct complex z = { 0.0, 0.0 };
    int i;
    for (i = 0; i < MAX_ITERATIONS; i++) {
        double real_temp = z.real * z.real - z.imag * z.imag + c.real;
        double imag_temp = 2.0 * z.real * z.imag + c.imag;
        z.real = real_temp;
        z.imag = imag_temp;
        if (z.real * z.real + z.imag * z.imag > 4.0) {
            break;
        }
    }
    return i;
}

void write_ppm(char *filename, int *data, int width, int height) {
    FILE *fp = fopen(filename, "wb");
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    int x, y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int value = data[y * width + x];
            unsigned char r, g, b;
            if (value == MAX_ITERATIONS) {
                r = g = b = 0;
            } else {
                r = (value * 7) % 256;
                g = (value * 5) % 256;
                b = (value * 3) % 256;
            }
            fputc(r, fp);
            fputc(g, fp);
            fputc(b, fp);
        }
    }
    fclose(fp);
}

void main() {
    int rank, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int work_chunk_size = HEIGHT / size;
    int *data = malloc(WIDTH * HEIGHT * sizeof(int));
    memset(data, 0, WIDTH * HEIGHT * sizeof(int));
    int *task_buffer = malloc(WIDTH * sizeof(int));
    int *result_buffer = malloc(WIDTH * sizeof(int));
    double start_time = MPI_Wtime();
    if (rank == 0) {
        int y = 0;
        while (y < HEIGHT) {
            int worker;
            MPI_Recv(&worker, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&y, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
            y += work_chunk_size;
        }
        for (int worker = 1; worker < size; worker++) {
            MPI_Recv(&result_buffer[0], WIDTH, MPI_INT, worker, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int y = task_buffer[0];
            int count = task_buffer[1];
            memcpy(&data[y * WIDTH], result_buffer, WIDTH * count * sizeof(int));
        }
        double end_time = MPI_Wtime();
        printf("Execution time: %.2f seconds\n", end_time - start_time);
        char *filename = "mandelbrot2.ppm";
        write_ppm(filename, data, WIDTH, HEIGHT);

    }
    free(data);
    free(task_buffer);
    free(result_buffer);
    MPI_Finalize();
}
