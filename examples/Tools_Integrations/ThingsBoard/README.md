# ThingsBoard
* Integrations of Watmonitor's JSON endpoint into ThingsBoard
#
| Files | Integration | Article |
|-------|--------------|----------|
| - widget.json <br> - Dashboard.json | Pull - latest data (running when visible in ThingsBoard dashboard only) | [Widget integration](https://medium.com/@your-iot/watmonitor-thingsboard-pull-integration-widget-rule-chain-372416d12775) |
| - Rule_Chain.json <br> - Dashboard.json | Pull - timeseries data (running each 5 min on background) | [Rule Chain integration](https://medium.com/@your-iot/watmonitor-thingsboard-pull-integration-widget-rule-chain-372416d12775) |
| - Node-RED.json | Push - timeseries data (running each 5 min in Node-RED) | [Node-RED JSON parser, MQTT Integration](https://medium.com/@your-iot/watmonitor-thingsboard-node-red-integration-6042be932edd) |
| - uplink_converter.json | Push - timeseries data (executed on incomming HTTP request) | [HTTP integration, uplink converter](https://medium.com/@your-iot/watmonitor-thingsboard-http-integration-7f4b7c3049b9) |
