    "use strict";

    require("colors");

var PORT = 9292,
    FirstFramePostFix = "_first_frame.png",

    util = require("util"),
    express = require("express"),
    videoStreamer = require("vid-streamer"),
    execSync = require("execSync"),

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

    getMovieList = function() {
      return [
        {
          name: "circle_sample_1.avi",
          path: "circle_sample_1.avi",
          value: 1
        }
      ];
    },

    getById = function(array, id) {
      var result = array.filter(function(element) { return element.value === id; })[0];
      return !!result ? result : null;
    },

    extractFileName = function(path) {
      var index = path.lastIndexOf(".");

      if (index !== -1) {
        return path.substr(0, index);
      }

      return path;
    },

    extractFirstFrame = function(movieObject) {
      if (!!movieObject) {
        if (execSync.code(util.format("./bin/export-first-frame assets/%s", movieObject.path)) === 0) {
          return util.format("/%s", extractFileName(movieObject.path) + FirstFramePostFix);
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
  debug("Save coordinates for salient point " + JSON.stringify(request.body));

  sendJSON(response, { status: "OK" });
});

app.listen(PORT);
console.log("Listening on port ".rainbow + PORT);