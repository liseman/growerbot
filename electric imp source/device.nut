//read serial value and display on imp node
hardware.uart1289.configure(2400, 8, PARITY_NONE, 1, NO_CTSRTS);

function writeStuff()
{
    local c = "";
    local b = hardware.uart1289.read();
    
    while (b != -1) {
        c += b.tochar();
        b = hardware.uart1289.read();
    }
    
    if (c.len())
    {
        server.log(c);
        
        //find location of T in sensor data string
        local tStart = c.find("T");
        //add 1 to get rid of T at start of data string
        tStart = tStart + 1;
        //trim data string c to get rid of T and everything before
        local temp = c.slice(tStart);
        //get rid of T in temp and everything after it
        local tEnd = temp.find("T");
        temp = temp.slice(0,tEnd);
        server.log("Temp : "+temp);
    
        //find location of M in sensor data string
        local mStart = c.find("M");
        //add 1 to get rid of M at start of data string
        mStart = mStart + 1;
        //trim data string c to get rid of M and everything before
        local moist = c.slice(mStart);
        //get rid of M in moist and everything after it
        local mEnd = moist.find("M");
        moist = moist.slice(0,mEnd);
        server.log("Moist : "+moist);
            
        
        //find location of H in sensor data string
        local hStart = c.find("H");
        hStart = hStart + 1;
        local humid = c.slice(hStart);
        local hEnd = humid.find("H");
        humid = humid.slice(0,hEnd);
        server.log("Humidity : "+humid);
        
        //find location of L in sensor data string
        local lStart = c.find("L");
        lStart = lStart + 1;
        local light = c.slice(lStart);
        local lEnd = light.find("L");
        light = light.slice(0,lEnd);
        server.log("Light : "+light);
        
        //Setup data to be send to agent
        data <- {};
         
        data.temp <- temp;
        data.moist <- moist;
        data.humid <- humid;
        data.light <- light;

        //Call agent save function
        agent.send("update_date",data);
        
    }
    
    //example of changing light proportion remotely
    //hardware.uart1289.write("L0.4L\n");
    //example of changing moisture goal remotely
    //hardware.uart1289.write("M500M\n");
    //example of changing light goal remotely
    //hardware.uart1289.write("B50B\n");
    //example of turning on light remotely
    //hardware.uart1289.write("Ll1\n");
    //example of turning on water remotely
    //hardware.uart1289.write("Pp1\n");
    
    imp.wakeup(5, writeStuff);
}
writeStuff();
