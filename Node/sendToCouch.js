var test="0000AAA552E52AEBFF58000000030004000500060007FFFFFFFF75306D607D0002BC04BA000000000000FFFFFFFF000A007BFFFFFFFF0000FFFFFFFF00000000FEADCA\r0000AAA652E52AEDFF58000000030004000500060007FFFFFFFF75306D607D0002BC04BA000000000000FFFFFFFF000A007BFFFFFFFF0000FFFFFFFF00000000FEADCF\r0000AAA752E52AEFFF58000000030004000500060007FFFFFFFF75306D607D0002BC04BA000000000000FFFFFFFF000A007BFFFFFFFF0000FFFFFFFF00000000FEADCC\r0000AAA852E52AF1FF58000000030004000500060007FFFFFFFF75306D607D0002BC04BA000000000000FFFFFFFF000A007BFFFFFFFF0000FFFFFFFF00000000FEADDD";


var lines=test.split(/[\r\n]+/);
for (var i=0; i<lines.length; i++) {
  var line=lines[i];
  var entry={};
  if (line.length!=134) {
      console.log("Not correct length: "+line);
    } else {
      if (checkDigit(line)) {
        entry.id=parseInt("0x"+line.substring(0,8));
        entry.epoch=parseInt("0x"+line.substring(8,16));
        for (var j=0; j<26; j++) {
          entry[String.fromCharCode(65+j)]=convertSignedIntHexa(line.substring(16+(j*4),20+(j*4)));
        }
        entry.event=parseInt("0x"+line.substring(120,124));
        entry.eventParameter=parseInt("0x"+line.substring(124,128));
        entry.mac=line.substring(128,132);
      } else {
        console.log("Check digit error: "+line);
      }
  }
  console.log(entry);
}

function checkDigit(line) {
  var checkDigit=0;
  for (var i=0; i<line.length; i=i+2) {
    checkDigit^=parseInt("0x"+line[i]+line[i+1]);
  }
  if (checkDigit==0) return true;
  return false;
}

function convertSignedIntHexa(hexa) {
  var value=parseInt("0x"+hexa);
  if (value>32767) {
    return (65536-value)*-1;
  } 
  return value;
}

