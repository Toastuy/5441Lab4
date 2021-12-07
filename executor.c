#include "executor.h"

void control_node(int rank, int num_procs) {
    transform_t buffer[BUFFER_SIZE], recv[BUFFER_SIZE];
    int size, i, j, data_size;
    MPI_Status status;
    MPI_Request recv_request;

    size = reader(buffer);
    data_size = size * (int) sizeof(transform_t);

    for(i = 1; i < num_procs; ++i) {
        MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(buffer, data_size, MPI_BYTE, i, 0, MPI_COMM_WORLD);
    }

    execute_workflow(buffer, size, num_procs, rank);

    for(i = 1; i < num_procs; ++i) {
        // Asynchronously wait for first send from sibling process
        MPI_Irecv(recv, data_size, MPI_BYTE, i, 0, MPI_COMM_WORLD, &recv_request);
        MPI_Wait(&recv_request, &status);
        for(j = i; j < size; j += num_procs)
            buffer[j] = recv[j];
    }
    output_entries(buffer, size);
}

void process_node(int rank, int num_procs) {
    MPI_Status status;
    MPI_Request send_request;
    transform_t buffer[BUFFER_SIZE];
    int size, data_size;

    // Get our data to work on
    MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    data_size = size * (int) sizeof(transform_t);
    MPI_Recv(buffer, data_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
    // Change the data in place
    execute_workflow(buffer, size, num_procs, rank);

    // Async send and wait for rank 0 to respond
    MPI_Isend(buffer, data_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &send_request);
    MPI_Wait(&send_request, &status);
}

void execute_workflow(transform_t *buffer, int size, int num_procs, int rank) {
    int i, j, k;
    transform_t encoded[BUFFER_SIZE], decoded[BUFFER_SIZE];

    #pragma omp parallel num_threads(CPU_COUNT)
    {
        // Encoder region
        #pragma omp for
            for (i = rank; i < size; i += num_procs) {
                encoder(&buffer[i], encoded, i);
            }
        // First decoder region
        #pragma omp for
            for (j = rank; j < size; j += num_procs) {
                first_decode(&encoded[j], decoded, j);
            }
        // Second decoder region
        #pragma omp for
            for (k = rank; k < size; k += num_procs) {
                second_decode(&decoded[k], buffer);
            }
    }
    // Load final results into original buffer

}

int reader(transform_t *q) {
    int index;
    char cmd;
    uint16_t key;
    index = 1;
    while(fscanf(stdin, "%c %hu", &cmd, &key)) {
        if(cmd == 'X') {
            break;
            // If key is valid and cmd is valid create transform struct for
            // insertion.
        } else if((cmd == 'A' || cmd == 'B' || cmd == 'C' ||
                cmd == 'D' || cmd == 'E')) {
            q[index - 1].index = index;
            q[index - 1].key   = key;
            q[index - 1].cmd   = cmd;
            q[index - 1].valid = 1;
            index++;
        }
    }
    return index - 1;
}

void encoder(transform_t *t, transform_t *q, size_t i) {
    switch(t->cmd) {
        case 'A':
            t->encoded_key = transformAE(t->key, &t->retval);
            break;
        case 'B':
            t->encoded_key = transformBE(t->key, &t->retval);
            break;
        case 'C':
            t->encoded_key = transformCE(t->key, &t->retval);
            break;
        case 'D':
            t->encoded_key = transformDE(t->key, &t->retval);
            break;
        case 'E':
            t->encoded_key = transformEE(t->key, &t->retval);
            break;
        default:
            break;
    }
    q[i] = *t;
}

void first_decode(transform_t *t, transform_t *q, size_t i) {
    switch(t->cmd) {
        case 'A':
            t->first_decoded = transformAD1(t->encoded_key, &t->retval);
            break;
        case 'B':
            t->first_decoded = transformBD1(t->encoded_key, &t->retval);
            break;
        case 'C':
            t->first_decoded = transformCD1(t->encoded_key, &t->retval);
            break;
        case 'D':
            t->first_decoded = transformDD1(t->encoded_key, &t->retval);
            break;
        case 'E':
            t->first_decoded = transformED1(t->encoded_key, &t->retval);
            break;
        default:
            break;
    }
    q[i] = *t;
}

// Overwrites retval from previous decoder call.
void second_decode(transform_t *t, transform_t *o) {
    switch(t->cmd) {
        case 'A':
            t->second_decoded = transformAD2(t->first_decoded, &t->retval);
            break;
        case 'B':
            t->second_decoded = transformBD2(t->first_decoded, &t->retval);
            break;
        case 'C':
            t->second_decoded = transformCD2(t->first_decoded, &t->retval);
            break;
        case 'D':
            t->second_decoded = transformDD2(t->first_decoded, &t->retval);
            break;
        case 'E':
            t->second_decoded = transformED2(t->first_decoded, &t->retval);
            break;
        default:
            break;
    }
    o[t->index - 1] = *t;
}

void output_entries(transform_t *t, int size) {
    int i = 0;
    while(i < size)
        if(t[i].valid == 1) {
            t[i].valid = 0;
            fprintf(stdout, "%6d %6c %6hu %6hu %6hu\n",
                    t[i].index,         t[i].cmd,             t[i].encoded_key,
                    t[i].first_decoded, t[i].second_decoded);
            i++;
        }
}
