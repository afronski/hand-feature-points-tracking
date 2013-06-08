    "use strict";

var fs = require("fs"),
    path = require("path"),

    common = require("./common"),

    repository = require("./repository"),

    ToMB = 1024,

    reporting = exports = module.exports = {};

// Helpers.
function extract(element, index) {
  return { x: index + 1, y: element };
}

function extractMemory(element, index) {
  return { x: index + 1, y: element / toMB };
}

// Memory data preparation.
reporting.memory = function(file) {
  return {
    series: [
      { values: [ 1, 2, 3, 4 ].map(extract), key: "Linear" },
      { values: [ 1, 4, 9, 16 ].map(extract), key: "Quadratic" },
      { values: [ 1, 8, 27, 64 ].map(extract), key: "Cubic" }
    ],

    name: "Memory Usage - " + file
  };
};

// Quality data preparation.
reporting.quality = function(file) {
  return {
    series: [
      { values: [ 1, 2, 3, 4 ].map(extract), key: "Linear" },
      { values: [ 1, 4, 9, 16 ].map(extract), key: "Quadratic" },
      { values: [ 1, 8, 27, 64 ].map(extract), key: "Cubic" }
    ],

    name: "Quality - " + file
  };
};

// Timing data preparation.
reporting.timing = function(file) {
  return {
    series: [
      { values: [ 1, 2, 3, 4 ].map(extract), key: "Linear" },
      { values: [ 1, 4, 9, 16 ].map(extract), key: "Quadratic" },
      { values: [ 1, 8, 27, 64 ].map(extract), key: "Cubic" }
    ],

    name: "Timing - " + file
  };
};

// Overhead data preparation.
reporting.overhead = function(file) {
  return {
    series: [
      { values: [ 1, 2, 3, 4 ].map(extract), key: "Linear" },
      { values: [ 1, 4, 9, 16 ].map(extract), key: "Quadratic" },
      { values: [ 1, 8, 27, 64 ].map(extract), key: "Cubic" }
    ],

    name: "Overhead - " + file
  };
};