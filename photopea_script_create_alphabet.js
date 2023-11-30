// ----------------------------------------------------------------------------
// This Photoshop/Photopea script creates an image for a Tiny4KOled Arduino library font that is across two pages for height, i.e. height = 16px
// before running this script, please make sure to have a document with a text layer of a preffered font, and that layer should be selected
// after running this script, a popup window will appear, the content has to be copied, as it includes widths of the individual characters
// that need to be used inside the font .h file
// the generated image needs to be converted to C code using the image2cpp website, make sure to set the draw mode to Vertical
//
// more details in the YouTube video: https://youtu.be/ksHLeNBC0IQ
// Source files for the project: https://github.com/upiir/attiny85_oled_menu
// WOKWI sketch using the generated file: https://wokwi.com/projects/382831807597597697
// 
// created by upir, 2023
// youtube channel: https://www.youtube.com/upir_upir
//
// ----------------------------------------------------------------------------


var space_character_width = 3; // 3px -- the size of the space character has to be hardcoded, since there is not way how to get size of an empty (space) character, as it does not have any visible pixels

// character names are used for the layer names, but are no longer used for exporting, as the entire image will be exported
// it´s still nice to have those layers properly named
const character_names = ["space","!","quotation","#","$","%","&","'","(",")","asterisk","+",",","-",".","slash","0","1","2","3","4","5","6","7","8","9","colon","semicolon","lessthan","=","greaterthan","questionmark","atsign","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","[","backslash","]","^","_","`","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","{","verticalbar","}","tilde"];
const character_first_ASCII_code = 32; // ASCII code for the first character in our array above, in this case, it´s the space characeter, ASCII DEC=32

var string_sizes = ""; // string to hold sizes of individual characters, should be later used in the Arduino code
var string_sizes_16s = ""; // another helper variable to hold sizes of characters, grouped by 16, this is required for the used tiny4koled library
var string_sizes_16_counter = 0; // helper counter

app.preferences.rulerUnits = Units.PIXELS; // set Photoshop/Photopea units to pixels
var orig_layer = app.activeDocument.activeLayer; // we will be copying the active layer - this layer has to have the correct Y position and also the font and font size
var xoffset = -1; // x offset helper variable


orig_layer.visible = true; // make the selected layer visible (otherwise, all the copied layers would be invisible)


for (let i = 0; i < character_names.length; i++) { // we are creating new character for every item in character_names array

  var new_layer = orig_layer.duplicate(); // two layers are copied, one for the upper half...
  var new_layer2 = orig_layer.duplicate();  // ...and another one for the lower half

  var ascii_value = i + character_first_ASCII_code;
  new_layer.textItem.contents = String.fromCharCode(ascii_value); // text layer string (text)
  new_layer2.textItem.contents = String.fromCharCode(ascii_value); // text layer string (text) 
  
  new_layer.name = "ascii" + String(ascii_value).padStart(3, '0') + " " + character_names[i]; // set layer name
  new_layer2.name = "ascii" + String(ascii_value).padStart(3, '0') + " " + character_names[i]; // set layer name 
  
  var theBounds = new_layer.bounds; // get the size of the layer
  var layerWidth = theBounds[2] - theBounds[0]; // calculate the layer width
  //var layerHeight = theBounds[3] - theBounds[1]; - layer height is not used 
  
  // space character has no width = use hardcoded value instead
  if (ascii_value == 32) {
    layerWidth = space_character_width+1;
  } 
  
  if (ascii_value != 32) { // for any other character other than space character.. 
    // ..align to left of the document
    new_layer.translate(-new_layer.bounds[0].value, 0);
    new_layer2.translate(-new_layer2.bounds[0].value, -8); // move in Y by -8px   
  }
  
  
  if (ascii_value == 32) {
    xoffset = xoffset + (layerWidth-1);
    xoffset = xoffset + (layerWidth-1);
  } else {
    // align based on the width
    new_layer.translate(xoffset,0);
    xoffset = xoffset + (layerWidth-1); // layer size is reported as +1px in Photopea, this is different from real Photoshop = account for this (might be related only to this font)
    new_layer2.translate(xoffset,0);
    xoffset = xoffset + (layerWidth-1); // layer size is reported as +1px in Photopea, this is different from real Photoshop = account for this (might be related only to this font)
  }
  

  // store the character width in the string_sizes and string_sizes_16s strings
  string_sizes = string_sizes + (layerWidth-1) + ", ";
  string_sizes_16s = string_sizes_16s + (layerWidth-1);
  
  // for the string_sizes_16s string, group 16 values with "+" in between, and add comma after every group
  string_sizes_16_counter++;
  if (string_sizes_16_counter > 15) {
    // for every 16 entries, add either new line or new line with comma
    string_sizes_16_counter = 0;
    string_sizes_16s = string_sizes_16s + ", \n"; 
    string_sizes = string_sizes + " \n"
  } else {
    string_sizes_16s = string_sizes_16s + "+ "; 
  }
  
  
}

xoffset = xoffset + 1; // we need one more pixel for a proper image resize

orig_layer.visible = false; // make the original layer invisible

// after all the characters are created, the xoffset now holds the width of the document
// = crop the image to the size of all the labels
// the height of the document is hardcoded to 8px
app.activeDocument.resizeCanvas(xoffset, 8, AnchorPosition.TOPLEFT);


// after everything is done, show the helper strings in a popup window
// this allows the user to copy the content into the clipboard and use it inside the Arduino code
// this is a hack, because it is not possible to copy text into the Clipboard or save it as file in Photopea - both options are available in Photoshop
alert(string_sizes);
var value = prompt("Please COPY THIS ARRAY! before closing this dialog, as it includes the sizes for individual characters that needs to be used in the Arduino code", string_sizes + "\n\n\n\n" + string_sizes_16s); 

