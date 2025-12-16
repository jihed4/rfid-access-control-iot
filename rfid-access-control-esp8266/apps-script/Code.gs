function doGet(e) {
  e = e || {};
  var action = e.parameter && e.parameter.action;
  var page   = e.parameter && e.parameter.page;

  // Default: if no action/page → show dashboard
  if (!action && !page) {
    return HtmlService
      .createTemplateFromFile("dashboard")
      .evaluate()
      .setTitle("Door Lock System Dashboard");
  }

  if (page == "dashboard") {
    return HtmlService
      .createTemplateFromFile("dashboard")
      .evaluate()
      .setTitle("Door Lock System Dashboard");
  }

  if (action == "log")       return logAccess(e);
  if (action == "get_users") return getUsers(e);
  if (action == "get_logs")  return getLogs(e);

  return ContentService.createTextOutput("Unknown action");
}

// 🔍 Find real user name from "users" sheet using UID
function findUserName(uid) {
  var ss = SpreadsheetApp.getActive();
  var sheet = ss.getSheetByName("users");
  if (!sheet) return "Unknown_User";

  var data = sheet.getDataRange().getValues(); // header + rows

  for (var i = 1; i < data.length; i++) {
    if (!data[i][0]) continue;
    if (String(data[i][0]).trim() === String(uid).trim()) {
      return data[i][1] || "Unknown_User";  // column B = name
    }
  }
  return "Unknown_User";
}

// --------- LOG ACCESS ----------
function logAccess(e) {
  var uid    = e.parameter.uid    || "";
  var status = e.parameter.status || "UNKNOWN";
  var door   = e.parameter.door_id || "door1";

  // 🔥 auto‑use the real name from users sheet
  var name = findUserName(uid);

  var ss    = SpreadsheetApp.getActive();
  var sheet = ss.getSheetByName("logs");

  sheet.appendRow([new Date(), uid, name, status, door]);

  return ContentService
    .createTextOutput("OK")
    .setMimeType(ContentService.MimeType.TEXT);
}

// --------- USERS API ----------
function getUsers(e) {
  var ss    = SpreadsheetApp.getActive();
  var sheet = ss.getSheetByName("users");
  var data  = sheet.getDataRange().getValues();
  var list  = [];

  for (var i = 1; i < data.length; i++) {
    if (!data[i][0]) continue; // skip empty uid
    list.push({
      uid:    data[i][0],
      name:   data[i][1],
      role:   data[i][2],
      active: data[i][3]
    });
  }

  return ContentService
    .createTextOutput(JSON.stringify(list))
    .setMimeType(ContentService.MimeType.JSON);
}

// --------- LOGS API FOR DASHBOARD ----------
function getLogs(e) {
  var limit = parseInt(e.parameter.limit || "100", 10);

  var ss    = SpreadsheetApp.getActive();
  var sheet = ss.getSheetByName("logs");

  var lastRow = sheet.getLastRow();
  if (lastRow < 2) {
    return ContentService
      .createTextOutput("[]")
      .setMimeType(ContentService.MimeType.JSON);
  }

  var numRows = lastRow - 1;
  var data = sheet.getRange(2, 1, numRows, 5).getValues(); // A2:E

  var logs = [];

  // newest → oldest
  for (var i = data.length - 1; i >= 0 && logs.length < limit; i--) {
    var row = data[i];
    logs.push({
      timestamp: row[0],
      uid:       row[1],
      name:      row[2],
      status:    row[3],
      door_id:   row[4]
    });
  }

  return ContentService
    .createTextOutput(JSON.stringify(logs))
    .setMimeType(ContentService.MimeType.JSON);
}

// ---------- include() helper if you ever split HTML ----------
function include(filename) {
  return HtmlService.createHtmlOutputFromFile(filename).getContent();
}
