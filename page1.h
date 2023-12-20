const char page1[] PROGMEM = R"=====(
<!DOCTYPE html>
<!DOCTYPE html>
<html>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Smart Farm Setup</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            background-color: #f4f4f4;
            margin: 0;
            padding: 0;
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
        h1 {
            color: #333;
            text-align: center;
        }
        label {
            display: block;
            margin-top: 20px;
            color: #666;
        }
        input[type='text'] {
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
        .button.reset {
            background-color: #f44336;
        }
        .button.reset:hover {
            background-color: #e53935;
        }
        .links {
            text-align: center;
            margin-top: 20px;
        }
        .link {
            margin: 0 10px;
            color: #4CAF50;
            text-decoration: none;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <div class='container'>
        <h1>Smart Farm Configuration</h1>
        <label for='ssid'>WiFi SSID:</label>
        <input type='text' id='ssid' name='ssid' value>
        <label for='pass'>Password:</label>
        <input type='text' id='pass' name='pass' value>
        <button id='update' class='button'>Update</button>
        <button id='reset' class='button reset'>Reset</button>
        <div class='links'>
            <a href="/page3" class="link">Sensor</a>
            <a href="/page4" class="link">Camera</a>
        </div>
    </div>
<script>
  document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('update').addEventListener('click', function() {
      fetch('/setAP', {
         method: 'POST',
         headers: { 'Content-Type': 'application/json' },
         body: JSON.stringify({
           ssid:document.getElementById('ssid').value,
           pass:document.getElementById('pass').value
            })
         })
        .then(response => response.json())
        .then(response => { document.getElementById('ssid').value = response['ssid'],
        document.getElementById('pass').value = response['pass'],  
        alert(JSON.stringify(response))})
   }),
   document.getElementById('reset').addEventListener('click', function() {
      fetch('/reset', {
         method: 'POST',
         headers: { 'Content-Type': 'application/json' },
         body: JSON.stringify({
           reset:document.getElementById('reset').value,
         })
      })
      .then(response => response.json()) 
   })   
  })
 
  document.addEventListener('DOMContentLoaded', async function () {
    const ssid = document.getElementById('ssid');
    const pass = document.getElementById('pass');

    let response = await fetch('/getAP');
    let result =   await response.json();

    ssid.value = result['ssid'];
    pass.value = result['pass'];
  })
  </script>
</body>
</html>
)=====";