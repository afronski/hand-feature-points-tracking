#include <iostream>
#include <string>

#include "../common/debug.hpp"

#include "../common/path.hpp"
#include "../common/vision.hpp"

#include "../common/VideoStream.hpp"
#include "../common/CommandLineInterface.hpp"

#include "implementations/PointsMarker.hpp"
#include "factories/AlgorithmFactory.hpp"

class TrackerApplication : public common::CommandLineInterface {
  public:
    TrackerApplication(): CommandLineInterface(1), printResultsToOutput(false) {}

  protected:
    virtual void help() {
      std::cout << "tracking                                              Wojciech Gawroński (2013)" << std::endl;
      std::cout << "Usage:                                                                         " << std::endl;
      std::cout << " ./tracking [--include-results] input_file_AVI method_name [arguments]         " << std::endl;
      std::cout << "     --include-results - Print profiling results to console (optional)         " << std::endl;
      std::cout << "     input_file_AVI    - AVI file                                              " << std::endl;
      std::cout << "     method_name       - Algorithm name                                        " << std::endl;
      std::cout << "     arguments         - Further arguments provided as an algorithm input      " << std::endl;
      std::cout << "Or:                                                                            " << std::endl;
      std::cout << " ./tracking --list-algorithms                                                  " << std::endl;
      std::cout << "     --list-algorithms - list all implemented algorithms for tracking  points  " << std::endl;
      std::cout << std::endl;
    }

    virtual int main() {
      std::string input(extractArgument(0));
      std::string method(extractArgument(1));

      return saveMovie(input, method);
    }

    virtual bool handle_special_case() {
      if (extractArgument(0) == "--list-algorithms") {
        AlgorithmFactory::listAllAlgorithms();

        return true;
      }

      if (extractArgument(0) == "--include-results") {
        arguments.erase(std::remove(arguments.begin(), arguments.end(), "--include-results"));

        common::debug::printEnabled = false;
        printResultsToOutput = true;
      }

      return false;
    }

  private:
    bool printResultsToOutput;

    int saveMovie(const std::string& input, const std::string& method) {
      std::string baseFilename = common::path::extractFileName(input);

      std::string safeMethod(method);
      std::replace(safeMethod.begin(), safeMethod.end(), ' ', '_');

      std::string output = baseFilename + "_tracking_result_for_" + safeMethod + ".avi";

      if (AlgorithmFactory::isAlgorithmPresent(method)) {
        try {
          ArgumentsAwareFrameTransformer* transformer = AlgorithmFactory::createAlgorithm(method);
          ArgumentsAwareFrameTransformer* marker = AlgorithmFactory::createAlgorithm(PointsMarker::Name);

          marker->fill(this->getArguments());
          transformer->fill(this->getArguments());

          common::vision::VideoStream stream;

          stream.add(marker);
          stream.add(transformer);

          stream.open(input);
          stream.transfer(output);

          if (printResultsToOutput) {
            transformer->printResults();
          }
        } catch(const std::exception& exception) {
          std::cerr << exception.what() << std::endl;

          return -1;
        }
      }

      return 0;
    }
};

int main(int argc, char* argv[]) {
  TrackerApplication cli;

  return cli.run(argc, argv);
}