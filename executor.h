#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

#define BUFFER_SIZE     2500
#define CONTROL_NODE    0

/**
 * Struct to contain all the required values throughout the process
 * (int) valid,
 * (char) cmd, (uint16_t) key, (int) sequence_number,
 * (uint16_t) encoded and 1/2 decoded key,
 * (double) 2nd decoded return from transforms.
 */
typedef struct {
    int         valid,
                index;
    char        cmd;
    uint16_t    key,
                encoded_key,
                first_decoded,
                second_decoded;
} transform_t;

/**
 * Parses I/O and distributes workload to child nodes for processing.
 * Upon completion retrieves entries from children nodes and outputs
 * results.
 * @param   -   pointer to array of data
 * @param   -   rank
 * @param   -   number of nodes
 * @return  -   non-zero return on failure
 */
void control_node(int, int);

/**
 * Receives data and does intermediary computations returning result
 * to caller via MPI.
 * @param   -   pointer to array of data
 * @param   -   rank
 * @param   -   number of nodes
 * @return  -   non-zero return on failure
 */
void process_node(int, int);

void execute_workflow(transform_t *, int, int, int);

void create_transform_structures(transform_t *, transform_t *, transform_t *, transform_t *);

void destroy_transform_structures(transform_t *, transform_t *, transform_t *, transform_t *);

void send_data(transform_t *, int , int);

void receive_data(transform_t *, int, int);

void send_size(int, int);

int receive_size(int);

/**
 * Reference to encoder/decoder functions in provided object file.
 * @param  - supplied key
 * @param  - updated return value
 * @return - coded key
 */
uint16_t transformAE(uint16_t, double *);
uint16_t transformAD1(uint16_t, double *);
uint16_t transformAD2(uint16_t, double *);

uint16_t transformBE(uint16_t, double *);
uint16_t transformBD1(uint16_t, double *);
uint16_t transformBD2(uint16_t, double *);

uint16_t transformCE(uint16_t, double *);
uint16_t transformCD1(uint16_t, double *);
uint16_t transformCD2(uint16_t, double *);

uint16_t transformDE(uint16_t, double *);
uint16_t transformDD1(uint16_t, double *);
uint16_t transformDD2(uint16_t, double *);

uint16_t transformEE(uint16_t, double *);
uint16_t transformED1(uint16_t, double *);
uint16_t transformED2(uint16_t, double *);

/**
 * Reads the input filling the buffer.
 * @param  - reference to array of transform structs
 * @return - size of all entries
 */
int reader(transform_t *);

/**
 * Handles the encoding portion for the program.
 * @param  - reference to array of transform structs
 * @param  - reference to array of transform structs
 * @param  - index to input given
 */
void encoder(transform_t *, transform_t *, size_t);

/**
 * Handles the encoding portion for the program.
 * @param  - reference to array of transform structs
 * @param  - reference to array of transform structs
 * @param  - index to input given
 */
void first_decode(transform_t *, transform_t *, size_t);

/**
 * Conducts the final transform and reports results to STDOUT.
 * @param  - reference to array of transform structs
 */
void second_decode(transform_t *, transform_t *);

void output_entries(transform_t *);
