#include "executor.h"

int main(int argc, char**argv) {
    time_t          start,
                    end;
    transform_t     *buffer;
    int             num_procs,
                    rank;

    // Instantiate buffer
    if(!(buffer = (transform_t *) malloc(sizeof(transform_t) * BUFFER_SIZE))) {
        fprintf(stderr, "Buffer failed to allocated.\n");
        exit(EXIT_FAILURE);
    }

    // Read items
    start = time(NULL);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Main flow

    // Report timing.
    end = time(NULL);
    fprintf(stderr,
        "\nTotal Time: %ld:%02ld\n",
        (end - start) / 60, (end - start) % 60);

    return 0;
}