function(doc) {
	if (doc.event!=0) {
  		emit([doc.epoch, doc.type], null);
	}
}