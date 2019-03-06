//
//    rDUINOScope - Arduino based telescope control system (GOTO).
//    Copyright (C) 2016 Dessislav Gouzgounov (Desso)
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    PROJECT Website: http://rduinoscope.byethost24.com
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

// ......................................................................
//  This part of the code take care of:
//  - Drawing interface screens;
//  - Day / Night mode set;
//  - Draws symbols on certain screens (e.g. XX/XX/XXXX in day/time screen)
//  - Draws OnScreen Messages
//
//  Screens are separated like:
//    * CURRENT_SCREEN==0  - drawGPSScreen() Where the GPS coordinates are displayed
//    * CURRENT_SCREEN==1  - drawClockScreen() Captures updates on the time and date
//    * CURRENT_SCREEN==2  -                - not used
//    * CURRENT_SCREEN==3  - drawSelectAlignment() Select Alignment method (only have 3 buttons)
//    * CURRENT_SCREEN==4  - drawMainScreen() Captures all clicks on the MAIN Screen of the application
//    * CURRENT_SCREEN==5  - drawCoordinatesScreen() Only have "back" button
//    * CURRENT_SCREEN==6  - drawLoadScreen() Captures input on Load screen (all of them: Messier && Treasurres)
//    * CURRENT_SCREEN==7  - drawOptionsScreen();
//    * CURRENT_SCREEN==8  -                - not used
//    * CURRENT_SCREEN==9  -                - not used
//    * CURRENT_SCREEN==10  - drawSTATScreen()
//    * CURRENT_SCREEN==11  - drawStarMap();
//    * CURRENT_SCREEN==12  - drawStarSyncScreen() - To Select Alignment Star;
//    * CURRENT_SCREEN==13  - drawConstelationScreen(int indx) - to actually align on Star. Called few times per alignment procedure.
//

void removeTime_addXX() {
  if (dateEntryPos == 0) {
    tft.fillRect(70, 80, 200, 30, BLACK);
    tft.fillRect(110, 115, 200, 30, BLACK);
    tft.setTextColor(btn_l_text);
    tft.setCursor(70, 80);
    tft.print("XX/XX/XXXX");
    tft.setCursor(110, 115);
    tft.print("XX:XX");
  }
}

void drawGPSScreen()
{
  CURRENT_SCREEN = 0;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 400, 50, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.drawLine(0, 52, 400, 52, title_bg); // yellow line
  tft.setCursor(17, 13);
  tft.setTextColor(title_texts);
  tft.setTextSize(4);
  tft.print("GPS Location");
  tft.setTextSize(3);
  tft.setTextColor(d_text);
  tft.setCursor(60, 65);
  tft.print("Waiting for");
  tft.setCursor(45, 100);
  tft.print("GPS satellite");
  tft.setTextSize(1);
  tft.setCursor(1, 150);
  tft.print("Once the GPS connects to satellites, it will calculate the LAT and LNG of your current position and update the Greenwich Time (GMT).");

  // Draw SKIP BTN
  tft.drawRect(60, 400, 200, 65, btn_l_border);
  tft.setTextSize(3);
  tft.setTextColor(btn_l_text);
  tft.setCursor(125, 410);
  tft.println("SKIP");
  tft.setTextSize(1);
  tft.setCursor(90, 440);
  tft.print("Will assign coordinates");
  tft.setCursor(100, 450);
  tft.print("for Tanta, Egypt");
}


void drawClockScreen()
{
  CURRENT_SCREEN = 1;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 400, 50, title_bg);
  tft.drawLine(0, 52, 400, 52, title_bg); // yellow line
  tft.setCursor(27, 13);
  tft.setTextColor(title_texts);
  tft.setTextSize(4);
  tft.print("Date & Time");
  tft.setTextSize(3);
  tft.setTextColor(l_text);
  tft.setCursor(70, 80);
  tft.print(rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/'));
  //  tft.print(returnDate(0));
  tft.setCursor(110, 115);
  tft.print(rtc.getTimeStr(FORMAT_SHORT));
  //  tft.print(returnTime());

  // Draw keypad....
  tft.setTextColor(btn_l_text);
  int kk = 1;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      if (kk == 10) {
        if (Summer_Time == 1) {
          DrawButton(30, 405, 71, 56, "SUMMER", btn_d_border, btn_l_border, btn_l_text, 1);
        } else {
          DrawButton(30, 405, 71, 56, "SUMMER", 0, btn_l_border, btn_l_text, 1);
        }
      } else if (kk == 11) {
        DrawButton(125, 405, 71, 56, "0", btn_d_border, 0, btn_l_text, 3);
      } else if (kk == 12) {
        DrawButton(225, 405, 71, 56, "OK", 0, btn_l_border, btn_l_text, 3);
      } else {
        DrawButton(((j * 100) + 25), ((i * 80) + 170), 71, 56, String(kk), btn_d_border, 0, btn_l_text, 3);
        //        100 box width ,25start from left , 80 box hight , 170 start from up
      }
      kk += 1;
    }
  }
}


