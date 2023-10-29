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

    <script src="https://rawgit.com/schmich/instascan-builds/master/instascan.min.js"></script>
    <script>
        // Inicializácia QR scanneru
        let scanner = new Instascan.Scanner({ video: document.getElementById('qr-video'), mirror: false });

        scanner.addListener('scan', function (content) {
            if (content.startsWith("https://arduino.clanweb.eu/studna_s_prekladom/full_vypis.php")) {
                document.getElementById('qr-result').textContent = content;
                fetchTableData(content);
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

        // Načítanie údajov z URL a zobrazenie tabuľky
        function fetchTableData(url) {
            let xhr = new XMLHttpRequest();
            xhr.open('GET', url, true);

            xhr.onload = function () {
                if (xhr.status >= 200 && xhr.status < 300) {
                    let tableData = parseTable(xhr.responseText);
                    displayTable(tableData);
                } else {
                    console.error('Error loading data from URL');
                }
            };

            xhr.send();
        }

        // Analýza tabuľky z HTML
        function parseTable(html) {
            let parser = new DOMParser();
            let doc = parser.parseFromString(html, 'text/html');
            let table = doc.querySelector('table');
            if (!table) {
                return [];
            }

            let data = [];
            let rows = table.querySelectorAll('tr');
            for (let i = 0; i < rows.length; i++) {
                let row = rows[i];
                let columns = row.querySelectorAll('td');
                if (columns.length === 2) {
                    let rowData = {
                        name: columns[0].querySelector('h4').textContent,
                        value: columns[1].textContent.trim()
                    };
                    data.push(rowData);
                }
            }

            return data;
        }

        // Zobrazenie tabuľky používateľovi
        function displayTable(data) {
            let tableDataDiv = document.getElementById('table-data');
            tableDataDiv.innerHTML = ''; // Vyprázdni predchádzajúci obsah

            let table = document.createElement('table');
            table.className = 'table table-striped flat-table flat-table-3';
            let thead = table.createTHead();
            let headerRow = thead.insertRow(0);
            let header1 = headerRow.insertCell(0);
            let header2 = headerRow.insertCell(1);
            header1.innerHTML = '<h4>Údaj</h4>';
            header2.innerHTML = '<h4>Hodnota</h4';

            let tbody = table.createTBody();
            for (let i = 0; i < data.length; i++) {
                let row = tbody.insertRow(i);
                let cell1 = row.insertCell(0);
                let cell2 = row.insertCell(1);
                cell1.innerHTML = `<h4>${data[i].name}</h4>`;
                cell2.innerHTML = data[i].value;
            }

            tableDataDiv.appendChild(table);
        }
    </script>
</body>
</html>
