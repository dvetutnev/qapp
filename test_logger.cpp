#include "logger.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>


const std::filesystem::path path = "test.log";


TEST(Logger, file) {
    std::filesystem::remove_all(path);

    core_set_error_handler(logging_error_handler);
    core_handle_error(43, "test_logging");

    std::string result;
    std::ifstream f{path};
    std::getline(f, result);

    ASSERT_EQ(result, "[Loglevel: 4] Message 43, test_logging");
}