void drawSelectAlignment() {
  CURRENT_SCREEN = 3;
  tft.fillScreen(BLACK);;
  tft.fillRect(0, 0, 400, 50, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.drawLine(0, 52, 400, 52, title_bg); // yellow line
  tft.setCursor(50, 13);
  tft.setTextColor(title_texts); // black
  tft.setTextSize(4);
  tft.print("Alignment");
  tft.setTextColor(btn_l_text); // dark green
  tft.setTextSize(3);

  DrawButton(30, 150, 250, 65, "1 Star Alignment", btn_d_border, btn_l_border, btn_l_text, 2);
  //  DrawButton(30,150,250,65, "2 Star Alignment", btn_d_border, btn_l_border, btn_l_text, 2);
  if (OBSERVATION_LONGITUDE > 0) {
    DrawButton(30, 250, 250, 65, "Iterative Align", btn_d_border, btn_l_border, btn_l_text, 2);

    //        Left down length up
    tft.drawLine(31, 249, 279, 215, btn_l_border); // Green line
    //tft.drawLine(31, 215, 279, 249, btn_l_border); // Green line
  }
  DrawButton(30, 400, 250, 55, "Skip Alignment", 0, btn_l_border, btn_l_text, 2);

  //          Left down length up
  tft.drawLine(31, 400, 279, 315, btn_l_border); // Green line
  //tft.drawLine(31, 315, 279, 400, btn_l_border); // Green line
}


void drawMainScreen()
{
  tft.fillScreen(BLACK);
  CURRENT_SCREEN = 4;
  w_DateTime[0] = 0;
  calculateLST_HA();
  update_time = millis();
  tft.fillRect(0, 1, 320, 90, title_bg);
  tft.drawLine(0, 92, 320, 92, title_bg);
  drawStatusBar();

#ifdef use_battery_level
  drawBatteryLevel(260, 120, calculateBatteryLevel());
#endif


  tft.setTextColor(title_texts);
  tft.setCursor(1, 35);
  tft.setTextSize(3);
  tft.print("Time:");
  tft.print(String(rtc.getTimeStr()).substring(0, 5));

  tft.setCursor(1, 65);
  tft.print("LST :");
  if ((int)LST < 10) {
    tft.print("0");
    tft.print((int)LST);
  } else {
    tft.print((int)LST);
  }
  tft.print(":");
  if ((LST - (int)LST) * 60 < 10) {
    tft.print("0");
    tft.print((LST - (int)LST) * 60, 0);
  } else {
    tft.print((LST - (int)LST) * 60, 0);
  }

  tft.setTextSize(2);
  tft.setTextColor(Button_Title);

  tft.setCursor(200, 36);
  tft.print("Temp:");

  tft.setCursor(205, 69);
  tft.print("Hum:");

  tft.setTextSize(2);
  tft.setTextColor(title_texts);

  tft.setCursor(261, 38);
  tft.print(_temp, 0);
  tft.setCursor(290, 30);
  tft.setTextSize(2);
  tft.print("o");
  tft.setTextSize(2);
  tft.setCursor(295, 38);
  tft.print(" C");
  tft.setCursor(261, 70);
  tft.print(_humid, 0);
  tft.setCursor(290, 70);
  tft.print(" %");

  tft.setTextSize(1);
  tft.setTextColor(btn_l_border); //  Green   >>> Beautiful

  tft.setCursor(1, 102);
  tft.print("Date:");
  tft.setCursor(110, 102);
  tft.print("@");

  tft.setTextColor(l_text);
  tft.setCursor(35, 102);
  tft.print(String(rtc.getDateStr()).substring(0, 2));
  tft.print(" ");
  tft.print(rtc.getMonthStr(FORMAT_SHORT));
  tft.print(" ");
  tft.print(String(rtc.getDateStr()).substring(6));

  tft.setCursor(120, 102);
  tft.print("LAT:");
  tft.print(OBSERVATION_LATTITUDE, 4);
  tft.print(" LONG:");
  tft.print(OBSERVATION_LONGITUDE, 4);
  tft.print(" ALT:");
  tft.print(OBSERVATION_ALTITUDE, 0);

  tft.setTextSize(3);
  tft.setCursor(1, 117);
  tft.println("OBSERVING:");

  // Data for the observed object....
  if (OBJECT_NAME != "") {
    tft.setTextSize(3);
    tft.setTextColor(title_bg);
     tft.setCursor(1, 145);
    tft.print(OBJECT_NAME);
    tft.setTextColor(l_text);
    tft.setTextSize(2);
    if ((OBJECT_NAME != "CP") && (IS_BT_MODE_ON == false)) {
      tft.setCursor(92, 290);
      tft.println("HA:");
      tft.setCursor(135, 290);
      tft.setTextColor(l_text);
      tft.print(HAHour, 0);
      tft.print("h");
      tft.print(HAMin, 1);
      tft.print("m");
    }
    tft.setCursor(0, 273);
    if ((OBJECT_NAME != "") && (ALT < 0)) {
      tft.setTextColor(RED);
      tft.println("OBJECT NOT VISIBLE!");
      tft.fillRect(250, 235, 70, 70, Maroon);
      tft.setTextSize(5);
      tft.setCursor(272, 252);
      tft.print("!");
    }
    else
    {
      char obj_name[50];
      String OBJ_NAME = "objects/";
      OBJ_NAME += String(OBJECT_NAME) + ".bmp";
#ifdef serial_debug
      Serial.print("file name: ");
      Serial.println(OBJ_NAME);
      Serial.print("SD.open(OBJ_NAME) = ");
      Serial.println(SD.open(OBJ_NAME));
#endif
      if (SD.open(OBJ_NAME))
      {
        OBJ_NAME.toCharArray(obj_name, 50);
        bmpDraw(obj_name, 235, 200);
        //delay(200);
      }

      tft.setTextColor(title_bg);
      tft.print(OBJECT_DESCR);
      tft.setTextColor(l_text);
    }

    tft.setCursor(0, 175);
    tft.setTextColor(l_text);
    tft.setTextSize(1);
    tft.println(OBJECT_DETAILS);
    tft.println("");
    tft.setTextSize(2);
    tft.setCursor(0, 220);
    tft.print("RA :");
    tft.print(OBJECT_RA_H, 0);
    tft.print("h ");
    tft.print(OBJECT_RA_M, 2);
    tft.println("'");
    tft.setCursor(0, 240);
    tft.print("DEC:");
    tft.print(OBJECT_DEC_D, 0);
    tft.setTextSize(1);
    tft.print("o ");
    tft.setTextSize(2);
    tft.print(abs(OBJECT_DEC_M), 2);
    tft.println("'");
    // End data for the observed object...
  } else {
    tft.setTextSize(2);
    tft.setTextColor(texts);
    tft.println("");
    tft.println("No object is selected!");
    tft.setTextSize(1);
    tft.println("Use LOAD button below to select objects from Solar System, Messier, NGC or custom.csv catalogues");
  }

  tft.drawLine(1, 321, 320, 321, btn_d_border);
  tft.drawLine(1, 321, 320, 321, btn_d_border);
  tft.drawLine(1, 321, 320, 321, btn_l_border);

  //  Draw Main Screen Menu:
  drawMainScreen_Menu(0);
}


void drawCoordinatesScreen() {
  CURRENT_SCREEN = 5;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 400, 50, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.drawLine(0, 52, 400, 52, title_bg); // yellow line
  tft.setCursor(5, 15);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Coordinates");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  DrawButton(210, 5, 100, 40, "<back", btn_d_border, btn_l_border, btn_l_text, 3);

  // Draw Coordinates screen...
  tft.setTextColor(l_text);
  tft.setTextSize(3);
  tft.setCursor(5, 85);
  tft.print("HA : ");
  tft.setCursor(5, 140);
  tft.print("RA : ");
  tft.setCursor(5, 195);
  tft.print("DEC: ");
  tft.fillRect(85, 75, 200, 35, btn_d_border);
  tft.drawRect(86, 76, 201, 36, btn_l_border);
  tft.fillRect(85, 130, 200, 35, btn_d_border);
  tft.drawRect(86, 131, 201, 36, btn_l_border);
  tft.fillRect(85, 185, 200, 35, btn_d_border);
  tft.drawRect(86, 186, 201, 36, btn_l_border);
  tft.setCursor(1, 250);
  tft.setTextSize(2);
  tft.print("  Coordinates represented here are based on the current motor positions and re-calculated back to Hour Angle, Right Ascension and Declination. NB: Note that alignment affects the scales and small variations are possible!");
}


void drawLoadScreen() {
  CURRENT_SCREEN = 6;
  //  MESS_PAGER = 0;
  tft.fillScreen(BLACK);;
  tft.fillRect(0, 0, 400, 50, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.drawLine(0, 52, 400, 52, title_bg); // yellow line
  tft.setCursor(5, 13);
  tft.setTextColor(title_texts);
  tft.setTextSize(4);
  tft.print("Load...");

  // Draw buttons to load CSVs
  tft.setTextSize(2);
  tft.setTextColor(btn_l_text);
  if (LOAD_SELECTOR == 1)
  {
    DrawButton(0, 60, 150, 40, "Messier", btn_d_border, btn_l_border, btn_l_text, 2);
  } else {
    DrawButton(0, 60, 150, 40, "Messier", 0, btn_l_border, btn_l_text, 2);
  }
  if (LOAD_SELECTOR == 2) {
    DrawButton(170, 60, 150, 40, "Treasures", btn_d_border, btn_l_border, btn_l_text, 2);
  } else {
    DrawButton(170, 60, 150, 40, "Treasures", 0, btn_l_border, btn_l_text, 2);
  }
  if (LOAD_SELECTOR == 3)
  {
    DrawButton(0, 110, 150, 40, "Solar System", btn_d_border, btn_l_border, btn_l_text, 2);
  } else {
    DrawButton(0, 110, 150, 40, "Solar System", 0, btn_l_border, btn_l_text, 2);
  }
  if (LOAD_SELECTOR == 4)
  {
    DrawButton(170, 110, 150, 40, "custom.csv", btn_d_border, btn_l_border, btn_l_text, 2);
  } else {
    DrawButton(170, 110, 150, 40, "custom.csv", 0, btn_l_border, btn_l_text, 2);
  }


  tft.fillRect(0, 414, 320, 2, btn_d_border);
  tft.drawLine(0, 415, 320, 415, btn_l_border);

  DrawButton(210, 5, 100, 40, "<Back", btn_d_border, btn_l_border, btn_l_text, 3);
  DrawButton(0, 420, 100, 60, "< Prev", btn_d_border, btn_l_border, btn_l_text, 2);
  DrawButton(220, 420, 100, 60, "Next >", btn_d_border, btn_l_border, btn_l_text, 2);

  //  ///////     Messier Screen /////////////
  //  if (LOAD_SELECTOR == 1)
  //  {
  //    // I'll draw 24 objects per page, thus "(pager*24)" will give me the start of the [index_]
  //    int kk = MESS_PAGER * 24;
  //
  //    for (int i = 0; i < 6; i++)
  //    {
  //      for (int j = 0; j < 4; j++)
  //      {
  //        String M_NAME = Messier_Array[kk].substring(0, Messier_Array[kk].indexOf(';'));
  //        if (M_NAME == "") {
  //          break;
  //        }
  //        DrawButton(((j * 75) + 12), ((i * 50) + 110), 71, 45, M_NAME, messie_btn, 0, btn_l_text, 2);
  //        // 75 box width ,12start from left , 50 box hight , 110 start from up
  //        kk += 1;
  //      }
  //    }
  //  }
  //  ///////     Treasures Screen /////////////
  //  else if (LOAD_SELECTOR == 2)
  //  {
  //    tft.setTextSize(1);
  //    int ll = TREAS_PAGER * 24;
  //
  //    for (int i = 0; i < 6; i++) {
  //      for (int j = 0; j < 4; j++) {
  //        String M_NAME = Treasure_Array[ll].substring(0, Treasure_Array[ll].indexOf(';'));
  //        if (M_NAME == "") {
  //          break;
  //        }
  //        DrawButton(((j * 75) + 12), ((i * 50) + 110), 71, 45, M_NAME, messie_btn, 0, btn_l_text, 1);
  //        ll += 1;
  //      }
  //    }
  //  }
  drawLoadObjects();
}


void drawOptionsScreen()
{
  CURRENT_SCREEN = 7;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 400, 50, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.drawLine(0, 52, 400, 52, title_bg); // yellow line
  tft.setCursor(5, 13);
  tft.setTextColor(title_texts);
  tft.setTextSize(4);
  tft.print("Options");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  DrawButton(210, 5, 100, 40, "<back", btn_d_border, btn_l_border, btn_l_text, 3);

  // Draw Options...
  // Tracking Mode
  //  tft.setCursor(5, 60);
  //  tft.setTextColor(btn_l_text);
  //  tft.setTextSize(2.5);
  //  tft.println("Tracking Modes");
  //   if (Tracking_type == 0){
  //      DrawButton(0,85,100,40, "Celest", 0, btn_l_border, btn_l_text, 2);
  //      DrawButton(110,85,100,40, "Lunar", btn_d_border, btn_l_border, btn_l_text, 2);
  //      DrawButton(220,85,100,40, "Solar", 0, btn_l_border, btn_l_text, 2);
  //   }else if (Tracking_type == 1){
  //      DrawButton(0,85,100,40, "Celest", btn_d_border, btn_l_border, btn_l_text, 2);
  //      DrawButton(110,85,100,40, "Lunar", 0, btn_l_border, btn_l_text, 2);
  //      DrawButton(220,85,100,40, "Solar", 0, btn_l_border, btn_l_text, 2);
  //  }else if (Tracking_type == 2) {
  //      DrawButton(0,85,100,40, "Celest", 0, btn_l_border, btn_l_text, 2);
  //      DrawButton(110,85,100,40, "Lunar", 0, btn_l_border, btn_l_text, 2);
  //      DrawButton(220,85,100,40, "Solar", btn_d_border, btn_l_border, btn_l_text, 2);
  //  }
  updateTrackingMode_opt();

  //tft.drawLine (5, 110, 235, 110, btn_l_text);
  // Screen Brightnes
  tft.setCursor(5, 140);
  tft.println("Screen Brightness: ");

  updateTriangleBrightness_opt();

  // Screen Brightnes
  tft.setCursor(5, 210);
  tft.setTextSize(2);
  tft.print("Screen Auto Off");

  updateScreenTimeout_opt();

  updateMeridianFlip_opt();

  updateSound_opt();

  updateStepper_opt();

  DrawButton(230, 300, 80, 180, "", 0, btn_l_border, btn_l_text, 2);
  tft.setCursor(252, 330);
  tft.print("GPS");
  tft.setCursor(240, 375);
  tft.print("Clock");
  tft.setCursor(240, 420);
  tft.print("Align");
}


void drawSTATScreen() {
  CURRENT_SCREEN = 10;
  tft.fillScreen(BLACK);;
  tft.fillRect(0, 0, 400, 50, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.drawLine(0, 52, 400, 52, title_bg); // yellow line
  tft.setCursor(5, 13);
  tft.setTextColor(title_texts);
  tft.setTextSize(4);
  tft.print("Stats");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  DrawButton(210, 5, 100, 40, "<back", btn_d_border, btn_l_border, btn_l_text, 3);

  // Draw staistics...
  tft.setTextColor(l_text);
  tft.setTextSize(2);
  tft.setCursor(0, 58);
  tft.print("Observation on ");
  tft.print(Start_date);
  tft.print(" @");
  tft.print(START_TIME);
  tft.println("h");
  tft.print("Report generated at: ");
  tft.println(String(rtc.getTimeStr()).substring(0, 5));
  tft.println(" ");
  tft.setCursor(100, 105);
  tft.println("LOCATION:");
  tft.setCursor(70, 125);
  tft.print("LAT : ");
  tft.println(OBSERVATION_LATTITUDE, 4);
  tft.setCursor(70, 145);
  tft.print("LONG: ");
  tft.println(OBSERVATION_LONGITUDE, 4);
  tft.setCursor(70, 165);
  tft.print("ALT : ");
  tft.println(OBSERVATION_ALTITUDE, 0);
  tft.println("");
  tft.setCursor(5, 195);
  tft.print(" OBSERVATION: ");
  double st;
  int st_h;
  int st_m;
  st = (String(rtc.getTimeStr()).substring(0, 2).toInt() * 60) + String(rtc.getTimeStr()).substring(3, 5).toInt();
  st -= (START_TIME.substring(0, 2).toInt() * 60) + START_TIME.substring(3, 5).toInt();
  if (st < 0) {
    st += 1440;
  }
  st_h = int(st / 60);
  st_m = ((st / 60) - st_h) * 60;
  tft.print(st_h);
  tft.print("h ");
  tft.print(st_m);
  tft.println("m");
  tft.setTextSize(1);
  tft.setCursor(0, 220);
  tft.print("Observation started at ");
  tft.print(START_TIME);
  tft.print("h where environment tempersture was ");
  tft.print(_Stemp, 0);
  tft.print("C and ");
  tft.print(_Shumid, 0);
  tft.println("% humidity\n");
  tft.print("OBJECTS: ");
  tft.print(Observed_Obj_Count);
  tft.println(" object(s) observed:");
  tft.println("");
  for (int i = 0; i < Observed_Obj_Count; i++) {
    int i1 = ObservedObjects[i].indexOf(';');
    int i2 = ObservedObjects[i].indexOf(';', i1 + 1);
    int i3 = ObservedObjects[i].indexOf(';', i2 + 1);
    int i4 = ObservedObjects[i].indexOf(';', i3 + 1);
    int i5 = ObservedObjects[i].indexOf(';', i4 + 1);
    int i6 = ObservedObjects[i].indexOf(';', i5 + 1);
    int i7 = ObservedObjects[i].indexOf(';', i6 + 1);
    int tt;
    String ha_;
    String degs_;
    if (i == (Observed_Obj_Count - 1)) {
      tt = (((String(rtc.getTimeStr()).substring(0, 2).toInt()) * 60)  + (String(rtc.getTimeStr()).substring(3, 5).toInt())) - ((ObservedObjects[i].substring(i2 + 1, i2 + 3).toInt() * 60) + ObservedObjects[i].substring(i2 + 4, i3).toInt());
      if (tt < 0) {
        tt += 1440;
      }
    } else {
      tt = ObservedObjects[i].substring(i7 + 1, ObservedObjects[i].length()).toInt();
    }
    degs_ = ObservedObjects[i].substring(i6 + 1, i7);
    ha_ = ObservedObjects[i].substring(i5 + 1, i6);
    String Composed = ObservedObjects[i].substring(0, i1) + " @" + ObservedObjects[i].substring(i2 + 1, i3) + " for " + String(tt) + "min | Alt: " + degs_ + "deg\n";
    //Composed += "\nAt the time of observation the object was "+degs_+"deg. above horizon, with HA:"+ha_+" Environment wise: " + ObservedObjects[i].substring(i3+1, i4) + " C and " + ObservedObjects[i].substring(i4+1, i5) + "% humidity. ";
    tft.print(Composed);
  }
}

void drawStarSyncScreen() {
  CURRENT_SCREEN = 12;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 400, 50, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.drawLine(0, 52, 400, 52, title_bg); // yellow line
  tft.setCursor(5, 13);
  tft.setTextColor(title_texts);
  tft.setTextSize(4);
  tft.print("Sync...");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  DrawButton(210, 5, 100, 40, "DONE>", btn_d_border, btn_l_border, btn_l_text, 3);

  tft.fillRect(0, 414, 320, 2, btn_d_border);
  tft.drawLine(0, 415, 320, 415, btn_l_border);

  DrawButton(0, 420, 100, 60, "< Prev", btn_d_border, btn_l_border, btn_l_text, 2);
  DrawButton(220, 420, 100, 60, "Next >", btn_d_border, btn_l_border, btn_l_text, 2);

  // I'll draw 24 objects per page, thus "(pager*24)" will give me the start of the [index_]

  drawAlignObjects_ali();

  //  if (ALLIGN_TYPE == 3)
  //  {
  //    int kk = STARS_PAGER * 24;
  //    for (int i = 0; i < 6; i++) {
  //      for (int j = 0; j < 4; j++) {
  //        int i1 = Iter_Stars[kk].indexOf(';');
  //        int i2 = Iter_Stars[kk].indexOf(';', i1 + 1);
  //        String S_NAME = Iter_Stars[kk].substring(i1 + 1, i2);
  //        String C_NAME = Iter_Stars[kk].substring(0, i1);
  //        if (S_NAME == "") {
  //          break;
  //        }
  //        tft.fillRect(((j * 75) + 12), ((i * 50) + 85), 71, 45, messie_btn);
  //        int l = (S_NAME.length() / 2) * 6;
  //        tft.setCursor(((j * 75) + (44 - l)), ((i * 50) + 93));
  //        tft.setTextSize(1);
  //        tft.print(S_NAME);
  //        tft.setTextSize(2);
  //        tft.setCursor(((j * 75) + 29), ((i * 50) + 110));
  //        tft.print(C_NAME);
  //        kk += 1;
  //      }
  //    }
  //  }
  //  else
  //  {
  //    int kk = STARS_PAGER * 24;
  //    for (int i = 0; i < 6; i++) {
  //      for (int j = 0; j < 4; j++) {
  //        int i1 = Stars[kk].indexOf(';');
  //        int i2 = Stars[kk].indexOf(';', i1 + 1);
  //        String S_NAME = Stars[kk].substring(i1 + 1, i2);
  //        String C_NAME = Stars[kk].substring(0, i1);
  //        if (S_NAME == "") {
  //          break;
  //        }
  //        tft.fillRect(((j * 75) + 12), ((i * 50) + 85), 71, 45, messie_btn);
  //        int l = (S_NAME.length() / 2) * 6;
  //        tft.setCursor(((j * 75) + (44 - l)), ((i * 50) + 93));
  //        tft.setTextSize(1);
  //        tft.print(S_NAME);
  //        tft.setTextSize(2);
  //        tft.setCursor(((j * 75) + 29), ((i * 50) + 110));
  //        tft.print(C_NAME);
  //        kk += 1;
  //      }
  //    }
  //  }
}

void drawConstelationScreen(int indx)
{
  CURRENT_SCREEN = 13;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 400, 50, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.drawLine(0, 52, 400, 52, title_bg); // yellow line
  tft.setCursor(60, 13);
  tft.setTextColor(title_texts);
  tft.setTextSize(4);
  tft.print("Alignment");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);

  tft.fillRect(0, 414, 320, 2, btn_d_border);
  tft.drawLine(0, 415, 320, 415, btn_l_border);

  if (ALLIGN_TYPE != 3)
  {
    DrawButton(0, 420, 100, 60, "<Repeat", 0, btn_l_border, btn_l_text, 2);
  }
  else
  {
    DrawButton(0, 420, 100, 60, "<Exit", 0, btn_l_border, btn_l_text, 2);
  }
  if (ALLIGN_STEP == 1)
  {
    DrawButton(220, 420, 100, 60, "SYNC !", 0, btn_l_border, btn_l_text, 2);
    tft.setCursor(0, 80);
    tft.println("Use manual motor movement to center the star!\n");
    tft.println("Once centered, hit 'SYNC !' button\n");

    tft.print("Selected Object:");
    tft.setCursor(50, 210);
    tft.setTextColor(title_bg);
    tft.println(OBJECT_NAME);
    tft.setTextColor(l_text);
  }
  else if (ALLIGN_STEP == 2)
  {
    if (ALLIGN_TYPE == 3)
    {
      DrawButton(220, 420, 100, 60, "NEXT>", 0, btn_l_border, btn_l_text, 2);
      tft.setCursor(0, 80);
      tft.println("Use Alt/Az (phisical) knobs on your mount to center on Polaris!\n");
      tft.println("Once centered, hit 'ALIGN' button.");
      tft.println("");
      tft.println("NB! Only correct halfway to center!:");
      tft.setTextColor(title_bg);
      tft.println(OBJECT_NAME);
      tft.setTextColor(l_text);
    }
    else
    {
      DrawButton(220, 420, 100, 60, "CENTER", 0, btn_l_border, btn_l_text, 2);
      tft.setCursor(0, 80);
      tft.println("Use manual motor movement to center the star!");
      tft.println("");
      tft.println("Once centered, hit 'SYNC !' button.");
    }
  }
}

void OnScreenMsg(int Msg) {
  // Msg = 1 -> Moving;
  // Msg = 2 -> Tracking Off;
  // Msg = 3 -> Select Different Star;
  String m1, m2, m3;
  tft.fillRect(80, 215, 160, 100, MsgBox_bg );
  tft.drawRect(80, 215, 160, 100, MsgBox_t);
  tft.setTextColor(MsgBox_t);
  if (Msg == 1) {
    m1 = "MOVING";
    m2 = "Step Back";
    m3 = "PLEASE!";
    tft.setCursor(107, 230);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(102, 265);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(122, 290);
    tft.print(m3);

  } else if (Msg == 2) {
    m1 = "WARNING";
    m2 = "TRACKING was";
    m3 = "Turned Off!";
    tft.setCursor(98, 230);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(88, 265);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(98, 290);
    tft.print(m3);
    delay(1000);
    drawMainScreen();

  } else if (Msg == 3) {
    m1 = "ERROR!";
    m2 = "Not Visible";
    m3 = "";
    //Timer3.stop(); //
    //IS_TRACKING = false;
    tft.setCursor(109, 230);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(88, 265);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(98, 290);
    tft.print(m3);

  } else if (Msg == 4) {
    m1 = "PICK AGAIN";
    m2 = "ALT=" + String(ALT, 0);
    m3 = "RA= " + String(OBJECT_RA_H, 0);
    //Timer3.stop(); //
    //IS_TRACKING = false;
    tft.setCursor(95, 230);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(95, 265);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(95, 290);
    tft.print(m3);

  } else if (Msg == 5) {
    m1 = "ERROR!";
    m2 = "Please Turn";
    m3 = "Steppers ON!";
    tft.setCursor(109, 230);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(94, 265);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(90, 290);
    tft.print(m3);
    if (IS_SOUND_ON) {
      SoundOn(note_C, 32);
      delay(200);
      SoundOn(note_C, 32);
      delay(200);
      SoundOn(note_C, 32);
      delay(1000);
    }

  } else if (Msg == 6) {
    m1 = "BACKING UP!";
    m2 = "Step Back";
    m3 = "PLEASE!";
    tft.setCursor(95, 230);
    tft.setTextSize(2);
    tft.println(m1);
    tft.setCursor(106, 260);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(122, 290);
    tft.print(m3);
  }
  else if (Msg == 7) {
    m1 = "JUST LOOK";
    m2 = "AROUND";
    m3 = "YOURSELF :)";
    //Timer3.stop(); //
    //IS_TRACKING = false;
    tft.setCursor(110, 230);
    tft.setTextSize(2);
    tft.println(m1);
    tft.setCursor(124, 260);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(95, 290);
    tft.print(m3);
  }
}
void considerDayNightMode() {
  boolean prev_night_mode = IS_NIGHTMODE;
  if (analogRead(DAY_NIGHT_PIN) < 800) {
    IS_NIGHTMODE = true;
  } else {
    IS_NIGHTMODE = false;
  }
  // Now make sure it works for all screen - redraws them when mode switched
  if (prev_night_mode != IS_NIGHTMODE) {
    if (IS_NIGHTMODE == true) {
      // Night Mode
      IS_NIGHTMODE = true;
      texts = Maroon;
      l_text = RED;
      d_text = Maroon;
      title_bg = RED;
      title_texts = BLACK;
      messie_btn = Maroon;
      btn_l_text = RED;
      btn_d_text = Maroon;
      btn_l_border = RED;
      btn_d_border = Maroon;
      btn_l_selection = RED;
      MsgBox_bg = RED;
      MsgBox_t = BLACK;
      Button_State_ON = BLACK;
      Button_State_OFF = BLACK;
      Button_Title = BLACK;
    } else {
      IS_NIGHTMODE = false;
      texts = LightGrey;
      l_text = WHITE;
      d_text = LightGrey;
      btn_l_text = GreenYellow;
      btn_d_text = DarkGreen;
      btn_l_border = GREEN;
      btn_d_border = DarkGreen;
      btn_l_selection = DarkGreen;
      title_bg = Orange;
      title_texts = BLACK;
      messie_btn = DarkGrey;
      MsgBox_bg = Purple;
      MsgBox_t = GreenYellow;
      Button_State_ON = DarkGreen;
      Button_State_OFF = RED;
      Button_Title = Purple;
    }
    if (CURRENT_SCREEN == 0)
    {
      drawGPSScreen();
    }
    else if (CURRENT_SCREEN == 1)
    {
      drawClockScreen();
    } else if (CURRENT_SCREEN == 3) {
      drawSelectAlignment();
    } else if (CURRENT_SCREEN == 4) {
      drawMainScreen();
    } else if (CURRENT_SCREEN == 5) {
      drawCoordinatesScreen();
    } else if (CURRENT_SCREEN == 6) {
      drawLoadScreen();
    } else if (CURRENT_SCREEN == 7) {
      drawOptionsScreen();
    } else if (CURRENT_SCREEN == 10) {
      drawSTATScreen();
    } else if (CURRENT_SCREEN == 12) {
      drawStarSyncScreen();
    } else if (CURRENT_SCREEN == 13) {
      drawConstelationScreen(0);
    }
  }
}


void drawMainScreen_Menu(int fill) {
  //  if (fill == 1){
  //    tft.fillRect(0, 278, 239, 122, BLACK);
  //  }

  if (MAIN_SCREEN_MENU == 0) {
    if (IS_BT_MODE_ON == false) {
      DrawButton( 1, 325, 100, 70, "LOAD", 0, btn_l_border, btn_l_text, 2);
      DrawButton( 110, 325, 100, 70, "HOME", 0, btn_l_border, btn_l_text, 2);
      DrawButton( 1, 405, 100, 70, "STATs", 0, btn_l_border, btn_l_text, 2);
      DrawButton( 110, 405, 100, 70, "+", 0, btn_l_border, btn_l_text, 2);
      DrawButton( 220, 405, 100, 70, "BT", 0, btn_l_border, btn_l_text, 2);
    } else {
      DrawButton( 1, 325, 100, 70, "", 0, btn_d_border, btn_l_text, 2);
      DrawButton( 110, 325, 100, 70, "", 0, btn_d_border, btn_l_text, 2);
      DrawButton( 1, 405, 100, 70, "", 0, btn_d_border, btn_l_text, 2);
      DrawButton( 110, 405, 100, 70, "+", 0, btn_l_border, btn_l_text, 2);
      DrawButton( 220, 405, 100, 70, "BT", btn_d_border, btn_l_border, btn_l_text, 2);
    }
    if (IS_TRACKING == true) {
      DrawButton( 220, 325, 100, 70, "TRACK", btn_d_border, btn_l_border, btn_l_text, 2);
      tft.setTextSize(3);
      tft.fillRect(0, 116, 200, 25, BLACK);
      tft.setCursor(1, 117);
      tft.println("TRACKING:");

    } else {
      DrawButton( 220, 325, 100, 70, "TRACK", 0, btn_l_border, btn_l_text, 2);
    }
    MAIN_SCREEN_MENU = 0;
  } else {
    if (IS_BT_MODE_ON == false) {
      DrawButton( 1, 325, 100, 70, "COORD", 0, btn_l_border, btn_l_text, 2);
      DrawButton( 110, 325, 100, 70, "MAP", 0, btn_l_border, btn_l_text, 2);
      DrawButton( 1, 405, 100, 70, "OPTION", 0, btn_l_border, btn_l_text, 2);
      DrawButton( 110, 405, 100, 70, "+", 0, btn_l_border, btn_l_text, 2);
    } else {
      DrawButton( 1, 325, 100, 70, "", 0, btn_d_border, btn_l_text, 2);
      DrawButton( 110, 325, 100, 70, "", 0, btn_d_border, btn_l_text, 2);
      DrawButton( 1, 405, 100, 70, "", 0, btn_d_border, btn_l_text, 2);
      DrawButton( 110, 405, 100, 70, "+", 0, btn_l_border, btn_l_text, 2);
    }
    if (IS_FAN1_ON) {
      DrawButton( 220, 325, 100, 70, "FAN 1", btn_d_border, btn_l_border, btn_l_text, 2);
    } else {
      DrawButton( 220, 325, 100, 70, "FAN 1", 0, btn_l_border, btn_l_text, 2);
    }
    if (IS_FAN2_ON) {
      DrawButton( 220, 405, 100, 70, "FAN 2", btn_d_border, btn_l_border, btn_l_text, 2);
    } else {
      DrawButton( 220, 405, 100, 70, "FAN 2", 0, btn_l_border, btn_l_text, 2);
    }
    MAIN_SCREEN_MENU = 1;
  }
}

void TimerUpdateDraw(int z) {
  tft.setTextColor(d_text);
  w_DateTime[dateEntryPos] = z;
  if (dateEntryPos >= 0 && dateEntryPos < 2) {
    tft.fillRect((dateEntryPos * 18) + 70, 80, 18, 22, BLACK);
    tft.setCursor((dateEntryPos * 18) + 70, 80);
  } else if (dateEntryPos > 1 && dateEntryPos < 4) {
    tft.fillRect((dateEntryPos * 18) + 88, 80, 18, 22, BLACK);
    tft.setCursor((dateEntryPos * 18) + 88, 80);
  } else if (dateEntryPos > 3 && dateEntryPos < 8) {
    tft.fillRect((dateEntryPos * 18) + 106, 80, 18, 22, BLACK);
    tft.setCursor((dateEntryPos * 18) + 106, 80);
  } else if (dateEntryPos > 7 && dateEntryPos < 10) {
    tft.fillRect(((dateEntryPos - 8) * 18) + 110, 115, 18, 22, BLACK);
    tft.setCursor(((dateEntryPos - 8) * 18) + 110, 115);
  } else if (dateEntryPos > 9) {
    tft.fillRect(((dateEntryPos - 8) * 18) + 128, 115, 18, 22, BLACK);
    tft.setCursor(((dateEntryPos - 8) * 18) + 128, 115);
  }
  tft.print(w_DateTime[dateEntryPos]);
  if (dateEntryPos > 10) {
    dateEntryPos = 0;
  } else {
    dateEntryPos += 1;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drawStarMap() {
  CURRENT_SCREEN = 11;
  tft.fillScreen(BLACK);;
  tft.fillRect(1, 1, 319, 35, title_bg);
  tft.drawLine(0, 37, 320, 37, title_bg);
  tft.setCursor(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("StarMap");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  DrawButton(240, 5, 70, 25, "<back", btn_d_border, btn_l_border, btn_l_text, 2);

  // Draw Star Map...
  String PIC_StarMap = "starmap/";
  if (IS_NIGHTMODE) {
    PIC_StarMap += "night/";
  } else {
    PIC_StarMap += "day/";
  }

  // Need to calculate which image to show
  // Images are named as in a matrix
  // (1,8 1,7 1,6 .... 1,1)
  // (2,8 2,7 2,6 .... 2,1)
  // (3,8 3,7 3,6 .... 3,1)
  // 1,1  = DEC > 30 deg and 3h > RA > 0h;
  // 1,2  = DEC > 30 deg and 6h> RA > 3h;
  // 2,1 = -30 deg > DEC > 30 deg and 3h > RA > 0h;
  // ......

  Current_RA_DEC();
  float DEC_dd = curr_DEC_D + curr_DEC_M / 60;
  int telescope_X = 0;
  int telescope_Y = 0;

  int tmp_map_r = 0;
  int tmp_map_c = 0;
  if (DEC_dd > 30) {
    tmp_map_r = 1;
  } else if (DEC_dd < -30) {
    tmp_map_r = 3;
  } else {
    tmp_map_r = 2;
  }
  tmp_map_c = int((curr_RA_H + curr_RA_M / 60) / 3) + 1;

  if ((tmp_map_c == map_c) && (tmp_map_r == map_r)) {
    IS_CUSTOM_MAP_SELECTED = false;
  }

  // First time only after you Start the rDUINOScope map, it needs this routine!
  if ((map_c == 0) && (map_r == 0)) {
    map_c = tmp_map_c;
    map_r = tmp_map_r;
  }


  if (!IS_CUSTOM_MAP_SELECTED) {
    map_c = tmp_map_c;
    map_r = tmp_map_r;
    // Now decide where is the Telescope Pointing...
    // Only calculate Telescope IF: -70 < DEC < 70 deg.
    if ((DEC_dd < 70) && (DEC_dd > -70)) {
      telescope_X = 320 - (106 * ((curr_RA_H + curr_RA_M / 60) - (tmp_map_c - 1) * 3));

      if (tmp_map_r == 2) {

        if ((DEC_dd > -20) && (DEC_dd < 20)) {
          telescope_Y = 280 + DEC_dd * -7.35;  //  Was   -5.35

          // Example Targets M 15,M53  //M5, M77, M61 > 0 //M2, M14, M73 <0

        } else if (DEC_dd > 20) {
          telescope_Y = 138 - (DEC_dd - 20) * 9.1;
          // Example Targets  M1,M64,M3

        } else if (DEC_dd < -20) {
          telescope_Y = 420 + (abs(DEC_dd) - 20) * 9.1; // was 327
          // Example Targets  M4, M30, M75,M83

        }
      } else {
        if ((abs(DEC_dd) > 30) && (abs(DEC_dd) < 40)) {
          telescope_Y = 505 - (DEC_dd - 30) * 7.2;        // Was 483
          // Example Targets  M13,M36, M37, M38,M57


        } else if ((abs(DEC_dd) > 40) && (abs(DEC_dd) < 60)) {
          telescope_Y = 452 - (DEC_dd - 40) * 10.4;              // 438         // Was 8.4
          // Example Targets   M110 , M92, M39, M102,M101, M51,M63, M76

        } else if (DEC_dd > 60) {
          telescope_Y = 243 - (DEC_dd - 60) * 18.7;   //Was 177    // 13.7
          // Example Targets  M82, M81
        }
        if (DEC_dd < 0) {
          telescope_Y = 480 - telescope_Y; // Was 480

        }
      }
    }
  }

  char My_Map[50];
  PIC_StarMap += String(map_r) + "-" + String(map_c) + ".bmp";
  PIC_StarMap.toCharArray(My_Map, 50);
  bmpDraw(My_Map, 0, 40);

  if (!IS_CUSTOM_MAP_SELECTED) {
    tft.drawCircle(telescope_X, telescope_Y, 16, RED);
    tft.drawLine(0, telescope_Y, 320, telescope_Y, RED);
    tft.drawLine(telescope_X, 40, telescope_X, 480, RED);
  }
  IS_CUSTOM_MAP_SELECTED = false;
}

void drawStatusBar()
{
  tft.fillRect(0, 1, 320, 26, title_bg);
  tft.drawLine(0, 27, 320, 27, Button_State_OFF);
  tft.setTextSize(1);
  tft.setTextColor(Button_Title);

  tft.setCursor(3, 4);
  tft.print("Bright");

  tft.setCursor(47, 4);
  tft.print("T-out");

  tft.setCursor(85, 4);
  tft.print("Track-M");

  tft.setCursor(138, 4);
  tft.print("Mer-F");

  tft.setCursor(177, 4);
  tft.print("Fan1");

  tft.setCursor(210, 4);
  tft.print("Fan2");

  tft.setCursor(244, 4);
  tft.print("Sound");

  tft.setCursor(283, 4);
  tft.print("Motors");

  if (TFT_Brightness == 255)
  {
    tft.setTextColor(Button_State_ON);
    tft.setCursor(5, 15);
    tft.print(TFT_Brightness * 100 / 255);
  }

  if (TFT_Brightness <= 254)
  {
    tft.setTextColor(Button_State_OFF);
    tft.setCursor(7, 15);
    tft.print(TFT_Brightness * 100 / 255);
  }

  tft.setCursor(29, 15);
  tft.print("%");

  if (TFT_Time == "AL-ON") {
    tft.setCursor(47, 15);
    tft.setTextColor(Button_State_ON);
    tft.print("AL-ON");
  }

  if (TFT_Time == "30 s") {
    tft.setCursor(49, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("30 S");
  }

  if (TFT_Time == "60 s") {
    tft.setCursor(49, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("60 S");
  }

  if (TFT_Time == "2 min") {
    tft.setCursor(49, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("2 M");
  }

  if (TFT_Time == "5 min") {
    tft.setCursor(49, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("5 M");
  }

  if (TFT_Time == "10 min") {
    tft.setCursor(48, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("10 M");
  }


  if (Tracking_Mode == "Celest") {
    tft.setCursor(88, 15);
    tft.setTextColor(Button_State_ON);
    tft.print("Celest");
  }

  if (Tracking_Mode == "Lunar") {
    tft.setCursor(88, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("Lunar");
  }

  if (Tracking_Mode == "Solar") {
    tft.setCursor(88, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("Solar");
  }

  tft.setCursor(141, 15);
  tft.setTextColor(Button_State_ON);
  tft.print("AUTO");

  if (Mer_Flip_State == "AUTO") {
    tft.fillRect(140, 14, 30, 10, title_bg);
    tft.setCursor(141, 15);
    tft.setTextColor(Button_State_ON);
    tft.print("AUTO");
  }

  if (Mer_Flip_State == "OFF") {
    tft.fillRect(140, 14, 30, 10, title_bg);
    tft.setCursor(141, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }

  if (Fan1_State == "ON") {
    tft.setCursor(180, 15);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
  }

  if (Fan1_State == "OFF") {
    tft.setCursor(180, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }

  if (Fan2_State == "ON") {
    tft.setCursor(214, 15);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
  }

  if (Fan2_State == "OFF") {
    tft.setCursor(214, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }

  if (Sound_State == "ON") {
    tft.setCursor(252, 15);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
  }

  if (Sound_State == "OFF") {
    tft.setCursor(252, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }

  if (Stepper_State == "ON") {
    tft.setCursor(292, 15);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
  }

  if (Stepper_State == "OFF") {
    tft.setCursor(292, 15);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");

  }
}


void updateTrackingMode_opt()
{
  tft.setCursor(5, 60);
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2.5);
  tft.println("Tracking Modes");
  if (Tracking_type == 0) {
    DrawButton(0, 85, 100, 40, "Celest", 0, btn_l_border, btn_l_text, 2);
    DrawButton(110, 85, 100, 40, "Lunar", btn_d_border, btn_l_border, btn_l_text, 2);
    DrawButton(220, 85, 100, 40, "Solar", 0, btn_l_border, btn_l_text, 2);
  } else if (Tracking_type == 1) {
    DrawButton(0, 85, 100, 40, "Celest", btn_d_border, btn_l_border, btn_l_text, 2);
    DrawButton(110, 85, 100, 40, "Lunar", 0, btn_l_border, btn_l_text, 2);
    DrawButton(220, 85, 100, 40, "Solar", 0, btn_l_border, btn_l_text, 2);
  } else if (Tracking_type == 2) {
    DrawButton(0, 85, 100, 40, "Celest", 0, btn_l_border, btn_l_text, 2);
    DrawButton(110, 85, 100, 40, "Lunar", 0, btn_l_border, btn_l_text, 2);
    DrawButton(220, 85, 100, 40, "Solar", btn_d_border, btn_l_border, btn_l_text, 2);
  }
}

void updateScreenTimeout_opt()
{
  if (TFT_timeout == 0) {
    DrawButton(0, 230, 45, 40, "NEVER", btn_d_border, btn_l_border, btn_l_text, 1);
  } else {
    DrawButton(0, 230, 45, 40, "NEVER", 0, btn_l_border, btn_l_text, 1);
  }
  if (TFT_timeout == 30000) {
    DrawButton(55, 230, 45, 40, "30s", btn_d_border, btn_l_border, btn_l_text, 1);
  } else {
    DrawButton(55, 230, 45, 40, "30s", 0, btn_l_border, btn_l_text, 1);
  }
  if (TFT_timeout == 60000) {
    DrawButton(110, 230, 45, 40, "60s", btn_d_border, btn_l_border, btn_l_text, 1);
  } else {
    DrawButton(110, 230, 45, 40, "60s", 0, btn_l_border, btn_l_text, 1);
  }
  if (TFT_timeout == 120000) {
    DrawButton(165, 230, 45, 40, "2m", btn_d_border, btn_l_border, btn_l_text, 1);
  } else {
    DrawButton(165, 230, 45, 40, "2m", 0, btn_l_border, btn_l_text, 1);
  }
  if (TFT_timeout == 300000) {
    DrawButton(220, 230, 45, 40, "5m", btn_d_border, btn_l_border, btn_l_text, 1);
  } else {
    DrawButton(220, 230, 45, 40, "5m", 0, btn_l_border, btn_l_text, 1);
  }
  if (TFT_timeout == 600000) {
    DrawButton(275, 230, 45, 40, "10m", btn_d_border, btn_l_border, btn_l_text, 1);
  } else {
    DrawButton(275, 230, 45, 40, "10m", 0, btn_l_border, btn_l_text, 1);
  }
}

void updateMeridianFlip_opt()
{
  tft.setCursor(5, 280);
  tft.setTextSize(2.5);
  tft.println("Meridian Flip");
  if (IS_MERIDIAN_FLIP_AUTOMATIC) {
    DrawButton(0, 300, 100, 40, "AUTO", btn_d_border, btn_l_border, btn_l_text, 2);
    DrawButton(110, 300, 100, 40, "OFF", 0, btn_l_border, btn_l_text, 2);
  } else {
    DrawButton(0, 300, 100, 40, "AUTO", 0, btn_l_border, btn_l_text, 2);
    DrawButton(110, 300, 100, 40, "OFF", btn_d_border, btn_l_border, btn_l_text, 2);
  }
}

void updateSound_opt()
{
  tft.setCursor(5, 350);
  tft.setTextSize(2.5);
  tft.println("Sound On/Off");
  if (IS_SOUND_ON) {
    DrawButton(0, 370, 100, 40, "ON", btn_d_border, btn_l_border, btn_l_text, 2);
    DrawButton(110, 370, 100, 40, "OFF", 0, btn_l_border, btn_l_text, 2);
  } else {
    DrawButton(0, 370, 100, 40, "ON", 0, btn_l_border, btn_l_text, 2);
    DrawButton(110, 370, 100, 40, "OFF", btn_d_border, btn_l_border, btn_l_text, 2);

  }
}

void updateStepper_opt()
{
  tft.setCursor(5, 420);
  tft.println("Stepper Motors");
  if (IS_STEPPERS_ON)
  {
    DrawButton(0, 440, 100, 40, "ON", btn_d_border, btn_l_border, btn_l_text, 2);
    DrawButton(110, 440, 100, 40, "OFF", 0, btn_l_border, btn_l_text, 2);
  }
  else
  {
    DrawButton(0, 440, 100, 40, "ON", 0, btn_l_border, btn_l_text, 2);
    DrawButton(110, 440, 100, 40, "OFF", btn_d_border, btn_l_border, btn_l_text, 2);
  }
}

void updateTriangleBrightness_opt()
{
  if (TFT_Brightness >= 255)
  {
    DrawButton(275, 160, 45, 40, "MAX", btn_d_border, btn_l_border, btn_l_text, 1); //DrawButton(X, Y, Width, Height, Caption, BodyColor, BorderColor, TextColor, tSize)
  }
  else
  {
    DrawButton(275, 160, 45, 40, "MAX", 0, btn_l_border, btn_l_text, 1);
  }

  //Dark Triangle
  tft.fillTriangle(5, 200, 265, 200, 265, 155, btn_d_border);

  //Clear Triangle
  int x_br = (TFT_Brightness - 190) * 4 + 5;
  int tria_h = floor((265 - x_br) * 0.17307);
  tft.fillTriangle(5, 200, x_br, 200, x_br, tria_h + 156, btn_l_border);
  //int bright_perc = round(TFT_Brightness * 0.39215);
  int bright_perc = (TFT_Brightness * 100 / 255);

#ifdef serial_debug
  Serial.print("tria_h = ");
  Serial.println(tria_h);
  Serial.print("brightness: ");
  Serial.println(TFT_Brightness);
#endif

  tft.setTextSize(2);
  tft.setTextColor(Purple);
  tft.setCursor(135, 183);
  tft.print(bright_perc);
  tft.print(" %");
  tft.setTextColor(btn_l_text);
}

void drawAlignObjects_ali()
{
  tft.fillRect(0, 80, 320, 340, BLACK);
  if (ALLIGN_TYPE == 3)
  {
    int kk = STARS_PAGER * 24;
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 4; j++) {
        int i1 = Iter_Stars[kk].indexOf(';');
        int i2 = Iter_Stars[kk].indexOf(';', i1 + 1);
        String S_NAME = Iter_Stars[kk].substring(i1 + 1, i2);
        String C_NAME = Iter_Stars[kk].substring(0, i1);
        if (S_NAME == "") {
          break;
        }
        tft.fillRect(((j * 75) + 12), ((i * 50) + 85), 71, 45, messie_btn);
        int l = (S_NAME.length() / 2) * 6;
        tft.setCursor(((j * 75) + (44 - l)), ((i * 50) + 93));
        tft.setTextSize(1);
        tft.print(S_NAME);
        tft.setTextSize(2);
        tft.setCursor(((j * 75) + 29), ((i * 50) + 110));
        tft.print(C_NAME);
        kk += 1;
      }
    }
  }
  else
  {
    int kk = STARS_PAGER * 24;
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 4; j++) {
        int i1 = Stars[kk].indexOf(';');
        int i2 = Stars[kk].indexOf(';', i1 + 1);
        String S_NAME = Stars[kk].substring(i1 + 1, i2);
        String C_NAME = Stars[kk].substring(0, i1);
        if (S_NAME == "") {
          break;
        }
        tft.fillRect(((j * 75) + 12), ((i * 50) + 85), 71, 45, messie_btn);
        int l = (S_NAME.length() / 2) * 6;
        tft.setCursor(((j * 75) + (44 - l)), ((i * 50) + 93));
        tft.setTextSize(1);
        tft.print(S_NAME);
        tft.setTextSize(2);
        tft.setCursor(((j * 75) + 29), ((i * 50) + 110));
        tft.print(C_NAME);
        kk += 1;
      }
    }
  }
}

void drawLoadObjects()
{
  ///////     Messier Screen /////////////
  if (LOAD_SELECTOR == 1)
  {
    // I'll draw 20 objects per page, thus "(pager*20)" will give me the start of the [index_]
    int kk = MESS_PAGER * 20;
    //tft.fillRect(10,160, 310, 250, ILI9488_BLACK);

    for (int i = 0; i < 5; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        String M_NAME = Messier_Array[kk].substring(0, Messier_Array[kk].indexOf(';'));
        if (M_NAME == "")
        {
          tft.fillRect(((j * 75) + 12), ((i * 50) + 160), 71, 45, BLACK);
        }
        else
        {
          DrawButton(((j * 75) + 12), ((i * 50) + 160), 71, 45, M_NAME, messie_btn, 0, btn_l_text, 2);
          // 75 box width ,12start from left , 50 box hight , 110 start from up
          kk += 1;
        }
      }
    }
  }
  ///////     Treasures Screen /////////////
  else if (LOAD_SELECTOR == 2)
  {
    tft.setTextSize(1);
    int ll = TREAS_PAGER * 20;
    //tft.fillRect(10,160, 310, 250, ILI9488_BLACK);

    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 4; j++) {
        String M_NAME = Treasure_Array[ll].substring(0, Treasure_Array[ll].indexOf(';'));
        if (M_NAME == "")
        {
          tft.fillRect(((j * 75) + 12), ((i * 50) + 160), 71, 45, BLACK);
        }
        else
        {
          DrawButton(((j * 75) + 12), ((i * 50) + 160), 71, 45, M_NAME, messie_btn, 0, btn_l_text, 1);
          ll += 1;
        }
      }
    }
  }
  else if (LOAD_SELECTOR == 3)  // solar system planets
  {
    tft.setTextSize(1);
    int ll = 0;

    for (int i = 0; i < 5; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        if (ll >= 11)
        {
          tft.fillRect(((j * 75) + 12), ((i * 50) + 160), 71, 45, BLACK);
        }
        else
        {
          String M_NAME = ss_planet_names[ll];
          DrawButton(((j * 75) + 12), ((i * 50) + 160), 71, 45, M_NAME, messie_btn, 0, btn_l_text, 1);
        }
        ll += 1;
      }
    }
  }
  else if (LOAD_SELECTOR == 4) // custom.csv
  {
    tft.setTextSize(1);
    int ll = CUSTOM_PAGER * 20;
    //tft.fillRect(10,160, 310, 250, ILI9488_BLACK);

    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 4; j++) {
        String M_NAME = custom_Array[ll].substring(0, custom_Array[ll].indexOf(';'));
        if (M_NAME == "")
        {
          tft.fillRect(((j * 75) + 12), ((i * 50) + 160), 71, 45, BLACK);
        }
        else
        {
          DrawButton(((j * 75) + 12), ((i * 50) + 160), 71, 45, M_NAME, messie_btn, 0, btn_l_text, 1);
          ll += 1;
        }
      }
    }
  }
}

void drawConfirmSunTrack()
{
  CURRENT_SCREEN = 15;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 320, 100, RED);
  tft.drawRect(10, 10, 300, 80, YELLOW);
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.setCursor(42, 35);
  tft.println("!! WARNING !!");

  tft.setTextColor(WHITE);
  tft.setCursor(0, 120);
  tft.setTextSize(2);
  tft.print("Looking directly at Sun, without any protection filter like the BAADER ASTROSOLAR FILTER, may result in ");
  tft.setTextColor(RED);
  tft.print("permanent damage to your eye or telescope!\n\n");
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("The programmer is not resposible for damages caused by the improper use of the rDUINOScope GoTo controller.\n");

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Do you want to continue?");

  DrawButton(10, 380, 140, 80, "YES", 0, btn_l_border, btn_l_text, 3);
  DrawButton(170, 380, 140, 80, "NO", 0, btn_l_border, btn_l_text, 3);


  /////////////////////////////////// Change This part /////////////
  /*
    while (!myTouch.touched()) {}

    int tx = 0;
    int ty = 0;

    while (ty < 10 || ty > 150 && ty < 170 || ty > 310 || tx < 380 || tx > 460)
    {
      if(myTouch.touched())
      {

        p = myTouch.getPoint();
        while (p.z < 600)
        {
          p = myTouch.getPoint(); //to remove noise
          delay(200);
        }

        tx = (p.x - 257) / calx;
        ty = (p.y - 445) / caly;

        //Useful to debug touch:
        #ifdef serial_debug
          Serial.print(" -> Touched: x = ");
          Serial.print(tx);
          Serial.print(", y = ");
          Serial.println(ty);
        #endif
      }
      else  delay(100);
    }
    considerTouchInput(ty, tx);
    }
  */
}
