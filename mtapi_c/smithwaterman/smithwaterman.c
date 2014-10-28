
// ////////////////////////////////////////////////////////////////////////////
// 
// MTAPI example:
// porting a Smith-Waterman wafe-front algorithm from OpenMP to MTAPI
//
// remarks: 
//  - no error handling included (mtapi_status_t not evaluated)
//  - attibutes to be set for action, group, and task not specified
//
// ////////////////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <sys/time.h>
#include <time.h>

// ////////////////////////////////////////////////////////////////////////////
// MTAPI: define entry function for tasks
// (parallel acivities have to be implemented as functions)
// ////////////////////////////////////////////////////////////////////////////

// define data structure for task parameters
// was in OpenMP: #pragma omp task default(none) firstprivate(np, mp, i, k, l) private(a, b) shared(h, e, f, sequences, open, extension)
typedef struct  {
    unsigned int np;        // firstprivate - const?
    unsigned int mp;        // firstprivate - const?
    unsigned int i;         // firstprivate - const?
    unsigned int k;         // firstprivate - const?
    unsigned int l;         // firstprivate - const?
    int **h;                // shared
    int **e;                // shared
    int **f;                // shared
    unsigned int sequences; // shared - const?
    unsigned int open;      // shared - const?
    unsigned int extension; // shared - const?
} element_args;

// task entry function (signuature according to MTAPI specification)
void calculate_element(void* parameters, int param_count, mtapi_status_t status) {
    int a;                  // private
    int b;                  // private

    // get agruments (cast to concrete parameter struct)
    element_args* args = (element_args*) parameters;
    
    // optionally extract agruments (alternatively access dirctly further down)
    unsigned int np = args.np;
    unsigned int mp = args.mp;
    unsigned int i = args.i;
    unsigned int k = args.k;
    unsigned int l = args.l;
    int **h = args.h;
    int **e = args.e;
    int **f = args.f;
    unsigned int sequences = args.sequences;
    unsigned int open = args.open;
    unsigned int extension = args.extension;
    
    //printf("(%d,%d)", np-i, mp+i);
    // Calculate e
    a = h[(np-i)][(mp+i)-1] - open;
    b = e[(np-i)][(mp+i)-1] - extension;
    e[(np-i)][(mp+i)] = ((a>b)?a:b);

    // Calculate f
    a = h[(np-i)-1][(mp+i)] - open;
    b = f[(np-i)-1][(mp+i)] - extension;
    f[(np-i)][(mp+i)] = ((a>b)?a:b);

    // Calculate h
    a = h[(np-i)-1][(mp+i)-1] + similarity(sequences[k][(np-i)-1], sequences[l][(mp+i)-1]);
    if(e[(np-i)][(mp+i)] > f[(np-i)][(mp+i)]) {
        if(a > e[(np-i)][(mp+i)])	h[(np-i)][(mp+i)] = ((a>0)?a:0);
        else						h[(np-i)][(mp+i)] = ((e[(np-i)][(mp+i)]>0)?e[(np-i)][(mp+i)]:0);
    }
    else {
        if(a > f[(np-i)][(mp+i)])	h[(np-i)][(mp+i)] = ((a>0)?a:0);
        else						h[(np-i)][(mp+i)] = ((f[(np-i)][(mp+i)]>0)?f[(np-i)][(mp+i)]:0);
    }
}

// ////////////////////////////////////////////////////////////////////////////
// END MTAPI
// ////////////////////////////////////////////////////////////////////////////


int similarity(char x, char y) {
	return((x==y)?2:-1);
}

