function(doc) {
  	emit([doc.host, doc.id, doc.epoch], null);
}