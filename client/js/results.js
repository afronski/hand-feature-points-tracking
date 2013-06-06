(function(Common, d3, nv) {
  "use strict";

  // Alias and variables.
  var $ = Common.$,
      jsonFiles = [];

  // Get JSON file URI.
  function getActualJsonUrl(url) {
    return "/" + url;
  }

  // Filters.
  function extractMemoryData(results) {
    var toMB = 1024,

        extractMemory = function(element, index) {
          return { x: index + 1, y: element / toMB };
        },

        residentSparse = results.residentSetMemoryUsageSparse.map(extractMemory),
        residentDense = results.residentSetMemoryUsageDense.map(extractMemory),
        residentRandom = results.residentSetMemoryUsageRandom.map(extractMemory);

    return [
      {
        values: residentSparse,
        key: "Pamięć fizyczna (rzadki przepływ optyczny)",
        color: "#FF7F0E"
      },
      {
        values: residentDense,
        key: "Pamięć fizyczna (gęsty przepływ optyczny)",
        color: "#2CA02C"
      },
      {
        values: residentRandom,
        key: "Pamięć fizyczna (las drzew losowych)",
        color: "#C20AC2"
      }
    ];
  }

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
          .transition()
          .duration(config.onShowTransitionDuration || 500)
            .call(chart);

    d3.select("#charts")
      .insert("h2", ":first-child")
      .text(config.name);

    nv.utils.windowResize(chart.update);
  }

  function chartsFactory(name) {
    var create = function() {
      $("#charts").innerHTML = "";
    };

    switch(name) {
      case "Memory":
        create = function() {
          var counter = 3,

              remappedResults,
              results = {},

              execute = function() {
                remappedResults = {
                  residentSetMemoryUsageSparse: results["Sparse"].residentSetMemoryUsage,
                  residentSetMemoryUsageDense: results["Dense"].residentSetMemoryUsage,
                  residentSetMemoryUsageRandom: results["Random"].residentSetMemoryUsage
                };

                linearChart({
                  name: "Person 'A' - Gesture 'C' - Memory usage",

                  xLabel: "Numer klatki animacji",
                  yLabel: "Zużycie pamięci [MB]",

                  extractor: extractMemoryData
                }, remappedResults);
              },

              handleFiles = function(name, result) {
                results[name] = result;

                --counter;

                if (counter <= 0) {
                  execute();
                }
              },

              i;

          for(i = 0; i < 3; ++i) {
            var name = jsonFiles[i].name,
                key = "Random";

            if (name.indexOf("Sparse") !== -1) {
              key = "Sparse";
            }

            if (name.indexOf("Dense") !== -1) {
              key = "Dense";
            }

            d3.json(getActualJsonUrl(name), handleFiles.bind(null, key));
          }
        };
      break;
    }

    return {
      create: create
    }
  }

  // Changing result.
  function onTypeChanged(event) {
    chartsFactory(Common.getOptionText("#types")).create();
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