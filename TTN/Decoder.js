//For uint16_t
function decodeUplink(input) {
  try {
    if (!input || !input.bytes || input.bytes.length < 2) {
      return {
        errors: ["Invalid payload format"],
        warnings: []
      };
    }

    // Extract the bytes from the input
    var payloadBytes = input.bytes;

    // Combine the two bytes into a 16-bit unsigned integer (little-endian)
    var distance = (payloadBytes[1] << 8) | payloadBytes[0];

    return {
      data: {
        distance: distance
      },
      warnings: [],
      errors: []
    };
  } catch (error) {
    return {
      errors: ["Error decoding payload"],
      warnings: [error.toString()]
    };
  }
}
