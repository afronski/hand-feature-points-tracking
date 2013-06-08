    "use strict";

var glob = require("glob"),
    execSync = require("execSync"),

    common = require("./common"),

    repository = exports = module.exports = {};

repository.getMovieList = function() {
  var movies = glob.sync("./assets/*.avi").filter(common.notTrackingResults).map(common.fileMapper);

  common.debug("Listing movie files from assets directory: " + common.prettyPrint(movies));

  return movies;
};

repository.getResultsList = function() {
  var results = glob.sync("./assets/*.json").filter(common.notTrackingResults).map(common.fileMapper);

  common.debug("Listing JSON files from assets directory: " + common.prettyPrint(results));

  return results;
};

repository.getMovieListWithoutKeypoints = function() {
  var moviesToFiltering = glob.sync("./assets/*.avi")
                                .filter(common.notTrackingResults)
                                .map(common.fileMapper),

      keypoints = glob.sync("./assets/*.keypoints")
                         .map(common.onlyBaseName),

      result = moviesToFiltering.filter(function(element) {
        return keypoints.indexOf(common.onlyBaseName(element.name)) === -1;
      });

  common.debug("Listing movie files without keypoints from assets directory: " +
               common.prettyPrint(result));

  return result;
};

repository.getAlgorithmsList = function() {
  var output = execSync.stdout("./bin/tracking --list-algorithms"),
      algorithms = output.split("\n").filter(common.nonEmpty).map(common.toAlgorithm);

  return algorithms;
};