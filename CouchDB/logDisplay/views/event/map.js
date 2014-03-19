function(doc) {
	if (doc.event!=0 && doc.event!=3 && doc.epoch!=4294967295) {
  		emit([doc.epoch, doc._id], doc);
	}
}