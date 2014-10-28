#include "fibonacci.h"
#include <partest/partest.h>
#include <embb_mtapi_test_config.h>
#include <embb_mtapi_test_init_finalize.h> 

#define MTAPI_DOMAIN_ID 0
#define MTAPI_NODE_ID 0
#define JOB_TEST_TASK 42
#define DOMAIN_ID 0
#define NODE_ID 1
#define TASK_TEST_ID 23

static void FibonacciActionFunction(
		const void* args,
		mtapi_size_t arg_size,
		void* result_buffer,
		mtapi_size_t result_size,
		const void * node_local_data,
		mtapi_size_t node_local_data_size,
		mtapi_task_context_t* task_context
		) {
	mtapi_task_hndl_t task;
	mtapi_status_t status;
	mtapi_job_hndl_t job;

	int n;
	int* result;
	int x, y, a, b;

	if (arg_size != sizeof(int)) {
		printf("wrong size of arguments\n");
		mtapi_context_status_set (
			task_context, 
			MTAPI_ERR_ARG_SIZE, 
			&status
			);
	}

	n = *(int *) args;
	if (result_buffer == MTAPI_NULL) {
		mtapi_context_status_set(
			task_context,
			MTAPI_ERR_RESULT_SIZE,
			&status
			);
	} else {
		if (result_size== sizeof(int)) {
			result = (int *) result_buffer;
		} else {
			printf("wrong size of result buffer\n");
			mtapi_context_status_set(
				task_context,
				MTAPI_ERR_RESULT_SIZE,
				&status
				);
			return;
		}
	}
	
	job = mtapi_job_get(JOB_TEST_TASK, DOMAIN_ID, &status);
	if (n < 2) *result = n;
	else {
		a = n-1;
		task = mtapi_task_start(
			TASK_TEST_ID,
			job,
			(void *) &a,
			sizeof(int),
			(void *) &x,
			sizeof(int),
			MTAPI_DEFAULT_TASK_ATTRIBUTES,
			MTAPI_GROUP_NONE,
			&status
			);
		b = n - 2;
		FibonacciActionFunction(
			&b,
			sizeof(int), 
			&y,
			sizeof(int),
			MTAPI_NULL,
			0,
			task_context
			);
		mtapi_task_wait(task, MTAPI_INFINITE, &status);
		*result = x + y;
	}
}

int Fibonacci(int n) {
	mtapi_action_hndl_t fibonacciAction;
	mtapi_task_hndl_t task;
	mtapi_job_hndl_t job;
	mtapi_status_t status;
	mtapi_affinity_t affinity;
	mtapi_action_attributes_t action_attr;
	mtapi_actionattr_init(&action_attr, &status);
	mtapi_affinity_init(&affinity, MTAPI_TRUE, &status);
  	mtapi_actionattr_set(
   		&action_attr,
    		MTAPI_ACTION_AFFINITY,
    		&affinity,
    		MTAPI_ACTION_AFFINITY_SIZE,
    		&status);
	int result;
  	fibonacciAction = mtapi_action_create(
    		JOB_TEST_TASK,
    		FibonacciActionFunction,
    		MTAPI_NULL,
    		0,
    		&action_attr,
    		&status);
	job = mtapi_job_get(JOB_TEST_TASK, DOMAIN_ID, &status);
	task = mtapi_task_start(
		TASK_TEST_ID,
		job,
		(void *) &n,
		sizeof(int),
		(void *) &result,
		sizeof(int),
		MTAPI_DEFAULT_TASK_ATTRIBUTES,
		MTAPI_GROUP_NONE,
		&status
		);
	mtapi_task_wait(task, MTAPI_INFINITE, &status);
	printf("fib %d  result %d\n", n, result);
	return result;
}

int main() {
	Fibonacci(3);
	return 0;
}















