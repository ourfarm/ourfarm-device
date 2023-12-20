// page4.h
const char page4[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>Sensor Data Settings</title>
  <style>
    body {
      font-family: 'Arial', sans-serif;
      background-color: #f4f4f4;
      padding: 0;
      margin: 0;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
    }
    .container {
      background-color: white;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      width: 300px;
    }
    h2 {
      text-align: center;
      color: #333;
    }
    label {
      display: block;
      margin-top: 20px;
      color: #666;
    }
    input[type='text'], input[type='number'], select {
      width: 100%;
      padding: 10px;
      margin-top: 5px;
      border: 1px solid #ddd;
      border-radius: 4px;
      box-sizing: border-box;
    }
    .button {
      width: 100%;
      padding: 10px;
      margin-top: 20px;
      border: none;
      border-radius: 4px;
      color: white;
      background-color: #4CAF50;
      cursor: pointer;
    }
    .button:hover {
      background-color: #45a049;
    }
  </style>
</head>
<body>
  <div class='container'>
    <h2>Camera Data Transmission Settings</h2>
    <form id="sensorForm">
      <label for="url">URL:</label>
      <input type="text" id="url" name="url">
      <label for="interval">Interval (seconds):</label>
      <input type="number" id="interval" name="interval">
      <label for="state">State (on/off):</label>
      <select id="state">
        <option value="on">On</option>
        <option value="off">Off</option>
      </select>
      <button type="button" class='button' onclick="submitSettings()">Submit</button>
    </form>
  </div>
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