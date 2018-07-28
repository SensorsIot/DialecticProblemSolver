function doGet(e){

 // Change Spread Sheet url
 var ss = SpreadsheetApp.openByUrl(" ..... URL to your Spreatsheet......");

// Sheet Name, Chnage Sheet1 to Users in Spread Sheet. Or any other name as you wish
 var sheet = ss.getSheetByName("Questions");
  
 return getUsers(sheet); 
  
}


function getUsers(sheet){
  var jo = {};
  var dataArray = [];

// collecting data from 2nd Row , 1st column to last row and last column
  var rows = sheet.getRange(2,1,sheet.getLastRow()-1, sheet.getLastColumn()).getValues();
  
  var max= sheet.getLastRow()-1;
  var rnd=Math.random();
  
  var i = Math.floor(rnd * max);
  if (i<0) i=0;
  if (i>max) i=max;
    var dataRow = rows[i];
    var record = {};
    record['question'] = dataRow[0];
    
    dataArray.push(record);
     
  
  jo.user = dataArray;
  
  var result = JSON.stringify(jo);
  Logger.log(result);
  
  return ContentService.createTextOutput(result).setMimeType(ContentService.MimeType.JSON);
  
}  
  
