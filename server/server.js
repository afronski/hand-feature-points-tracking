    require("colors");

var PORT = 9292,

    express = require("express"),
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
    };

app.use(express.static("client"));
app.use(express.bodyParser());

app.get("/movies", function(request, response) {
  debug("Get movie list");

  sendJSON(response, [ { name: "1.avi", value: 1 } ]);
});

app.get("/frame/:id", function(request, response) {
  debug("Get first frame from AVI with ID " + request.params.id);

  sendJSON(response, { status: "OK" });
});

app.post("/coordinates", function(request, response) {
  debug("Save coordinates for salient point " + JSON.stringify(request.body));

  sendJSON(response, { status: "OK" });
});

app.listen(PORT);
console.log("Listening on port ".rainbow + PORT);