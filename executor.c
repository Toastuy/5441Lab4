#include "executor.h"

int control_node(transform_t *buffer, int rank, int num_procs) {

}

int process_node(transform_t *buffer, int rank, int num_procs) {

}

size_t reader(transform_t *q) {
    size_t index;
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

void output_entries(transform_t *t) {
    t->valid = 0;
    fprintf(stdout, "%6lu %6c %6hu %6hu %6hu %23.1lf\n",
            t->index,         t->cmd,             t->encoded_key,
            t->first_decoded, t->second_decoded,  t->retval);
}
