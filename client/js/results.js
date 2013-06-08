(function(Common, d3, nv) {
  "use strict";

  // Alias and variables.
  var $ = Common.$,

      toMB = 1024,

      jsonFiles = [];

  // Utility methods.
  function getPersonAndGestureFromName(name) {
    var splitted = name.split("_");

    return {
      person: splitted[1],
      gesture: splitted[2]
    };
  }

  function extractParameter(name) {
    var result = /.*?\((.*?)\).*?/gi.exec(name);

    return result ? result[1] : "";
  }

  // Get JSON file URI.
  function getActualJsonUrl(url) {
    return "/" + url;
  }

  // Filters - Memory.
  function extractMemory(element, index) {
    return { x: index + 1, y: element / toMB };
  }

  function extractSpecialisedMemoryData(results) {
    var output = [],
        name;

    for (name in results) {
      if (results.hasOwnProperty(name)) {
        output.push({
          values: results[name].map(extractMemory),
          key: "Pamięć fizyczna (parametry wywołania: " + name + ")"
        });
      }
    }

    return output;
  }

  function extractMemoryData(results) {
    var residentSparse = results.residentSetMemoryUsageSparse.map(extractMemory),
        residentDense = results.residentSetMemoryUsageDense.map(extractMemory),
        residentRandom = results.residentSetMemoryUsageRandom.map(extractMemory),

        virtualSparse = results.virtualSetMemoryUsageSparse.map(extractMemory),
        virtualDense = results.virtualSetMemoryUsageDense.map(extractMemory),
        virtualtRandom = results.virtualSetMemoryUsageRandom.map(extractMemory);

    return [
      {
        values: virtualSparse,
        key: "Pamięć wirtualna (rzadki przepływ optyczny)"
      },
      {
        values: residentSparse,
        key: "Pamięć fizyczna (rzadki przepływ optyczny)"
      },
      {
        values: virtualDense,
        key: "Pamięć wirtualna (gęsty przepływ optyczny)",
      },
      {
        values: residentDense,
        key: "Pamięć fizyczna (gęsty przepływ optyczny)",
      },
      {
        values: virtualtRandom,
        key: "Pamięć wirtualna (las drzew losowych)",
      },
      {
        values: residentRandom,
        key: "Pamięć fizyczna (las drzew losowych)",
      }
    ];
  }

  // Charts.
  function linearChart(config, results) {
    var chart = nv.models.lineChart();

    chart.xAxis
        .axisLabel(config.xLabel)
        .tickFormat(d3.format(config.xFormat || ",r"));

    chart.yAxis
        .axisLabel(config.yLabel)
        .tickFormat(d3.format(config.yFormat || ".02f"));

    d3.select("#charts")
      .insert("section", ":first-child")
        .append("svg")
          .datum(config.extractor(results))
            .call(chart);

    d3.select("#charts")
      .insert("h2", ":first-child")
      .text(config.name);

    nv.utils.windowResize(chart.update);
  }

  // Helpers.
  function parallel(arrayOfLinks, done, mapper) {
    var counter = arrayOfLinks.length,
        actualMapper = mapper || function(input) { return input; },
        allResults = {},

        downloadCompleted = function(name, result) {
          allResults[name] = result;

          --counter;

          if (counter <= 0) {
            done(actualMapper(allResults));
          }
        };

    arrayOfLinks.forEach(function(object, index) {
      d3.json(getActualJsonUrl(object.link), downloadCompleted.curry(object.name || index.toString()));
    });
  }

  // Data preparation - Memory charts.
  function memoryUsageForAllPeopleAndGestures() {
    var execute = function(person, gesture, results) {
          linearChart({
            name: "Person '" + person + "' - Gesture '" + gesture + "' - Memory usage",

            xLabel: "Numer klatki animacji",
            yLabel: "Zużycie pamięci [MB]",

            extractor: extractMemoryData
          }, results);
        },

        mapper = function(results) {
          return {
            residentSetMemoryUsageSparse: results["Sparse"].residentSetMemoryUsage,
            residentSetMemoryUsageDense: results["Dense"].residentSetMemoryUsage,
            residentSetMemoryUsageRandom: results["Random"].residentSetMemoryUsage,

            virtualSetMemoryUsageSparse: results["Sparse"].virtualMemoryUsage,
            virtualSetMemoryUsageDense: results["Dense"].virtualMemoryUsage,
            virtualSetMemoryUsageRandom: results["Random"].virtualMemoryUsage
          };
        },

        peopleResults,
        i, j, k,

        invokeParallel = function(files, k) {
          var data = getPersonAndGestureFromName(files[k].name);

          parallel([
            { name: "Dense",  link: files[k    ].name },
            { name: "Random", link: files[k + 1].name },
            { name: "Sparse", link: files[k + 2].name }
          ], execute.curry(data.person, data.gesture), mapper);
        };

    peopleResults = jsonFiles.filter(function(element) {
      return element.name.indexOf("parameters") === -1;
    });

    for (k = 0; k < peopleResults.length; k += 3) {
      setTimeout(invokeParallel.curry(peopleResults, k), 500);
    }
  }

  function memoryUsageForSpecialisedRandomForestTracker() {
    var execute = function(person, gesture, results) {
          linearChart({
            name: "Person '" + person +
                   "' - Gesture '" + gesture +
                   "' - Random Forest Tracker - Memory usage",

            xLabel: "Numer klatki animacji",
            yLabel: "Zużycie pamięci [MB]",

            extractor: extractSpecialisedMemoryData
          }, results);
        },

        mapper = function(results) {
          var key;

          for(key in results) {
            if (results.hasOwnProperty(key)) {
              results[key] = results[key].residentSetMemoryUsage;
            }
          }

          return results;
        },

        specialised,
        i, k,

        invokeParallel = function(files) {
          var data = getPersonAndGestureFromName(files[0].orginalName);

          parallel(files, execute.curry(data.person, data.gesture), mapper);
        },

        files = [];

    specialised = jsonFiles.filter(function(element) {
      return element.name.indexOf("Random_Forest_Tracker_parameters") !== -1;
    });

    for (i = 0; i < 2; ++i) {
      files = [];

      for (k = 0; k < specialised.length / 2; ++k) {
        files.push({
          orginalName: specialised[k + i * specialised.length / 2].name,
          name: extractParameter(specialised[k + i * specialised.length / 2].name),
          link: specialised[k + i * specialised.length / 2].name
        });
      }

      invokeParallel(files);
    }
  }

  function memoryUsageForSpecialisedSparseOpticalFlow() {
    var execute = function(person, gesture, results) {
          linearChart({
            name: "Person '" + person +
                   "' - Gesture '" + gesture +
                   "' - Sparse Optical Flow - Memory usage",

            xLabel: "Numer klatki animacji",
            yLabel: "Zużycie pamięci [MB]",

            extractor: extractSpecialisedMemoryData
          }, results);
        },

        mapper = function(results) {
          var key;

          for(key in results) {
            if (results.hasOwnProperty(key)) {
              results[key] = results[key].residentSetMemoryUsage;
            }
          }

          return results;
        },

        specialised,
        i, k,

        invokeParallel = function(files) {
          var data = getPersonAndGestureFromName(files[0].orginalName);

          parallel(files, execute.curry(data.person, data.gesture), mapper);
        },

        files = [];

    specialised = jsonFiles.filter(function(element) {
      return element.name.indexOf("Sparse_Optical_Flow_parameters(10 ") !== -1;
    });

    for (i = 0; i < 2; ++i) {
      files = [];

      for (k = 0; k < specialised.length / 2; ++k) {
        files.push({
          orginalName: specialised[k + i * specialised.length / 2].name,
          name: extractParameter(specialised[k + i * specialised.length / 2].name),
          link: specialised[k + i * specialised.length / 2].name
        });
      }

      invokeParallel(files);
    }

    specialised = jsonFiles.filter(function(element) {
      return element.name.search(/.*?Sparse_Optical_Flow_parameters\(\d+\).*?/gi) !== -1;
    });

    for (i = 0; i < 2; ++i) {
      files = [];

      for (k = 0; k < specialised.length / 2; ++k) {
        files.push({
          orginalName: specialised[k + i * specialised.length / 2].name,
          name: extractParameter(specialised[k + i * specialised.length / 2].name),
          link: specialised[k + i * specialised.length / 2].name
        });
      }

      invokeParallel(files);
    }
  }

  function memoryUsageForSpecialisedDenseOpticalFlow() {
    var execute = function(person, gesture, results) {
          linearChart({
            name: "Person '" + person +
                   "' - Gesture '" + gesture +
                   "' - Dense Optical Flow - Memory usage",

            xLabel: "Numer klatki animacji",
            yLabel: "Zużycie pamięci [MB]",

            extractor: extractSpecialisedMemoryData
          }, results);
        },

        mapper = function(results) {
          var key;

          for(key in results) {
            if (results.hasOwnProperty(key)) {
              results[key] = results[key].residentSetMemoryUsage;
            }
          }

          return results;
        },

        specialised,
        i, k,

        invokeParallel = function(files) {
          var data = getPersonAndGestureFromName(files[0].orginalName);

          parallel(files, execute.curry(data.person, data.gesture), mapper);
        },

        files = [];

    specialised = jsonFiles.filter(function(element) {
      return element.name.indexOf("Dense_Optical_Flow_parameters") !== -1;
    });

    for (i = 0; i < 2; ++i) {
      files = [];

      for (k = 0; k < specialised.length / 2; ++k) {
        files.push({
          orginalName: specialised[k + i * specialised.length / 2].name,
          name: extractParameter(specialised[k + i * specialised.length / 2].name),
          link: specialised[k + i * specialised.length / 2].name
        });
      }

      invokeParallel(files);
    }
  }

  // Chart Factory.
  function chartsFactory(type) {
    var create = function() {
      $("#charts").innerHTML = "";
    };

    switch(type) {
      case 0:
        create = memoryUsageForAllPeopleAndGestures;
        break;

      case 1:
        create = memoryUsageForSpecialisedRandomForestTracker;
        break;

      case 2:
        create = memoryUsageForSpecialisedSparseOpticalFlow;
        break;

      case 3:
        create = memoryUsageForSpecialisedDenseOpticalFlow;
        break;
    }

    return {
      create: create
    }
  }

  // Changing result.
  function onTypeChanged(event) {
    chartsFactory(Common.getOptionValue("#types")).create();
  }

  // Enable UI and hide overlay after loading files.
  function enableUI() {
    var types = $("#types");

    jsonFiles = JSON.parse(this.responseText);

    types.removeAttribute("disabled");
    types.addEventListener("change", onTypeChanged);

    Common.hideOverlay();
  }

  // Initialization.
  function init() {
    Common.xhrGet("/results", enableUI);
  }

  document.addEventListener("DOMContentLoaded", init, true);

} (window.Common, window.d3, window.nv));