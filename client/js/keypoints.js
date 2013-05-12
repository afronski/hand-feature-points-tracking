(function(Common) {
  "use strict";

  // Alias and variables.
  var $ = Common.$,

      points = [],
      removed = [],

      Color = "red",

      Zoom = 0.5,
      InvertedZoom = 1.0 / Zoom,

      context;

  function getBoundingCircleRadius() {
    return parseFloat($("#circle-radius").value);
  }

  // Points getter and drawing helper.
  function drawRect(x, y, color) {
    context.fillStyle = color;
    context.fillRect(x - 1, y - 1, 2, 2);
  }

  function drawCircle(x, y, r, color) {
    context.strokeStyle = color;
    context.beginPath();
    context.arc(x, y, r, 0, 2 * Math.PI);
    context.stroke();
  }

  function drawLine(from, to, color) {
    context.strokeStyle = color;
    context.beginPath();
    context.moveTo(from.x, from.y);
    context.lineTo(to.x, to.y);
    context.stroke();
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

  function onMouseDown(event) {
    var X = event.offsetX,
        Y = event.offsetY;

    points.push({ x: X, y: Y });
  }

  // Canvas helpers.
  function getImageCanvas() {
    return $("#frame");
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

  // Drawing image on canvas element.
  function drawImageOnCanvas() {
    var canvas = getImageCanvas(),
        context = getImageCanvasContext(),

        videoContainer = $("#player"),

        width = videoContainer.videoWidth * Zoom,
        height = videoContainer.videoHeight * Zoom,

        pixelizedWidth = width + "px",
        pixelizedHeight = height + "px";

    canvas.width = width;
    canvas.height = height;

    canvas.style.width = pixelizedWidth;
    canvas.style.height = pixelizedHeight;

    context.drawImage(videoContainer, 0, 0, width, height);
  }

  // Saving method.
  function convertToNaturalPosition(point) {
    return {
      x: point.x * InvertedZoom,
      y: point.y * InvertedZoom
    };
  }

  function onSave() {
    var id = $("#movies").value,
        data = {
          id: id,
          keypoints: points.map(convertToNaturalPosition),
          boundingCircleRadius: getBoundingCircleRadius() * InvertedZoom
        };

    if (data.keypoints.length <= 0) {
      alert("You didn't select any keypoint. Please select any and try again.");
      return;
    }

    if (id < 0) {
      alert("You didn't select any video. Please select any and try again.");
      return;
    }

    Common.xhrPost("/keypoints", data);
  }

  // Keypoints drawing and manipulating methods.
  function drawKeypoint(element, index, array) {
    drawRect(element.x, element.y, Color);
    drawCircle(element.x, element.y, getBoundingCircleRadius(), Color);

    if (index > 0) {
      drawLine(array[index - 1], element, Color);
    }
  }

  function drawKeypoints() {
    if (points.length > 0) {
      points.forEach(drawKeypoint);
    }
  }

  function onClear() {
    points = [];

    drawKeypoints();
  }

  function onUndo() {
    var popped = points.pop();

    if (!!popped) {
      removed.push(popped);
    }

    drawKeypoints();
  }

  function onRedo() {
    var popped = removed.pop();

    if (!!popped) {
      points.push(popped);
    }

    drawKeypoints();
  }

  // Helpers.
  function makeVideoMoved() {
    var player = $("#player");

    player.play();
    setTimeout(player.pause.bind(player), 0);
  }

  // Changing movie.
  function onMovieChanged(event) {
    var videoContainer = $("#video-container"),
        canvasContainer = $("#canvas-container"),

        movies = $("#movies"),
        selectedValue = ~~movies.options[movies.selectedIndex].value,

        movie;

    if (selectedValue !== -1) {
      Common.toggleButton("#save", true);
      Common.toggleButton("#undo", true);
      Common.toggleButton("#clear", true);

      Common.toggleButton("#circle-radius", true);

      videoContainer.classList.remove("hidden");
      canvasContainer.classList.remove("hidden");

      movie = Common.getMovieName().replace("avi", "webm");
      Common.changeVideo(movie);

      makeVideoMoved();
      attachMouseEventHandlers();
    } else {
      clearCanvas();
      $("#player").removeAttribute("src");

      Common.toggleButton("#save", false);
      Common.toggleButton("#undo", false);
      Common.toggleButton("#clear", false);

      Common.toggleButton("#circle-radius", false);

      videoContainer.classList.add("hidden");
      canvasContainer.classList.add("hidden");

      detachMouseEventHandlers();
    }
  }

  // Computing frames.
  function computeFrame() {
    if (this.paused || this.ended) {
      return;
    }

    drawImageOnCanvas();
    drawKeypoints();

    setTimeout(computeFrame, 0);
  }

  // Enable UI and hide overlay after loading movies.
  function enableUI() {
    var movies = $("#movies");

    movies.removeAttribute("disabled");
    movies.addEventListener("change", onMovieChanged);

    $("#save").addEventListener("click", onSave);
    $("#undo").addEventListener("click", onUndo);
    $("#clear").addEventListener("click", onClear);

    getImageCanvasContext();
    $("#player").addEventListener("play", computeFrame, false);

    Common.hideOverlay();
  }

  // Initialization.
  function init() {
    Common.getMovies(enableUI);
  }

  function keyboardEventHandler(event) {
    var player;

    // "u"
    if (event.keyCode === 85) {
      onUndo();
    }

    // "r"
    if (event.keyCode === 82) {
      onRedo();
    }

    // "Right arrow"
    if (event.keyCode === 39) {
      player = $("#player");

      player.currentTime = player.currentTime + 0.01;
    }

    // "Left arrow"
    if (event.keyCode === 37) {
      player = $("#player");

      player.currentTime = player.currentTime - 0.01;
    }
  }

  document.addEventListener("DOMContentLoaded", init, true);
  document.addEventListener("keydown", keyboardEventHandler, true);

  window.setPointsFromString = function(string) {
    points = JSON.parse(string);
  };

} (window.Common));