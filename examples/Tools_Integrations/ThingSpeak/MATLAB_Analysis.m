%% MATLAB Analysis - JSON Watmonitor parser, write to pre-defined channel id, field 1 %%
url = 'https://hladinomer.eu/json_output.php';
targetString = 'value';  % názov poľa v JSON

writeChannelID = [3136784];  
writeAPIKey = 'JV4ZF1SQFJK9UFRC';

jsonText = webread(url);

if ischar(jsonText) || isstring(jsonText)
    dataStruct = jsondecode(jsonText);
else
    dataStruct = jsonText;
end

data = dataStruct.(targetString);

analyzedData = data;

%% Write Data %%
thingSpeakWrite(writeChannelID, analyzedData, 'WriteKey', writeAPIKey);
