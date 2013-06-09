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

// Charts REST API.

// Memory charts.
application.get("/charts/memory/file/:file", function(request, response) {
  common.debug(util.format("Get memory usage chart data for file '%s'", request.params.file));

  common.sendJSON(response, reporting.memory(request.params.file));
});

application.get("/charts/memory/person/:person/gesture/:gesture", function(request, response) {
  common.debug(util.format("Get memory usage chart data for one person '%s' and one gesture '%s'",
                           request.params.person,
                           request.params.gesture));

  common.sendJSON(response, reporting.memoryUsageForOneGestureAndOnePerson(
                                        request.params.person,
                                        request.params.gesture));
});

application.get("/charts/memory/specialised/method/:method", function(request, response) {
  common.debug(util.format("Get specialised memory usage chart data for method '%s'", request.params.method));

  common.sendJSON(response, reporting.memoryUsageForSpecialisedMethod(request.params.method));
});

application.get("/charts/memory/resident/method/:method", function(request, response) {
  common.debug(util.format("Get resident memory usage chart data for method '%s'",
                           request.params.method));

  common.sendJSON(response, reporting.residentMemoryForMethod(request.params.method));
});

application.get("/charts/memory/virtual/method/:method", function(request, response) {
  common.debug(util.format("Get virtual memory usage chart data for method '%s'",
                           request.params.method));

  common.sendJSON(response, reporting.virtualMemoryForMethod(request.params.method));
});

application.get("/charts/memory/resident/method/:method/gesture/:gesture", function(request, response) {
  common.debug(util.format("Get resident memory usage chart data for method '%s' and gesture '%s'",
                           request.params.method,
                           request.params.gesture));

  common.sendJSON(response, reporting.residentMemoryForMethodAndGesture(
                                        request.params.method,
                                        request.params.gesture));
});

application.get("/charts/memory/virtual/method/:method/gesture/:gesture", function(request, response) {
  common.debug(util.format("Get virtual memory usage chart data for method '%s' and gesture '%s'",
                           request.params.method,
                           request.params.gesture));

  common.sendJSON(response, reporting.virtualMemoryForMethodAndGesture(
                                        request.params.method,
                                        request.params.gesture));
});

application.get("/charts/memory/resident/method/:method/person/:person", function(request, response) {
  common.debug(util.format("Get resident memory usage chart data for method '%s' and person '%s'",
                           request.params.method,
                           request.params.person));

  common.sendJSON(response, reporting.residentMemoryForMethodAndPerson(
                                        request.params.method,
                                        request.params.person));
});

application.get("/charts/memory/virtual/method/:method/person/:person", function(request, response) {
  common.debug(util.format("Get virtual memory usage chart data for method '%s' and person '%s'",
                           request.params.method,
                           request.params.person));

  common.sendJSON(response, reporting.virtualMemoryForMethodAndPerson(
                                        request.params.method,
                                        request.params.person));
});

// Quality related charts.
application.get("/charts/quality/:file", function(request, response) {
  common.debug(util.format("Get quality chart data from file '%s'", request.params.file));

  common.sendJSON(response, reporting.quality(request.params.file));
});

application.get("/charts/quality/person/:person/gesture/:gesture", function(request, response) {
  common.debug(util.format("Get quality chart data for person '%s' and gesture '%s'",
                           request.params.person,
                           request.params.gesture));

  common.sendJSON(response, reporting.qualityForPersonAndGesture(request.params.person, request.params.gesture));
});

application.get("/charts/quality/path/:file", function(request, response) {
  common.debug(util.format("Get quality path chart data from file '%s'", request.params.file));

  common.sendJSON(response, reporting.qualityPath(request.params.file));
});

application.get("/charts/quality/method/:method/person/:person", function(request, response) {
  common.debug(util.format("Get quality chart data for method '%s' and person '%s'",
                           request.params.method,
                           request.params.person));

  common.sendJSON(response, reporting.qualityForMethodAndPerson(request.params.method, request.params.person));
});

application.get("/charts/quality/path/method/:method/person/:person", function(request, response) {
  common.debug(util.format("Get quality path chart data for method '%s' and person '%s'",
                           request.params.method,
                           request.params.person));

  common.sendJSON(response, reporting.qualityPathForMethodAndPerson(request.params.method, request.params.person));
});

application.get("/charts/quality/method/:method/gesture/:gesture", function(request, response) {
  common.debug(util.format("Get quality chart data for method '%s' and gesture '%s'",
                           request.params.method,
                           request.params.gesture));

  common.sendJSON(response, reporting.qualityForMethodAndGesture(request.params.method, request.params.gesture));
});

application.get("/charts/quality/path/method/:method/gesture/:gesture", function(request, response) {
  common.debug(util.format("Get quality path chart data for method '%s' and gesture '%s'",
                           request.params.method,
                           request.params.gesture));

  common.sendJSON(response, reporting.qualityPathForMethodAndGesture(request.params.method, request.params.gesture));
});

application.get("/charts/quality/specialised/method/:method", function(request, response) {
  common.debug(util.format("Get specialised quality chart data from method '%s'", request.params.method));

  common.sendJSON(response, reporting.qualitySpecialisedForMethod(request.params.method));
});

application.get("/charts/quality/specialised/path/method/:method", function(request, response) {
  common.debug(util.format("Get specialised quality path chart data from method '%s'", request.params.method));

  common.sendJSON(response, reporting.qualityPathSpecialisedForMethod(request.params.method));
});

application.get("/charts/scatter-plot/file/:file", function(request, response) {
  common.debug(util.format("Get scatter plot data from file '%s'", request.params.file));

  common.sendJSON(response, reporting.qualityScatterPlot(request.params.file));
});

// Overhead charts.
application.get("/charts/overhead/person/:person/gesture/:gesture", function(request, response) {
  common.debug(util.format("Get overhead chart data from person '%s' and gesture '%s'",
                           request.params.person,
                           request.params.gesture));

  common.sendJSON(response, reporting.overhead(request.params.person, request.params.gesture));
});

application.get("/charts/overhead/full/person/:person/gesture/:gesture", function(request, response) {
  common.debug(util.format("Get full overhead chart data from person '%s' and gesture '%s'",
                           request.params.person,
                           request.params.gesture));

  common.sendJSON(response, reporting.fullOverheadForRandomForestTracker(
                                        request.params.person,
                                        request.params.gesture));
});

application.get("/charts/overhead/specialised/method/:method", function(request, response) {
  common.debug(util.format("Get specialised overhead chart data for method '%s'", request.params.method));

  common.sendJSON(response, reporting.fullOverheadForSpecialisedMethod(request.params.method));
});

// TODO:
// Timing charts.
application.get("/charts/timing/:file", function(request, response) {
  common.debug(util.format("Get chart data from file '%s' for timing", request.params.file));

  common.sendJSON(response, reporting.timing(request.params.file));
});

application.listen(Port);
common.log("Listening on port", Port);