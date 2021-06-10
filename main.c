#include "logger.h"
#include <stdlib.h>


int main(int argc, char** argv) {
    core_handle_error(42, "test");
    core_set_error_handler(logging_error_handler);
    core_handle_error(42, "test_logging");

    return EXIT_SUCCESS;
}
