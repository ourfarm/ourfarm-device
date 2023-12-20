// page4.h
const char page4[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <style>

  </style>
  <title>Sensor Data Settings</title>
</head>
<body>
  <h2>Sensor Data Transmission Settings</h2>
  <form id="sensorForm">
    <label for="url">URL:</label>
    <input type="text" id="url" name="url"><br><br>
    <label for="interval">Interval (seconds):</label>
    <input type="number" id="interval" name="interval"><br><br>
    <label for="state">State (on/off):</label>
    <select id="state">
      <option value="on">On</option>
      <option value="off">Off</option>
    </select><br><br>
    <button type="button" onclick="submitSettings()">Submit</button>
  </form>
  <script>
    function submitSettings() {
      var url = document.getElementById('url').value;
      var interval = document.getElementById('interval').value;
      var state = document.getElementById('state').value;

      fetch('/setCamera', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ url, interval, state })
      });
    }
    document.addEventListener('DOMContentLoaded', function() {
      fetch('/getCamera')
        .then(response => response.json())
        .then(data => {
          document.getElementById('url').value = data.url;
          document.getElementById('interval').value = data.interval;
          document.getElementById('state').value = data.state;
        });
    });
  </script>
</body>
</html>
)=====";