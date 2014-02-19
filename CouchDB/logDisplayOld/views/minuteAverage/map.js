function(doc) {
  emit((doc.epoch/60>>0)*60, doc);
}