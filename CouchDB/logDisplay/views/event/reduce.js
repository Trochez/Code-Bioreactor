function(key, values, rereduce) {

  var servers={
    "172.17.0.107": "Luc's office",
    "10.0.0.105": "Julien's office",
    "172.17.0.104": "Microbio master"
  }

  var result={
    host: values[0].host,
    event: values[0].event,
    eventParameter: values[0].eventParameter,
    epoch: values[0].epoch,
    key: values[0]._id
  };

  return result;
}