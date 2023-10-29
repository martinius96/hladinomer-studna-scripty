<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>QR Scanner</title>
</head>
<body style="margin: 0; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh;">
    <h1>QR Code Scanner</h1>
    <video id="qr-video" style="width: 100%; height: auto;" autoplay></video>
    <canvas id="qr-canvas" style="display: none;"></canvas>
    <div id="qr-result" style="margin-top: 20px;"></div>
    <div id="table-data"></div>
    <!-- Nový div pro zobrazení textu -->
    <div id="text-overlay" style="position: absolute; top: 0; left: 0; width: 100%; height: 100%; display: none; align-items: center; justify-content: center; background-color: rgba(0, 0, 0, 0.7); z-index: 999;">
        <h1 style="color: white; font-size: 48px;">HELLO WORLD</h1>
    </div>

    <script src="https://rawgit.com/schmich/instascan-builds/master/instascan.min.js"></script>
    <script>
        let scanner = new Instascan.Scanner({ video: document.getElementById('qr-video'), mirror: false });
        let loadingQR = false;
        let loadingTimeout;

        // Funkce pro zobrazení textu na obrazovce
        function showTextOverlay() {
            const overlay = document.getElementById('text-overlay');
            overlay.style.display = 'flex';
            setTimeout(() => {
                overlay.style.display = 'none';
            }, 2000); // Skrytí textu po 2 sekundách
        }

        scanner.addListener('scan', function (content) {
            if (content.startsWith("https://arduino.clanweb.eu/studna_s_prekladom/full_vypis.php")) {
                document.getElementById('qr-result').textContent = content;
                if (!loadingQR) {
                    loadingQR = true;
                    fetchAndDisplayQRContent(content);
                    showTextOverlay(); // Zobrazí text "HELLO WORLD"
                }
            } else {
                document.getElementById('qr-result').textContent = "Invalid QR Code";
            }
        });

        Instascan.Camera.getCameras().then(function (cameras) {
            if (cameras.length > 1) {
                scanner.start(cameras[1]); // 1 ==> BACK camera, 0 ==> FRONT camera
            } else {
                console.error('No cameras found.');
            }
        }).catch(function (e) {
            console.error(e);
        });

        function fetchAndDisplayQRContent(url) {
            let xhr = new XMLHttpRequest();
            xhr.open('GET', url, true);

            xhr.onload = function () {
                if (xhr.status >= 200 && xhr.status < 300) {
                    let tableData = parseTable(xhr.responseText);
                    displayTable(tableData);
                    loadingTimeout = setTimeout(function () {
                        loadingQR = false;
                        document.getElementById('table-data').innerHTML = '';
                    }, 5000); // Po 5 sekundách zastaviť zobrazovanie
                } else {
                    console.error('Error loading data from URL');
                }
            };

            xhr.send();
        }

        function parseTable(html) {
            // Rovnaký kód na analýzu tabuľky
        }

        function displayTable(data) {
            // Rovnaký kód na zobrazenie tabuľky
        }
    </script>
</body>
</html>