(function(Common, d3) {
  "use strict";

  // Alias and variables.
  var $ = Common.$;

  // Changing result.
  function onResultChanged(event) {

  }

  // Enable UI and hide overlay after loading results.
  function enableUI() {
    var results = $("#results");

    results.removeAttribute("disabled");
    results.addEventListener("change", onResultChanged);

    Common.hideOverlay();
  }

  // Initialization.
  function init() {
    Common.getCollection(enableUI, "results", "#results");
  }

  document.addEventListener("DOMContentLoaded", init, true);

} (window.Common, window.d3));