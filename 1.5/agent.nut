const FEED_ID = "874455613";
const API_KEY = "wu15xJAHf39lmYqOsIaTNfGFBRR8vxYfAdq0UYumkgqKlshM";
function send_xively(body) {         //take in csv value
    local xively_url = "https://api.xively.com/v2/feeds/" + FEED_ID + ".csv";       //setup url for csv
    server.log(xively_url);
    server.log(body);       //pring body for testing
    local req = http.put(xively_url, {"X-ApiKey":API_KEY, "Content-Type":"text/csv", "User-Agent":"Xively-Imp-Lib/1.0"}, body);     //add headers
    local res = req.sendsync();         //send request
    if(res.statuscode != 200) {
        server.log("error sending message: "+res.body);
    }else device.send("status", (res.statuscode + " OK"));      //sends status to uart. this can be removed if not desired
}
  
device.on("data", function(feedCSV) {       //take csv body in from device
    server.log("device on");
     
    //send preformatted multi-ds csv
    send_xively(feedCSV);         //send to function to call xively
});