#include <cctype>

#include <algorithm>
#include <sstream>

#include "JsonReporter.hpp"

// Private method.
std::string JsonReporter::handleType(const std::string& value) {
  std::string lowerCaseString(value);
  std::transform(lowerCaseString.begin(), lowerCaseString.end(), lowerCaseString.begin(), ::tolower);

  if (value.find_first_of("0123456789") != std::string::npos) {
    return value;
  } else if (value.find_first_of("[]") != std::string::npos) {
    return value;
  } else if (lowerCaseString == "true" || lowerCaseString == "false") {
    return lowerCaseString;
  } else {
    return "\"" + value + "\"";
  }
}

// Public methods.
void JsonReporter::merge(const Dictionary& toMerge) {
  dictionary.insert(toMerge.begin(), toMerge.end());
}

std::string JsonReporter::str() {
  std::ostringstream output;

  output << "{" << std::endl;

  Dictionary::iterator iterator = dictionary.begin();

  while(iterator != dictionary.end()) {
    output << "\t\"" << iterator->first << "\": ";
    output << handleType(iterator->second);

    Dictionary::iterator isJustBeforeLast = ++iterator;

    if (isJustBeforeLast != dictionary.end()) {
      output << "," << std::endl;
    } else {
      output << std::endl;
    }
  }

  output << "}" << std::endl;

  return output.str();
}