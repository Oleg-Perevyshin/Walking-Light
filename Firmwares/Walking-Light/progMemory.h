
#ifndef PROGMEMORY_H
#define PROGMEMORY_H

// ########################################### //
// ###########   DATA IN PROGMEM   ########### //
// ########################################### //

// ******************************************* //
//               MAIN index_htm                //
// ******************************************* //
static const char index_htm[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <title>SOI Tech | Device Manager</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="shortcut icon" href="data:image/x-icon;base64,AAABAAEAEBAAAAEACABoBQAAFgAAACgAAAAQAAAAIAAAAAEACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/plUAP2aWAD4pW0A9at5ACHchwAi3YgAJt2JADDcjgA/3JUAT9ycAFPdngBW3Z8Aat2oAHHdqwBz3awA8LSMAOu+oQCJ3LYAjt25AOrApADlyrgApNzDAKjdxQC93M4Ay9zUAM/d1gDR3NcA1tzZANnc2wDc3NwA4tLGAOPe2wDj3twA1ODbANnh3QDi4uIA4eTjAOXk4wDl5eUA6urqAO/v7wDx8fEA9fX1APr6+gD9/f0A0fv/AP///wAAAAAAAC8hAABQNwAAcEwAAJBjAACweQAAz48AAPCmABH/tAAx/74AUf/IAHH/0wCR/9wAsf/lANH/8AD///8AAAAAAAAvDgAAUBgAAHAiAACQLAAAsDYAAM9AAADwSgAR/1sAMf9xAFH/hwBx/50Akf+yALH/yQDR/98A////AAAAAAACLwAABFAAAAZwAAAIkAAACrAAAAvPAAAO8AAAIP8SAD3/MQBb/1EAef9xAJj/kQC1/7EA1P/RAP///wAAAAAAFC8AACJQAAAwcAAAPZAAAEywAABZzwAAZ/AAAHj/EQCK/zEAnP9RAK7/cQDA/5EA0v+xAOT/0QD///8AAAAAACYvAABAUAAAWnAAAHSQAACOsAAAqc8AAMLwAADR/xEA2P8xAN7/UQDj/3EA6f+RAO//sQD2/9EA////AAAAAAAvJgAAUEEAAHBbAACQdAAAsI4AAM+pAADwwwAA/9IRAP/YMQD/3VEA/+RxAP/qkQD/8LEA//bRAP///wAAAAAALxQAAFAiAABwMAAAkD4AALBNAADPWwAA8GkAAP95EQD/ijEA/51RAP+vcQD/wZEA/9KxAP/l0QD///8AAAAAAC8DAABQBAAAcAYAAJAJAACwCgAAzwwAAPAOAAD/IBIA/z4xAP9cUQD/enEA/5eRAP+2sQD/1NEA////AAAAAAAvAA4AUAAXAHAAIQCQACsAsAA2AM8AQADwAEkA/xFaAP8xcAD/UYYA/3GcAP+RsgD/scgA/9HfAP///wAAAAAALwAgAFAANgBwAEwAkABiALAAeADPAI4A8ACkAP8RswD/Mb4A/1HHAP9x0QD/kdwA/7HlAP/R8AD///8AAAAAACwALwBLAFAAaQBwAIcAkAClALAAxADPAOEA8ADwEf8A8jH/APRR/wD2cf8A95H/APmx/wD70f8A////AAAAAAAbAC8ALQBQAD8AcABSAJAAYwCwAHYAzwCIAPAAmRH/AKYx/wC0Uf8AwnH/AM+R/wDcsf8A69H/AP///wAAAAAACAAvAA4AUAAVAHAAGwCQACEAsAAmAM8ALADwAD4R/wBYMf8AcVH/AIxx/wCmkf8Av7H/ANrR/wD///8AAAAAAAAoIR8fISgAAAAAAAAAACgVBAICAQIEFSgAAAAAACcQAgECAQEBAQEQJwAAACgUAwMDAwMDAwMDAxEoAAAeIR4eHh4eHiQeHh4eHgAoKR4eHh4eHh4eHh4kHQ8oJS0oHh4eHh4eHh4eHhMHIyQtLR4eHh4eHh4eHhoIBhgkLS8pHh4eHh4eHh4OBgYYJC0vLSQeHh4eHh4YBwYHIygrLy8rHh4eHh4eDAYGCygAJy0vLSceHh4eFgYGBhcAACgqLy8sHh4eHAkGBg0oAAAAJyotLygeHhIFBg0lAAAAAAAoJystIRkIChYoAAAAAAAAAAAoJB4bIigAAAAAAPgfAADgBwAAwAMAAIABAACAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAEAAIABAADAAwAA4AcAAPgfAAA=" />
    <link type="text/css" rel="stylesheet" href="style.css">
  </head>
  <body class="loading">
    <div id="app"></div>
    <div id="loader"></div>
    <script src="main.js"></script>
  </body>
</html>
)=====";

