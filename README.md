# Air
---
About this project: I discovered the popular Plantower sensors. The fanciest sensor available senses 8 different things.

This sensor is useless without a way to see its data realtime, so the ESP32 seemed like a good way to read its data and upload it to my web server. The Python CGI script was a fun way to receive the data with no further dependencies on the server - just Linux, Apache, and Python3. Historical data then gets served from an NDJSON file, air.dat, and a realtime file, air.json. The dashboard is vanilla HTML/JS.

More about the PTQS1005 sensor: it measures PM 1.0, PM 2.5, PM 10, TVOC, Formaldehyde, CO2, Humidity, and Temperature. Humidity and Temperature are not at all calibrated but a very simple calibration makes them somewhat usable.

### Wire up the Plantower PTQS1005 Sensor

### Program your ESP32

### Install the CGI data receiver

### Serve the dashboard
