    "use strict";

var fs = require("fs"),
    path = require("path"),
    util = require("util"),

    common = require("./common"),

    repository = require("./repository"),

    ToMB = 1024,
    FileFormat = "Person_%s_%s_results_for_%s.json",

    Gestures = {
      "C": "Litera C",
      "Crush": "Zgniatanie",
      "L": "Litera L",
      "O": "Okrąg",
      "Stretch": "Rozszerzanie",
      "X": "Krzyż"
    },

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

function extractSpecialised(key, element) {
  return { x: element.index, y: element[key] };
}

function extractCollectedErrors(element, index) {
  return { x: index + 1, y: element.value };
}

function extractWithGesture(key, element) {
  return { label: element.gesture, value: element[key] };
}

function extractWithPerson(key, element) {
  return { label: element.person, value: element[key] };
}

function mapToClosest(average, element, index) {
  return average[element.key];
}

function extractDataPointForScatterPlot(sizes, point, index) {
  return {
    x: point[0],
    y: -point[1],
    size: sizes[index].value / max(sizes.map(convertToPoint)),
  };
}

function extractDataPointForScatterPlotWithSize(size, point) {
  return {
    x: point[0],
    y: -point[1],
    size: size
  };
}

function toPoint(element, index) {
  return { x: index + 1, y: element };
}

function convertToMB(value) {
  return value / ToMB;
}

function convertToPoint(element) {
  return element.value;
}

