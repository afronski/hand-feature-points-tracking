    require("colors");

var path = require("path"),
    glob = require("glob"),
    execSync = require("execSync"),
    util = require("util"),
    fs = require("fs"),

    resultFileNamePostFix = "_results_for_%s.json",
    parametrizedResultFileNamePostFix = "_results_for_%s_parameters(%s).json",
    deprecatedFragment = "tracking_result",

    command = './bin/tracking --include-results %s "%s" > %s',
    paramterizedCommand = './bin/tracking --include-results %s "%s" %s > "%s"',

    algorithms = [
      "Sparse Optical Flow",
      "Dense Optical Flow",
      "Random Forest Tracker"
    ],

    parametrized = JSON.parse(fs.readFileSync("parametrized.json")).builds || [],

    pathWithoutExtension = function(element) {
      return element.replace(path.extname(element), "");
    },

    sanitize = function(name) {
      return name.replace(/\s/gi, "_");
    },

    getProperDate = function() {
      var zeroPad = function(input) {
            return input[1] ? input : "0" + input[0];
          },

          date = new Date(),

          yyyy = date.getFullYear().toString(),
          mm = (date.getMonth() + 1).toString(),
          dd  = date.getDate().toString(),

          HH = date.getHours().toString(),
          MM = date.getMinutes().toString(),
          ss = date.getSeconds().toString();

      return util.format("%s_%s_%s_%s_%s_%s",
                         yyyy, zeroPad(mm), zeroPad(dd), zeroPad(HH), zeroPad(MM), zeroPad(ss));
    },

    backup = function(prefix) {
      var folderName = (prefix || "") + "tests_with_results_from_" + getProperDate();

      console.log("Backuping movie files with results...".rainbow);

      execSync.exec("mkdir assets/backup/" + folderName);

      execSync.exec("mv assets/*_tracking_result_for_*.avi assets/backup/" + folderName);
      execSync.exec("mv assets/*_results_for_*.json assets/backup/" + folderName);
    };

console.log("Compiling ./bin/tracking in release mode...".rainbow);

if (execSync.code("scons") === 0) {

  backup("before_");

  algorithms.forEach(function(algorithm) {
    glob.sync("./assets/*.avi").forEach(function(fileName) {
      var resultFileName = pathWithoutExtension(fileName) +
                           util.format(resultFileNamePostFix, sanitize(algorithm)),
          builtCommand = util.format(command, fileName, algorithm, resultFileName);

      if (fileName.indexOf(deprecatedFragment) === -1) {
        console.log("Processing '%s' file with algorithm '%s'".yellow, fileName, algorithm);

        if (execSync.code(builtCommand) === 0) {
          console.log("Results saved to file '%s'".green, resultFileName)
        } else {
          console.log("[EE] Command for file name '%s' and algorithm '%s' failed!".red,
                      fileName, algorithm);
        }
      }
    });
  });

  backup();

  parametrized.forEach(function(object) {
    var resultFileName = pathWithoutExtension(object.fileName) +
                         util.format(parametrizedResultFileNamePostFix, sanitize(object.algorithm), object.params),
        builtCommand = util.format(paramterizedCommand,
                                   object.fileName, object.algorithm, object.params, resultFileName);

    if (object.fileName.indexOf(deprecatedFragment) === -1) {
      console.log("Processing '%s' file with algorithm '%s' and parameters '%s'".yellow,
                  object.fileName, object.algorithm, object.params);

      if (execSync.code(builtCommand) === 0) {
        console.log("Results saved to file '%s'".green, resultFileName)
      } else {
        console.log("[EE] Command for file name '%s', algorithm '%s' and parameters '%s' failed!".red,
                    object.fileName, object.algorithm, object.params);
      }
    }
  });

  backup("parametrized_");

}