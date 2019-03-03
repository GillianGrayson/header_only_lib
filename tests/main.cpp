/*
 *  Distributed under the MIT License (See accompanying file /LICENSE )
 */
#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include "header_only_lib/logger.h"

int main(int argc, char* argv[]) {
  using namespace header_only_lib;
  Logger::level(LogLevel::debug);

  auto result = Catch::Session().run(argc, argv);

  return (result < 0xff ? result : 0xff);
}