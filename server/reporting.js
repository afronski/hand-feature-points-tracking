    "use strict";

var fs = require("fs"),
    path = require("path"),
    util = require("util"),

    common = require("./common"),

    repository = require("./repository"),

    ToMB = 1024,
    FileFormat = "Person_%s_%s_results_for_%s.json",

    reporting = exports = module.exports = {};

// Extracting data from name.
function extractDataFromName(name) {
  var splitted = name.replace(".json", "").split("_");

  return {
    person: splitted[1],
    gesture: splitted[2],
    method: util.format("%s %s %s", splitted[5], splitted[6], splitted[7])
  };
}

function extractDataAndParametersFromName(name) {
  var result = extractDataFromName(name),
      parameters = /.*?parameters\((.*?)\).*/gi.exec(name)[1];

  result.parameters = parameters;

  return result;
}

// File filters.
function getFileByMethodAndOnlyParametrized(method) {
  var validMethod = method.replace(/\s/gi, "_"),
      mapped = repository.getResultsList().map(function(record) { return record.name; }),
      filtered = mapped.filter(function(name) {
        return name.indexOf(validMethod) !== -1 && name.indexOf("parameters") !== -1;
      });

  return filtered;
}

function getFileByMethodWithoutParametrized(method) {
  var validMethod = method.replace(/\s/gi, "_"),
      mapped = repository.getResultsList().map(function(record) { return record.name; }),
      filtered = mapped.filter(function(name) {
        return name.indexOf(validMethod) !== -1 && name.indexOf("parameters") === -1;
      });

  return filtered;
}

function getFileByMethodAndGestureWithoutParametrized(method, gesture) {
  var validMethod = method.replace(/\s/gi, "_"),
      mapped = repository.getResultsList().map(function(record) { return record.name; }),
      filtered = mapped.filter(function(name) {
        return name.indexOf(validMethod) !== -1 && name.indexOf("parameters") === -1;
      }),
      onlyOneGesture = filtered.filter(function(name) {
        return name.split("_")[2] === gesture;
      });

  return onlyOneGesture;
}

function getFileByMethodAndPersonWithoutParametrized(method, person) {
  var validMethod = method.replace(/\s/gi, "_"),
      mapped = repository.getResultsList().map(function(record) { return record.name; }),
      filtered = mapped.filter(function(name) {
        return name.indexOf(validMethod) !== -1 && name.indexOf("parameters") === -1;
      }),
      onlyOnePerson = filtered.filter(function(name) {
        return name.split("_")[1] === person;
      });

  return onlyOnePerson;
}

// Helpers.
function extract(element, index) {
  return { x: index + 1, y: element };
}

function extractMemory(element, index) {
  return { x: index + 1, y: element / ToMB };
}

function extractSpecialisedMemory(key, element) {
  return { x: element.index, y: element[key] };
}

function convertToMB(value) {
  return value / ToMB;
}

function min(array) {
  return Math.min.apply(Math, array);
}

function max(array) {
  return Math.max.apply(Math, array);
}

function sum(previous, next) {
  return previous + next;
}

function avg(array) {
  return array.reduce(sum, 0) / array.length;
}

// Common methods.
function prepareName(chartName, file) {
  var data = extractDataFromName(file);

  return {
    name: util.format("%s - Person '%s' - Gesture '%s' - Method '%s'",
                      chartName, data.person, data.gesture, data.method)
  }
}

function byX(a, b) {
  return a.x - b.x;
}

function get(file, key) {
  return JSON.parse(fs.readFileSync("./assets/" + file))[key];
}

function getFirstParameter(point) {
  return point.parameters.indexOf(" ") === -1;
}

function getOnlyOneGesture(gesture, point) {
  return point.gesture === gesture;
}

function getDataPointFromKeyInFile(key, file) {
  var data = extractDataAndParametersFromName(file),
      memory = get(file, key),
      rawParameters = data.parameters.indexOf(" ") !== -1 ?
                        data.parameters.split(" ")[1] :
                        data.parameters;

  return {
    minimum: min(memory.map(convertToMB)),
    maximum: max(memory.map(convertToMB)),
    average: avg(memory.map(convertToMB)),
    index: parseInt(rawParameters, 10),
    parameters: data.parameters,
    person: data.person,
    gesture: data.gesture
  };
}

