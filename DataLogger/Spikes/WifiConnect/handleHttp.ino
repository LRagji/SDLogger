/** Handle root or redirect to captive portal */
//void handleRoot(AsyncWebServerRequest *request) {
//  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
//    return;
//  }
//  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
//  server.sendHeader("Pragma", "no-cache");
//  server.sendHeader("Expires", "-1");
//
//  String Page;
//  Page += F(
//            "<!DOCTYPE html><html lang='en'><head>"
//            "<meta name='viewport' content='width=device-width'>"
//            "<title>CaptivePortal</title></head><body>"
//            "<h1>HELLO WORLD!!</h1>");
//  //  if (server.client().localIP() == apIP) {
//  //    Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
//  //  } else {
//  Page += String(F("<p>You are connected through the wifi network: ")) + ssid + F("</p>");
//  //  }
//  Page += F(
//            "<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
//            "</body></html>");
//
//  server.send(200, "text/html", Page);
//}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
//boolean captivePortal() {
//  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname) + ".local")) {
//    Serial.println("Request redirected to captive portal");
//    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
//    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
//    server.client().stop(); // Stop is needed because we sent no content length
//    return true;
//  }
//  return false;
//}

/** Wifi config page handler */
void handleWifi(AsyncWebServerRequest *request) {
  //  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  //  server.sendHeader("Pragma", "no-cache");
  //  server.sendHeader("Expires", "-1");

  String Page;
  Page += F(
            "<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>CaptivePortal</title></head><body>"
            "<h1>Wifi config</h1>");
  //  if (server.client().localIP() == apIP) {
  //    Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
  //  } else {
  Page += String(F("<p>You are connected through the wifi network: ")) + ssid + F("</p>");
  //}
  Page +=
    String(F(
             "\r\n<br />"
             "<table><tr><th align='left'>SoftAP config</th></tr>"
             "<tr><td>SSID ")) +
    // String(softAP_ssid) +
    F("</td></tr>") +
    //toStringIp(WiFi.softAPIP()) +
    F("</table>"
      "\r\n<br />"
      "<table><tr><th align='left'>WLAN config</th></tr>"
      "<tr><td>SSID ") +
    String(ssid) +
    F("</td></tr>"
      "<tr><td>IP ") +
    toStringIp(WiFi.localIP()) +
    F("</td></tr>"
      "</table>"
      "\r\n<br />"
      "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>");
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      Page += String(F("\r\n<tr><td>SSID ")) + WiFi.SSID(i) + ((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? F(" ") : F(" *")) + F(" (") + WiFi.RSSI(i) + F(")</td></tr>");
    }
  } else {
    Page += F("<tr><td>No WLAN found</td></tr>");
  }
  Page += F(
            "</table>"
            "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
            "<input type='text' placeholder='network' name='n'/>"
            "<br /><input type='password' placeholder='password' name='p'/>"
            "<br /><input type='submit' value='Connect/Disconnect'/></form>"
            "<p>You may want to <a href='/'>return to the home page</a>.</p>"
            "</body></html>");
  //  server.send(200, "text/html", Page);
  //  server.client().stop(); // Stop is needed because we sent no content length
  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", Page);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");
  request->send(response);
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave(AsyncWebServerRequest *request) {
  Serial.println("wifi save");
  if (request->hasParam("n", true) && request->hasParam("p", true))
  {
    AsyncWebParameter* n = request->getParam("n", true);
    AsyncWebParameter* p = request->getParam("p", true);
    n->value().toCharArray(ssid, sizeof(ssid) - 1);
    p->value().toCharArray(password, sizeof(password) - 1);
    saveCredentials();
  }
  else
  {
    Serial.println("Param n :" + request->hasParam("n"));
    Serial.println("Param p :" + request->hasParam("p"));
  }
  request->redirect("/");
}

void handleNotFound(AsyncWebServerRequest *request) {
  String message = F("File Not Found\n\n");
  message += timeClient.getFormattedTime();
  message += F("\nURI: ");
  message += request->url();
  message += F("\nMethod: ");
  message += request->method();

  message += F("\n\nHeaders: ");
  int headers = request->headers();
  int i;
  for (i = 0; i < headers; i++) {
    message += request->headerName(i);
    message += F(" : ");
    message += request->header(i);
    message += F("\n");
  }

  message += F("\nParams: ");
  int params = request->params();
  for (int i = 0; i < params; i++) {
    AsyncWebParameter* p = request->getParam(i);
    if (p->isFile()) { //p->isPost() is also true
      message += F("\nFile: ");
      message += p->name();
      message += F(" Size: ");
      message += p->size();
    } else if (p->isPost()) {
      message += F("\nPost: ");
      message += p->name();
      message += F(" = ");
      message += p->value();
    } else {
      message += F("\nGet: ");
      message += p->name();
      message += F(" = ");
      message += p->value();
    }
  }

  AsyncWebServerResponse *response = request->beginResponse(404, "text/plain", message);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");
  request->send(response);
}

void handleReset(AsyncWebServerRequest *request)
{
  resetStore();
  String Page;
  Page += F(
            "<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>CaptivePortal</title></head><body>"
            "Configuration is cleared, please restart the device to entire config mode.</body></html>");
  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", Page);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");
  request->send(response);
}
