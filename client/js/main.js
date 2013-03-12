(function() {
  "use strict";

  // Helpers.
  var $ = function(selector) {
        var results = document.querySelectorAll(selector);

        return results.length == 1 ? results[0] : results;
      },

      context;

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

  // Canvas helpers.
  function getImageCanvas() {
    return $("#image");
  }

  function getImageCanvasContext() {
    if (!context) {
      context = getImageCanvas().getContext("2d");
    }

    return context;
  }

  function clearCanvas() {
    var canvas = getImageCanvas();

    getImageCanvasContext().clearRect(0, 0, canvas.width, canvas.height);
  }

  // Sending coordinates.
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

  // Changing movies in select control.
  function getTextFromActivePositionInSelect() {
    return $("option[value='%value']".replace("%value", $("#movies").value)).innerText;
  }

  function onMovieChanging() {
    var value = $("#movies").value,
        movie;

    if (value === "-1") {
      clearCanvas();
      toggleSendButton(false);

      $("#video-container").classList.add("hidden");
      $("#player").removeAttribute("src");
    } else {
      xhrGet("/frame/" + value, loadFirstFrame);

      toggleSendButton(true);

      $("#video-container").classList.remove("hidden");

      movie = getTextFromActivePositionInSelect().replace("avi", "webm");
      $("#player").setAttribute("src", "/videos-converted/%webm".replace("%webm", movie))
    }
  }

  // Drawing image on canvas element.
  function drawImageOnCanvas() {
    var canvas = getImageCanvas(),
        context = getImageCanvasContext();

    context.drawImage(this, 0, 0, this.width, this.height, 0, 0, canvas.width, canvas.height);
    hideOverlay();
  }

  function loadFirstFrame() {
    var response = JSON.parse(this.responseText),
        image = new Image();

    image.addEventListener("load", drawImageOnCanvas);
    image.src = response.imageURI;
  }

  // UI state helpers.
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