// ******************************************* //
//                ICON DEVICE                  //
// ******************************************* //
static const char icon[] PROGMEM = R"=====(
<img style="height:4em;" src='data:image/svg+xml;utf8,
  <svg xmlns="http://www.w3.org/2000/svg" width="50" height="50" viewBox="0 0 13.229166 13.229166" id="icon">
    <style>path,
      circle{stroke-linecap:round;stroke-linejoin:round}
      .a{stroke:#d1cc19}
      .b{cx:6.615;cy:6.615}
      .c{fill:none}
      .d{stroke:#999}
      stroke-width:.3
    </style>
    <circle class="a b" r="5.821" fill="#fdfcdc" stroke-width="1.5" paint-order="stroke fill markers"/>
    <circle class="b c" r="6.548" stroke="#555" stroke-width=".1" paint-order="stroke fill markers"/>
    <path class="a c" d="M2.738 9.914h7.606M4.198 8.604l6.85-.01M5.146 7.273h5.78M5.12 5.953h4.545M4.218 4.633h3.598M2.737 3.312l3.172.02" stroke-width=".6"/>
    <path class="c d" d="M9.454 7.962c3.273-2.95-.053-4.756-3.198-6.54M7.273 4.17V2.084M9.275 5.488V3.4M10.108 9.443V7.355"/>
  </svg>'
/>
)=====";

// ******************************************* //
//      QUICK SETTINGS IN DEVICE BLOCK         //
// ******************************************* //
static const char dev[] PROGMEM = R"=====(
<p class="bold">Режим работы</p>
<button id="gmd" class="button w30 red large" value="0">Выключено</button>
<button id="gmd" class="button w30 red large" value="1">Включено</button>
<button id="gmd" class="button w30 red large" value="2">Имитация</button>
)=====";

// ******************************************* //
//               TEMPLATE SSDP                 //
// ******************************************* //
const char* ssdpTemplate = R"=====(
<?xml version="1.0"?>
<root xmlns="urn:schemas-upnp-org:device-1-0">
  <specVersion>
    <major>1</major>
    <minor>0</minor>
  </specVersion>
//  <URLBase>http://%u.%u.%u.%u/</URLBase>
  <device>
    <deviceType>upnp:rootdevice</deviceType>
    <friendlyName>%s | %s</friendlyName>
    <manufacturer>%s</manufacturer>
    <manufacturerURL>%s</manufacturerURL>
    <modelDescription>UPnP Lighting Control</modelDescription>
    <modelName>%s</modelName>
    <modelNumber>%s</modelNumber>
    <modelURL>%s</modelURL>
    <serialNumber>%s</serialNumber>
    <UDN>uuid:edc3b25e-c2d0-411e-b04b-f9bd5e%02x%02x%02x</UDN>
    <presentationURL>/</presentationURL>
  </device>
  <iconList>
    <icon>
      <mimetype>image/png</mimetype>
      <height>48</height>
      <width>48</width>
      <depth>24</depth>
      <url>icon48.png</url>
    </icon>
    <icon>
      <mimetype>image/png</mimetype>
      <height>120</height>
      <width>120</width>
      <depth>24</depth>
      <url>icon120.png</url>
    </icon>
  </iconList>
</root>\r\n\r\n
)=====";

#endif
