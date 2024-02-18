#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_ITERATIONS 1000

struct complex {
    double re;
    double im;
};

int mandelbrot_set(struct complex c) {
    struct complex z = { 0.0, 0.0 };
    int i;
    for (i = 0; i < MAX_ITERATIONS; i++) {
        double real_temp = z.re * z.re - z.im * z.im + c.re;
        double imag_temp = 2.0 * z.re * z.im + c.im;
        z.re = real_temp;
        z.im = imag_temp;
        if (z.re * z.re + z.im * z.im > 4.0) {
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
    int chunk_size = HEIGHT / size;
    int *data = malloc(WIDTH * HEIGHT * sizeof(int));
    memset(data, 0, WIDTH * HEIGHT * sizeof(int));
    double start_time = MPI_Wtime();
    int x, y;
    for (y = rank * chunk_size; y < (rank + 1) * chunk_size; y++) {
        for (x = 0; x < WIDTH; x++) {
            double real = -2.0 + 3.0 * (double) x / (double) WIDTH;
            double imag = -1.5 + 3.0 * (double) y / (double) HEIGHT;
            struct complex c = { real, imag };
            int value = mandelbrot_set(c);
            data[y * WIDTH + x] = value;
        }
    }
    MPI_Gather(data + rank * chunk_size * WIDTH, chunk_size * WIDTH, MPI_INT,
               data, chunk_size * WIDTH, MPI_INT, 0, MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    if (rank == 0) {
        printf("Execution time: %.2f seconds\n", end_time - start_time);
        char *filename = "mandelbrot.ppm";
        write_ppm(filename, data, WIDTH, HEIGHT);
        printf("Image saved to %s\n", filename);
    }
    free(data);
    MPI_Finalize();
}#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_ITERATIONS 1000

struct complex {
    double re;
    double im;
};

int mandelbrot_set(struct complex c) {
    struct complex z = { 0.0, 0.0 };
    int i;
    for (i = 0; i < MAX_ITERATIONS; i++) {
        double real_temp = z.re * z.re - z.im * z.im + c.re;
        double imag_temp = 2.0 * z.re * z.im + c.im;
        z.re = real_temp;
        z.im = imag_temp;
        if (z.re * z.re + z.im * z.im > 4.0) {
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
    int chunk_size = HEIGHT / size;
    int *data = malloc(WIDTH * HEIGHT * sizeof(int));
    memset(data, 0, WIDTH * HEIGHT * sizeof(int));
    double start_time = MPI_Wtime();
    int x, y;
    for (y = rank * chunk_size; y < (rank + 1) * chunk_size; y++) {
        for (x = 0; x < WIDTH; x++) {
            double real = -2.0 + 3.0 * (double) x / (double) WIDTH;
            double imag = -1.5 + 3.0 * (double) y / (double) HEIGHT;
            struct complex c = { real, imag };
            int value = mandelbrot_set(c);
            data[y * WIDTH + x] = value;
        }
    }
    MPI_Gather(data + rank * chunk_size * WIDTH, chunk_size * WIDTH, MPI_INT,
               data, chunk_size * WIDTH, MPI_INT, 0, MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    if (rank == 0) {
        printf("Execution time: %.2f seconds\n", end_time - start_time);
        char *filename = "mandelbrot.ppm";
        write_ppm(filename, data, WIDTH, HEIGHT);
        printf("Image saved to %s\n", filename);
    }
    free(data);
    MPI_Finalize();
}
