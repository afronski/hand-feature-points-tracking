(function(Common, d3, nv) {
  "use strict";

  // Alias and variables.
  var $ = Common.$;

  // Charts.
  function linearChart(config) {
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
          .datum(config.series)
            .call(chart);

    d3.select("#charts")
      .insert("h2", ":first-child")
      .text(config.name);

    nv.utils.windowResize(chart.update);
  }

  // Chart Factory.
  function chartsFactory(type, file) {
    switch(type) {
      case "Memory":
        d3.json("/charts/memory/" + file, function(results) {
          linearChart({
            name: results.name,

            xLabel: "Numer klatki animacji",
            yLabel: "Zużycie pamięci [MB]",

            series: results.series
          });
        });
        break;
    }
  }

  // Chart types handling.
  function isChartTypeSpecial() {
    var select = $("#types"),
        selected = select.options[select.selectedIndex];

    return selected.getAttribute("data-special") === "true";
  }

  function isChartTypeValid() {
    var select = $("#types"),
        selected = select.options[select.selectedIndex];

    return selected.value !== "-1" && selected.getAttribute("data-special") !== "true";
  }

  // Clearing.
  function clearCanvas() {
    $("#charts").innerHTML = "";
  }

  // Changing type and file.
  function onTypeChanged(event) {
    var files = $("#files");

    files.selectedIndex = 0;

    if (!isChartTypeValid()) {
      files.setAttribute("disabled");

      clearCanvas();
    } else {
      if (isChartTypeSpecial()) {
        // TODO: Special charts.
      } else {
        files.removeAttribute("disabled");
      }
    }
  }

  function onFileChanged(event) {
    if (Common.getOptionValue("#files") !== -1) {
      chartsFactory(Common.getOptionText("#types"), Common.getOptionText("#files"));
    } else {
      clearCanvas();
    }
  }

  // Enable UI and hide overlay after loading files.
  function enableUI() {
    var types = $("#types");

    types.removeAttribute("disabled");

    types.addEventListener("change", onTypeChanged);
    files.addEventListener("change", onFileChanged)

    Common.hideOverlay();
  }

  // Initialization.
  function init() {
    Common.getCollection(enableUI, "/results", "#files");
  }

  document.addEventListener("DOMContentLoaded", init, true);

} (window.Common, window.d3, window.nv));