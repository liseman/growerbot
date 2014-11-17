function requestHandler(request, response) {
    server.log("loaded");
  try {
    // check if the user sent led as a query parameter
    local lastData = server.load();
    local output = "<html><head><title>Test Growerbot</title></head><body><h1>Output data from Growerbot</h1>";
    
    if (lastData.len() != 0)
    {
        output = output + "<ul><li> Temperature : <strong>"+ lastData.temp +" &deg;C</strong></li>";
        output = output + "<li> Moisture level : <strong>"+ lastData.moist +" </strong></li>";
        output = output + "<li> Humidity : <strong>"+ lastData.humid +" %</strong></li>";
        output = output + "<li> Light : <strong>"+ lastData.light +" Lux</strong></li></ul>";
    }
    else
    {
        output = output = output + "<h2>NO DATA YET!</h2>";
    }
    
    output = output + "</body></html>";
    // send a response back saying everything was OK.
    response.send(200, output);
  } catch (ex) {
    response.send(500, "Internal Server Error: " + ex);
  }
}

function updateData(data){
    server.save(data);
    server.log("saved");
}
 
// register the HTTP handler
http.onrequest(requestHandler);
device.on("update_date", updateData);
