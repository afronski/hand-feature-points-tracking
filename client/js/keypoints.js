(function(Common) {
  "use strict";

  // Alias and variables.
  var $ = Common.$,

      points = [],
      context;

  // Points getter and drawing helper.
  function getPoints() {
    return points;
  }

  function drawRect(x, y, a, color) {
    context.fillStyle = color;
    context.fillRect(x - a / 2, y - a / 2, a, a);
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
    var canvas = getImageCanvas(),
        context = getImageCanvasContext(),

        X = event.offsetX,
        Y = event.offsetY;

    drawRect(X, Y, 6, "rgba(0,     0,   0, 255)");
    drawRect(X, Y, 3, "rgba(255, 255, 255, 255)");
    drawRect(X, Y, 1, "rgba(255,   0,   0, 255)");

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

        pixelizedWidth = this.width + "px",
        pixelizedHeight = this.height + "px";

    canvas.width = this.width;
    canvas.height = this.height;

    canvas.style.width = pixelizedWidth;
    canvas.style.height = pixelizedHeight;

    videoContainer.style.width = pixelizedWidth;
    videoContainer.style.height = pixelizedHeight;

    context.drawImage(this, 0, 0, this.width, this.height);
    Common.hideOverlay();
  }

  // Frames helper.
  function loadFrame() {
    var response = JSON.parse(this.responseText),
        image = new Image();

    image.addEventListener("load", drawImageOnCanvas);
    image.src = response.imageURI;

    Common.hideOverlay();
  }

  function getFrameNumber() {
    return -1;
  }

  function getSelectedFrame() {
    var uri = "/frame/%1?frameNumber=%2"
            .replace("%1", $("#movies").value)
            .replace("%2", getFrameNumber());

    Common.xhrGet(uri, loadFrame);
  }

  // Initialization.
  function init() {}

  document.addEventListener("DOMContentLoaded", init, true);

} (window.Common));