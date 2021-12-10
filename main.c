// Initial Commit

#include "executor.h"

int main(int argc, char**argv) {
    int             num_procs,
                    rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    (rank) ? process_node(rank, num_procs) : control_node(rank, num_procs);

    MPI_Finalize();

    return 0;
}