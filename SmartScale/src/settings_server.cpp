#include "settings_server.hpp"

AsyncWebServer Settings_Server::server(AP_SERVER_PORT);
IPAddress Settings_Server::myAPIP(0,0,0,0);

DeviceSettings* Settings_Server::settings;

char* Settings_Server::html = "\
<form>\
	<p>WI-FI</p>\
	<label for='ssid'>SSID:</label><br>\
	<input type='text' id='ssid' name='ssid'><br>\
	<label for='wifi_password'>Password:</label><br>\
	<input type='password' id='wifi_password' name='wifi_password'><br><br>\
	<p>Server Login</p>\
	<label for='username'>Username:</label><br>\
	<input type='text' id='username' name='username'><br>\
	<label for='server_password'>Password:</label><br>\
	<input type='password' id='server_password' name='server_password'><br><br>\
	<input type='submit' value='Submit'>\
</form>\
";

/**
 * @brief Initialise local server ready for use
 * 
 * @param meshAPIP ip of the mesh
 */
void Settings_Server::setupServer(IPAddress meshAPIP){
    settings = DeviceSettings::getInstance();

    myAPIP = meshAPIP;
    Serial.println("My AP IP is " + myAPIP.toString());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", html);

        bool shouldRestart = false;

        if (request->hasArg("ssid") && request->hasArg("wifi_password")){
            String ssid = request->arg("ssid");
            String pass = request->arg("wifi_password");
            if(ssid != "" && pass != ""){
                Serial.println("SSID = " + ssid + " - Password = " + pass);
                // Save
                settings->setSSID(ssid);
                settings->setWIFIPassword(pass);

                shouldRestart = true;
            }
        }

        if(request->hasArg("username") && request->hasArg("server_password")){
            String username = request->arg("username");
            String pass = request->arg("server_password");
            if(username != "" && pass != ""){
                Serial.println("User = " + username + " - Password = " + pass);
                // Save
                settings->setUsername(username);
                settings->setPassword(pass);
                
                shouldRestart = true;
            }
        }

        // Updating WIFI info requires restart to use
        if(shouldRestart){
            ESP.restart();
        }
    });

}

/**
 * @brief Function to get a string of the ip and port the http server is on so you can connect
 * 
 * @return String of the full ip and port number
 */
String Settings_Server::getAPIPStr(void){
    return myAPIP.toString() + ":" + String(AP_SERVER_PORT);
}

/**
 * @brief Function to turn the http server on
 * 
 */
void Settings_Server::httpServerOn(){
    server.begin();
}

/**
 * @brief Function to stop hosting the http server
 * 
 */
void Settings_Server::httpServerOff(){
    server.end();
}