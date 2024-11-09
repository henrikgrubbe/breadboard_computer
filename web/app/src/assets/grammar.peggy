{{ 
  function toBinary(number, length) {
    return number.toString(2).padStart(length, "0");
  }

  function tag(tag, value) {
  	return `<token-${tag}>${value}</token-${tag}>`;
  }

  function tagWithAttribute(tag, value, attributeName, attributeValue) {
  	return `<token-${tag} ${attributeName}="${attributeValue}">${value}</token-${tag}>`;
  }

  function _checkBinaryLength(value, radix, length) {
    return toBinary(parseInt(value, radix), length).length === length;
  }

  function _parseBinary(prefix, digits, radix, length) {
 	const str = digits.join("");
    const raw = prefix + str;
    const parsed = parseInt(str, radix); 

    return {
      f: () => toBinary(parsed, length), 
      h: () => tag("immediate", raw),
      raw
    };
  }
}}

{
  const labelMap = {};
  let numLines = 0;

  function addLine() {
  	numLines++;
  }

  function addLabel(lbl) {
  	labelMap[lbl] = {
      label: lbl, 
      location: numLines
    };
  }

  function isLabelDefined(lbl) {
    return labelMap[lbl] !== undefined;
  }

  function getLabel(lbl) {
    if (!isLabelDefined(lbl)) {
      error(`Use of undefined label: "${lbl}"`);
    }

    return labelMap[lbl];
  }

  function parseNibble(prefix, digits, radix) {
    return _parseInteger(prefix, digits, radix, 4);
  }

  function parseByte(prefix, digits, radix) {
    return _parseInteger(prefix, digits, radix, 8);
  }

  function _parseInteger(prefix, digits, radix, length) {
    const result = _parseBinary(prefix, digits, radix, length);

    if (result.f().length > length) {
      error(`Value ${result.raw} (0b${result.f()}) too large for memory!`);
    }

    return result;
  }

  function compile(lines) {
    if (options.compile) {
      console.info("labels", labelMap);
      console.info("lines", lines);

      return lines
        .map((line, index) => `${toBinary(index, 4)}:${line.f()}`)
        .join("\n");
    }

    if (options.highlight) {
      return lines
        .map((line) => line.h())
        .join("");
    }

    error("Option 'compile' or 'highlight' must be passed to parse function");
  }
}

program
  = lines:line* { return compile(lines) } 

line
  = lbls:(lbl:label":" ws:__ { addLabel(lbl.f()); return {h: () => tag("label-definition", `${lbl.h()}:`) + ws} } )* 
  ws1:__ cmd:command ws2:__ 
    { addLine(); return {f: cmd.f, h: () => `${lbls.map((lbl) => lbl.h()).join("")}${ws1}${cmd.h()}${ws2}`} }

command
  = i:instruction 
    { return {f: i.f, h: () => tag("instr", i.h())} }
  / b:byte 
    { return {f: b.f, h: () => tag("instr-pseudo", b.h())} }

instruction
  = mne:"lda" ws:_ a:address   { return {f: () => "0001" + a.f(), h: () => tag("mnemonic", mne)+ws+a.h()} }
  / mne:"add" ws:_ a:address   { return {f: () => "0010" + a.f(), h: () => tag("mnemonic", mne)+ws+a.h()} }
  / mne:"sub" ws:_ a:address   { return {f: () => "0011" + a.f(), h: () => tag("mnemonic", mne)+ws+a.h()} }
  / mne:"sta" ws:_ a:address   { return {f: () => "0100" + a.f(), h: () => tag("mnemonic", mne)+ws+a.h()} }
  / mne:"ldi" ws:_ i:immediate { return {f: () => "0101" + i.f(), h: () => tag("mnemonic", mne)+ws+i.h()} }
  / mne:"jmp" ws:_ a:address   { return {f: () => "0110" + a.f(), h: () => tag("mnemonic", mne)+ws+a.h()} }
  / mne:"jc"  ws:_ a:address   { return {f: () => "0111" + a.f(), h: () => tag("mnemonic", mne)+ws+a.h()} }
  / mne:"jz"  ws:_ a:address   { return {f: () => "1000" + a.f(), h: () => tag("mnemonic", mne)+ws+a.h()} }
  / mne:"nop"                  { return {f: () => "00000000",     h: () => tag("mnemonic", mne)} }
  / mne:"out"                  { return {f: () => "11100000",     h: () => tag("mnemonic", mne)} }
  / mne:"hlt"                  { return {f: () => "11110000",     h: () => tag("mnemonic", mne)} }

address
  = imm:immediate 
    { return {f: imm.f, h: () => tag("jump-target", imm.h()) } }
  / lbl:label 
    { return {f: () => toBinary(getLabel(lbl.f()).location, 4), h: () => tag("jump-target", lbl.h())} }

immediate "4 bit word"
  = prefix:"0x" digits:[0-9A-Fa-f]+
    { return parseNibble(prefix, digits, 16) }
  / prefix:"0b" digits:[0-1]+
    { return parseNibble(prefix, digits, 2) }
  / digits:[0-9]+
    { return parseNibble("", digits, 10) }

label "label"
  = [a-zA-Z]+[a-zA-Z0-9]* 
    { const lbl = text(); return {f: () => lbl, h: () => tagWithAttribute("label", lbl, "defined", isLabelDefined(lbl))} }

byte "8 bit word"
  = prefix:"0x" digits:[0-9A-Fa-f]+
    { return parseByte(prefix, digits, 16) }
  / prefix:"0b" digits:[0-1]+
    { return parseByte(prefix, digits, 2) }
  / digits:[0-9]+
    { return parseByte("", digits, 10) }

_ "whitespace"
  = $ [ \t]*

__ "whitespace and newline"
  = $ [ \n\t\r]*
