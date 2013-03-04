(function() {
  "use strict";

  // Helpers.
  var $ = function(selector) {
    var results = document.querySelectorAll(selector);

    return results.length == 1 ? results[0] : results;
  };

  // Methods related with overlay.
  function hideOverlay() {
    $(".overlay").classList.add("hidden");
  }

  function showOverlay() {
    $(".overlay").classList.remove("hidden");
  }

  // XHR methods.
  function xhr(method, path, callback, args) {
    var request = new XMLHttpRequest();

    request.open(method, path, true);

    if (method.toUpperCase() === "POST") {
      request.setRequestHeader("Content-Type", "application/json");
    }

    request.addEventListener("load", callback || hideOverlay);
    request.send(JSON.stringify(args));

    showOverlay();
  }

  function xhrGet(path, callback) {
    xhr("GET", path, callback);
  }

  function xhrPost(path, args, callback) {
    xhr("POST", path, callback, args);
  }

  // UI handlers.
  function getData() {
    return [
      { id: 1, x: 100, y: 100 },
      { id: 1, x: 200, y: 200 }
    ];
  }

  function onDataSending() {
    var data = getData();

    if (data.length <= 0) {
      alert("You didn't select any salient point. Please select any and try again.");
      return;
    }

    data.forEach(function(coordinates) {
      xhrPost("/coordinates", coordinates);
    });
  }

  function onMovieChanging() {
    var value = $("#movies").value;

    if (value === "-1") {
      toggleSendButton(false);
    } else {
      xhrGet("/frame/" + value);
      toggleSendButton(true);
    }
  }

  function toggleSendButton(value) {
    var send = $("#send-coordinates");

    if (!!value) {
      send.removeAttribute("disabled");
    } else {
      send.setAttribute("disabled", "disabled");
    }
  }

  function enableUI() {
    var movies = $("#movies"),
        send = $("#send-coordinates");

    movies.removeAttribute("disabled");
    toggleSendButton(false);

    movies.addEventListener("change", onMovieChanging);
    send.addEventListener("click", onDataSending);
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

    enableUI();
    hideOverlay();
  }

  function getMovies() {
    xhrGet("/movies", buildMovieList);
  }

  function init() {
    getMovies();
  }

  document.addEventListener("DOMContentLoaded", init, true);
} ());