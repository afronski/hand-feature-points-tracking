window.Common = (function() {

  // Because all JavaScript projects have jQuery ;)
  function $(selector) {
    var results = document.querySelectorAll(selector);

    return results.length == 1 ? results[0] : results;
  };

  // Methods related with overlay.
  function hideOverlay() {
    $(".overlay").classList.add("hidden");
  }

  function showOverlay() {
    $(".overlay").classList.remove("hidden");
  }

  // XHR methods.
  function xhr(method, path, callback, args) {
    var request = new XMLHttpRequest();

    request.open(method, path, true);

    if (method.toUpperCase() === "POST") {
      request.setRequestHeader("Content-Type", "application/json");
    }

    request.addEventListener("load", callback || hideOverlay);
    request.send(JSON.stringify(args));

    showOverlay();
  }

  function xhrGet(path, callback) {
    xhr("GET", path, callback);
  }

  function xhrPost(path, args, callback) {
    xhr("POST", path, callback, args);
  }

  // Button helpers.
  function toggleButton(selector, value) {
    var button = $(selector);

    if (!!value) {
      button.removeAttribute("disabled");
    } else {
      button.setAttribute("disabled", "disabled");
    }
  }

  return {
    $: $,

    xhrGet: xhrGet,
    xhrPost: xhrPost,

    toggleButton: toggleButton,

    hideOverlay: hideOverlay
  };
} ());