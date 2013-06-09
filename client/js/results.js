(function(Common, d3, nv, canvg) {
  "use strict";

  // Alias and variables.
  var $ = Common.$,

      memoryChart,
      memoryChartWithX,

      qualityChart,
      qualityChartWithX,

      qualityPerSpecial,
      qualityScatterPlot,

      discriteBarChart,
      overheadLinearChart;

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
          .attr("class", config.svgClassName || "")
          .datum(config.series)
            .call(chart);

    d3.select("#charts")
      .insert("h2", ":first-child")
      .text(config.name);

    nv.utils.windowResize(chart.update);
  }

  function horizontalBarChart(config) {
    var chart = nv.models.multiBarHorizontalChart()
                          .x(function(data) { return data.label; })
                          .y(function(data) { return data.value; })
                            .tooltips(false)
                            .showControls(false)
                            .showValues(true);

    chart.yAxis
      .axisLabel(config.yLabel)
      .tickFormat(d3.format(config.yFormat || ".02f"));

    d3.select("#charts")
      .insert("section", ":first-child")
        .append("svg")
          .attr("class", config.svgClassName || "")
          .datum(config.series)
            .call(chart);

    d3.select("#charts")
      .insert("h2", ":first-child")
      .text(config.name);

    nv.utils.windowResize(chart.update);
  }

  function scatterPlot(config) {
    var chart = nv.models.scatterChart()
                          .showDistX(true)
                          .showDistY(true);

    chart.xAxis
      .axisLabel(config.xLabel)
      .tickFormat(d3.format(config.xFormat || ".02f"));

    chart.yAxis
      .axisLabel(config.yLabel)
      .tickFormat(d3.format(config.yFormat || ".02f"));

    d3.select("#charts")
      .insert("section", ":first-child")
        .append("svg")
          .attr("class", config.svgClassName || "")
          .datum(config.series)
            .call(chart);

    d3.select("#charts")
      .insert("h2", ":first-child")
      .text(config.name);

    nv.utils.windowResize(chart.update);
  }

  function discriteBars(config) {
    var chart = nv.models.discreteBarChart()
                          .x(function(point) { return point.label })
                          .y(function(point) { return point.value })
                            .staggerLabels(true)
                            .tooltips(false)
                            .showValues(true);

    chart.yAxis
      .axisLabel(config.yLabel)
      .tickFormat(d3.format(config.yFormat || ".02f"));

    d3.select("#charts")
      .insert("section", ":first-child")
        .append("svg")
          .attr("class", config.svgClassName || "")
          .datum(config.series)
            .call(chart);

    d3.select("#charts")
      .insert("h2", ":first-child")
      .text(config.name);

    nv.utils.windowResize(chart.update);
  }

  function drawHorizontalBarChart(yLabel, className, results) {
    if (typeof(results) === "undefined") {
      results = className;
      className = "";
    }

    horizontalBarChart({
      name: results.name,

      yLabel: yLabel,

      svgClassName: className,

      series: results.series
    });
  }

  function drawLinearChart(yLabel, xLabel, className, results) {
    if (typeof(results) === "undefined") {
      results = className;
      className = "";
    }

    linearChart({
      name: results.name,

      xLabel: xLabel,
      yLabel: yLabel,

      svgClassName: className,

      series: results.series
    });
  }

  function drawScatterPlot(yLabel, xLabel, className, results) {
    if (typeof(results) === "undefined") {
      results = className;
      className = "";
    }

    scatterPlot({
      name: results.name,

      xLabel: xLabel,
      yLabel: yLabel,

      svgClassName: className,

      series: results.series
    });
  }

  function drawDiscriteBarChart(yLabel, className, results) {
    if (typeof(results) === "undefined") {
      results = className;
      className = "";
    }

    discriteBars({
      name: results.name,

      yLabel: yLabel,

      svgClassName: className,

      series: results.series
    });
  }

  qualityChart = drawLinearChart.curry("Odległość [piksel]", "Numer punktu kluczowego");
  qualityChartWithX = drawLinearChart.curry("Odległość [piksel]");

  memoryChart = drawLinearChart.curry("Zużycie pamięci [MB]", "Numer klatki animacji");
  memoryChartWithX = drawLinearChart.curry("Zużycie pamięci [MB]");

  qualityPerSpecial = drawHorizontalBarChart.curry("Odległość i rozmiar [piksel]");

  qualityScatterPlot = drawScatterPlot.curry("Y [piksel]", "X [piksel]");

  discriteBarChart = drawDiscriteBarChart.curry("Narzut czasowy [s]");
  overheadLinearChart = drawLinearChart.curry("Narzut czasowy [s]");

  // Chart Factory.
  function chartsFactory(type, file) {
    var argument,
        special,
        specialName;

    switch(type) {
      case 0:
        d3.json("/charts/memory/file/" + file, memoryChart);
        break;

      case 1:
      case 2:
      case 3:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");
        special = getActiveTextFromSpecialUI();
        specialName = getSpecialName();

        d3.json("/charts/memory/resident/method/" + argument + "/" + specialName + "/" + special,
                 memoryChart);
        break;

      case 4:
      case 5:
      case 6:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");
        special = getActiveTextFromSpecialUI();
        specialName = getSpecialName();

        d3.json("/charts/memory/virtual/method/" + argument + "/" + specialName + "/" + special,
                 memoryChart);
        break;

      case 7:
      case 8:
      case 9:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/memory/resident/method/" + argument, memoryChart.curry("huge"));
        break;

      case 10:
      case 11:
      case 12:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/memory/virtual/method/" + argument, memoryChart.curry("huge"));
        break;

      case 13:
      case 14:
      case 15:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");
        special = getActiveTextFromSpecialUI();
        specialName = getSpecialName();

        d3.json("/charts/memory/resident/method/" + argument + "/" + specialName + "/" + special,
                 memoryChart);
        break;

      case 16:
      case 17:
      case 18:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");
        special = getActiveTextFromSpecialUI();
        specialName = getSpecialName();

        d3.json("/charts/memory/virtual/method/" + argument + "/" + specialName + "/" + special,
                 memoryChart);
        break;

      case 19:
        argument = Common.getOptionText("#people");
        special = Common.getOptionText("#gestures");

        d3.json("/charts/memory/person/" + argument + "/gesture/" + special, memoryChart);
        break;

      case 20:
        special = "Rozmiar siatki"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/memory/specialised/method/" + argument, memoryChartWithX.curry(special));
        break;

      case 21:
        special = "Liczba wytrenowanych drzew losowych"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/memory/specialised/method/" + argument, memoryChartWithX.curry(special));
        break;

      case 22:
        special = "Minimalna odległość między punktami charakterystycznymi"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/memory/specialised/method/" + argument, memoryChartWithX.curry(special));
        break;

      case 23:
        d3.json("/charts/quality/" + file, qualityChart);
        break;

      case 24:
        d3.json("/charts/quality/path/" + file, qualityChart);
        break;

      case 25:
        special = "Rozmiar siatki"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/quality/specialised/method/" + argument, qualityChartWithX.curry(special));
        break;

      case 26:
        special = "Liczba wytrenowanych drzew losowych"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/quality/specialised/method/" + argument, qualityChartWithX.curry(special));
        break;

      case 27:
        special = "Minimalna odległość między punktami charakterystycznymi"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/quality/specialised/method/" + argument, qualityChartWithX.curry(special));
        break;

      case 28:
        special = "Rozmiar siatki"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/quality/specialised/path/method/" + argument, qualityChartWithX.curry(special));
        break;

      case 29:
        special = "Liczba wytrenowanych drzew losowych"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/quality/specialised/path/method/" + argument, qualityChartWithX.curry(special));
        break;

      case 30:
        special = "Minimalna odległość między punktami charakterystycznymi"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/quality/specialised/path/method/" + argument, qualityChartWithX.curry(special));
        break;

      case 31:
      case 32:
      case 33:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");
        special = getActiveTextFromSpecialUI();
        specialName = getSpecialName();

        d3.json("/charts/quality/method/" + argument + "/" + specialName + "/" + special, qualityPerSpecial);
        break;

      case 34:
      case 35:
      case 36:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");
        special = getActiveTextFromSpecialUI();
        specialName = getSpecialName();

        d3.json("/charts/quality/path/method/" + argument + "/" + specialName + "/" + special, qualityPerSpecial);
        break;

      case 37:
      case 38:
      case 39:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");
        special = getActiveTextFromSpecialUI();
        specialName = getSpecialName();

        d3.json("/charts/quality/method/" + argument + "/" + specialName + "/" + special, qualityPerSpecial);
        break;

      case 40:
      case 41:
      case 42:
        argument = getSelectedOptionFromTypes().getAttribute("data-method");
        special = getActiveTextFromSpecialUI();
        specialName = getSpecialName();

        d3.json("/charts/quality/path/method/" + argument + "/" + specialName + "/" + special, qualityPerSpecial);
        break;

      case 43:
        argument = Common.getOptionText("#people");
        special = Common.getOptionText("#gestures");

        d3.json("/charts/quality/person/" + argument + "/gesture/" + special, qualityChart);
        break;

      case 44:
        d3.json("/charts/scatter-plot/file/" + file, qualityScatterPlot);
        break;

      case 45:
        argument = Common.getOptionText("#people");
        special = Common.getOptionText("#gestures");

        d3.json("/charts/overhead/person/" + argument + "/gesture/" + special, discriteBarChart);
        break;

      case 46:
        argument = Common.getOptionText("#people");
        special = Common.getOptionText("#gestures");

        d3.json("/charts/overhead/full/person/" + argument + "/gesture/" + special, discriteBarChart);
        break;

      case 47:
        special = "Rozmiar siatki"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/overhead/specialised/method/" + argument, overheadLinearChart.curry(special));
        break;

      case 48:
        special = "Liczba wytrenowanych drzew losowych"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/overhead/specialised/method/" + argument, overheadLinearChart.curry(special));
        break;

      case 49:
        special = "Minimalna odległość między punktami charakterystycznymi"
        argument = getSelectedOptionFromTypes().getAttribute("data-method");

        d3.json("/charts/overhead/specialised/method/" + argument, overheadLinearChart.curry(special));
        break;
    }
  }

  // Chart types handling and selected options.
  function getSelectedOptionFromTypes() {
    var select = $("#types");

    return select.options[select.selectedIndex];
  }

  function getActiveTextFromSpecialUI() {
    var specialSelector = getSelectedOptionFromTypes().getAttribute("data-special"),
        special = $("#" + specialSelector);

    return special.options[special.selectedIndex].innerText;
  }

  function getSpecialName() {
    var special = getSelectedOptionFromTypes().getAttribute("data-special");

    if (special === "gestures") {
      return "gesture";
    }

    return "person";
  }

  function isChartTypeSpecial() {
    var select = $("#types"),
        selected = select.options[select.selectedIndex];

    return !!selected.getAttribute("data-special");
  }

  function isChartTypeValid() {
    var select = $("#types"),
        selected = select.options[select.selectedIndex];

    return selected.value !== "-1";
  }

  // Clearing.
  function clearCanvas() {
    $("#charts").innerHTML = "";
  }

  // Event handlers.
  function onTypeChanged(event) {
    var files = $("#files"),
        invoke = $("#invoke");

    files.selectedIndex = 0;
    files.setAttribute("disabled");

    invoke.setAttribute("disabled");

    resetSpecialUI();

    if (!isChartTypeValid()) {
      clearCanvas();
    } else {
      if (isChartTypeSpecial()) {
        enableSpecialUI(getSelectedOptionFromTypes().getAttribute("data-special"));
      } else {
        files.removeAttribute("disabled");
      }

      if (!!$("#files").getAttribute("disabled")) {
        invoke.removeAttribute("disabled");
      }
    }
  }

  function onFileChanged(event) {
    var invoke = $("#invoke");

    if (Common.getOptionValue("#files") === -1) {
      clearCanvas();
      invoke.setAttribute("disabled");
    } else {
      invoke.removeAttribute("disabled");
    }
  }

  function onChanged(what) {
    var invoke = $("#invoke");

    if (Common.getOptionValue(what) === -1) {
      clearCanvas();
      invoke.setAttribute("disabled");
    }
  }

  function onInvoke() {
    chartsFactory(Common.getOptionValue("#types"), Common.getOptionText("#files"));
  }

  // Enable UI and hide overlay after loading files.
  function enableSpecialUI(specialElement) {
    var splitted;

    if (specialElement.indexOf(",") !== -1) {
      splitted = specialElement.split(",");

      splitted.forEach(function(element) {
        $("#" + element).removeAttribute("disabled");
      });
    } else if (specialElement !== "both" && specialElement !== "specialised") {
      $("#" + specialElement).removeAttribute("disabled");
    }
  }

  function resetSpecialUI() {
    var gestures = $("#gestures"),
        people = $("#people");

    gestures.selectedIndex = 0;
    gestures.setAttribute("disabled");

    people.selectedIndex = 0;
    people.setAttribute("disabled");
  }

  function enableUI() {
    var files = $("#files"),
        types = $("#types"),

        gestures = $("#gestures"),
        people = $("#people"),

        charts = $("#charts"),

        invoke = $("#invoke");

    types.removeAttribute("disabled");

    types.addEventListener("change", onTypeChanged);
    files.addEventListener("change", onFileChanged);

    gestures.addEventListener("change", onChanged.curry("#gestures"));
    people.addEventListener("change", onChanged.curry("#people"));

    invoke.addEventListener("click", onInvoke);

    Common.hideOverlay();
  }

  // Initialization.
  function init() {
    Common.getCollection(enableUI, "/results", "#files");
  }

  document.addEventListener("DOMContentLoaded", init, true);

} (window.Common, window.d3, window.nv, window.canvg));