// Initial Commit

#include "executor.h"

int main(int argc, char**argv) {
    time_t          start,
                    end;
    int             num_procs,
                    rank;

    // Read items
    start = time(NULL);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Main flow
    (rank) ? process_node(rank) : control_node(rank, num_procs);

    MPI_Finalize();

    // Report timing.
    end = time(NULL);
    fprintf(stderr, "\nTotal Time: %ld:%02ld\n", (end - start) / 60, (end - start) % 60);

    return 0;
}