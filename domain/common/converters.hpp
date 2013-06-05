#ifndef __CONVERTERS_HPP__
#define __CONVERTERS_HPP__

#include <vector>
#include <sstream>

namespace common {

  template<typename T>
    std::string toString(const T& value) {
      std::ostringstream result;
      result << value;

      return result.str();
    }

  template<typename T>
    std::string toString(const std::vector<T>& values) {
      typedef typename std::vector<T>::const_iterator ValueIterator;

      std::ostringstream result;

      result << "[ ";

      ValueIterator iterator = values.begin();
      while (iterator != values.end()) {
        result << *iterator;

        ValueIterator isJustBeforeLast = (++iterator);

        if (isJustBeforeLast != values.end()) {
          result << ", ";
        }
      }

      result << " ]";

      return result.str();
    }

  template<typename K, typename V>
    std::string toString(const std::vector< std::pair<K, V> >& values) {
      typedef typename std::vector< std::pair<K, V> >::const_iterator ValueIterator;

      std::ostringstream result;

      result << "[ ";

      ValueIterator iterator = values.begin();
      while (iterator != values.end()) {
        result << "{ \"key\": " << iterator->first << ", \"value\": " << iterator->second << "}";

        ValueIterator isJustBeforeLast = (++iterator);

        if (isJustBeforeLast != values.end()) {
          result << ", ";
        }
      }

      result << " ]";

      return result.str();
    }

}

#endif