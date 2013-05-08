(function(Common) {
  "use strict";

  // Alias.
  var $ = Common.$;

  // Invoking method.
  function onInvoke() {
    var id = $("#movies").value,
        algorithmId = $("#algorithms").value,
        data = {
          id: id,
          algorithmId: algorithmId,
          additionalArguments: []
        };

    if (algorithmId < 0) {
      alert("You didn't select any algorithm. Please select any and try again.");
      return;
    }

    if (id < 0) {
      alert("You didn't select any video. Please select any and try again.");
      return;
    }

    Common.xhrPost("/invoke", data, replaceMovie);
  }

  // Changing movies in select control.
  function getTextFromActivePositionInSelect() {
    return $("#movies option[value='%value']".replace("%value", $("#movies").value)).innerText;
  }

  // Change source for video tag.
  function changeVideo(name) {
    if (!!name) {
      $("#player").setAttribute("src", "/videos-converted/%webm".replace("%webm", name));
    }
  }

  function replaceMovie() {
    var response = JSON.parse(this.responseText);

    changeVideo(response.resultMovieURI);
    Common.hideOverlay();
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

      movie = getTextFromActivePositionInSelect().replace("avi", "webm");
      changeVideo(movie);
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
    send.addEventListener("click", onInvoke);
  }

  // Application logic.
  function buildMovieList() {
    var response = JSON.parse(this.responseText),
        movies = $("#movies");

    response.forEach(function(movie) {
      var option = document.createElement("option");

      option.setAttribute("value", movie.value);
      option.innerText = movie.name;

      movies.appendChild(option);
    });

    getAlgorithms();
  }

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

  function getMovies() {
    Common.xhrGet("/movies", buildMovieList);
  }

  function getAlgorithms() {
    Common.xhrGet("/algorithms", buildAlgorithmsList);
  }

  function init() {
    getMovies();
  }

  document.addEventListener("DOMContentLoaded", init, true);

} (window.Common));