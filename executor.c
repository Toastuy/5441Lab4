#include "executor.h"

void control_node(int rank, int num_procs) {
    transform_t *buffer;
    int size;
    buffer = (transform_t *) malloc(sizeof(transform_t) * BUFFER_SIZE);
    size = reader(buffer);
    send_data(buffer, size);

    execute_workflow(buffer, size);
}

void process_node(int rank, int num_procs) {
    transform_t *buffer;
    int size;
    // TODO - send and receive logic for non-zero ranks
    execute_workflow(buffer, size);
}

void execute_workflow(transform_t *buffer, int size) {
    transform_t *input, *encoded, *decoded, *output;
    create_transform_structures(input, encoded, decoded, output);

    // TODO - implement workload

    destroy_transform_structures(input, encoded, decoded, output);
}

void create_transform_structures(transform_t *input, transform_t *encoded,
                                 transform_t *decoded, transform_t *output) {
    input   = (transform_t *) malloc(sizeof(transform_t) * BUFFER_SIZE);
    if(!input)
        exit(EXIT_FAILURE);
    encoded = (transform_t *) malloc(sizeof(transform_t) * BUFFER_SIZE);
    if(!encoded) {
        free(input);
        exit(EXIT_FAILURE);
    }
    decoded = (transform_t *) malloc(sizeof(transform_t) * BUFFER_SIZE);
    if(!decoded) {
        free(input);
        free(encoded);
        exit(EXIT_FAILURE);
    }
    output = (transform_t *) malloc(sizeof(transform_t) * BUFFER_SIZE);
    if(!output) {
        free(input);
        free(encoded);
        free(decoded);
        exit(EXIT_FAILURE);
    }
}

void destroy_transform_structures(transform_t *t1, transform_t *t2,
                                  transform_t *t3, transform_t *t4) {
    free(t1); free(t2); free(t3); free(t4);
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
    printf("%lu\n", index);
    return index - 1;
}

void encoder(transform_t *t, transform_t *q, size_t i) {
    double retval;
    switch(t->cmd) {
        case 'A':
            t->encoded_key = transformAE(t->key, &retval);
            break;
        case 'B':
            t->encoded_key = transformBE(t->key, &retval);
            break;
        case 'C':
            t->encoded_key = transformCE(t->key, &retval);
            break;
        case 'D':
            t->encoded_key = transformDE(t->key, &retval);
            break;
        case 'E':
            t->encoded_key = transformEE(t->key, &retval);
            break;
        default:
            break;
    }
    q[i] = *t;
}

void first_decode(transform_t *t, transform_t *q, size_t i) {
    double retval;
    switch(t->cmd) {
        case 'A':
            t->first_decoded = transformAD1(t->encoded_key, &retval);
            break;
        case 'B':
            t->first_decoded = transformBD1(t->encoded_key, &retval);
            break;
        case 'C':
            t->first_decoded = transformCD1(t->encoded_key, &retval);
            break;
        case 'D':
            t->first_decoded = transformDD1(t->encoded_key, &retval);
            break;
        case 'E':
            t->first_decoded = transformED1(t->encoded_key, &retval);
            break;
        default:
            break;
    }
    q[i] = *t;
}

// Overwrites retval from previous decoder call.
void second_decode(transform_t *t, transform_t *o) {
    double retval;
    switch(t->cmd) {
        case 'A':
            t->second_decoded = transformAD2(t->first_decoded, &retval);
            break;
        case 'B':
            t->second_decoded = transformBD2(t->first_decoded, &retval);
            break;
        case 'C':
            t->second_decoded = transformCD2(t->first_decoded, &retval);
            break;
        case 'D':
            t->second_decoded = transformDD2(t->first_decoded, &retval);
            break;
        case 'E':
            t->second_decoded = transformED2(t->first_decoded, &retval);
            break;
        default:
            break;
    }
    o[t->index - 1] = *t;
}

void output_entries(transform_t *t) {
    t->valid = 0;
    fprintf(stdout, "%6d %6c %6hu %6hu %6hu\n",
            t->index,         t->cmd,             t->encoded_key,
            t->first_decoded, t->second_decoded);
}
