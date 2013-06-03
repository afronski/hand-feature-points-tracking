    require("colors");

var path = require("path"),
    glob = require("glob"),
    execSync = require("execSync"),
    util = require("util"),

    resultFileNamePostFix = "_results_for_%s.json",
    deprecatedFragment = "tracking_result",

    command = './bin/tracking --include-results %s "%s" > %s'

    algorithms = [
      // "Sparse Optical Flow",
      // "Dense Optical Flow",
      "Random Forest Tracker"
    ],

    pathWithoutExtension = function(element) {
      return element.replace(path.extname(element), "");
    },

    sanitize = function(name) {
      return name.replace(/\s/gi, "_");
    };

console.log("Compiling ./bin/tracking in release mode...".rainbow);

if (execSync.code("scons") === 0) {

  algorithms.forEach(function(algorithm) {
    glob.sync("./assets/*.avi").forEach(function(fileName) {
      var resultFileName = pathWithoutExtension(fileName) + util.format(resultFileNamePostFix, sanitize(algorithm)),
          builtCommand = util.format(command, fileName, algorithm, resultFileName);

      if (fileName.indexOf(deprecatedFragment) === -1) {
        console.log("Processing '%s' file with algorithm '%s'".yellow, fileName, algorithm);

        if (execSync.code(builtCommand) === 0) {
          console.log("Results saved to file '%s'".green, resultFileName)
        } else {
          console.log("[EE] Command for file name '%s' and algorithm '%s' failed!".red, fileName, algorithm);
          process.exit(-123);
        }
      }
    });
  });

}