    "use strict";

var fs = require("fs"),
    path = require("path"),
    util = require("util"),
    execSync = require("execSync"),

    common = require("./common"),

    repository = require("./repository"),

    XmlHeader = '<?xml version="1.0" encoding="utf-8"?>\n<FeaturePointList>\n    <hand>',

    RadiusTag = '        <FeaturePointRadius r="%s" />',
    PointTag = '        <FeaturePoint x="%s" y="%s" />',

    XmlEnding = '    </hand>\n</FeaturePointList>',

    ResultMovieBeforeConversionPostFix = "_tracking_result_for_%s.avi",
    ResultMoviePostFix = "_tracking_result_for_%s.webm",

    keypoints = exports = module.exports = {};

keypoints.saveKeypoints = function(movieObject, keypoints, radius) {
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
  common.debug(util.format("File with keypoints (%s) saved.", keypointsFile));

  xmlContent.push(XmlHeader);
  xmlContent.push(util.format(RadiusTag, radius.toString()));

  keypoints.forEach(function(element) {
    xmlContent.push(util.format(PointTag, element.x, element.y));
  });

  xmlContent.push(XmlEnding);

  fs.writeFile(util.format("./assets/%s", xmlFile), xmlContent.join("\n"));
  common.debug(util.format("XML file with keypoints (%s) saved.", xmlFile));
};

keypoints.trackPoints = function(additionalArguments, movieObject, algorithmId) {
  var trackingInvocation,
      conversionInvocation,
      argumentsList,
      algorithm,
      filename;

  if (!!movieObject) {
    argumentsList = additionalArguments.map(common.toArgument).join(" ");
    algorithm = repository.getAlgorithmsList()[algorithmId].name;

    trackingInvocation = util.format('./bin/tracking %s "%s" %s',
                                     movieObject.path,
                                     algorithm,
                                     argumentsList);

    common.debug("Invoking: " + trackingInvocation);

    if (execSync.code(trackingInvocation) === 0) {
        filename = common.onlyBaseName(movieObject.path);

        conversionInvocation = util.format("./convert-one.sh %s",
                                           filename +
                                           common.sanitize(ResultMovieBeforeConversionPostFix, algorithm));

        common.debug("Invoking: " + conversionInvocation);

        if (execSync.code(conversionInvocation) === 0) {
          common.debug("Returning new video: " + filename + common.sanitize(ResultMoviePostFix, algorithm));

          return util.format("%s", filename + common.sanitize(ResultMoviePostFix, algorithm));
        }
    }
  }

  return null;
};