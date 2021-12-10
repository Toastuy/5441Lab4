#include "executor.h"

void control_node(int rank, int num_procs) {
    int         indices[BUFFER_SIZE];
    char        cmds[BUFFER_SIZE];
    uint16_t    keys[BUFFER_SIZE],
                encoded[BUFFER_SIZE],
                first_decode[BUFFER_SIZE],
                second_decode[BUFFER_SIZE],
                tmp_encode[BUFFER_SIZE],
                tmp_first[BUFFER_SIZE],
                tmp_second[BUFFER_SIZE];
    int size, i, j;
    MPI_Status status;

    // Read in data
    size = reader(cmds, indices, keys);

    for(i = 1; i < num_procs; ++i) {
        MPI_Send(&size, 1, MPI_INT, i, i, MPI_COMM_WORLD);
        MPI_Send(cmds, size, MPI_CHAR, i, i, MPI_COMM_WORLD);
        MPI_Send(keys, size, MPI_UINT16_T, i, i, MPI_COMM_WORLD);
    }

    // Conduct workflow
    execute_workflow(cmds, keys, encoded,first_decode, second_decode, size, num_procs, rank);

    for(i = 1; i < num_procs; ++i) {
        MPI_Recv(tmp_encode, size, MPI_UINT16_T, i, i, MPI_COMM_WORLD, &status);
        MPI_Recv(tmp_first, size, MPI_UINT16_T, i, i, MPI_COMM_WORLD, &status);
        MPI_Recv(tmp_second, size, MPI_UINT16_T, i, i, MPI_COMM_WORLD, &status);
        for(j = i; j < size; j += num_procs) {
            encoded[j] = tmp_encode[j];
            first_decode[j] = tmp_first[j];
            second_decode[j] = tmp_second[j];
        }
    }

    // Print outputs
    output_entries(cmds, indices, encoded, first_decode, second_decode, size);
}

void process_node(int rank, int num_procs) {
    char        cmds[BUFFER_SIZE];
    uint16_t    keys[BUFFER_SIZE],
                encoded[BUFFER_SIZE],
                first_decode[BUFFER_SIZE],
                second_decode[BUFFER_SIZE];
    int size;
    MPI_Status status;

    MPI_Recv(&size, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
    MPI_Recv(cmds, size, MPI_CHAR, 0, rank, MPI_COMM_WORLD, &status);
    MPI_Recv(keys, size, MPI_UINT16_T, 0, rank, MPI_COMM_WORLD, &status);

    execute_workflow(cmds, keys, encoded,first_decode, second_decode, size, num_procs, rank);

    MPI_Send(encoded, size, MPI_UINT16_T, 0, rank, MPI_COMM_WORLD);
    MPI_Send(first_decode, size, MPI_UINT16_T, 0, rank, MPI_COMM_WORLD);
    MPI_Send(second_decode, size, MPI_UINT16_T, 0, rank, MPI_COMM_WORLD);
}

void execute_workflow(char *cmds, uint16_t *keys, uint16_t *encoded,
                      uint16_t *first_decoded, uint16_t *second_decoded,
                      int size, int num_procs, int rank) {
    int i, j, k;
    double retvals[size];
    #pragma omp parallel num_threads(CPU_COUNT)
    {
        // Encoder region
        #pragma omp for
            for (i = rank; i < size; i += num_procs) {
                encoder(&cmds[i], &keys[i], &encoded[i], &retvals[i]);
            }
        // First decoder region
        #pragma omp for
            for (j = rank; j < size; j += num_procs) {
                first_decode(&cmds[j], &encoded[j], &first_decoded[j], &retvals[j]);
            }
        // Second decoder region
        #pragma omp for
            for (k = rank; k < size; k += num_procs) {
                second_decode(&cmds[k], &first_decoded[k], &second_decoded[k], &retvals[k]);
            }
    }
    // Synchronization barrier for each block
    MPI_Barrier(MPI_COMM_WORLD);
}

int reader(char *cmds, int *indices, uint16_t *keys) {
    char cmd;
    uint16_t key;
    int index = 1;
    while(fscanf(stdin, "%c %hu", &cmd, &key)) {
        if(cmd == 'X') {
            break;
            // If key is valid and cmd is valid create transform struct for
            // insertion.
        } else if((cmd == 'A' || cmd == 'B' || cmd == 'C' ||
                cmd == 'D' || cmd == 'E')) {
            indices[index - 1]      = index;
            keys[index - 1]         = key;
            cmds[index - 1]         = cmd;
            index++;
        }
    }
    return index - 1;
}

void encoder(const char *cmds, const uint16_t *keys, uint16_t *encoded, double *retval) {
    switch(*cmds) {
        case 'A':
            *encoded = transformAE(*keys, retval);
            break;
        case 'B':
            *encoded = transformBE(*keys, retval);
            break;
        case 'C':
            *encoded = transformCE(*keys, retval);
            break;
        case 'D':
            *encoded = transformDE(*keys, retval);
            break;
        case 'E':
            *encoded = transformEE(*keys, retval);
            break;
        default:
            break;
    }
}

void first_decode(const char *cmds, const uint16_t *encoded, uint16_t *first_decoded, double *retval) {
    switch(*cmds) {
        case 'A':
            *first_decoded = transformAD1(*encoded, retval);
            break;
        case 'B':
            *first_decoded = transformBD1(*encoded, retval);
            break;
        case 'C':
            *first_decoded = transformCD1(*encoded, retval);
            break;
        case 'D':
            *first_decoded = transformDD1(*encoded, retval);
            break;
        case 'E':
            *first_decoded = transformED1(*encoded, retval);
            break;
        default:
            break;
    }
}

// Overwrites retval from previous decoder call.
void second_decode(const char *cmds, const uint16_t *first_decoded, uint16_t *second_decoded, double *retval) {
    switch(*cmds) {
        case 'A':
            *second_decoded = transformAD2(*first_decoded, retval);
            break;
        case 'B':
            *second_decoded = transformBD2(*first_decoded, retval);
            break;
        case 'C':
            *second_decoded = transformCD2(*first_decoded, retval);
            break;
        case 'D':
            *second_decoded = transformDD2(*first_decoded, retval);
            break;
        case 'E':
            *second_decoded = transformED2(*first_decoded, retval);
            break;
        default:
            break;
    }
}

void output_entries(const char *cmds, const int *indices, const uint16_t *encoded,
                    const uint16_t *first_decode, const uint16_t *second_decode, int size) {
    int i = 0;
    while(i < size) {
        fprintf(stdout, "%6d %6c %6hu %6hu %6hu\n",
                indices[i],         cmds[i],             encoded[i],
                first_decode[i], second_decode[i]);
        i++;
    }
}
