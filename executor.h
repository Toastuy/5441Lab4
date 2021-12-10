#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <string.h>
#include <omp.h>

#define BUFFER_SIZE     2500
#define CPU_COUNT       28

/**
 * Struct to contain all the required values throughout the process
 * (int) valid,
 * (char) cmd, (uint16_t) key, (int) sequence_number,
 * (uint16_t) encoded and 1/2 decoded key,
 * (double) 2nd decoded return from transforms.
 */
typedef struct {
    int         index;
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

void execute_workflow(char *, uint16_t *, uint16_t *, uint16_t *, uint16_t *, int, int, int);

/**
 * Reads the input filling the buffer.
 * @param  - reference to array of transform structs
 * @return - size of all entries
 */
int reader(char *, int *, uint16_t *);

/**
 * Handles the encoding portion for the program.
 * @param  - reference to array of transform structs
 * @param  - reference to array of transform structs
 * @param  - index to input given
 */
void encoder(const char *, const uint16_t *, uint16_t *, double *);

/**
 * Handles the encoding portion for the program.
 * @param  - reference to array of transform structs
 * @param  - reference to array of transform structs
 * @param  - index to input given
 */
void first_decode(const char *, const uint16_t *, uint16_t *, double *);

/**
 * Conducts the final transform and reports results to STDOUT.
 * @param  - reference to array of transform structs
 */
void second_decode(const char *, const uint16_t *, uint16_t *, double *);

void output_entries(const char *, const int *, const uint16_t *,
                    const uint16_t *, const uint16_t *, int);
