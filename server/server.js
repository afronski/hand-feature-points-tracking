    "use strict";

    require("colors");
    // SKĄD TE STRATY JAKOŚCI ? -> OpenCV Writer? -> FFMPEG?
var PORT = 9292,

    FirstFramePostFix = "_first_frame.png",
    ResultMovieBeforeConversionPostFix = "_tracking_result.avi",
    ResultMoviePostFix = "_tracking_result.webm",

    path = require("path"),
    glob = require("glob"),
    util = require("util"),
    express = require("express"),
    videoStreamer = require("vid-streamer"),
    execSync = require("execSync"),

    movies = [],
    app = express(),

    info = function(text) {
      console.log(("\t" + text).green);
    },

    debug = function(text) {
      console.log(("\t" + text).yellow);
    },

    sendJSON = function(response, object) {
      var body = JSON.stringify(object);

      response.setHeader("Content-Type", "application/json");
      response.setHeader("Content-Length", body.length);

      response.end(body);
    },

    prettyPrint = function(object) {
      return "\n" + JSON.stringify(object, null, 2);
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

    getById = function(array, id) {
      var result = array.filter(function(element) { return element.value === id; })[0];
      return !!result ? result : null;
    },

    extractFirstFrame = function(movieObject) {
      if (!!movieObject) {
        if (execSync.code(util.format("./bin/export-first-frame %s", movieObject.path)) === 0) {
          var filename = path
                          .basename(movieObject.path)
                          .replace(path.extname(movieObject.path), "");

          return util.format("/%s", filename + FirstFramePostFix);
        }
      }

      return null;
    },

    toCoordinates = function(element) {
      return util.format("%s %s", element.x, element.y);
    },

    trackPoints = function(points, movieObject) {
      var trackingInvocation,
          conversionInvocation,
          pointsList,
          filename;

      if (!!movieObject) {
        pointsList = points.map(toCoordinates).join(" ");
        trackingInvocation = util.format("./bin/tracking %s %s", movieObject.path, pointsList)

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

app.get("/frame/:id", function(request, response) {
  var id = parseInt(request.params.id, 10),
      uri;

  debug("Get first frame from AVI with ID " + id);

  uri = extractFirstFrame(getById(getMovieList(), id));
  sendJSON(response, { status: "OK", imageURI: uri });
});

app.post("/coordinates", function(request, response) {
  var points = request.body,
      uri = null;

  if (points.length > 0) {
    debug("Save coordinates for salient point: " + prettyPrint(points));

    uri = trackPoints(points, getById(getMovieList(), parseInt(points[0].id, 10)));
  }

  sendJSON(response, { status: "OK", resultMovieURI: uri });
});

app.listen(PORT);
console.log("Listening on port ".rainbow + PORT);