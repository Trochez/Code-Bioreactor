function(doc) {
	if ((doc.event==0) && (doc.epoch!=4294967295) && doc.epoch>1000000000) {
  		emit([(doc.epoch/3600>>0)*3600,(doc.epoch/60>>0)*60,(doc.epoch/10>>0)*10], doc);
	}
}