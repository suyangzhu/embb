#include "smithwaterman.h"
#include <partest/partest.h>
#include <embb_mtapi_test_config.h>
#include <embb_mtapi_test_init_finalize.h> 

int main() {
  embb_mtapi_log_info("running testInitFinalize...\n");

  mtapi_node_attributes_t node_attr;
  mtapi_info_t info;
  mtapi_status_t status;

  for (int ii = 0; ii < 100; ii++) {
	  printf("ii = %d\n", ii);
    status = MTAPI_ERR_UNKNOWN;
    mtapi_nodeattr_init(&node_attr, &status);
    MTAPI_CHECK_STATUS(status);

    status = MTAPI_ERR_UNKNOWN;
    mtapi_nodeattr_set(&node_attr,
      MTAPI_NODE_TYPE,
      MTAPI_ATTRIBUTE_VALUE(MTAPI_NODE_TYPE_SMP),
      MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
      &status);
    MTAPI_CHECK_STATUS(status);

    status = MTAPI_ERR_UNKNOWN;
    mtapi_initialize(
      THIS_DOMAIN_ID,
      THIS_NODE_ID,
      &node_attr,
      &info,
      &status);
    MTAPI_CHECK_STATUS(status);

    status = MTAPI_ERR_UNKNOWN;
    mtapi_finalize(&status);
    MTAPI_CHECK_STATUS(status);
  }

  embb_mtapi_log_info("...done\n\n");
	printf("Hello Smithwaterman !\n");
	return 0;
}
