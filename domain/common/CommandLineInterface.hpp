#ifndef __COMMAND_LINE_INTERFACE_HPP__
#define __COMMAND_LINE_INTERFACE_HPP__

#include <string>
#include <vector>

namespace common {

  class CommandLineInterface {
    public:
      static const int MISSING_ARGUMENTS = -127;
      static const int SUCCESS_EXIT_CODE = 0;

      CommandLineInterface() {}

      CommandLineInterface(int at_least_so_much_arguments) {
        minimum_amount_of_arguments = at_least_so_much_arguments;
      }

      std::string extractArgument(unsigned int index) const;
      unsigned int size() const;

      int run(int argc, char* argv[]);

    protected:
      virtual void help() = 0;
      virtual int main() = 0;

      virtual bool handle_special_case() {
        return false;
      }

    private:
      std::vector<std::string> arguments;
      unsigned int minimum_amount_of_arguments;
  };

}

#endif