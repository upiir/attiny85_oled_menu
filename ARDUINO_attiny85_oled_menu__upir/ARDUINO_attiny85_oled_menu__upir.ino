// ATtiny85 with 128x64px SSD1306 IIC OLED Display to create a simple menu

// created by upir, 2023
// youtube channel: https://www.youtube.com/upir_upir

// YOUTUBE VIDEO: https://youtu.be/ksHLeNBC0IQ
// SOURCE FILES: https://github.com/upiir/attiny85_oled_menu
// WOKWI simulation: https://wokwi.com/projects/382831807597597697

// Links from the video:
// ATtiny85 chip: https://s.click.aliexpress.com/e/_DeT7tBh
// Arduino UNO: https://s.click.aliexpress.com/e/_AXDw1h
// Arduino prototyping shield: https://s.click.aliexpress.com/e/_ApbCwx
// USB to 5V power cable with crocodile clips: https://s.click.aliexpress.com/e/_DcjKaRH
// Image2cpp (convert array to image): https://javl.github.io/image2cpp/
// Photopea (online graphics editor like Photoshop): https://www.photopea.com
// Transparent OLED display: https://s.click.aliexpress.com/e/_Dns6eLz
// 128x64 SSD1306 OLED Display 1.54": https://s.click.aliexpress.com/e/_DCYdWXb 
// 128x64 SSD1306 OLED Display 0.96": https://s.click.aliexpress.com/e/_DCKdvnh
// 128x64 SSD1306 OLED Display 2.42": https://s.click.aliexpress.com/e/_DFdMoTh
// Additional boards URL for attiny85: https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json

// Related videos from the video:
// Videos using ATtiny85 chip: https://www.youtube.com/playlist?list=PLjQRaMdk7pBbt-is-fkRmUoRcV4dzraYH
// Arduino + OLED displays: https://www.youtube.com/playlist?list=PLjQRaMdk7pBZ1UV3IL5ol8Qc7R9k-kwXA
// Arduino UNO menu with u8g: https://youtu.be/HVHVkKt-ldc
// Arduino UNO menu with u8g2: https://youtu.be/K5e0lFRvZ2E
// Xmas PCB badge: https://youtu.be/Zqb2OhphVPo



#include <TinyWireM.h> // To use the Adafruit's TinyWireM library:
#include <Tiny4kOLED.h> // we are using the Tiny4kOLED library to draw on the display

#define BUTTON_UP_PIN PB4 // pin for UP button 
#define BUTTON_DOWN_PIN PB3 // pin for DOWN button

#include "fontpixelop16b.h" // font pixel operator bold 
#include "fontpixelop16.h" // font pixel operator regular
#include "bg.h" // 128x64px fullscreen background image
#include "icons.h" // 16x16px icons


const unsigned char scrollbar_empty [] PROGMEM =  { B00000000, B01010101, B00000000  }; // image for scrollbar background 3x8px
const unsigned char scrollbar_filled [] PROGMEM = { B11111110, B11111111, B11111110  }; // image for scrollbar handle 3x8px


const int NUM_ITEMS = 8; // number of items in the list 
const int MAX_ITEM_LENGTH = 20; // maximum characters for the item name

char menu_items [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  // array with item names
  { "3D Cube" }, 
  { "Battery" }, 
  { "Dashboard" }, 
  { "Fireworks" }, 
  { "GPS Speed" }, 
  { "Big Knob" },   
  { "Park Sensor" }, 
  { "Turbo Gauge" }
 };

int item_selected = 0; // which item in the menu is selected
int item_selected_displayed = 10; // currently displayed menu item on the OLED display, so we don´t need to redraw the screen all the time
int item_sel_previous; // previous item - used in the menu screen to draw the item before the selected one
int item_sel_next; // next item - used in the menu screen to draw next item after the selected one


