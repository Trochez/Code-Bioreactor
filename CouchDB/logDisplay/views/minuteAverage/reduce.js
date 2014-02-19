function(key, values, rereduce) {

  var servers={
    "172.17.0.106": {
      "temperature" : {
        "A":"plate"
      }
    }
  }

  if (rereduce) {
    var toReturn=createResult(servers);
    for (var i=0; i<values.length; i++) {
      var value=values[i];
      for (server in servers) {
        for (type in servers[server]) {
          for (field in servers[server][type]) {
            var key=servers[server][type][field];
            toReturn[type][key+"Min"]=Math.min(toReturn[type][key+"Min"], value[type][key+"Min"]);
            toReturn[type][key+"Max"]=Math.max(toReturn[type][key+"Max"], value[type][key+"Max"]);
            toReturn[type][key+"Total"]=toReturn[type][key+"Total"]+value[type][key+"Total"];
            toReturn[type][key+"Count"]=toReturn[type][key+"Count"]+value[type][key+"Count"];
            toReturn[type][key+"Average"]=toReturn[type][key+"Total"]/toReturn[type][key+"Count"];
          }
        }
      }
    }
  } else {
    var toReturn=createResult(servers);
    for (var i=0; i<values.length; i++) {
      var value=values[i];
      if (value.host) {
        var targets=servers[value.host];
        for (type in targets) {
          for (var key in targets[type]) {
            addKeys(toReturn, type, targets[type][key], value[key]);
          }
        }
      }
    }
  }

  return toReturn;

  function createResult(servers) {
    var result={};
    for (server in servers) {
      for (type in servers[server]) {
        result[type]=result[type] || {};
        for (field in servers[server][type]) {
          var key=servers[server][type][field];
          result[type][key+"Min"]=Number.POSITIVE_INFINITY;
          result[type][key+"Max"]=Number.NEGATIVE_INFINITY;
          result[type][key+"Total"]=0;
          result[type][key+"Count"]=0;
          result[type][key+"Average"]=0;
        }
      }
    }
    return result;
  }

  function addKeys(result, type, key, value) {
    result[type][key+"Min"]=Math.min(result[type][key+"Min"], value);
    result[type][key+"Max"]=Math.max(result[type][key+"Max"], value);
    result[type][key+"Total"]=result[type][key+"Total"]+value;
    result[type][key+"Count"]++;
    result[type][key+"Average"]=result[type][key+"Total"]/result[type][key+"Count"];
  }

}