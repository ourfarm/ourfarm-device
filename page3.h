const char page3[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
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

    <h2>LED Control</h2>
    <label for="auto_url">Auto Mode URL:</label>
    <input type="text" id="auto_url" name="auto_url"><br><br>
    <label for="led_url">Manual Mode URL:</label>
    <input type="text" id="led_url" name="led_url"><br><br>

    <button type="button" onclick="submitSettings()">Submit</button>
  </form>

  <script>
    function submitSettings() {
      var url = document.getElementById('url').value;
      var interval = document.getElementById('interval').value;
      var state = document.getElementById('state').value;
      var auto_url = document.getElementById('auto_url').value;
      var led_url = document.getElementById('led_url').value;

      fetch('/setSensor', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ url, interval, state, auto_url, led_url })
      }).then(response => {
        if (!response.ok) {
          throw new Error('Network response was not ok');
        }
        return response.json();
      }).then(data => {
        console.log('Success:', data);
      }).catch((error) => {
        console.error('Error:', error);
      });
    }

    document.addEventListener('DOMContentLoaded', function() {
      fetch('/getSensor')
        .then(response => response.json())
        .then(data => {
          document.getElementById('url').value = data.url;
          document.getElementById('interval').value = data.interval;
          document.getElementById('state').value = data.state;
          document.getElementById('auto_url').value = data.auto_url;
          document.getElementById('led_url').value = data.led_url;
        });
    });
  </script>
</body>
</html>
)=====";