function repeat(value, times) {
  var array = [],
      i;

  for (i = 0; i < times; ++i) {
    array.push(value);
  }

  return array;
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

function avgFromValue(array) {
  return avg(array.map(convertToPoint));
}

// Common methods.
function prepareName(chartName, file) {
  var data = extractDataFromName(file);

  return {
    name: util.format("%s - Person '%s' - Gesture '%s' - Method '%s'",
                      chartName, data.person, data.gesture, data.method)
  }
}

function toSeconds(value) {
  return value / 1000.0;
}

function byInteger(a, b) {
  return a.x - b.x;
}

function byString(a, b) {
  return a.label.localeCompare(b.label);
}

function get(file, key) {
  return JSON.parse(fs.readFileSync("./assets/" + file))[key];
}

function getBoundingCircleSeries(radius, errors) {
  return {
    values: repeat(radius, errors.length).map(toPoint),
    key: "Promień okręgu otaczającego",
    color: "#FF0000"
  };
}

function getFirstParameter(point) {
  return point.parameters.indexOf(" ") === -1;
}

function getOnlyOneGesture(gesture, point) {
  return point.gesture === gesture;
}

function getDataPointFromMemoryKeyInFile(key, file) {
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

function getSpecialisedDataPointsFromErrorKeyInFile(key, file) {
  var data = extractDataAndParametersFromName(file),
      errors = get(file, key),
      rawParameters = data.parameters.indexOf(" ") !== -1 ?
                        data.parameters.split(" ")[1] :
                        data.parameters;

  return {
    minimum: min(errors.map(convertToPoint)),
    maximum: max(errors.map(convertToPoint)),
    average: avg(errors.map(convertToPoint)),
    radius: get(file, "pointsBoundaryRadius"),
    index: parseInt(rawParameters, 10),
    parameters: data.parameters,
    person: data.person,
    gesture: data.gesture
  };
}

function getDataPointFromErrorKeyInFile(key, file) {
  var data = extractDataFromName(file),
      errors = get(file, key);

  return {
    minimum: min(errors.map(convertToPoint)),
    maximum: max(errors.map(convertToPoint)),
    average: avg(errors.map(convertToPoint)),
    radius: get(file, "pointsBoundaryRadius"),
    index: Gestures[data.gesture],
    person: data.person,
    gesture: Gestures[data.gesture]
  };
}

function extractVideoDurationAndProcessingTime(file) {
  var data = extractDataAndParametersFromName(file),
      videoDuration = get(file, "videoDuration"),
      videoProcessingTime = get(file, "totalVideoProcessingTime"),
      rawParameters = data.parameters.indexOf(" ") !== -1 ?
                        data.parameters.split(" ")[1] :
                        data.parameters;

  return {
    value: toSeconds(videoProcessingTime - videoDuration),
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

      physical = files.map(getDataPointFromMemoryKeyInFile.curry("residentSetMemoryUsage")),

      physicalMemoryGestureC = physical.filter(getOnlyOneGesture.curry("C")),
      physicalMemoryGestureO = physical.filter(getOnlyOneGesture.curry("O")),

      label = "Pamięć fizyczna - %s (osoba '%s', gest '%s')";

  if (method === "Sparse Optical Flow") {
    physicalMemoryGestureC = physicalMemoryGestureC.filter(getFirstParameter);
    physicalMemoryGestureO = physicalMemoryGestureO.filter(getFirstParameter);
  }

  result.series.push({
    key: util.format(label, "minimalne zużycie", "G", "O"),
    values: physicalMemoryGestureO.map(extractSpecialised.curry("minimum")).sort(byInteger)
  });

  result.series.push({
    key: util.format(label, "maksymalne zużycie", "G", "O"),
    values: physicalMemoryGestureO.map(extractSpecialised.curry("maximum")).sort(byInteger)
  });

  result.series.push({
    key: util.format(label, "średnie zużycie", "G", "O"),
    values: physicalMemoryGestureO.map(extractSpecialised.curry("average")).sort(byInteger)
  });

  result.series.push({
    key: util.format(label, "minimalne zużycie", "G", "C"),
    values: physicalMemoryGestureC.map(extractSpecialised.curry("minimum")).sort(byInteger)
  });

  result.series.push({
    key: util.format(label, "maksymalne zużycie", "G", "C"),
    values: physicalMemoryGestureC.map(extractSpecialised.curry("maximum")).sort(byInteger)
  });

  result.series.push({
    key: util.format(label, "średnie zużycie", "G", "C"),
    values: physicalMemoryGestureC.map(extractSpecialised.curry("average")).sort(byInteger)
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

// Quality data preparation.
reporting.quality = function(file) {
  var result = prepareName("Quality - Keypoint distance", file),
      errors = get(file, "collectedErrors");

  result.series = [
    {
      values: errors.map(extractCollectedErrors),
      key: "Odległości od punktów kluczowych"
    },
    getBoundingCircleSeries(get(file, "pointsBoundaryRadius"), errors)
  ];

  return result;
};

reporting.qualityPath = function(file) {
  var result = prepareName("Quality - Path distance", file),
      errors = get(file, "collectedPathErrors");

  result.series = [
    {
      values: errors.map(extractCollectedErrors),
      key: "Odległości od ścieżek kluczowych"
    },
    getBoundingCircleSeries(get(file, "pointsBoundaryRadius"), errors)
  ];

  return result;
};

function specialisedQuality(key, name, method) {
  var result = {
        name: util.format(name, method),
        series: []
      },

      files = getFileByMethodAndOnlyParametrized(method),

      errors = files.map(getSpecialisedDataPointsFromErrorKeyInFile.curry(key)),

      errorsGestureC = errors.filter(getOnlyOneGesture.curry("C")),
      errorsGestureO = errors.filter(getOnlyOneGesture.curry("O")),

      label = "Odległości %s od punktów kluczowych (osoba '%s', gest '%s')";

  if (method === "Sparse Optical Flow") {
    errorsGestureC = errorsGestureC.filter(getFirstParameter);
    errorsGestureO = errorsGestureO.filter(getFirstParameter);
  }

  result.series.push({
    key: "Promień okręgu otaczającego dla gestu O",
    values: errorsGestureO.map(extractSpecialised.curry("radius")).sort(byInteger),
    color: "#FF0000"
  });

  result.series.push({
    key: util.format(label, "minimalne", "G", "O"),
    values: errorsGestureO.map(extractSpecialised.curry("minimum")).sort(byInteger),
    color: "#0000FF"
  });

  result.series.push({
    key: util.format(label, "maksymalne", "G", "O"),
    values: errorsGestureO.map(extractSpecialised.curry("maximum")).sort(byInteger),
    color: "#00FF00"
  });

  result.series.push({
    key: util.format(label, "średnie", "G", "O"),
    values: errorsGestureO.map(extractSpecialised.curry("average")).sort(byInteger),
    color: "#00FFFF"
  });

  result.series.push({
    key: "Promień okręgu otaczającego dla gestu C",
    values: errorsGestureC.map(extractSpecialised.curry("radius")).sort(byInteger),
    color: "#880000"
  });

  result.series.push({
    key: util.format(label, "minimalne", "G", "C"),
    values: errorsGestureC.map(extractSpecialised.curry("minimum")).sort(byInteger),
    color: "#000088"
  });

  result.series.push({
    key: util.format(label, "maksymalne", "G", "C"),
    values: errorsGestureC.map(extractSpecialised.curry("maximum")).sort(byInteger),
    color: "#008800"
  });

  result.series.push({
    key: util.format(label, "średnie", "G", "C"),
    values: errorsGestureC.map(extractSpecialised.curry("average")).sort(byInteger),
    color: "#008888"
  });

  return result;
};

reporting.qualitySpecialisedForMethod = specialisedQuality.curry(
                                                            "collectedErrors",
                                                            "Quality - Keypoints distance - Method '%s'");

reporting.qualityPathSpecialisedForMethod = specialisedQuality.curry(
                                                            "collectedPathErrors",
                                                            "Quality - Path distance - Method '%s'");

function qualityForMethodAndPerson(key, name, method, person) {
  var result = {
      name: util.format(name, method, person),
      series: []
    },

    files = getFileByMethodAndPersonWithoutParametrized(method, person),

    label = "Odległości %s od punktów kluczowych",

    circles = [],
    minimum = [],
    maximum = [],
    average = [],

    errors,
    i;

  for (i = 0; i < files.length; ++i) {
    errors = getDataPointFromErrorKeyInFile(key, files[i]);

    circles.push(extractWithGesture("radius", errors));
    minimum.push(extractWithGesture("minimum", errors));
    maximum.push(extractWithGesture("maximum", errors));
    average.push(extractWithGesture("average", errors));
  }

  result.series.push({
    key: util.format(label, "minimalne"),
    values: minimum.sort(byString)
  });

  result.series.push({
    key: util.format(label, "maksymalne"),
    values: maximum.sort(byString)
  });

  result.series.push({
    key: util.format(label, "średnie"),
    values: average.sort(byString)
  });

  result.series.push({
    key: "Wartości promieni okręgów otaczających dla każdego gestu",
    values: circles.sort(byString),
    color: "#FF0000"
  });

  return result;
}

reporting.qualityForMethodAndPerson = qualityForMethodAndPerson.curry(
                                        "collectedErrors",
                                        "Quality - Keypoints distance - Method '%s' - Person '%s'");

reporting.qualityPathForMethodAndPerson = qualityForMethodAndPerson.curry(
                                        "collectedPathErrors",
                                        "Quality - Path distance - Method '%s' - Person '%s'");

function qualityForMethodAndGesture(key, name, method, gesture) {
  var result = {
      name: util.format(name, method, gesture),
      series: []
    },

    files = getFileByMethodAndGestureWithoutParametrized(method, gesture),

    label = "Odległości %s od punktów kluczowych",

    circles = [],
    minimum = [],
    maximum = [],
    average = [],

    errors,
    i;

  for (i = 0; i < files.length; ++i) {
    errors = getDataPointFromErrorKeyInFile(key, files[i]);
    radius = extractWithPerson("radius", errors);

    minimum.push(extractWithPerson("minimum", errors));
    maximum.push(extractWithPerson("maximum", errors));
    average.push(extractWithPerson("average", errors));
  }

  result.series.push({
    key: util.format(label, "minimalne"),
    values: minimum.sort(byString)
  });

  result.series.push({
    key: util.format(label, "maksymalne"),
    values: maximum.sort(byString)
  });

  result.series.push({
    key: util.format(label, "średnie"),
    values: average.sort(byString)
  });

  result.series.push({
    key: "Wartości promieni okręgów otaczających dla każdej osoby",
    values: circles.sort(byString),
    color: "#FF0000"
  });

  return result;
}

reporting.qualityForMethodAndGesture = qualityForMethodAndGesture.curry(
                                        "collectedErrors",
                                        "Quality - Keypoints distance - Method '%s' - Gesture '%s'");

reporting.qualityPathForMethodAndGesture = qualityForMethodAndGesture.curry(
                                            "collectedPathErrors",
                                            "Quality - Path distance - Method '%s' - Gesture '%s'");

reporting.qualityForPersonAndGesture = function(person, gesture) {
  var result = {
        name: util.format("Quality - Person '%s' - Gesture '%s'", person, gesture)
      },
      sparseOpticalFlowFile = util.format(FileFormat, person, gesture, "Sparse_Optical_Flow"),
      denseOpticalFlowFile = util.format(FileFormat, person, gesture, "Dense_Optical_Flow"),
      randomForestTrackerFile = util.format(FileFormat, person, gesture, "Random_Forest_Tracker"),

      sparseKeypointErrors = get(sparseOpticalFlowFile, "collectedErrors"),
      denseKeypointErrors = get(denseOpticalFlowFile, "collectedErrors"),
      randomKeypointErrors = get(randomForestTrackerFile, "collectedErrors"),

      sparsePathErrors = get(sparseOpticalFlowFile, "collectedPathErrors"),
      densePathErrors = get(denseOpticalFlowFile, "collectedPathErrors"),
      randomPathErrors = get(randomForestTrackerFile, "collectedPathErrors");

  result.series = [
    {
      values: sparseKeypointErrors.map(extractCollectedErrors),
      key: "Odległość od punktów kluczowych (rzadki przepływ optyczny)"
    },
    {
      values: sparsePathErrors.map(extractCollectedErrors),
      key: "Odległość od ścieżek kluczowych (rzadki przepływ optyczny)"
    },
    {
      values: denseKeypointErrors.map(extractCollectedErrors),
      key: "Odległość od punktów kluczowych (gęsty przepływ optyczny)"
    },
    {
      values: densePathErrors.map(extractCollectedErrors),
      key: "Odległość od ścieżek kluczowych (gęsty przepływ optyczny)"
    },
    {
      values: randomKeypointErrors.map(extractCollectedErrors),
      key: "Odległość od punktów kluczowych (las drzew losowych)"
    },
    {
      values: randomPathErrors.map(extractCollectedErrors),
      key: "Odległość od ścieżek kluczowych (las drzew losowych)"
    },

    {
      values: repeat(avgFromValue(sparseKeypointErrors), sparseKeypointErrors.length).map(toPoint),
      key: "Średnia odległość od punktów kluczowych (rzadki przepływ optyczny)"
    },
    {
      values: repeat(avgFromValue(sparsePathErrors), sparsePathErrors.length).map(toPoint),
      key: "Średnia odległość od ścieżek kluczowych (rzadki przepływ optyczny)"
    },
    {
      values: repeat(avgFromValue(denseKeypointErrors), denseKeypointErrors.length).map(toPoint),
      key: "Średnia odległość od punktów kluczowych (gęsty przepływ optyczny)"
    },
    {
      values: repeat(avgFromValue(densePathErrors), densePathErrors.length).map(toPoint),
      key: "Średnia odległość od ścieżek kluczowych (gęsty przepływ optyczny)"
    },
    {
      values: repeat(avgFromValue(randomKeypointErrors), randomKeypointErrors.length).map(toPoint),
      key: "Średnia odległość od punktów kluczowych (las drzew losowych)"
    },
    {
      values: repeat(avgFromValue(randomPathErrors), randomPathErrors.length).map(toPoint),
      key: "Średnia odległość od ścieżek kluczowych (las drzew losowych)"
    },
  ];

  return result;
};

reporting.qualityScatterPlot = function(file) {
  var result = prepareName("Quality - Scatter Plot", file),
      errors = get(file, "collectedErrors"),
      average = get(file, "averagePoints");

  result.series = [
    {
      values: get(file, "baseKeypoints").map(extractDataPointForScatterPlot.curry(errors)),
      key: "Punkty kluczowe",
      color: "#FF0000"
    },
    {
      values: average.map(extractDataPointForScatterPlotWithSize.curry(0.1)),
      key: "Uśrednione punkty na wyznaczonej ścieżce",
      color: "#008800"
    },
    {
      values: errors.map(mapToClosest.curry(average)).map(extractDataPointForScatterPlotWithSize.curry(0.3)),
      key: "Nabliższy uśredniony punkt z wyznaczonej ścieżki",
      color: "#0000FF"
    }
  ];

  return result;
};

// Overhead data preparation.
reporting.overhead = function(person, gesture) {
  var result = {
        name: util.format("Overhead - Person '%s' - Gesture '%s'", person, gesture)
      },

      sparseOpticalFlowFile = util.format(FileFormat, person, gesture, "Sparse_Optical_Flow"),
      denseOpticalFlowFile = util.format(FileFormat, person, gesture, "Dense_Optical_Flow"),
      randomForestTrackerFile = util.format(FileFormat, person, gesture, "Random_Forest_Tracker"),

      sparseVideoDuration = get(sparseOpticalFlowFile, "videoDuration"),
      denseVideoDuration = get(denseOpticalFlowFile, "videoDuration"),
      randomVideoDuration = get(randomForestTrackerFile, "videoDuration"),

      sparseVideoProcessingDuration = get(sparseOpticalFlowFile, "totalVideoProcessingTime"),
      denseVideoProcessingDuration = get(denseOpticalFlowFile, "totalVideoProcessingTime"),
      randomVideoProcessingDuration = get(randomForestTrackerFile, "totalVideoProcessingTime");

  result.series = [
    {
      key: "Narzut czasowy",
      values: [
        {
          label: "Rzadki przepływ optyczny",
          value: toSeconds(sparseVideoProcessingDuration - sparseVideoDuration)
        },
        {
          label: "Las drzew losowych",
          value: toSeconds(randomVideoProcessingDuration - randomVideoDuration)
        },
        {
          label: "Gęsty przepływ optyczny",
          value: toSeconds(denseVideoProcessingDuration - denseVideoDuration)
        }
      ]
    }
  ];

  return result;
};

reporting.fullOverheadForRandomForestTracker = function(person, gesture) {
  var result = {
        name: util.format("Overhead - Random Forest Tracker - Person '%s' - Gesture '%s'", person, gesture)
      },

      randomForestTrackerFile = util.format(FileFormat, person, gesture, "Random_Forest_Tracker"),

      randomVideoDuration = get(randomForestTrackerFile, "videoDuration"),
      randomVideoProcessingDuration = get(randomForestTrackerFile, "totalVideoProcessingTime"),

      randomLearningDuration = get(randomForestTrackerFile, "trainingTimeOverhead") +
                               get(randomForestTrackerFile, "loadingTrainingBaseOverhead"),

      randomTrainingBaseCreationDuration = get(randomForestTrackerFile, "buildingTrainingBaseTimeOverhead") +
                                           get(randomForestTrackerFile, "savingTrainingBaseOverhead");

  result.series = [
    {
      key: "Narzut czasowy",
      values: [
        {
          label: "Bez uczenia oraz generacji danych treningowych",
          value: toSeconds(randomVideoProcessingDuration - randomVideoDuration)
        },
        {
          label: "Bez generacji danych treningowych",
          value: toSeconds(randomLearningDuration + randomVideoProcessingDuration - randomVideoDuration)
        },
        {
          label: "Całkowity narzut",
          value: toSeconds(randomTrainingBaseCreationDuration + randomLearningDuration +
                           randomVideoProcessingDuration - randomVideoDuration)
        }
      ]
    }
  ];

  return result;
}

reporting.fullOverheadForSpecialisedMethod = function(method) {
  var result = {
        name: util.format("Overhead - Method '%s'", method),
        series: []
      },

      files = getFileByMethodAndOnlyParametrized(method),

      videosDuration = files.map(extractVideoDurationAndProcessingTime),

      timeOverheadGestureC = videosDuration.filter(getOnlyOneGesture.curry("C")),
      timeOverheadGestureO = videosDuration.filter(getOnlyOneGesture.curry("O")),

      label = "Narzut czasowy (osoba '%s', gest '%s')";

  if (method === "Sparse Optical Flow") {
    timeOverheadGestureC = timeOverheadGestureC.filter(getFirstParameter);
    timeOverheadGestureO = timeOverheadGestureO.filter(getFirstParameter);
  }

  result.series.push({
    key: util.format(label, "G", "O"),
    values: timeOverheadGestureO.map(extractSpecialised.curry("value")).sort(byInteger)
  });

  result.series.push({
    key: util.format(label, "G", "C"),
    values: timeOverheadGestureC.map(extractSpecialised.curry("value")).sort(byInteger)
  });

  return result;
};

// TODO:
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