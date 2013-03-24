(function() {
  "use strict";

  // Helpers.
  var $ = function(selector) {
        var results = document.querySelectorAll(selector);

        return results.length == 1 ? results[0] : results;
      },

      context,
      points = [];

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
    return points;
  }

  function drawRect(x, y, a, color) {
    context.fillStyle = color;
    context.fillRect(x - a / 2, y - a / 2, a, a);
  }

  function onMouseDown(event) {
    var canvas = getImageCanvas(),
        context = getImageCanvasContext();

    drawRect(event.offsetX, event.offsetY, 6, "rgba(0,     0,   0, 255)");
    drawRect(event.offsetX, event.offsetY, 3, "rgba(255, 255, 255, 255)");
    drawRect(event.offsetX, event.offsetY, 1, "rgba(255,   0,   0, 255)");

    points.push({
      id: $("#movies").value,
      x: event.offsetX,
      y: event.offsetY
    });
  }

  function onDataSending() {
    var data = getData();

    if (data.length <= 0) {
      alert("You didn't select any salient point. Please select any and try again.");
      return;
    }

    xhrPost("/coordinates", data, replaceMovie);
  }

  // Changing movies in select control.
  function getTextFromActivePositionInSelect() {
    return $("option[value='%value']".replace("%value", $("#movies").value)).innerText;
  }

  // Mouse event handlers.
  function attachMouseEventHandlers() {
    var canvas = getImageCanvas();

    canvas.addEventListener("mousedown", onMouseDown, false);
  }

  function detachMouseEventHandlers() {
    var canvas = getImageCanvas();

    canvas.removeEventListener("mousedown", onMouseDown, false);
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
    hideOverlay();
  }

  // Clearing canvas.
  function onClear() {
    clearCanvas();
    points = [];

    xhrGet("/frame/" + $("#movies").value, loadFirstFrame);
  }

  function onMovieChanging() {
    var value = $("#movies").value,
        movie;

    points = [];
    detachMouseEventHandlers();

    if (value === "-1") {
      clearCanvas();
      toggleButton("#send-coordinates", false);
      toggleButton("#clear", false);

      $("#video-container").classList.add("hidden");
      $("#player").removeAttribute("src");
    } else {
      xhrGet("/frame/" + value, loadFirstFrame);

      toggleButton("#send-coordinates", true);
      toggleButton("#clear", true);

      $("#video-container").classList.remove("hidden");

      movie = getTextFromActivePositionInSelect().replace("avi", "webm");
      changeVideo(movie);
    }
  }

  // Drawing image on canvas element.
  function drawImageOnCanvas() {
    var canvas = getImageCanvas(),
        context = getImageCanvasContext();

    context.drawImage(this, 0, 0, this.width, this.height, 0, 0, canvas.width, canvas.height);
    hideOverlay();
    attachMouseEventHandlers();
  }

  function loadFirstFrame() {
    var response = JSON.parse(this.responseText),
        image = new Image();

    image.addEventListener("load", drawImageOnCanvas);
    image.src = response.imageURI;
  }

  // UI state helpers.
  function toggleButton(selector, value) {
    var button = $(selector);

    if (!!value) {
      button.removeAttribute("disabled");
    } else {
      button.setAttribute("disabled", "disabled");
    }
  }

  function enableUI() {
    var movies = $("#movies"),
        clear = $("#clear"),
        send = $("#send-coordinates");

    movies.removeAttribute("disabled");
    toggleButton("#send-coordinates", false);
    toggleButton("#clear", false);

    movies.addEventListener("change", onMovieChanging);
    send.addEventListener("click", onDataSending);
    clear.addEventListener("click", onClear);
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