// Memory data preparation.
reporting.memory = function(file) {
  var result = prepareName("Memory usage", file);

  result.series = [
    { values: get(file, "residentSetMemoryUsage").map(extractMemory), key: "Pamięć fizyczna" },
    { values: get(file, "virtualMemoryUsage").map(extractMemory), key: "Pamięć wirtualna" }
  ];

  return result;
};

reporting.memoryUsageForSpecialisedMethod = function(method) {
  var result = {
        name: util.format("Memory usage - Method '%s'", method),
        series: []
      },

      files = getFileByMethodAndOnlyParametrized(method),

      physical = files.map(getDataPointFromKeyInFile.curry("residentSetMemoryUsage")),

      physicalMemoryGestureC = physical.filter(getOnlyOneGesture.curry("C")),
      physicalMemoryGestureO = physical.filter(getOnlyOneGesture.curry("O")),

      residentLabel = "Pamięć fizyczna - %s (osoba '%s', gest '%s')";

  if (method === "Sparse Optical Flow") {
    physicalMemoryGestureC = physicalMemoryGestureC.filter(getFirstParameter);
    physicalMemoryGestureO = physicalMemoryGestureO.filter(getFirstParameter);
  }

  result.series.push({
    key: util.format(residentLabel, "minimalne zużycie", "G", "O"),
    values: physicalMemoryGestureO.map(extractSpecialisedMemory.curry("minimum")).sort(byX)
  });

  result.series.push({
    key: util.format(residentLabel, "maksymalne zużycie", "G", "O"),
    values: physicalMemoryGestureO.map(extractSpecialisedMemory.curry("maximum")).sort(byX)
  });

  result.series.push({
    key: util.format(residentLabel, "średnie zużycie", "G", "O"),
    values: physicalMemoryGestureO.map(extractSpecialisedMemory.curry("average")).sort(byX)
  });

  result.series.push({
    key: util.format(residentLabel, "minimalne zużycie", "G", "C"),
    values: physicalMemoryGestureC.map(extractSpecialisedMemory.curry("minimum")).sort(byX)
  });

  result.series.push({
    key: util.format(residentLabel, "maksymalne zużycie", "G", "C"),
    values: physicalMemoryGestureC.map(extractSpecialisedMemory.curry("maximum")).sort(byX)
  });

  result.series.push({
    key: util.format(residentLabel, "średnie zużycie", "G", "C"),
    values: physicalMemoryGestureC.map(extractSpecialisedMemory.curry("average")).sort(byX)
  });

  return result;
};

reporting.memoryUsageForOneGestureAndOnePerson = function(person, gesture) {
  var result = {
        name: util.format("Memory usage - Person '%s' - Gesture '%s'", person, gesture)
      },
      sparseOpticalFlowFile = util.format(FileFormat, person, gesture, "Sparse_Optical_Flow"),
      denseOpticalFlowFile = util.format(FileFormat, person, gesture, "Dense_Optical_Flow"),
      randomForestTrackerFile = util.format(FileFormat, person, gesture, "Random_Forest_Tracker");

  result.series = [
    {
      values: get(sparseOpticalFlowFile, "residentSetMemoryUsage").map(extractMemory),
      key: "Pamięć fizyczna (rzadki przepływ optyczny)"
    },
    {
      values: get(sparseOpticalFlowFile, "virtualMemoryUsage").map(extractMemory),
      key: "Pamięć wirtualna (rzadki przepływ optyczny)"
    },
    {
      values: get(denseOpticalFlowFile, "residentSetMemoryUsage").map(extractMemory),
      key: "Pamięć fizyczna (gęsty przepływ optyczny)"
    },
    {
      values: get(denseOpticalFlowFile, "virtualMemoryUsage").map(extractMemory),
      key: "Pamięć wirtualna (gęsty przepływ optyczny)"
    },
    {
      values: get(randomForestTrackerFile, "residentSetMemoryUsage").map(extractMemory),
      key: "Pamięć fizyczna (las drzew losowych)"
    },
    {
      values: get(randomForestTrackerFile, "virtualMemoryUsage").map(extractMemory),
      key: "Pamięć wirtualna (las drzew losowych)"
    }
  ];

  return result;
};

