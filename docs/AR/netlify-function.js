// netlify-function.js
const fetch = require('node-fetch');

exports.handler = async (event, context) => {
    try {
        const apiUrl = 'https://arduino.clanweb.eu/studna_s_prekladom/json_output.php';
        const response = await fetch(apiUrl);
        const data = await response.json();

        return {
            statusCode: 200,
            body: JSON.stringify(data),
        };
    } catch (error) {
        return {
            statusCode: 500,
            body: JSON.stringify({ error: 'Failed to fetch data' }),
        };
    }
}
