    "use strict";

var Port = 9292,

    util = require("util"),
    express = require("express"),
    videoStreamer = require("vid-streamer"),

    common = require("./common"),

    repository = require("./repository"),
    reporting = require("./reporting"),
    keypoints = require("./keypoints"),

    application = express();

application.use(express.static("client"));
application.use(express.static("assets"));

application.use(express.bodyParser());

application.get("/videos-converted/:path", videoStreamer);

application.get("/results", function(request, response) {
  common.debug("Get results list");

  common.sendJSON(response, repository.getResultsList());
});

application.get("/movies", function(request, response) {
  common.debug("Get movie list");

  common.sendJSON(response, repository.getMovieList());
});

application.get("/movies-without-keypoints", function(request, response) {
  common.debug("Get movie list without associated keypoints file");

  common.sendJSON(response, repository.getMovieListWithoutKeypoints());
});

application.get("/algorithms", function(request, response) {
  common.debug("Get algorithms list");

  common.sendJSON(response, repository.getAlgorithmsList());
});

application.post("/invoke", function(request, response) {
  var data = request.body,
      movieObject,
      uri = null;

  common.debug(util.format("Selected video %s with algorithm %s", data.id, data.algorithmId));
  common.debug("Additional arguments: " + common.prettyPrint(data.additionalArguments));

  movieObject = common.getById(repository.getMovieList(), parseInt(data.id, 10));
  uri = keypoints.trackPoints(data.additionalArguments, movieObject, data.algorithmId);

  common.sendJSON(response, { status: "OK", resultMovieURI: uri });
});

application.post("/keypoints", function(request, response) {
  var data = request.body,
      movieObject;

  movieObject = common.getById(repository.getMovieList(), parseInt(data.id, 10));
  keypoints.saveKeypoints(movieObject, data.keypoints, data.boundingCircleRadius);

  common.sendJSON(response, { status: "OK" });
});

application.get("/charts/memory/:file", function(request, response) {
  common.debug(util.format("Get chart data from file '%s' for memory usage", request.params.file));

  common.sendJSON(response, reporting.memory(request.params.file));
});

application.get("/charts/timing/:file", function(request, response) {
  common.debug(util.format("Get chart data from file '%s' for timing", request.params.file));

  common.sendJSON(response, reporting.timing(request.params.file));
});

application.get("/charts/quality/:file", function(request, response) {
  common.debug(util.format("Get chart data from file '%s' for quality", request.params.file));

  common.sendJSON(response, reporting.quality(request.params.file));
});

application.get("/charts/overhead/:file", function(request, response) {
  common.debug(util.format("Get chart data from file '%s' for overhead", request.params.file));

  common.sendJSON(response, reporting.overhead(request.params.file));
});

application.listen(Port);
common.log("Listening on port ", Port);