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
          .attr("class", config.svgClassName || "")
          .datum(config.series)
            .call(chart);

    d3.select("#charts")
      .insert("h2", ":first-child")
      .text(config.name);

    nv.utils.windowResize(chart.update);
  }

  function memoryChart(className, results) {
    if (typeof(results) === "undefined") {
      results = className;
      className = "";
    }

    linearChart({
      name: results.name,

      xLabel: "Numer klatki animacji",
      yLabel: "Zużycie pamięci [MB]",

      svgClassName: className,

      series: results.series
    });
  }

  function memoryChartWithX(xLabel, results) {
    if (typeof(results) === "undefined") {
      results = className;
      className = "";
    }

    linearChart({
      name: results.name,

      xLabel: xLabel,
      yLabel: "Zużycie pamięci [MB]",

      series: results.series
    });
  }

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

} (window.Common, window.d3, window.nv));