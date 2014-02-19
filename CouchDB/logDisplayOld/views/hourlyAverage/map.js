function(doc) {
  emit((doc.epoch/3600>>0)*3600, doc);
}