void setup() {

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); // up button, set to INPUT_PULLUP = when not pressed, pin is HIGH
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP); // down button, set to INPUT_PULLUP = when not pressed, pin is HIGH

  oled.begin(128, 64, sizeof(tiny4koled_init_128x64), tiny4koled_init_128x64); // display initialization
  oled.enableChargePump(); // The default is off, but most boards need this.
  oled.setRotation(1);     // The default orientation is not the most commonly used.
  oled.clear();  // clear the screen

  oled.bitmap(0, 0, 128, 8, epd_bitmap_bg_no_labels); // draw 128x64px background image
  
  oled.on(); // turn on the display
}

void fill_rest_of_line() {
  // this function is used to fill everyhing from the current X position to the 110px position with black color
  // this is required to draw over the previous string and cover any leftovers
  int stored_cursor_x = oled.getCursorX(); // get cursor X position in pixels
  int stored_cursor_y = oled.getCursorY(); // get cursor Y position in pages
  int clear_width = 110 - stored_cursor_x; // calculate how much pixels we need to fill
  oled.fillLength(/*B10101010*/ 0, clear_width); // fill specified number of columns with black color
  oled.setCursor(stored_cursor_x, stored_cursor_y + 1); // jump to the next page
  oled.fillLength(/*B10101010*/ 0, clear_width); // fill specified number of columns with black color
}

void loop() { // main loop

  if (item_selected_displayed != item_selected) { // only redraw the screen if the selected item is different from the last time
  
    // set correct values for the previous and next items
    item_sel_previous = item_selected - 1;
    if (item_sel_previous < 0) {item_sel_previous = NUM_ITEMS - 1;} // previous item would be below first = make it the last
    item_sel_next = item_selected + 1;  
    if (item_sel_next >= NUM_ITEMS) {item_sel_next = 0;} // next item would be after last = make it the first  

    // draw top item
    oled.setFont(FONTPIXELOP16); // regular font
    oled.setCursor(25, 0); // set cursor, X is in pixels, Y is in pages
    oled.print(menu_items[item_sel_previous]); // draw menu item label
    fill_rest_of_line(); // fill rest of the line with black color
    oled.bitmap(4, 0, 4+16, 2, bitmap_icons[item_sel_previous]); // draw icons

    // draw middle item
    oled.setFont(FONTPIXELOP16B); // bold font
    oled.setCursor(25, 3); // set cursor, X is in pixels, Y is in pages
    oled.print(menu_items[item_selected]); // draw menu item label 
    fill_rest_of_line(); // fill rest of the line with black color
    oled.bitmap(4, 3, 4+16, 5, bitmap_icons[item_selected]); // draw icons  

    // draw bottom item
    oled.setFont(FONTPIXELOP16); // regular font
    oled.setCursor(25, 6); // set cursor, X is in pixels, Y is in pages
    oled.print(menu_items[item_sel_next]); // draw menu item label   
    fill_rest_of_line(); // fill rest of the line with black color
    oled.bitmap(4, 6, 4+16, 8, bitmap_icons[item_sel_next]); // draw icons 

    // draw scrollbar
    for (byte i=0; i<8; i++) { // for all 8 pages
      if (i == item_selected) {
        // draw scrollbar handle = scrollbar_filled
        oled.bitmap(125, i, 125+3, i+1, scrollbar_filled);
      } else {
        // draw scrollbar background = scrollbar_empty
        oled.bitmap(125, i, 125+3, i+1, scrollbar_empty);      
      }
    }   

    item_selected_displayed = item_selected; // update which item is currently rendered on the display

  }


  if (digitalRead(BUTTON_UP_PIN) == LOW) { // button UP is pressed
    item_selected = (item_selected - 1 + NUM_ITEMS) % NUM_ITEMS; // switch to the previous item
  }

  if (digitalRead(BUTTON_DOWN_PIN) == LOW) { // button DOWN is pressed
    item_selected = (item_selected + 1) % NUM_ITEMS; // switch to the next item
  }

}

