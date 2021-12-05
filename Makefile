# Source and lib files
PROGRAM = lab4_hoke62_hoke62
FILES.c = executor.c
FILES.h = executor.h
FILES.o = ${FILES.c:.c=.o} transformMat_lab2.o
TEST	= input/PCS_data_t00100
TEST1	= input/PCS_data_test
TEST2	= input/PCS_data_t00500
TEST3	= input/PCS_data_test1

# Compiler options
CC 		= mpicc
SFLAGS	= -std=c11
GFLAGS	= -g
OFLAGS  = -O3
FFLAG1	= -fopenmp
FFLAG2	= -no-pie
WFLAG1  = -Wall
WFLAG2  = -Wextra
WFLAG3  = -Wstrict-prototypes
WFLAG4  = -Wmissing-prototypes
FFLAGS	= ${FFLAG1} ${FFLAG2}
WFLAGS	= ${WFLAG1} ${WFLAG2} ${WFLAG3} ${WFLAG4}
CFLAGS	= ${FFLAGS} ${SFLAGS} ${GFLAGS} ${OFLAGS} ${WFLAGS}

all: ${PROGRAM}

${PROGRAM}: ${FILES.o}
	${CC} -o $@ ${CFLAGS} ${FILES.o}

main.o:	${FILES.h}

executor.o:	${FILES.h}

test: ${PROGRAM} clean
	./${PROGRAM} <${TEST} 2> error.log

min-test: ${PROGRAM} clean
	./${PROGRAM} <${TEST1} 2> error.log

big-test: ${PROGRAM} clean
	./${PROGRAM} <${TEST2} 2> error.log

full-test: ${PROGRAM} clean
	./${PROGRAM} <${TEST3} 2> error.log

clean:
	rm -rf executor.o main.o