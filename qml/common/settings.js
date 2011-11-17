.pragma library

var LOGGING_TAG = "settings"

function log(message) {
    console.log("[" + LOGGING_TAG + "] " + message);
}

// First, let's create a short helper function to get the database connection
function getDatabase() {
     return openDatabaseSync("WikiOnBoard", "1.0", "StorageDatabase", 100000);
}

// At the start of the application, we can initialize the tables we need if they haven't been created yet
function initialize() {
    var db = getDatabase();
    db.transaction(
        function(tx) {
            // Create the settings table if it doesn't already exist
            // If the table exists, this is skipped
            tx.executeSql('CREATE TABLE IF NOT EXISTS settings(setting TEXT UNIQUE, value TEXT)');
          });
}

// This function is used to write a setting into the database
function setSetting(setting, value) {
   // setting: string representing the setting name (eg: “username”)
   // value: string representing the value of the setting (eg: “myUsername”)
   var db = getDatabase();
   var res = "";
   db.transaction(function(tx) {
        var rs = tx.executeSql('INSERT OR REPLACE INTO settings VALUES (?,?);', [setting,value]);
              //console.log(rs.rowsAffected)
              if (rs.rowsAffected > 0) {
                res = "OK";
              } else {
                res = "Error";
              }
        }
  );
  // The function returns “OK” if it was successful, or “Error” if it wasn't
  return res;
}
// This function is used to retrieve a setting from the database
function getSetting(setting) {
   var db = getDatabase();
   var res="";
   db.transaction(function(tx) {
     var rs = tx.executeSql('SELECT value FROM settings WHERE setting=?;', [setting]);
     if (rs.rows.length > 0) {
          res = rs.rows.item(0).value;
     } else {
         res = "Unknown";
     }
  })
  // The function returns “Unknown” if the setting was not found in the database
  // For more advanced projects, this should probably be handled through error codes
  return res
}

//Qt.include("Util.js")

function clearDb() {
    var db = getDatabase();
    db.transaction( function(tx) { try {
                           tx.executeSql('DROP TABLE Entry;');
                       } catch(error) {/*ignore*/} }
                   );
}

// Converts ListModel entries to JSON and passes to a writer function
function modelToJSON(model,writer) {
    // Loop through model entries and output required parts.
    for (var i=0; i < model.count; i++) {
        var json = "{"
        var obj = model.get(i)
        // Create pairs of "name":"value" from object properties
        // (not including attributes).
        for (var prop in obj) {
            if (!prop.match("^attributes")) {
                //log("JSON.stringify("+obj[prop]+")="+JSON.stringify(obj[prop]))
                json += "\""+prop+"\": " + JSON.stringify(obj[prop]) + ",";
            }
        }

        // Loop through entry attributes
        if (obj.attributes) {
            json += "\"attributes\":["
            for (var y=0; y < obj.attributes.count; y++) {
                if (y > 0) json += ","
                var attr = obj.attributes.get(y)
                // Here the whole attribute can be written as is
                // so JSON.stringify is used
                //log("JSON.stringify: attr: "+attr)
                json += JSON.stringify(attr)
            }
            json += "]"
        } else {
            // Remove last comma
            json = json.slice(0, -1)
        }

        json += "}"
        writer(i,json)
        //confirm validity: var parsed = JSON.parse(json)
    }
}

// Stores ListModel in persistent storage
function store(model) {
    // Writes one JSON-formatted ListModel entry to storage.
    function writeEntry(id, json) {
        //log("write json to db: id: "+id+" json:"+json)
        var db = getDatabase();
        db.transaction( function(tx) {
                           tx.executeSql('CREATE TABLE IF NOT EXISTS Entry(id INT, json TEXT)');
                           tx.executeSql('INSERT INTO Entry VALUES(?, ?)', [id, json]);
                       } );
    }

    // First get rid of all old data.
    clearDb()
    modelToJSON(model, writeEntry)
    log("Stored " + model.count + " entries")
}



// Restores ListModel contents from persistent storage
function restore(model)
{
    var db = getDatabase();
    var cleared = false

    // Appends one JSON-formatted entry to ListModel
    function readEntry(json) {
        var parsed = JSON.parse(json)
        if (!cleared) {
            // Succesfully read an parsed something.
            // Clear previous data before writing data
            // from storage.
            cleared = true
            model.clear()
        }
        model.append(parsed)
    }

    db.transaction( function(tx) {
                       try {
                           var rs = tx.executeSql('SELECT * FROM Entry');
                           for (var i = 0; i < rs.rows.length; i++) {
                               //log("restore: json: "+rs.rows.item(i).json)
                               readEntry(rs.rows.item(i).json)
                           }
                       } catch (error) {
                           log ("Error: "+error)
                       }
                   } );
    log("Restored " + model.count + " entries")
}
