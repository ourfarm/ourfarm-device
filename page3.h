// page3.h
const char page3[] PROGMEM = R"=====(
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
    <label for="ip">IP Address:</label>
    <input type="text" id="ip" name="ip"><br><br>
    <label for="port">Port:</label>
    <input type="number" id="port" name="port"><br><br>
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
      var ip = document.getElementById('ip').value;
      var port = document.getElementById('port').value;
      var interval = document.getElementById('interval').value;
      var state = document.getElementById('state').value;

      fetch('/setSensor', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ ip, port, interval, state })
      });
    }
    document.addEventListener('DOMContentLoaded', function() {
      fetch('/getSensor')
        .then(response => response.json())
        .then(data => {
          document.getElementById('ip').value = data.ip;
          document.getElementById('port').value = data.port;
          document.getElementById('interval').value = data.interval;
          document.getElementById('state').value = data.state;
        });
    });
  </script>
</body>
</html>
)=====";