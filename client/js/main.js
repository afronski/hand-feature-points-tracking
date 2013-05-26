(function(Common) {
  "use strict";

  // Alias.
  var $ = Common.$,
      AlgorithmsParameterSelector = "[data-algorithm-id='%id']";

  // Additional arguments logic.
  function getAdditionalArgumentsFor(algorithmId) {
    var result = [];

    switch (algorithmId) {
      // Sparse Optical Flow.
      case 0:
        result.push(parseFloat($("#minimal-distance-between-points").value));
        result.push(~~$("#maximum-features-count").value);
        break;

      // Dense Optical Flow.
      case 1:
        result.push(~~$("#grid-resolution").value);
        result.push(~~$("#window-size").value);
        result.push(~~$("#iterations").value);
        break;

      // Random Forest Tracker.
      case 2:
        result.push(~~$("#trees-amount").value);
        result.push(~~$("#feature-points-amount").value);
        result.push(~~$("#intensity-threshold").value);
        result.push(~~$("#generated-random-points-count").value);
        break;
    }

    return result;
  }

  // Invoking method.
  function onInvoke() {
    var id = $("#movies").value,
        algorithmId = ~~$("#algorithms").value,
        data = {
          id: id,
          algorithmId: algorithmId,
          additionalArguments: []
        };

    if (algorithmId < 0) {
      alert("You didn't select any algorithm. Please select any and try again.");
      return;
    }

    data.additionalArguments = getAdditionalArgumentsFor(algorithmId);

    if (id < 0) {
      alert("You didn't select any video. Please select any and try again.");
      return;
    }

    Common.xhrPost("/invoke", data, replaceMovie);
  }

  // Getting active option in select control.
  function getSelectedAlgorithmValue() {
    var algorithms = $("#algorithms");

    return ~~algorithms.options[algorithms.selectedIndex].value;
  }

  // Change source for video tag.
  function replaceMovie() {
    var response = JSON.parse(this.responseText);

    Common.changeVideo(response.resultMovieURI);
    Common.hideOverlay();
  }

  // Select controls change event handler.
  function onAlgorithmChanging() {
    var id = getSelectedAlgorithmValue();

    Common.splat($(".parameters-container")).forEach(Common.hideElement);

    if (id >= 0) {
      $(AlgorithmsParameterSelector.replace("%id", id)).classList.remove("hidden");
    }
  }

  function onMovieChanging() {
    var value = $("#movies").value,
        movie;

    if (value === "-1") {
      Common.toggleButton("#invoke", false);
      $("#video-container").classList.add("hidden");

      $("#player").removeAttribute("src");
    } else {
      Common.toggleButton("#invoke", true);
      $("#video-container").classList.remove("hidden");

      movie = Common.getMovieName().replace("avi", "webm");
      Common.changeVideo(movie);
    }
  }

  // UI state helpers.
  function enableUI() {
    var movies = $("#movies"),
        algorithms = $("#algorithms"),
        send = $("#invoke");

    movies.removeAttribute("disabled");
    algorithms.removeAttribute("disabled");

    Common.toggleButton("#invoke", false);

    movies.addEventListener("change", onMovieChanging);
    algorithms.addEventListener("change", onAlgorithmChanging);

    send.addEventListener("click", onInvoke);
  }

  // Application logic.
  function buildAlgorithmsList() {
    var response = JSON.parse(this.responseText),
        algorithms = $("#algorithms");

    response.forEach(function(algorithm) {
      var option = document.createElement("option");

      option.setAttribute("value", algorithm.value);
      option.innerText = algorithm.name;

      algorithms.appendChild(option);
    });

    enableUI();
    Common.hideOverlay();
  }

  function getAlgorithms() {
    Common.xhrGet("/algorithms", buildAlgorithmsList);
  }

  function init() {
    Common.getMovies(getAlgorithms);
  }

  document.addEventListener("DOMContentLoaded", init, true);

} (window.Common));