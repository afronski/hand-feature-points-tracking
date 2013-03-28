#include <iostream>
#include <string>

#include "../common/path.hpp"
#include "../common/vision.hpp"

#include "../common/VideoStream.hpp"
#include "../common/CommandLineInterface.hpp"

#include "factories/AlgorithmFactory.hpp"

class TrackerApplication : public common::CommandLineInterface {
  public:
    TrackerApplication(): CommandLineInterface(1) {}

  protected:
    virtual void help() {
      std::cout << "tracking                                              Wojciech Gawroński (2013)" << std::endl;
      std::cout << "Usage:                                                                         " << std::endl;
      std::cout << " ./tracking input_file_AVI                                                     " << std::endl;
      std::cout << "     - input_file_AVI - AVI file                                               " << std::endl;
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

      return false;
    }

  private:
    int saveMovie(const std::string& input, const std::string& method) {
      std::string output = common::path::extractFileName(input) + "_tracking_result.avi";

      if (AlgorithmFactory::isAlgorithmPresent(method)) {
        try {
          PointsAwareFrameTransformer* transformer = AlgorithmFactory::createAlgorithm(method);

          transformer->fill(common::vision::extract_points_from_arguments(*this, 2));
          common::vision::VideoStream stream;

          stream.add(transformer);
          stream.open(input);
          stream.transfer(output);
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