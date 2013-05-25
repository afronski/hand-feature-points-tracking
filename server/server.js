    "use strict";

    require("colors");

var PORT = 9292,

    ExcludedElementsWith = "_tracking_result",
    ResultMovieBeforeConversionPostFix = "_tracking_result_for_%s.avi",
    ResultMoviePostFix = "_tracking_result_for_%s.webm",

    XmlHeader = '<?xml version="1.0" encoding="utf-8"?>\n<FeaturePointList>\n    <hand>',

    RadiusTag = '        <FeaturePointRadius r="%s" />',
    PointTag = '        <FeaturePoint x="%s" y="%s" />',

    XmlEnding = '    </hand>\n</FeaturePointList>',

    path = require("path"),
    fs = require("fs"),
    glob = require("glob"),
    util = require("util"),
    express = require("express"),
    videoStreamer = require("vid-streamer"),
    execSync = require("execSync"),

    movies = [],
    algorithms = [],

    app = express(),

    sanitize = function(name, algorithm) {
      return util.format(name, algorithm.replace(/\s/gi, "_"));
    },

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
      return element.indexOf(ExcludedElementsWith) === -1;
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

    saveKeypoints = function(movieObject, keypoints, radius) {
      var filename = path.basename(movieObject.path)
                         .replace(path.extname(movieObject.path), ""),
          keypointsFile = filename + ".keypoints",
          xmlFile = filename + ".xml",

          xmlContent = [],
          content = [];

      content.push(radius.toString());
      content.push(keypoints.length.toString());

      keypoints.forEach(function(element) {
        content.push(util.format("%s %s", element.x, element.y));
      });

      fs.writeFile(util.format("./assets/%s", keypointsFile), content.join("\n"));
      debug(util.format("File with keypoints (%s) saved.", keypointsFile));

      xmlContent.push(XmlHeader);
      xmlContent.push(util.format(RadiusTag, radius.toString()));

      keypoints.forEach(function(element) {
        xmlContent.push(util.format(PointTag, element.x, element.y));
      });

      xmlContent.push(XmlEnding);

      fs.writeFile(util.format("./assets/%s", xmlFile), xmlContent.join("\n"));
      debug(util.format("XML file with keypoints (%s) saved.", xmlFile));
    },

    getById = function(array, id) {
      var result = array.filter(function(element) { return element.value === id; })[0];
      return !!result ? result : null;
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
                                               filename + sanitize(ResultMovieBeforeConversionPostFix, algorithm));

            debug("Invoking: " + conversionInvocation);

            if (execSync.code(conversionInvocation) === 0) {
              debug("Returning new video: " + filename + sanitize(ResultMoviePostFix, algorithm));

              return util.format("%s", filename + sanitize(ResultMoviePostFix, algorithm));
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

app.post("/keypoints", function(request, response) {
  var data = request.body,
      movieObject;

  movieObject = getById(getMovieList(), parseInt(data.id, 10));
  saveKeypoints(movieObject, data.keypoints, data.boundingCircleRadius);

  sendJSON(response, { status: "OK" });
});

app.listen(PORT);
console.log("Listening on port ".rainbow + PORT);