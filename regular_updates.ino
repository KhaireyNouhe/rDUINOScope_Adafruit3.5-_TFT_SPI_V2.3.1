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
//  - Measures Temperature and Humidity;
//  - On each TimeInterval (~10 sec.) - Updates screens accordingly (e.g. Time, LST, Temperature, Humidity and etc.)
//

void considerTempUpdates(){                                 // Temperature && Humidity Updates
  if (CURRENT_SCREEN == 4 && (millis()-Tupdate_time) > 30000){
       float tHum = dht.readHumidity();
       float tTemp = dht.readTemperature();
       if (isnan(tHum) || isnan(tTemp)) {
          return;
       }else{
          _temp = tTemp - 2;  // I need to calibrate my sensor... it reads 2 deg. higher temp.
          _humid = tHum;
       }
       tft.setTextSize(2);
       tft.setTextColor(title_texts);
       if (_temp > -75 && _temp < 75 && _humid < 100 && _humid > 0){
         tft.fillRect(258,35,30,55, title_bg);
           
         tft.setCursor(261, 38);
         tft.print(_temp,0);
        
         tft.setCursor(261, 70);
         tft.print(_humid,0);
       }
       Tupdate_time = millis();
  }
}

void considerTimeUpdates(){   // UPDATEs time on Screen1 && Screen4 -  Clock Screen and Main Screen
   int changes=0;
   for (int y=0; y<12; y++){
       if (w_DateTime[y]!=0){
           changes=1;
       }
   }
  int mer_flp_sound = HAHour + ((HAMin + MIN_TO_MERIDIAN_FLIP + MIN_SOUND_BEFORE_FLIP)/60);
  if((millis()- update_time) > 10000 && mer_flp_sound >= 24){
    if (IS_SOUND_ON){
      SoundOn(3830,8);
    }
  }
  if (CURRENT_SCREEN == 4 && (millis()-update_time) > 10000){
      tft.setTextSize(1);
      tft.setTextColor(l_text);
      if (old_d != rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/')){
            tft.fillRect(35,102,60,6, title_texts);
            tft.setCursor(35,102);
            tft.print(String(rtc.getDateStr()).substring(0,2));
            tft.print(" ");
            tft.print(rtc.getMonthStr(FORMAT_SHORT));
            tft.print(" ");
            tft.print(String(rtc.getDateStr()).substring(6));
      }
      tft.setTextColor(title_texts);
      tft.fillRect(90,35,95,22, title_bg);
      tft.setTextSize(3);
      tft.setCursor(90, 35);
      tft.print(String(rtc.getTimeStr()).substring(0,5));  
           
      tft.fillRect(90,65,95,22, title_bg);
      tft.setCursor(90, 65);
      if ((int)LST < 10){
        tft.print("0");
        tft.print((int)LST);
      }else{
        tft.print((int)LST);
      }
      tft.print(":");
      if ((LST - (int)LST)*60 < 10){
        tft.print("0");
        tft.print((LST - (int)LST)*60, 0);
      }else{
        tft.print((LST - (int)LST)*60, 0);
      }
      if ((OBJECT_NAME != "") && (OBJECT_NAME != "CP") && (IS_BT_MODE_ON == false)){
          tft.setCursor(135,290);
          tft.setTextSize(2);
          tft.setTextColor(l_text);
          tft.fillRect(133,290,112,20, BLACK);
          tft.print(HAHour,0);
          tft.print("h");
          tft.print(HAMin,1);
          tft.print("m");

          tft.setTextSize(1);
          tft.setCursor(2,240);
          tft.setTextColor(l_text);
          tft.fillRect(1,239,200,18, BLACK);
          tft.print("Az/Alt: ");
          tft.print(floor(AZ), 0);
          tft.print("^ ");
          tft.print((AZ - floor(AZ))*60, 0);
          tft.print("' / ");
          tft.print(floor(ALT),0);
          tft.print("^ ");
          tft.print((ALT - floor(ALT))*60, 0);
          tft.print("'");
      }

      if ((IS_BT_MODE_ON == true) && (OBJECT_DESCR == "Pushed via BlueTooth")){
          tft.setTextSize(1);
          tft.setCursor(5,256);
          tft.setTextColor(l_text);
          tft.fillRect(5,255,230,22, BLACK);
          tft.print("HA: ");
          tft.print(HAHour,0);
          tft.print("h");
          tft.print(HAMin,1);
          tft.print("m    | HS:");
          tft.println(SLEW_RA_microsteps);
          tft.print("               | CS:");
          tft.print(RA_microSteps);
      }   
          
      update_time = millis();
  }
  else if (CURRENT_SCREEN == 5 && (millis()-update_time) > 2000)
  {
      Current_RA_DEC();
      tft.setTextSize(3);
      
      tft.fillRect(87,77,198,30, btn_d_border);
      tft.setCursor(108, 85);
      tft.print(curr_DEC_lz);

      tft.fillRect(87,132,198,30, btn_d_border);
      tft.setCursor(115, 140);
      tft.print(curr_HA_lz);

      tft.fillRect(87,187,198,30, btn_d_border);
      tft.setCursor(115, 195);
      tft.print(curr_RA_lz);

      update_time = millis();  

   }
   else if (CURRENT_SCREEN == 1 && (millis()-update_time) > 10000 && changes == 0)
   {
      tft.setCursor(110, 115);
      tft.setTextColor(l_text);
      tft.fillRect(110,115,200,30, BLACK);
      tft.print(rtc.getTimeStr(FORMAT_SHORT));
      if (old_d != rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/')){
          tft.fillRect(70,80,200,30, BLACK);
          tft.setCursor(70, 80);
          tft.print(rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/'));
      }
      update_time = millis();      
  }
  else if (CURRENT_SCREEN == 0 && (millis()-update_time) > 5000)
  {
        tft.fillRect(10,200,320,200, BLACK);
        tft.setTextColor(btn_l_text);
        tft.setTextSize(3);
        tft.setCursor(55, 200);
        tft.print("LAT: ");
        tft.print(gps.location.lat(),4);
        tft.setCursor(55, 260);
        tft.print("LNG: ");
        tft.print(gps.location.lng(),4);
        tft.setCursor(90, 300);
        tft.setTextSize(2);
        tft.print("Satelites: ");
        tft.print(gps.satellites.value());
        tft.setCursor(110, 325);
        tft.print("GMT: ");
        tft.print(gps.time.hour());
        tft.print(":");
        tft.print(gps.time.minute());
        tft.setCursor(75, 350);
        tft.print("Altitude: ");
        tft.print(gps.altitude.meters());
        if (gps.satellites.value()==0)
        {
          smartDelay(1000);
        }
        else
        {
          GPS_iterrations += 1;
          smartDelay(1000);
        }

         if ((GPS_iterrations > 2) && (gps.location.lat() != 0))
         {
          OBSERVATION_LONGITUDE = gps.location.lng();
          OBSERVATION_LATTITUDE = gps.location.lat();
          OBSERVATION_ALTITUDE = gps.altitude.meters();
          // Set the earth rotation direction depending on the Hemisphere...
          // HIGH and LOW are substituted 
          if (OBSERVATION_LATTITUDE > 0)
          {
            STP_FWD = LOW;
            STP_BACK = HIGH;
          }
          else
          {
            STP_FWD = HIGH;
            STP_BACK = LOW;
          }
          Serial.println(OBSERVATION_LATTITUDE);
          Serial.print("STP_FWD = ");
          Serial.println(STP_FWD);
          Serial.print("STP_BACK = ");
          Serial.println(STP_BACK);
          if (IS_SOUND_ON){
            SoundOn(note_C,32);
          }
          CURRENT_SCREEN = 1;
          // Serial2.end();

          int time_delay = round(gps.location.lng() * 4/60); //rough calculation of the timezone delay
          if(isSummerTime(gps.time.value()))
          {
            time_delay += 1;
            Summer_Time = 1;
          }
          
          rtc.setDate(gps.date.day(), gps.date.month(), gps.date.year());
          rtc.setTime(gps.time.hour()+time_delay, gps.time.minute(), gps.time.second());
          
          drawClockScreen();
        }
        update_time = millis(); 
  }
  else if ((CURRENT_SCREEN == 13) && (IS_OBJ_FOUND == true) && ((millis()-update_time) > 2000))
  {
       tft.setTextColor(btn_l_text);
       tft.setTextSize(2);
       float HAHh;
       float HAMm;
       float HA_deci;
       
       if (HAHour >= 12){
          HAHh = HAHour - 12;
       }else{
          HAHh = HAHour;
       }

       HAMm = HAMin;
       HA_deci = (HAHh+(HAMm/60))*15;   // In degrees - decimal


        if (ALLIGN_STEP == 1)
        {                  
            tft.fillRect(0, 295, 320, 80, BLACK);
            tft.setCursor(0, 300);
            tft.setTextSize(2);

            delta_a_RA = (double(RA_microSteps) - double(HA_deci * HA_H_CONST))/double(HA_H_CONST);
            delta_a_DEC = (double(DEC_microSteps) - double(SLEW_DEC_microsteps))/double(DEC_D_CONST);
          
            tft.print("Delta_RA: ");
            tft.print(delta_a_RA*60, 6);
            tft.println(" arcmin");
            tft.print("Delta_DEC: ");
            tft.print(delta_a_DEC*60, 6);
            tft.println(" arcmin");
            
        }
        tft.setTextSize(2);           // To make sure that when a button is Pressed, it will be re-drawn sith Size 2 text!
        update_time = millis();
  }
    
}