reporting.residentMemoryForMethod = function(method) {
  var result = {
        name: util.format("Resident memory usage - Method '%s'", method)
      },

      files = getFileByMethodWithoutParametrized(method);

  result.series = files.map(function(file) {
    var data = extractDataFromName(file);

    return {
      values: get(file, "residentSetMemoryUsage").map(extractMemory),
      key: util.format("Pamięć fizyczna (osoba '%s', gest '%s')", data.person, data.gesture)
    };
  });

  return result;
};

reporting.virtualMemoryForMethod = function(method) {
  var result = {
        name: util.format("Virtual memory usage - Method '%s'", method)
      },

      files = getFileByMethodWithoutParametrized(method);

  result.series = files.map(function(file) {
    var data = extractDataFromName(file);

    return {
      values: get(file, "virtualMemoryUsage").map(extractMemory),
      key: util.format("Pamięć wirtualna (osoba '%s', gest '%s')", data.person, data.gesture)
    };
  });

  return result;
};

reporting.residentMemoryForMethodAndGesture = function(method, gesture) {
  var result = {
        name: util.format("Resident memory usage - Gesture '%s' - Method '%s'", gesture, method)
      },

      files = getFileByMethodAndGestureWithoutParametrized(method, gesture);

  result.series = files.map(function(file) {
    var data = extractDataFromName(file);

    return {
      values: get(file, "residentSetMemoryUsage").map(extractMemory),
      key: util.format("Pamięć fizyczna (osoba '%s')", data.person)
    };
  });

  return result;
};

reporting.virtualMemoryForMethodAndGesture = function(method, gesture) {
  var result = {
        name: util.format("Virtual memory usage - Gesture '%s' - Method '%s'", gesture, method)
      },

      files = getFileByMethodAndGestureWithoutParametrized(method, gesture);

  result.series = files.map(function(file) {
    var data = extractDataFromName(file);

    return {
      values: get(file, "virtualMemoryUsage").map(extractMemory),
      key: util.format("Pamięć wirtualna (osoba '%s')", data.person)
    };
  });

  return result;
};

reporting.residentMemoryForMethodAndPerson = function(method, person) {
  var result = {
        name: util.format("Resident memory usage - Person '%s' - Method '%s'", person, method)
      },

      files = getFileByMethodAndPersonWithoutParametrized(method, person);

  result.series = files.map(function(file) {
    var data = extractDataFromName(file);

    return {
      values: get(file, "residentSetMemoryUsage").map(extractMemory),
      key: util.format("Pamięć fizyczna (gest '%s')", data.gesture)
    };
  });

  return result;
};

reporting.virtualMemoryForMethodAndPerson = function(method, person) {
  var result = {
        name: util.format("Virtual memory usage - Person '%s' - Method '%s'", person, method)
      },

      files = getFileByMethodAndPersonWithoutParametrized(method, person);

  result.series = files.map(function(file) {
    var data = extractDataFromName(file);

    return {
      values: get(file, "virtualMemoryUsage").map(extractMemory),
      key: util.format("Pamięć wirtualna (gest '%s')", data.gesture)
    };
  });

  return result;
};

// TODO:
// Quality data preparation.
reporting.quality = function(file) {
  var result = prepareName("Quality", file);

  result.series = [
    { values: [ 1, 2, 3, 4 ].map(extract), key: "Linear" },
    { values: [ 1, 4, 9, 16 ].map(extract), key: "Quadratic" },
    { values: [ 1, 8, 27, 64 ].map(extract), key: "Cubic" }
  ];

  return result;
};

// Timing data preparation.
reporting.timing = function(file) {
  var result = prepareName("Timing", file);

  result.series = [
    { values: [ 1, 2, 3, 4 ].map(extract), key: "Linear" },
    { values: [ 1, 4, 9, 16 ].map(extract), key: "Quadratic" },
    { values: [ 1, 8, 27, 64 ].map(extract), key: "Cubic" }
  ];

  return result;
};

// Overhead data preparation.
reporting.overhead = function(file) {
  var result = prepareName("Overhead", file);

  result.series = [
    { values: [ 1, 2, 3, 4 ].map(extract), key: "Linear" },
    { values: [ 1, 4, 9, 16 ].map(extract), key: "Quadratic" },
    { values: [ 1, 8, 27, 64 ].map(extract), key: "Cubic" }
  ];

  return result;
};