int main(int argc, char *argv[]) {

    // ////////////////////////////////////////////////////////////////////////////
    // MTAPI: initialize MTAPI
    // ////////////////////////////////////////////////////////////////////////////

    mtapi_status_t mtapi_status;
    
    mtapi_node_attributes_t mtapi_node_attributes;
   
    mtapi_node_init_attributes( 
        &mtapi_node_attributes, 
        &mtapi_status 
    );
    
    mtapi_node_set_attribute(  
        &mtapi_node_attributes, 
        MTAPI_NODE_TYPE,       // example attribute
        MTAPI_NODE_TYPE_SMP,   // example attibute value
        MTAPI_NODE_TYPE_SIZE,  // example attribute size
        &mtapi_status 
    );

    // initialize MTAPI
    mtapi_initialize( 
        MTAPI_DOMAIN_ID,  // to be defined in a central header file
        MTAPI_NODE_ID,    // to be defined in a central header file
        &mtapi_node_attributes, 
        &mtapi_parameters, 
        &mtapi_info, 
        &mtapi_status 
    );

    // ////////////////////////////////////////////////////////////////////////////
    // MTAPI: create action handle for starting a task
    // (action handles allow to start remote tasks, i.e., task implemented on a 
    // different node)
    // ////////////////////////////////////////////////////////////////////////////

    mtapi_action_attributes_t mtapi_action_attributes;
    void mtapi_action_init_attributes(
        &mtapi_action_attributes,
        &mtapi_status
    );
    mtapi_action_set_attribute( 
        &mtapi_action_attributes, 
        // ...
    );

    mtapi_action_hndl_t action =  mtapi_action_create(
        WAVEFRONTACTION_ID,    // to be defined in a central header file
        calculate_element,
        mtapi_action_attributes,
        &mtapi_status
    );
    
    // ////////////////////////////////////////////////////////////////////////////
    // MTAPI: create group for synchronizing on tasks
    // ////////////////////////////////////////////////////////////////////////////

    mtapi_group_attributes_t mtapi_group_attributes;
    
    mtapi_group_init_attributes(
        &mtapi_group_attributes,
        &mtapi_status
    );
    
    mtapi_group_set_attribute( 
        &mtapi_group_attributes, 
        // ...
    );

    mtapi_group_hndl group mtapi_group_create(
        mtapi_group_attributes,
        &mtapi_status
    );

    // ////////////////////////////////////////////////////////////////////////////
    // END MTAPI
    // ////////////////////////////////////////////////////////////////////////////

	// If fewer than 2 arguments then exit program
	if(argc < 3 - 1) {
		printf("Error: Too few arguments!\n");
		exit(0);
	}

	// Variables
	int a, b;
	unsigned int wave, waves, i, elements;
	unsigned int np, mp, line, j;

	// Load sequences
	unsigned int k, l;
	FILE *fp;
	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		printf("Error: Could not open file!\n");
		exit(0);
	}
	unsigned int q, n;
	fscanf(fp, "%d %d\n", &q, &n);
	char **sequences = malloc(q * sizeof(char *));
	for(a = 0; a < q; ++a) {
		sequences[a] = malloc((n + 1) * sizeof(char));
		fscanf(fp, "%s\n", sequences[a]);
	}
	fclose(fp);
	unsigned int m = n;

	// Timing structures
	long interval_s, interval_ns;
	struct timespec start, end;

	// Declare and define open, extension gap penalties
	unsigned int open = atoi(argv[2]);
	unsigned int extension = atoi(argv[3]);
	
	// Allocate matrices h, e, f, sub
	int **h, **e, **f, **sub;
	h = malloc((n + 1) * sizeof(int *));
	e = malloc((n + 1) * sizeof(int *));
	f = malloc((n + 1) * sizeof(int *));
	sub = malloc(256 * sizeof(int *));
	#pragma omp parallel for
	for(i = 0; i < n + 1; ++i) {
		h[i] = malloc((m + 1) * sizeof(int));
		e[i] = malloc((m + 1) * sizeof(int));
		f[i] = malloc((m + 1) * sizeof(int));
		sub = malloc(256 * sizeof(int));
	}

	waves = n + m - 1;
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(k = 0; k < q; ++k) {
		for(l = k + 1; l < q; ++l) {
			// Write zeroes to top and left corner of matrices h, e, f
			#pragma omp parallel
			{
				#pragma omp for nowait
				for(i = 0; i < n + 1; ++i) h[i][0] = e[i][0] = f[i][0] = 0;
				#pragma omp for
				for(i = 1; i < m + 1; ++i) h[0][i] = e[0][i] = f[0][i] = 0;

				#pragma omp single
				{
					for(wave = 0; wave < waves; ++wave) {
						// 0 <= wave < n-1
						if(wave < n-1) {
							elements = wave+1;
							np = wave+1;
							mp = 0+1;
							line = (n-1)-wave;
						}
						// n-1 <= wave < m
						else if(wave < m) {
							elements = n;
							np = n-1+1;
							mp = wave-(n-1)+1;
							line = mp;
						}
						// m <= wave < m+n-1
						else {
							elements = n-1-(wave-m);
							np = n-1+1;
							mp = wave-(n-1)+1;
							line = mp;
						}

						for(i = 0; i < elements; ++i) {
							// was: #pragma omp task default(none) firstprivate(np, mp, i, k, l) private(a, b) shared(h, e, f, sequences, open, extension)

                            // ////////////////////////////////////////////////////////////////////////////
                            // MTAPI: copy arguments and start task
                            // ////////////////////////////////////////////////////////////////////////////

                            // copy arguments
                            element_args args;
                            args.np = np; args.mp = mp; args.i = i; args.k = k; args.l = l;
                            args.h = h; args.e = e; args.f = f; args.sequences = sequences; args.open = open; args.extensions = extensions;
                            
                            // configure task attributes
                            mtapi_task_attributes_t mtapi_task_attributes;
                            
                            mtapi_task_init_attributes(
                                &mtapi_task_attributes,
                                &mtapi_status
                            );
                            
                            mtapi_task_set_attribute( 
                                &mtapi_task_attributes, 
                                // ...
                            );                            
                            
                            // start task (remark: should be not so fine grain, use chunks!)
                            mtapi_task_start(
                                action,                // start routine
                                &args,                 // arguments struct
                                sizeof(element_args),  // size of arg. struct
                                mtapi_task_attributes
                                group,                 // tasks belong to a group
                                &mtapi_status 
                            );

                            // ////////////////////////////////////////////////////////////////////////////
                            // END MTAPI
                            // ////////////////////////////////////////////////////////////////////////////

                            
                            
                        }
                        // ////////////////////////////////////////////////////////////////////////////
                        // MTAPI: wait for task group completion
                        // ////////////////////////////////////////////////////////////////////////////
                        void mtapi_group_wait_all(
                            group,          // group handle
                            NULL,           // no time out
                            &mtapi_status 
                        );

                        // ////////////////////////////////////////////////////////////////////////////
                        // END MTAPI
                        // ////////////////////////////////////////////////////////////////////////////
					}
				}
			}
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	// Print time
	interval_s = end.tv_sec - start.tv_sec;
	interval_ns = end.tv_nsec - start.tv_nsec;
	printf("Real Time: %lds %ldns", interval_s, interval_ns);

	// Print matrix h
	/*printf("\n");
	for(i = 0; i < n + 1; ++i) {
		printf("[");
		for(j = 0; j < m; ++j) {
			printf("%d\t", h[i][j]);
		}
		printf("%d]\n", h[i][m]);
	}*/

    // finalize MTAPI
    void mtapi_finalize( 
        &mtapi_status 
    );

	printf("\n");
	return(0);
}
