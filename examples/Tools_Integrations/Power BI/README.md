# Power BI
* Integrations of Watmonitor's JSON endpoint into Power BI
* Tested on Power BI Desktop / Webapp https://app.powerbi.com/
* Power BI is requesting data from external JSON endpoint - Web (Pull model)
* User is doing a manual Refresh in Power BI Desktop. If there is automatic refresh required each XY mins, cloud is required to use
* For a standard account, 8 refresh requests per day is possible, with Premium Power BI account up to 48 refresh requests
* For more frequent refresh rate, use Power Automate tool from Power Platform and create Streaming Dataset in Power BI (to generate URL for push of data, also is possible to write data directly to Power BI Streaming Dataset from Watmonitor using cURL) 

#
| Files | Integration | Article |
|-------|--------------|----------|
| - powerbi.pbix | Pull - obtain the latest data from JSON endpoint - Watmonitor | No article yet |
<img width="704" height="198" alt="image" src="https://github.com/user-attachments/assets/5b65841e-f81b-4f3b-8507-175c569cd5fa" />
<img width="948" height="484" alt="image" src="https://github.com/user-attachments/assets/b2832976-eb63-4a9c-8661-093f6ebd6729" />
<img width="500" height="275" alt="image" src="https://github.com/user-attachments/assets/caacc7df-71c1-485d-8ebb-e7687319fe91" />
<img width="531" height="281" alt="image" src="https://github.com/user-attachments/assets/1bfa5277-7f1f-44fa-a3db-c6887e5f4c2f" />



