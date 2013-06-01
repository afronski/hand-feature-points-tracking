#ifndef __JSON_REPORTER_HPP__
#define __JSON_REPORTER_HPP__

#include <map>
#include <string>

typedef std::map<std::string, std::string> Dictionary;

class JsonReporter {
  public:
    void merge(const Dictionary& toMerge);

    std::string str();

  private:
    Dictionary dictionary;

    std::string handleType(const std::string& value);
};

#endif