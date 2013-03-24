#include <cstdlib>
#include <stdexcept>

#include "CommandLineInterface.hpp"

namespace common {

  int CommandLineInterface::run(int argc, char* argv[]) {
    if (minimum_amount_of_arguments != 0 &&
        static_cast<unsigned int>(argc - 1) < minimum_amount_of_arguments) {
      help();
      std::exit(CommandLineInterface::MISSING_ARGUMENTS);
    }

    for (int i = 1; i < argc; ++i) {
      arguments.push_back(argv[i]);
    }

    handle_special_case();
    return main();
  }

  unsigned int CommandLineInterface::size() const {
    return arguments.size();
  }

  std::string CommandLineInterface::extractArgument(unsigned int index) const {
    if (index >= arguments.size()) {
      throw new std::out_of_range("Invalid argument index for command line interface!");
    }

    return arguments[index];
  }

}