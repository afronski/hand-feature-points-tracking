    "use strict";

    require("colors");

var util = require("util"),
    path = require("path"),

    ExcludedElementsWith = "_tracking_result",

    common = exports = module.exports = {};

// Filtering.
common.getById = function(array, id) {
  var result = array.filter(function(element) { return element.value === id; })[0];

  return !!result ? result : null;
};

common.notTrackingResults = function(element) {
  return element.indexOf(ExcludedElementsWith) === -1;
};

// Converters.
common.toArgument = function(element) {
  return util.format("%s", element);
};

common.toAlgorithm = function(element, index) {
  return { value: index, name: element };
};

// Debugging helpers.
common.info = function(text) {
  console.log(text.green);
};

common.debug = function(text) {
  console.log(text.yellow);
};

common.log = function(text, port) {
  console.log(util.format("%s %s", text, port).rainbow);
};

common.prettyPrint = function(object) {
  return ("\n" + JSON.stringify(object, null, 2)).blue;
};

// Helpers.
common.sendJSON = function(response, object) {
  var body = JSON.stringify(object);

  response.setHeader("Content-Type", "application/json");
  response.setHeader("Content-Length", body.length);

  response.end(body);
};
common.sanitize = function(name, algorithm) {
  return util.format(name, algorithm.replace(/\s/gi, "_"));
};

common.nonEmpty = function(element) {
  return !!element;
};

// Path and file helpers.
common.onlyBaseName = function(element) {
  return path.basename(element).replace(path.extname(element), "");
};

common.fileMapper = function(element, index) {
  return {
    value: index + 1,
    name: path.basename(element),
    path: element
  };
};