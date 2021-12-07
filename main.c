// Initial Commit

#include "executor.h"

int main(int argc, char**argv) {
    time_t          start,
                    end;
    int             num_procs,
                    rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank)
        process_node(rank, num_procs);
    else {
        start = time(NULL);
        control_node(rank, num_procs);
        end = time(NULL);
        fprintf(stderr, "\nTotal Time: %ld:%02ld\n",
                (end - start) / 60, (end - start) % 60);
    }

    MPI_Finalize();

    return 0;
}