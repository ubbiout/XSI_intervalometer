/*
 * lcd_nav.ino - Example code using the menu system library
 *
 * This example shows using the menu system with a 16x2 LCD display
 * (controled over serial).
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h> // Using version 1.2.1

// renderer
// The LCD constructor - address shown is 0x27 - may or may not be correct for yours
// Also based on YWRobot LCM1602 IIC V1
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  


void clearLCD()
{
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}

class MyRenderer : public MenuComponentRenderer {
public:
    void render(Menu const& menu) const {
        clearLCD;
        lcd.setCursor(0,0);
        lcd.print(menu.get_name());
        lcd.setCursor(0,1);
        menu.get_current_component()->render(*this);
    }

    void render_menu_item(MenuItem const& menu_item) const {
        lcd.print(menu_item.get_name());
    }

    void render_back_menu_item(BackMenuItem const& menu_item) const {
        lcd.print(menu_item.get_name());
    }

    void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
        lcd.print(menu_item.get_name());
    }

    void render_menu(Menu const& menu) const {
        lcd.print(menu.get_name());
    }
};
MyRenderer my_renderer;

// Forward declarations

void on_item1_selected(MenuComponent* p_menu_component);
void on_item2_selected(MenuComponent* p_menu_component);
void on_item3_selected(MenuComponent* p_menu_component);

// Menu variables

MenuSystem ms(my_renderer);
MenuItem mm_mi1("Level 1 - Item 1 (Item)", &on_item1_selected);
MenuItem mm_mi2("Level 1 - Item 2 (Item)", &on_item2_selected);
Menu mu1("Level 1 - Item 3 (Menu)");
MenuItem mu1_mi1("Level 2 - Item 1 (Item)", on_item3_selected);

// Menu callback function

void on_item1_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("Item1 Selected  ");
    delay(1500); // so we can look the result on the LCD
}

void on_item2_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("Item2 Selected  ");
    delay(1500); // so we can look the result on the LCD
}

void on_item3_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("Item3 Selected  ");
    delay(1500); // so we can look the result on the LCD
}

void serial_print_help() {
    Serial.println("***************");
    Serial.println("w: go to previus item (up)");
    Serial.println("s: go to next item (down)");
    Serial.println("a: go back (right)");
    Serial.println("d: select \"selected\" item");
    Serial.println("?: print this help");
    Serial.println("h: print this help");
    Serial.println("***************");
}

void serial_handler() {
    char inChar;
    if ((inChar = Serial.read()) > 0) {
        switch (inChar) {
            case 'w': // Previus item
                ms.prev();
                ms.display();
                break;
            case 's': // Next item
                ms.next();
                ms.display();
                break;
            case 'a': // Back presed
                ms.back();
                ms.display();
                break;
            case 'd': // Select presed
                ms.select();
                ms.display();
                break;
            case '?':
            case 'h': // Display help
                serial_print_help();
                break;
            default:
                break;
        }
    }
}

void button_handler() {
  
}

// Standard arduino functions

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);

    serial_print_help();

    ms.get_root_menu().add_item(&mm_mi1);
    ms.get_root_menu().add_item(&mm_mi2);
    ms.get_root_menu().add_menu(&mu1);
    mu1.add_item(&mu1_mi1);

    ms.display();
}

void loop() {
    serial_handler();
}
