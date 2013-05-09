    "use strict";

    require("colors");

var PORT = 9292,

    FramePostFix = "_frame.png",
    ResultMovieBeforeConversionPostFix = "_tracking_result.avi",
    ResultMoviePostFix = "_tracking_result.webm",

    path = require("path"),
    glob = require("glob"),
    util = require("util"),
    express = require("express"),
    videoStreamer = require("vid-streamer"),
    execSync = require("execSync"),

    movies = [],
    algorithms = [],

    app = express(),

    info = function(text) {
      console.log(text.green);
    },

    debug = function(text) {
      console.log(text.yellow);
    },

    sendJSON = function(response, object) {
      var body = JSON.stringify(object);

      response.setHeader("Content-Type", "application/json");
      response.setHeader("Content-Length", body.length);

      response.end(body);
    },

    prettyPrint = function(object) {
      return ("\n" + JSON.stringify(object, null, 2)).blue;
    },

    notTrackingResults = function(element) {
      return element.indexOf(ResultMovieBeforeConversionPostFix) === -1;
    },

    movieMapper = function(element, index) {
      return {
        value: index + 1,
        name: path.basename(element),
        path: element
      };
    },

    getMovieList = function() {
      if (movies.length <= 0) {
        movies = glob.sync("./assets/*.avi").filter(notTrackingResults).map(movieMapper);
        debug("Listing movie files from assets directory: " + prettyPrint(movies));
      }

      return movies;
    },

    toAlgorithm = function(element, index) {
      return { value: index, name: element };
    },

    nonEmpty = function(element) {
      return !!element;
    },

    getAlgorithmsList = function() {
      var output;

      if (algorithms.length <= 0) {
        output = execSync.stdout("./bin/tracking --list-algorithms");
        algorithms = output.split("\n").filter(nonEmpty).map(toAlgorithm);
      }

      return algorithms;
    },

    getById = function(array, id) {
      var result = array.filter(function(element) { return element.value === id; })[0];
      return !!result ? result : null;
    },

    extractFrame = function(movieObject, frameNumber) {
      if (!!movieObject) {
        if (execSync.code(util.format("./bin/export-frame %s %s", movieObject.path, frameNumber)) === 0) {
          var filename = path
                          .basename(movieObject.path)
                          .replace(path.extname(movieObject.path), "");

          return util.format("/%s", filename + FramePostFix);
        }
      }

      return null;
    },

    toArgument = function(element) {
      return util.format("%s", element);
    },

    trackPoints = function(additionalArguments, movieObject, algorithmId) {
      var trackingInvocation,
          conversionInvocation,
          argumentsList,
          algorithm,
          filename;

      if (!!movieObject) {
        argumentsList = additionalArguments.map(toArgument).join(" ");
        algorithm = algorithms[algorithmId].name;

        trackingInvocation = util.format('./bin/tracking %s "%s" %s',
                                         movieObject.path,
                                         algorithm,
                                         argumentsList);

        debug("Invoking: " + trackingInvocation);

        if (execSync.code(trackingInvocation) === 0) {
            filename = path
                        .basename(movieObject.path)
                        .replace(path.extname(movieObject.path), "");

            conversionInvocation = util.format("./convert-one.sh %s",
                                               filename + ResultMovieBeforeConversionPostFix);

            debug("Invoking: " + conversionInvocation);

            if (execSync.code(conversionInvocation) === 0) {
              debug("Returning new video: " + filename + ResultMoviePostFix);

              return util.format("%s", filename + ResultMoviePostFix);
            }
        }
      }

      return null;
    };

app.use(express.static("client"));
app.use(express.static("assets"));

app.use(express.bodyParser());

app.get("/videos-converted/:path", videoStreamer);

app.get("/movies", function(request, response) {
  debug("Get movie list");

  sendJSON(response, getMovieList());
});

app.get("/algorithms", function(request, response) {
  debug("Get algorithms list");

  sendJSON(response, getAlgorithmsList());
});

app.get("/frame/:id", function(request, response) {
  var id = parseInt(request.params.id, 10),
      uri;

  debug(util.format("Get %s frame from AVI with ID %s", request.query.frameNumber, id));

  uri = extractFrame(getById(getMovieList(), id), request.query.frameNumber);
  sendJSON(response, { status: "OK", imageURI: uri });
});

app.post("/invoke", function(request, response) {
  var data = request.body,
      movieObject,
      uri = null;

  debug(util.format("Selected video %s with algorithm %s", data.id, data.algorithmId));
  debug("Additional arguments: " + prettyPrint(data.additionalArguments));

  movieObject = getById(getMovieList(), parseInt(data.id, 10));
  uri = trackPoints(data.additionalArguments, movieObject, data.algorithmId);

  sendJSON(response, { status: "OK", resultMovieURI: uri });
});

app.listen(PORT);
console.log("Listening on port ".rainbow + PORT);