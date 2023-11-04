
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>QR Scanner</title>
</head>
<body style="margin: 0; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh;">
    <h1>AR - Water level monitor</h1>
    <video id="qr-video" style="width: 100%; height: auto;" autoplay></video>
    <canvas id="qr-canvas" style="display: none;"></canvas>
    <div id="qr-result" style="margin-top: 20px;"></div>
    <!-- Nov? div pro zobrazen? tabulky -->
    <div id="table-overlay" style="position: absolute; top: 0; left: 0; width: 100%; height: 100%; display: none; align-items: center; justify-content: center; background-color: rgba(0, 0, 0, 0.7); z-index: 999;">
        <table id="table-data" style="color: white; font-size: 24px; border: 2px solid white; border-collapse: collapse;">
            <tr>
                <th style="border: 2px solid white; padding: 10px;">Attribute</th>
                <th style="border: 2px solid white; padding: 10px;">Value</th>
            </tr>
            <tr>
                <td style="border: 2px solid white; padding: 10px;">Height:</td>
                <td style="border: 2px solid white; padding: 10px;" id="height-value"></td>
            </tr>
            <tr>
                <td style="border: 2px solid white; padding: 10px;">Volume:</td>
                <td style="border: 2px solid white; padding: 10px;" id="volume-value"></td>
            </tr>
        </table>
    </div>

    <script src="https://rawgit.com/schmich/instascan-builds/master/instascan.min.js"></script>
    <script>
        let scanner = new Instascan.Scanner({ video: document.getElementById('qr-video'), mirror: false });
        let lastQRScanTime = 0;
        let tableTimeout;

        // Funkce pro zobrazen? tabulky na obrazovce
        function showTableOverlay() {
            const overlay = document.getElementById('table-overlay');
            overlay.style.display = 'flex';
        }

        // Funkce pro skryt? tabulky na obrazovce
        function hideTableOverlay() {
            const overlay = document.getElementById('table-overlay');
            overlay.style.display = 'none';
        }

        // Funkce pro aktualizaci casu posledn?ho skenov?n?
        function updateLastQRScanTime() {
            lastQRScanTime = Date.now();
        }

        let isTableVisible = false;

        scanner.addListener('scan', function (content) {
            if (content.startsWith("https://arduino.clanweb.eu/studna_s_prekladom/full_vypis.php")) {
                document.getElementById('qr-result').textContent = "QR Code Detected: " + content;
                updateLastQRScanTime(); // Aktualizovat cas posledn?ho skenov?n? QR k?du
                showTableOverlay();
                isTableVisible = true;
                loadJSONData();
            } else {
                document.getElementById('qr-result').textContent = "Invalid QR Code";
                hideTableOverlay();
                isTableVisible = false;
            }
        });

        Instascan.Camera.getCameras().then(function (cameras) {
            if (cameras.length > 1) {
                scanner.start(cameras[1]); // 1 ==> BACK camera, 0 ==> FRONT camera
            } else {
                console.error('No cameras found.');
            }
        }).catch(function (e) {
            console.error('Error accessing camera: ' + e);
        });

        // Kontrola na skryt? tabulky ka?dou sekundu
        setInterval(function() {
            if (isTableVisible && Date.now() - lastQRScanTime >= 5000) {
                hideTableOverlay();
                isTableVisible = false;
            }
        }, 1000);

        // Funkce pro nac?t?n? JSON dat a aktualizaci tabulky
        function loadJSONData() {
            const apiUrl = 'https://corsproxy.io/?https%3A%2F%2Farduino.clanweb.eu%2Fstudna_s_prekladom%2Fjson_output.php';

            fetch(apiUrl)
                .then(response => response.json())
                .then(data => {
                    // Process the data from the external API
                    console.log(data);
                    const valueInCm = data.value + " cm ";
                    const volumeInLiters = data.volume + " liters";
                    document.getElementById('height-value').textContent = valueInCm;
                    document.getElementById('volume-value').textContent = volumeInLiters;
                })
                .catch(error => {
                    document.getElementById('qr-result').textContent = "Error loading JSON data: " + error;
                });
        }
    </script>
</body>
</html>
