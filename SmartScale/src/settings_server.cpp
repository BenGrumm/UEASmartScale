#include "settings_server.hpp"

AsyncWebServer server(AP_SERVER_PORT);
IPAddress myAPIP(0,0,0,0);

void setupServer(IPAddress meshAPIP){

    myAPIP = meshAPIP;
    Serial.println("My AP IP is " + myAPIP.toString());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", "<form>Text to Broadcast<br><input type='text' name='BROADCAST'><br><br><input type='submit' value='Submit'></form>");
        if (request->hasArg("BROADCAST")){
            String msg = request->arg("BROADCAST");
        }
    });

}

String getAPIPStr(void){
    return myAPIP.toString() + ":" + String(AP_SERVER_PORT);
}

void httpServerOn(){
    server.begin();
}

void httpServerOff(){
    server.end();
}