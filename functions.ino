void calculateLST_HA() {
  //  HA = LST - RA
  String Date_q = String(rtc.getDateStr());
  String Time_q = String(rtc.getTimeStr());
  int D = Date_q.substring(0, 2).toInt();
  int M = Date_q.substring(3, 5).toInt();
  int Y = Date_q.substring(6).toInt();
  int S = Time_q.substring(6).toInt();
  int H = Time_q.substring(0, 2).toInt(); // hours
  if (Summer_Time == 1) {
    H -= 1;
  }
  int MN = Time_q.substring(3, 5).toInt();
  if (M < 3) {
    M = M + 12;
    Y = Y - 1;
  }

  float HH = H + ((float)MN / 60.00) + ((float)S / 3600.00);
  float AA = (int)(365.25 * (Y + 4716));
  float BB = (int)(30.6001 * (M + 1));
  double CurrentJDN = AA + BB + D - 1537.5 + (HH - TIME_ZONE) / 24;
  float current_day = CurrentJDN - 2451543.5;

  //calculate terms required for LST calcuation and calculate GMST using an approximation
  double MJD = CurrentJDN - 2400000.5;
  int MJD0 = (int)MJD;
  float ut = (MJD - MJD0) * 24.0;
  double t_eph  = (MJD0 - 51544.5) / 36525.0;
  double GMST = 6.697374558 + 1.0027379093 * ut + (8640184.812866 + (0.093104 - 0.0000062 * t_eph) * t_eph) * t_eph / 3600.0;

  LST = GMST + OBSERVATION_LONGITUDE / 15.0;

  //reduce it to 24 format
  int LSTint = (int)LST;
  LSTint /= 24;
  LST = LST - (double) LSTint * 24;

  // Now I'll use the global Variables OBJECT_RA_H and OBJECT_RA_M  To calculate the Hour angle of the selected object.

  double dec_RA = OBJECT_RA_M / 60 + OBJECT_RA_H;

  double HA_decimal = LST - dec_RA;

  HAHour = int(HA_decimal);
  HAMin = (HA_decimal - HAHour) * 60;

  if (HAMin < 0) {
    HAHour -= 1;
    HAMin += 60;
  }
  if (HAHour < 0) {
    HAHour += 24;
  }

  // Convert degrees into Decimal Radians
  double rDEC = 0;
  rDEC = OBJECT_DEC_D + (OBJECT_DEC_M / 60);


  //rDEC += delta_a_DEC;
  rDEC *= 0.0174532925199;
  double rHA =  HA_decimal * 0.26179938779915;   // 0.261799.. = 15 * 3.1415/180  (to convert to Deg. and * Pi) :)
  double rLAT = OBSERVATION_LATTITUDE * 0.0174532925199;

  IS_OBJ_VISIBLE = true;

  double sin_rDEC = sin(rDEC);
  double cos_rDEC = cos(rDEC);
  double sin_rLAT = sin(rLAT);
  double cos_rLAT = cos(rLAT);
  double cos_rHA = cos(rHA);
  double sin_rHA = sin(rHA);

  ALT = sin_rDEC * sin_rLAT;
  ALT += (cos_rDEC * cos_rLAT * cos_rHA);
  double sin_rALT = ALT;
  ALT =  asin(ALT);
  double cos_rALT = cos(ALT);
  ALT *= 57.2958;

  AZ = sin_rALT * sin_rLAT;
  AZ = sin_rDEC - AZ;
  AZ /= (cos_rALT * cos_rLAT);
  AZ = acos(AZ) * 57.2957795;
  if (sin_rHA > 0) {
    AZ = 360 - AZ;
  }

  if (ALT < 0) {
    IS_OBJ_VISIBLE = false;
    if ((IS_BT_MODE_ON == true) && (IS_OBJ_FOUND == false)) {
      Serial3.println("Object is out of sight! Telescope not moved.");
    }
    IS_OBJ_FOUND = true;
    IS_OBJECT_RA_FOUND = true;
    IS_OBJECT_DEC_FOUND = true;
    Slew_RA_timer = 0;
    RA_finish_last = 0;
  } else {
    IS_OBJ_VISIBLE = true;
  }


  // Take care of the Meridian Flip coordinates
  // This will make the telescope do Meridian Flip... depending on the current HA and predefined parameter: MIN_TO_MERIDIAN_FLIP
  if (IS_MERIDIAN_FLIP_AUTOMATIC) {
    mer_flp = HAHour + ((HAMin + MIN_TO_MERIDIAN_FLIP) / 60);
    float old_HAMin = HAMin;
    float old_HAHour = HAHour;
    if (IS_POSIBLE_MERIDIAN_FLIP == true) {
      if (mer_flp >= 24) {
        HAMin = HAMin - 60;
        HAHour = 0;
        if (MERIDIAN_FLIP_DO == false) {
          IS_TRACKING = false;
          Timer3.stop();
          OnScreenMsg(1);
          if (IS_SOUND_ON) {
            SoundOn(note_C, 32);
            delay(200);
            SoundOn(note_C, 32);
            delay(200);
            SoundOn(note_C, 32);
            delay(1000);
          }
          IS_OBJ_FOUND = false;
          IS_OBJECT_RA_FOUND = false;
          IS_OBJECT_DEC_FOUND = false;
          Slew_timer = millis();
          Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
          MERIDIAN_FLIP_DO = true;
          drawMainScreen();
        } else {
          if ((old_HAHour == HAHour) && (old_HAMin == HAMin)) {  // Meridian Flip is done so the code above will not execute
            MERIDIAN_FLIP_DO = false;
          }
        }
        //DEC is set as part of the SlewTo function
      }
    } else {
      if (mer_flp >= 24) {
        IS_TRACKING = false;
        Timer3.stop();
      }
    }
  }
}

void selectOBJECT_M(int index_, int objects) {
  OBJECT_Index = index_;

  if (objects == 0) {                                          // I've selected a Messier Object
    TRACKING_MOON = false;
    int i1 = Messier_Array[index_].indexOf(';');
    int i2 = Messier_Array[index_].indexOf(';', i1 + 1);
    int i3 = Messier_Array[index_].indexOf(';', i2 + 1);
    int i4 = Messier_Array[index_].indexOf(';', i3 + 1);
    int i5 = Messier_Array[index_].indexOf(';', i4 + 1);
    int i6 = Messier_Array[index_].indexOf(';', i5 + 1);
    int i7 = Messier_Array[index_].indexOf(';', i6 + 1);
    OBJECT_NAME = Messier_Array[index_].substring(0, i1);
    OBJECT_DESCR = Messier_Array[index_].substring(i7 + 1, Messier_Array[index_].length() - 1);
    String OBJ_RA = Messier_Array[index_].substring(i1, i2);
    OBJECT_RA_H = OBJ_RA.substring(1, OBJ_RA.indexOf('h')).toFloat();
    OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h') + 1, OBJ_RA.length() - 1).toFloat();
    String OBJ_DEC = Messier_Array[index_].substring(i2, i3);
    String sign = OBJ_DEC.substring(1, 2);
    OBJECT_DEC_D = OBJ_DEC.substring(2, OBJ_DEC.indexOf('°')).toFloat();
    OBJECT_DEC_M = OBJ_DEC.substring(OBJ_DEC.indexOf('°') + 1, OBJ_DEC.length() - 1).toFloat();
    if (sign.equals("-")) {
      OBJECT_DEC_D *= -1;
      OBJECT_DEC_M *= -1;
    }
    OBJECT_DETAILS = OBJECT_NAME + " is a ";
    OBJECT_DETAILS += Messier_Array[index_].substring(i4 + 1, i5) + " in constelation ";
    OBJECT_DETAILS += Messier_Array[index_].substring(i3 + 1, i4) + ", with visible magnitude of ";
    OBJECT_DETAILS += Messier_Array[index_].substring(i5 + 1, i6) + " and size of ";
    OBJECT_DETAILS += Messier_Array[index_].substring(i6 + 1, i7);

  } else if (objects == 1) {                                    // I've selected a Treasure Object
    TRACKING_MOON = false;
    int i1 = Treasure_Array[index_].indexOf(';');
    int i2 = Treasure_Array[index_].indexOf(';', i1 + 1);
    int i3 = Treasure_Array[index_].indexOf(';', i2 + 1);
    int i4 = Treasure_Array[index_].indexOf(';', i3 + 1);
    int i5 = Treasure_Array[index_].indexOf(';', i4 + 1);
    int i6 = Treasure_Array[index_].indexOf(';', i5 + 1);
    int i7 = Treasure_Array[index_].indexOf(';', i6 + 1);
    OBJECT_NAME = Treasure_Array[index_].substring(0, i1);
    OBJECT_DESCR = Treasure_Array[index_].substring(i7 + 1, Treasure_Array[index_].length() - 1);
    String OBJ_RA = Treasure_Array[index_].substring(i1, i2);
    OBJECT_RA_H = OBJ_RA.substring(1, OBJ_RA.indexOf('h')).toFloat();
    OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h') + 1, OBJ_RA.length() - 1).toFloat();
    String OBJ_DEC = Treasure_Array[index_].substring(i2, i3);
    String sign = OBJ_DEC.substring(1, 2);
    OBJECT_DEC_D = OBJ_DEC.substring(2, OBJ_DEC.indexOf('°')).toFloat();
    OBJECT_DEC_M = OBJ_DEC.substring(OBJ_DEC.indexOf('°') + 1, OBJ_DEC.length() - 1).toFloat();
    if (sign == "-") {
      OBJECT_DEC_D *= -1;
      OBJECT_DEC_M *= -1;
    }
    OBJECT_DETAILS = OBJECT_NAME + " is a ";
    OBJECT_DETAILS += Treasure_Array[index_].substring(i4 + 1, i5) + " in constelation ";
    OBJECT_DETAILS += Treasure_Array[index_].substring(i3 + 1, i4) + ", with visible magnitude of ";
    OBJECT_DETAILS += Treasure_Array[index_].substring(i5 + 1, i6) + " and size of ";
    OBJECT_DETAILS += Treasure_Array[index_].substring(i6 + 1, i7);

  } else if (objects == 2) {                                    // I'm selecting a STAR for Synchronization - 1 Star ALLIGNMENT
    TRACKING_MOON = false;
    int i1 = Stars[index_].indexOf(';');
    int i2 = Stars[index_].indexOf(';', i1 + 1);
    int i3 = Stars[index_].indexOf(';', i2 + 1);
    OBJECT_NAME = Stars[index_].substring(i1 + 1, i2) + " from " + Stars[index_].substring(0, i1);
    String OBJ_RA = Stars[index_].substring(i2 + 1, i3);
    OBJECT_RA_H = OBJ_RA.substring(0, OBJ_RA.indexOf('h')).toFloat();
    OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h') + 1, OBJ_RA.length() - 1).toFloat();
    String OBJ_DEC = Stars[index_].substring(i3, Stars[index_].length());
    String sign = OBJ_DEC.substring(0, 1);
    OBJECT_DEC_D = OBJ_DEC.substring(1, OBJ_DEC.indexOf('°')).toFloat();
    if (sign == "-") {
      OBJECT_DEC_D *= (-1);
    }
    OBJECT_DEC_M = 0;
  } else if (objects == 3) {                                    // I'm selecting a STAR for Synchronization - Iterative ALLIGNMENT
    TRACKING_MOON = false;
    int i1 = Iter_Stars[index_].indexOf(';');
    int i2 = Iter_Stars[index_].indexOf(';', i1 + 1);
    int i3 = Iter_Stars[index_].indexOf(';', i2 + 1);
    OBJECT_NAME = Iter_Stars[index_].substring(i1 + 1, i2) + " from " + Iter_Stars[index_].substring(0, i1);
    String OBJ_RA = Iter_Stars[index_].substring(i2 + 1, i3);
    OBJECT_RA_H = OBJ_RA.substring(0, OBJ_RA.indexOf('h')).toFloat();
    OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h') + 1, OBJ_RA.length() - 1).toFloat();
    String OBJ_DEC = Iter_Stars[index_].substring(i3, Iter_Stars[index_].length());
    String sign = OBJ_DEC.substring(0, 1);
    OBJECT_DEC_D = OBJ_DEC.substring(1, OBJ_DEC.indexOf('°')).toFloat();
    if (sign == "-") {
      OBJECT_DEC_D *= (-1);
    }
    OBJECT_DEC_M = 0;
  }
  else if (objects == 4)
  {
    // I've selected a custom Object
    TRACKING_MOON = false;
    int i1 = custom_Array[index_].indexOf(';');
    int i2 = custom_Array[index_].indexOf(';', i1 + 1);
    int i3 = custom_Array[index_].indexOf(';', i2 + 1);
    int i4 = custom_Array[index_].indexOf(';', i3 + 1);
    int i5 = custom_Array[index_].indexOf(';', i4 + 1);
    int i6 = custom_Array[index_].indexOf(';', i5 + 1);
    int i7 = custom_Array[index_].indexOf(';', i6 + 1);
    int i8 = custom_Array[index_].indexOf(';', i7 + 1);
    OBJECT_NAME = custom_Array[index_].substring(0, i1);
    OBJECT_DESCR = custom_Array[index_].substring(i7 + 1, i8);
    String OBJ_RA = custom_Array[index_].substring(i1, i2);
    OBJECT_RA_H = OBJ_RA.substring(1, OBJ_RA.indexOf('h')).toFloat();
    OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h') + 1, OBJ_RA.length() - 1).toFloat();
    String OBJ_DEC = custom_Array[index_].substring(i2, i3);
    String sign = OBJ_DEC.substring(1, 2);
    OBJECT_DEC_D = OBJ_DEC.substring(2, OBJ_DEC.indexOf('°')).toFloat();
    OBJECT_DEC_M = OBJ_DEC.substring(OBJ_DEC.indexOf('°') + 1, OBJ_DEC.length() - 1).toFloat();
    if (sign == "-")
    {
      OBJECT_DEC_D *= -1;
      OBJECT_DEC_M *= -1;
    }
    OBJECT_DETAILS = OBJECT_NAME + " is a ";
    OBJECT_DETAILS += custom_Array[index_].substring(i4 + 1, i5) + " in constelation ";
    OBJECT_DETAILS += custom_Array[index_].substring(i3 + 1, i4) + ", with visible magnitude of ";
    OBJECT_DETAILS += custom_Array[index_].substring(i5 + 1, i6) + " and size of ";
    OBJECT_DETAILS += custom_Array[index_].substring(i6 + 1, i7);
    OBJECT_DETAILS += "\n" + custom_Array[index_].substring(i8 + 1, custom_Array[index_].length() - 1);
  }
}

void Sidereal_rate(){
  // when a manual movement of the drive happens. - This will avoid moving the stepepers with a wrong Step Mode.
  if ((IS_MANUAL_MOVE == false) && (IS_TRACKING) && (IS_STEPPERS_ON)){
    if (RA_mode_steps != MICROSteps){
        setmStepsMode("R",MICROSteps);
    }
    digitalWrite(RA_DIR,STP_BACK);
    PIOC->PIO_SODR=(1u<<26);
    delayMicroseconds(2);
    PIOC->PIO_CODR=(1u<<26);
//    digitalWrite(RA_STP,HIGH);
    RA_microSteps += 1;
//    digitalWrite(RA_STP,LOW);
  }
}

void cosiderSlewTo(){
//  int RA_microSteps, DEC_microSteps;
//  int SLEW_RA_microsteps, SLEW_DEC_microsteps;
//  INT data type -> -2,147,483,648 to 2,147,483,647 
//  for more details see the XLS file with calculations
//...

    float HAH;
    float HAM;
    float DECD;
    float DECM;
    double HA_decimal, DEC_decimal;

    if (HAHour >= 12){
        HAH = HAHour - 12;
        HAM = HAMin;
        IS_MERIDIAN_PASSED = false;
    }else{
        HAH = HAHour;
        HAM = HAMin;
        IS_MERIDIAN_PASSED = true;
    }

    //  ADD Correction for RA && DEC according to the Star Alignment
    HA_decimal = ((HAH+(HAM/60))*15) + delta_a_RA;   // In degrees - decimal
    DEC_decimal = OBJECT_DEC_D + (OBJECT_DEC_M/60) + delta_a_DEC; //I n degrees - decimal   

    SLEW_RA_microsteps  = HA_decimal * HA_H_CONST;     // Hardware Specific Code  
    SLEW_DEC_microsteps = DEC_90 - (DEC_decimal * DEC_D_CONST);    // Hardware specific code

    if(IS_MERIDIAN_PASSED == true){
        SLEW_DEC_microsteps*= -1;
    }

// If Home Position selected .... Make sure it goes to 0.

// DO I REALLY NEED THIS.... ????
// CONSIDER THE CODE WHEN YOU HAVE TIME!!!
    int home_pos = 0;
    if ((OBJECT_RA_H == 12) && (OBJECT_RA_M == 0) && (OBJECT_DEC_D == 90) && (OBJECT_DEC_M == 0)){
        SLEW_RA_microsteps = RA_90;
        SLEW_DEC_microsteps = 0;
        home_pos = 1;
    }

// Make the motors START slow and then speed-up - using the microsteps!
// Speed goes UP in 2.2 sec....then ..... FULL Speed ..... then....Speed goes Down for 3/4 Revolution of the drive  
int delta_DEC_time = millis() - Slew_timer;
int delta_RA_timer = millis() - Slew_RA_timer;


    if (delta_DEC_time >= 0 && delta_DEC_time < 1800){
      
        if (DEC_mode_steps != 16){
            setmStepsMode("D",16);
        }
    }
    if (delta_DEC_time >= 1800 && delta_DEC_time < 2600){
        if (DEC_mode_steps != 8){
            setmStepsMode("D",8);
        }
    }
    if (delta_DEC_time >= 2600 && delta_DEC_time < 3400){
        if (DEC_mode_steps != 4){
            setmStepsMode("D",4);
        }
    }
    if (delta_DEC_time >= 3400){
         if (DEC_mode_steps != 2){
            setmStepsMode("D",2);
        }
    }



    if (delta_RA_timer >= 0 && delta_RA_timer < 1800){
        if (RA_mode_steps != 16){
            setmStepsMode("R",16);
        }
    }
    if (delta_RA_timer >= 1800 && delta_RA_timer < 2600){
        if (RA_mode_steps != 8){
            setmStepsMode("R",8);
        }
    }
    if (delta_RA_timer >= 2600 && delta_RA_timer < 3400){
        if (RA_mode_steps != 4){
            setmStepsMode("R",4);
        }
    }
    if (delta_RA_timer >= 3400){
        if (RA_mode_steps != 2){
            setmStepsMode("R",2);
        }
    }

    int delta_RA_steps = SLEW_RA_microsteps - RA_microSteps;
    int delta_DEC_steps = SLEW_DEC_microsteps - DEC_microSteps;

// Make the motors SLOW DOWN and then STOP - using the microsteps!
// Speed goes DOWN in 2.2 sec....then ..... FULL Speed ..... then....Speed goes Down for 3/4 Revolution of the drive  

    if ((abs(delta_DEC_steps) >= 1200) && (abs(delta_DEC_steps) <= 3000)){ 
        if (DEC_mode_steps != 8){
            setmStepsMode("D",8);
        }
    }
    if ((abs(delta_DEC_steps) < 3000)){ 
        if (DEC_mode_steps != 16){
            setmStepsMode("D",16);
        }
    }
    if ((abs(delta_RA_steps) >= 1200) && (abs(delta_RA_steps) <= 3000)){
        if (RA_mode_steps != 8){
            setmStepsMode("R",8);
        }
    }
    if (abs(delta_RA_steps) < 3000){
        if (RA_mode_steps != 16){
            setmStepsMode("R",16);
            RA_move_ending = 1;
        }
    }

    // Taking care of the RA Slew_To.... and make sure it ends Last
    // NB: This way we can jump to TRACK and be sure the RA is on target   
    if(abs(delta_RA_steps)>= abs(delta_DEC_steps)){
        if (RA_finish_last == 0){
          RA_finish_last = 1;
          Slew_RA_timer = millis();
        }
    }


// RA_STP, HIGH - PIOC->PIO_SODR=(1u<<26)
// RA_STP, LOW - PIOC->PIO_CODR=(1u<<26)
// DEC_STP, HIGH - PIOC->PIO_SODR=(1u<<24)
// DEC_STP, LOW - PIOC->PIO_CODR=(1u<<24)

    if ((IS_OBJECT_RA_FOUND == false) && (RA_finish_last == 1)){    
            if (SLEW_RA_microsteps >= (RA_microSteps-RA_mode_steps) && SLEW_RA_microsteps <= (RA_microSteps+RA_mode_steps)){
               IS_OBJECT_RA_FOUND = true;               
            } else {
                  if (SLEW_RA_microsteps > RA_microSteps){
                      digitalWrite(RA_DIR,STP_BACK);
                      //digitalWrite(RA_STP,HIGH);
                      //digitalWrite(RA_STP,LOW);
                      PIOC->PIO_SODR=(1u<<26);
                      delayMicroseconds(5);
                      PIOC->PIO_CODR=(1u<<26);
                      RA_microSteps += RA_mode_steps;
                  }else{
                      digitalWrite(RA_DIR,STP_FWD);
                      //digitalWrite(RA_STP,HIGH);
                      //digitalWrite(RA_STP,LOW);
                      PIOC->PIO_SODR=(1u<<26);
                      delayMicroseconds(5);
                      PIOC->PIO_CODR=(1u<<26);
                      RA_microSteps -= RA_mode_steps;
                  } 
            }
    }

    // Taking care of the DEC Slew_To....  
    if (IS_OBJECT_DEC_FOUND == false){    
        if (SLEW_DEC_microsteps >= (DEC_microSteps-DEC_mode_steps) && SLEW_DEC_microsteps <= (DEC_microSteps+DEC_mode_steps)){
              IS_OBJECT_DEC_FOUND = true;
        } else {
              if (SLEW_DEC_microsteps > DEC_microSteps){
                  digitalWrite(DEC_DIR,STP_BACK);
                  //digitalWrite(DEC_STP,HIGH);
                  //digitalWrite(DEC_STP,LOW);
                  PIOC->PIO_SODR=(1u<<24);
                  delayMicroseconds(5);
                  PIOC->PIO_CODR=(1u<<24);
                  DEC_microSteps += DEC_mode_steps;
              }else{
                  digitalWrite(DEC_DIR,STP_FWD);
                  //digitalWrite(DEC_STP,HIGH);
                  //digitalWrite(DEC_STP,LOW);
                  PIOC->PIO_SODR=(1u<<24);
                  delayMicroseconds(5);
                  PIOC->PIO_CODR=(1u<<24);
                  DEC_microSteps -= DEC_mode_steps;
              } 
        }
    }

    // Check if Object is found on both Axes...
    if (IS_OBJECT_RA_FOUND == true && IS_OBJECT_DEC_FOUND == true){
      IS_OBJ_FOUND = true;
      RA_move_ending = 0;

      if ((home_pos == 0 ) && (ALT > 0)){
          IS_TRACKING = true;
          setmStepsMode("R",MICROSteps);
          if (Tracking_type == 1){  // 1: Sidereal, 2: Solar, 0: Lunar;
              Timer3.start(Clock_Sidereal);
          }else if (Tracking_type == 2){
              Timer3.start(Clock_Solar);
          }else if (Tracking_type == 0){
              Timer3.start(Clock_Lunar);            
          }
      }
      if (IS_SOUND_ON){
       SoundOn(note_C,64);
      }
      Slew_RA_timer = 0;
      RA_finish_last = 0;
      if (IS_BT_MODE_ON == true){
         Serial3.println("Slew done! Object in scope!");
      }
      if (IS_IN_OPERATION == true){
        drawMainScreen();
      }else{
        drawConstelationScreen(SELECTED_STAR);
      }
    }
}

void consider_Manual_Move(int xP, int yP) {
  if ((xP > 0) && (xP <= 150)) {
    setmStepsMode("R", 1);
    digitalWrite(RA_DIR, STP_BACK);
    digitalWrite(RA_STP, HIGH);
    digitalWrite(RA_STP, LOW);
    RA_microSteps += RA_mode_steps;
  } else if ((xP > 150) && (xP <= 320)) {
    setmStepsMode("R", 4);
    digitalWrite(RA_DIR, STP_BACK);
    digitalWrite(RA_STP, HIGH);
    digitalWrite(RA_STP, LOW);
    RA_microSteps += RA_mode_steps;
  } else if ((xP > 320) && (xP <= 470)) {
    setmStepsMode("R", 8);
    digitalWrite(RA_DIR, STP_BACK);
    digitalWrite(RA_STP, HIGH);
    digitalWrite(RA_STP, LOW);
    RA_microSteps += RA_mode_steps;
  } else if ((xP > 620) && (xP <= 770)) {
    setmStepsMode("R", 8);
    digitalWrite(RA_DIR, STP_FWD);
    digitalWrite(RA_STP, HIGH);
    digitalWrite(RA_STP, LOW);
    RA_microSteps -= RA_mode_steps;
  } else if ((xP > 770) && (xP <= 870)) {
    setmStepsMode("R", 4);
    digitalWrite(RA_DIR, STP_FWD);
    digitalWrite(RA_STP, HIGH);
    digitalWrite(RA_STP, LOW);
    RA_microSteps -= RA_mode_steps;
  } else if ((xP > 870) && (xP <= 1023)) {
    setmStepsMode("R", 1);
    digitalWrite(RA_DIR, STP_FWD);
    digitalWrite(RA_STP, HIGH);
    digitalWrite(RA_STP, LOW);
    RA_microSteps -= RA_mode_steps;
  }

  if ((yP > 0) && (yP <= 150)) {
    setmStepsMode("D", 1);
    digitalWrite(DEC_DIR, STP_BACK);
    digitalWrite(DEC_STP, HIGH);
    digitalWrite(DEC_STP, LOW);
    DEC_microSteps += DEC_mode_steps;
  } else if ((yP > 150) && (yP <= 320)) {
    setmStepsMode("D", 4);
    digitalWrite(DEC_DIR, STP_BACK);
    digitalWrite(DEC_STP, HIGH);
    digitalWrite(DEC_STP, LOW);
    DEC_microSteps += DEC_mode_steps;
  } else if ((yP > 320) && (yP <= 470)) {
    setmStepsMode("D", 8);
    digitalWrite(DEC_DIR, STP_BACK);
    digitalWrite(DEC_STP, HIGH);
    digitalWrite(DEC_STP, LOW);
    DEC_microSteps += DEC_mode_steps;
  } else if ((yP > 620) && (yP <= 770)) {
    setmStepsMode("D", 8);
    digitalWrite(DEC_DIR, STP_FWD);
    digitalWrite(DEC_STP, HIGH);
    digitalWrite(DEC_STP, LOW);
    DEC_microSteps -= DEC_mode_steps;
  } else if ((yP > 770) && (yP <= 870)) {
    setmStepsMode("D", 4);
    digitalWrite(DEC_DIR, STP_FWD);
    digitalWrite(DEC_STP, HIGH);
    digitalWrite(DEC_STP, LOW);
    DEC_microSteps -= DEC_mode_steps;
  } else if ((yP > 870) && (yP <= 1023)) {
    setmStepsMode("D", 1);
    digitalWrite(DEC_DIR, STP_FWD);
    digitalWrite(DEC_STP, HIGH);
    digitalWrite(DEC_STP, LOW);
    DEC_microSteps -= DEC_mode_steps;
  }
  delayMicroseconds(1500);
}

// Keep the GPS sensor "fed" until we find the data.
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  if (IS_SOUND_ON) {
    SoundOn(note_c, 8);
  }
  do
  {
    while (Serial2.available())
      gps.encode(Serial2.read());
  } while (millis() - start < ms);
}

void setmStepsMode(char* P, int mod) {
  // P means the axis: RA or DEC; mod means MicroSteppping mode: x32, x16, x8....
  // setmStepsMode(R,2) - means RA with 1/2 steps; setmStepsMode(R,4) - means RA with 1/4 steps


  // PINS Mapping for fast switching
  // DEC_M2 - Pin 8 UP - PC22 - PIOC->PIO_SODR=(1u<<22);
  // DEC_M1 - Pin 9 UP - PC21 -  PIOC->PIO_SODR=(1u<<21);
  // DEC_M0 - Pin 10 UP - PC29 -  PIOC->PIO_SODR=(1u<<29);
  // RA_M0 - Pin 11 UP - PD7 -  PIOD->PIO_SODR=(1u<<7);
  // RA_M1 - Pin 12 UP - PD8 -  PIOD->PIO_SODR=(1u<<8);
  // RA_M2 - Pin 13 UP - PB27 -  PIOB->PIO_SODR=(1u<<27);
  // DEC_M2 - Pin 8 DOWN - PC22 - PIOC->PIO_CODR=(1u<<22);
  // DEC_M1 - Pin 9 DOWN - PC21 -  PIOC->PIO_CODR=(1u<<21);
  // DEC_M0 - Pin 10 DOWN - PC29 -  PIOC->PIO_CODR=(1u<<29);
  // RA_M0 - Pin 11 DOWN - PD7 -  PIOD->PIO_CODR=(1u<<7);
  // RA_M1 - Pin 12 DOWN - PD8 -  PIOD->PIO_CODR=(1u<<8);
  // RA_M2 - Pin 13 DOWN - PB27 -  PIOB->PIO_CODR=(1u<<27);
  //
  // PIOC->PIO_SODR=(1u<<25); // Set Pin High
  // PIOC->PIO_CODR=(1u<<25); // Set Pin Low

  if (P == "R") { // Set RA modes
    if (mod == 1) {                     // Full Step
      //digitalWrite(RA_MODE0, LOW);
      //digitalWrite(RA_MODE1, LOW);
      //digitalWrite(RA_MODE2, LOW);
      PIOD->PIO_CODR = (1u << 7);
      PIOD->PIO_CODR = (1u << 8);
      PIOB->PIO_CODR = (1u << 27);
    }
    if (mod == 2) {                     // 1/2 Step
      //digitalWrite(RA_MODE0, HIGH);
      //digitalWrite(RA_MODE1, LOW);
      //digitalWrite(RA_MODE2, LOW);
      PIOD->PIO_SODR = (1u << 7);
      PIOD->PIO_CODR = (1u << 8);
      PIOB->PIO_CODR = (1u << 27);
    }
    if (mod == 4) {                     // 1/4 Step
      //digitalWrite(RA_MODE0, LOW);
      //digitalWrite(RA_MODE1, HIGH);
      //digitalWrite(RA_MODE2, LOW);
      PIOD->PIO_CODR = (1u << 7);
      PIOD->PIO_SODR = (1u << 8);
      PIOB->PIO_CODR = (1u << 27);
    }
    if (mod == 8) {                     // 1/8 Step
      //digitalWrite(RA_MODE0, HIGH);
      //digitalWrite(RA_MODE1, HIGH);
      //digitalWrite(RA_MODE2, LOW);
      PIOD->PIO_SODR = (1u << 7);
      PIOD->PIO_SODR = (1u << 8);
      PIOB->PIO_CODR = (1u << 27);
    }
    if (mod == 16) {                     // 1/16 Step
      //digitalWrite(RA_MODE0, LOW);
      //digitalWrite(RA_MODE1, LOW);
      //digitalWrite(RA_MODE2, HIGH);
      PIOD->PIO_CODR = (1u << 7);
      PIOD->PIO_CODR = (1u << 8);
      PIOB->PIO_SODR = (1u << 27);
    }
    if (mod == 32) {                     // 1/32 Step
      //digitalWrite(RA_MODE0, HIGH);
      //digitalWrite(RA_MODE1, LOW);
      //digitalWrite(RA_MODE2, HIGH);
      PIOD->PIO_SODR = (1u << 7);
      PIOD->PIO_CODR = (1u << 8);
      PIOB->PIO_SODR = (1u << 27);
    }
    RA_mode_steps = MICROSteps / mod;
  }
  if (P == "D") { // Set RA modes
    if (mod == 1) {                     // Full Step
      //digitalWrite(DEC_MODE0, LOW);
      //digitalWrite(DEC_MODE1, LOW);
      //digitalWrite(DEC_MODE2, LOW);
      PIOC->PIO_CODR = (1u << 29);
      PIOC->PIO_CODR = (1u << 21);
      PIOC->PIO_CODR = (1u << 22);
    }
    if (mod == 2) {                     // 1/2 Step
      //digitalWrite(DEC_MODE0, HIGH);
      //digitalWrite(DEC_MODE1, LOW);
      //digitalWrite(DEC_MODE2, LOW);
      PIOC->PIO_SODR = (1u << 29);
      PIOC->PIO_CODR = (1u << 21);
      PIOC->PIO_CODR = (1u << 22);
    }
    if (mod == 4) {                     // 1/4 Step
      //digitalWrite(DEC_MODE0, LOW);
      //digitalWrite(DEC_MODE1, HIGH);
      //digitalWrite(DEC_MODE2, LOW);
      PIOC->PIO_CODR = (1u << 29);
      PIOC->PIO_SODR = (1u << 21);
      PIOC->PIO_CODR = (1u << 22);
    }
    if (mod == 8) {                     // 1/8 Step
      //digitalWrite(DEC_MODE0, HIGH);
      //digitalWrite(DEC_MODE1, HIGH);
      //digitalWrite(DEC_MODE2, LOW);
      PIOC->PIO_SODR = (1u << 29);
      PIOC->PIO_SODR = (1u << 21);
      PIOC->PIO_CODR = (1u << 22);
    }
    if (mod == 16) {                     // 1/16 Step
      //digitalWrite(DEC_MODE0, LOW);
      //digitalWrite(DEC_MODE1, LOW);
      //digitalWrite(DEC_MODE2, HIGH);
      PIOC->PIO_CODR = (1u << 29);
      PIOC->PIO_CODR = (1u << 21);
      PIOC->PIO_SODR = (1u << 22);
    }
    if (mod == 32) {                     // 1/32 Step
      //digitalWrite(DEC_MODE0, HIGH);
      //digitalWrite(DEC_MODE1, LOW);
      //digitalWrite(DEC_MODE2, HIGH);
      PIOC->PIO_SODR = (1u << 29);
      PIOC->PIO_CODR = (1u << 21);
      PIOC->PIO_SODR = (1u << 22);
    }
    DEC_mode_steps = MICROSteps / mod;
  }
  delayMicroseconds(5);   // Makes sure the DRV8825 can follow
}

void SoundOn(int note, int duration) {
  duration *= 10000;
  long elapsed_time = 0;
  while (elapsed_time < duration) {
    digitalWrite(speakerOut, HIGH);
    delayMicroseconds(note / 2);
    // DOWN
    digitalWrite(speakerOut, LOW);
    delayMicroseconds(note / 2);
    // Keep track of how long we pulsed
    elapsed_time += (note);
  }
}

void UpdateObservedObjects() {
  // Write down the Observed objects information: --- USED in the STATS screen and sent to BT as status.
  int Delta_Time = (((String(rtc.getTimeStr()).substring(0, 2).toInt()) * 60)  + (String(rtc.getTimeStr()).substring(3, 5).toInt())) - ((Prev_Obj_Start.substring(0, 2).toInt() * 60) + Prev_Obj_Start.substring(3).toInt());
  if (Delta_Time < 0) {
    Delta_Time += 1440;
  }
  ObservedObjects[Observed_Obj_Count - 1] += ";" + String(Delta_Time);
  ObservedObjects[Observed_Obj_Count] = OBJECT_NAME + ";" + OBJECT_DETAILS + ";" + String(rtc.getTimeStr()).substring(0, 5) + ";" + int(_temp) + ";" + int(_humid) + ";" + int(HAHour) + "h " + HAMin + "m;" + int(ALT);
  Observed_Obj_Count += 1;
  Prev_Obj_Start = String(rtc.getTimeStr()).substring(0, 5);
}

void Current_RA_DEC() {
  //curr_RA_H, curr_RA_M, curr_RA_S, curr_DEC_D, curr_DEC_M, curr_DEC_S;
  // curr_RA_lz, curr_DEC_lz, curr_HA_lz;
  // DEC

  // To ALSO correct for the Star Alignment offset
  float tmp_dec = (float(DEC_90) - float(abs(DEC_microSteps))) / float(DEC_D_CONST);
  tmp_dec -= delta_a_DEC;
  int sDEC_tel = 0;
  if (tmp_dec < 0) {
    sDEC_tel = 45;
  } else {
    sDEC_tel = 43;
  }
  if (tmp_dec > 0) {
    curr_DEC_D = floor(tmp_dec);
  } else {
    curr_DEC_D = ceil(tmp_dec);
  }
  curr_DEC_M = (tmp_dec - floor(curr_DEC_D)) * 60;
  curr_DEC_S = (curr_DEC_M - floor(curr_DEC_M)) * 60;

  sprintf(curr_DEC_lz, "%c%02d%c%02d:%02d", sDEC_tel, int(abs(curr_DEC_D)), 223, int(abs(curr_DEC_M)), int(curr_DEC_S));

  // HOUR ANGLE
  // To correct for the Star Alignment
  double tmp_ha = double(RA_microSteps) / double(HA_H_CONST);
  tmp_ha -= delta_a_RA;
  if (DEC_microSteps > 0) {
    tmp_ha += 180;
  }
  tmp_ha /= 15;

  float tmp_ha_h = 0;
  float tmp_ha_m = 0;
  float tmp_ha_s = 0;
  tmp_ha_h = floor(tmp_ha);
  tmp_ha_m = (tmp_ha - floor(tmp_ha)) * 60;
  tmp_ha_s = (tmp_ha_m - floor(tmp_ha_m)) * 60;
  sprintf(curr_HA_lz, "%02d:%02d:%02d", int(tmp_ha_h), int(tmp_ha_m), int(tmp_ha_s));

  // RIGHT ASC.
  double tmp_ra = LST - tmp_ha;
  if (LST < tmp_ha) {
    tmp_ra += 24;
  }

  float tmp_ra_h = 0;
  float tmp_ra_m = 0;
  float tmp_ra_s = 0;
  curr_RA_H = floor(tmp_ra);
  curr_RA_M = (tmp_ra - curr_RA_H) * 60;
  curr_RA_S = (curr_RA_M - floor(curr_RA_M)) * 60;
  sprintf(curr_RA_lz, "%02d:%02d:%02d", int(curr_RA_H), int(curr_RA_M), int(curr_RA_S));
}

void DrawButton(int X, int Y, int Width, int Height, String Caption, int16_t BodyColor, int16_t BorderColor, int16_t TextColor, int tSize) {
  //  TYPE: 0:Solid color, no Frame; 1: Frame Only button; 2: Solid color and Frame button;

  if ((BodyColor != 0) && (BorderColor == 0)) {
    // Button Type = 0 ... Solid color, no Frame
    tft.fillRect(X, Y, Width, Height, BodyColor);
  } else if ((BodyColor == 0) && (BorderColor != 0)) {
    // Button Type = 1 ... Frame Only button
    tft.drawRect(X, Y, Width, Height, BorderColor);
    tft.fillRect(X + 1, Y + 1, Width - 2, Height - 2, BLACK);
  } else if ((BodyColor != 0) && (BorderColor != 0)) {
    // Button Type = 1 ... Frame Only button
    tft.drawRect(X, Y, Width, Height, BorderColor);
    tft.fillRect(X + 1, Y + 1, Width - 2, Height - 2, BodyColor);
  } else {
    // Will not Draw Button and will return to code!
    return;
  }

  float TX = 0;
  float TY = 0;
  if (tSize == 2) {      // 10 x 14 px. (W x H)
    TX = (X + 1 + Width / 2) - (Caption.length() * 6);
    TY = Y + Height / 2 - 5;
  } else if (tSize == 1) { // 5 x 7 px. (W x H)
    TX = (X + 1 + Width / 2) - (Caption.length() * 3);
    TY = Y + Height / 2 - 3 ;
  } else if (tSize == 3) { // 15 x 21 px. (W x H)
    TX = (X + 1 + Width / 2) - (Caption.length() * 8);
    TY = Y + Height / 2 - 10;
  }
  tft.setCursor((int)TX, (int)TY);
  tft.setTextSize(tSize);
  tft.setTextColor(TextColor);
  if (Caption == "+") {
    TX -= 5;
    tft.drawLine((int)TX + 10, (int)TY - 5, (int)TX + 10, (int)TY - 5, TextColor);
    tft.drawLine((int)TX + 8, (int)TY - 4, (int)TX + 12, (int)TY - 4, TextColor);
    tft.drawLine((int)TX + 6, (int)TY - 3, (int)TX + 14, (int)TY - 3, TextColor);
    tft.drawLine((int)TX + 4, (int)TY - 2, (int)TX + 16, (int)TY - 2, TextColor);
    tft.drawLine((int)TX + 2, (int)TY - 1, (int)TX + 18, (int)TY - 1, TextColor);
    tft.drawLine((int)TX, (int)TY, (int)TX + 20, (int)TY, TextColor);
    tft.drawLine((int)TX - 2, (int)TY + 1, (int)TX + 22, (int)TY + 1, TextColor);
    tft.drawLine((int)TX - 4, (int)TY + 2, (int)TX + 24, (int)TY + 2, TextColor);

    tft.drawLine((int)TX - 4, (int)TY + 8, (int)TX + 24, (int)TY + 8, TextColor);
    tft.drawLine((int)TX - 2, (int)TY + 9, (int)TX + 22, (int)TY + 9, TextColor);
    tft.drawLine((int)TX, (int)TY + 10, (int)TX + 20, (int)TY + 10, TextColor);
    tft.drawLine((int)TX + 2, (int)TY + 11, (int)TX + 18, (int)TY + 11, TextColor);
    tft.drawLine((int)TX + 4, (int)TY + 12, (int)TX + 16, (int)TY + 12, TextColor);
    tft.drawLine((int)TX + 6, (int)TY + 13, (int)TX + 14, (int)TY + 13, TextColor);
    tft.drawLine((int)TX + 8, (int)TY + 14, (int)TX + 12, (int)TY + 14, TextColor);
    tft.drawLine((int)TX + 10, (int)TY + 15, (int)TX + 10, (int)TY + 15, TextColor);
  } else {
    tft.println(Caption);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BUFFPIXEL 80

void bmpDraw(char *filename, uint8_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ((x >= tft.width()) || (y >= tft.height())) return;
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL)
  {
#ifdef serial_debug
    Serial.println("file not found on the SD card");
#endif

    return;
  }
  // Parse BMP header
  if (read16(bmpFile) == 0x4D42 || read16(bmpFile) == 0x424D) // BMP signature
  {
#ifdef serial_debug
    Serial.print("BMP file found.\nImage offset: ");
    Serial.println(read32(bmpFile), HEX);
#else
    read32(bmpFile); //dummy read
#endif
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data

    // Read DIB header
#ifdef serial_debug
    Serial.print("Image DIB header: ");
    Serial.println(read32(bmpFile), HEX);
#else
    read32(bmpFile);  //dummy read
#endif
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) // # planes -- must be '1'
    {
      bmpDepth = read16(bmpFile); // bits per pixel

#ifdef serial_debug
      Serial.println("one plane file");
      Serial.print("bmpDepth: ");
      Serial.println(bmpDepth);
#endif

      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) // 0 = uncompressed
      {
        goodBmp = true; // Supported BMP format -- proceed!
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0)
        {
          bmpHeight = -bmpHeight;
          flip      = false;
        }
        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width())  w = tft.width()  - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

        for (row = 0; row < h; row++) { // For each scanline...

          if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos)
          { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col = 0; col < w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r, g, b));
          } // end pixel
        } // end scanline
      } // end goodBmp
    }
  }
  else
  {
#ifdef serial_debug
    Serial.println("invalid BMP file signature");
#endif
  }
  bmpFile.close();
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

/*
   isSummerTime() : uses Time.h
   ---------

   This function versifies if the date time needs to be updated by adding 1h due to summer-time

   Return: true if in summer-time, false otherwise
*/

bool isSummerTime() //in Italy: Summer time ends the last sunday of october and begins the last of march
{
  bool summer_time = false;

#ifdef serial_debug
  Serial.print(day());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(year());
  Serial.print("  weekday: ");
  Serial.println(weekday());
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
#endif

  // If I'm in October
  if (month() == 10)
  {
    // If it's Sunday
    if (weekday() == 1)
    {
      if (day() + 7 > 31 && hour() >= 3) summer_time = false;
      else summer_time = true;
    }
    else
    {
      // If last sunday has passed
      if ((day() + 7 - (weekday() - 1)) > 31) summer_time = false;
      else  summer_time = true;
    }
  }
  // If I'm in March
  else if (month() == 3)
  {
    // If It's Sunday
    if (weekday() == 1)
    {
      // If It is the Last Sunday
      if ((day() + 7) > 31 && hour() >= 2) summer_time = true;
      else summer_time = false;
    }
    else
    {
      // If it's not Sunday,but the last has already passed
      if ((day() + 7 - (weekday() - 1)) > 31) summer_time = true;
      else  summer_time = false;
    }
  }
  // If the Day Light Saving Time
  else if (month() >= 4 && month() <= 9) summer_time = true;
  // If we are in the Winter Time Period
  else if ((month() >= 1 && month() <= 2) || (month() >= 11 && month() <= 12)) summer_time = false;

  return summer_time;
}

// Performs ~310.000 readings to find the mean value of the joypad (error: 3*10^-4 % )
void calibrateJoypad(int *x_cal, int *y_cal)
{
  tft.println("\n");
  tft.setTextColor(title_bg);
  tft.println("Please avoid touching the Joypad for the next 3 seconds to let the software calibrate.\n");
  Serial1.print("Joy_warn.txt=");
  Serial1.print("\"");
  Serial1.print("> Please, avoid touching the Joypad for the next three        seconds to let the software calibrate...");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  tft.setTextColor(l_text);

  int now_t = millis();
  int prev_t = millis();
  int c = 3;

  tft.print("3.. ");
  while (millis() < now_t + 3000)
  {
    if (millis() > prev_t + 1000)
    {
      c--;
      tft.print(c);
      tft.print(".. ");
      Serial1.print("Joy_cal_3.txt=");
      Serial1.print("\"");
      Serial1.print("-> 3....");
      Serial1.print("\"");
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      delay(1000);
      Serial1.print("Joy_cal_2.txt=");
      Serial1.print("\"");
      Serial1.print("2....");
      Serial1.print("\"");
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      delay(1000);
      Serial1.print("Joy_cal_1.txt=");
      Serial1.print("\"");
      Serial1.print("1....");
      Serial1.print("\"");
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      prev_t = millis();
    }
    *x_cal = (*x_cal + analogRead(A0)) / 2;
    *y_cal = (*y_cal + analogRead(A1)) / 2;
  }

  tft.setTextColor(GREEN);
  tft.print("done!");
  Serial1.print("Joy_cal_Done.txt=");
  Serial1.print("\"");
  Serial1.print("Done");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  tft.setTextColor(l_text);
#ifdef serial_debug
  Serial.print("x_cal: ");
  Serial.println(*x_cal);
  Serial.print("y_cal: ");
  Serial.println(*y_cal);
#endif
}

void storeOptions_SD()
{
  SD.remove("options.txt");
  File dataFile = SD.open("options.txt", FILE_WRITE);

  if (dataFile)
  {
    String options =                       "(" +
                                           (String)TFT_Brightness             + ";" +
                                           TFT_Time                   + ";" +
                                           (String)Tracking_type              + ";" +
                                           (String)IS_MERIDIAN_FLIP_AUTOMATIC + ";" +
                                           Fan1_State                 + ";" +
                                           Fan2_State                 + ";" +
                                           (String)IS_SOUND_ON                + ";" +
                                           (String)IS_STEPPERS_ON             + ")" ;

    dataFile.print(options);
    dataFile.close();
  }
}

void loadOptions_SD()
{
  File dataFile = SD.open("options.txt");

  if (dataFile)
  {
    char optionsBuffer[100] = {""};
    dataFile.read(optionsBuffer, 100);
    String receivedString = optionsBuffer;

#ifdef serial_debug
    Serial.print("options:");
    Serial.println(optionsBuffer);
#endif

    int iniPac = receivedString.indexOf('(');
    int endPac = receivedString.indexOf(')');

    if (iniPac != -1 && endPac != -1 && endPac - iniPac > 1)
    {
      String packetIn = optionsBuffer;
      packetIn = packetIn.substring(1, endPac); //tolgo le parentesi
      String valoriIn[8] = {""};

#ifdef serial_debug
      Serial.print("packetIn:");
      Serial.println(packetIn);
#endif

      for (int i = 0; i < sizeof(valoriIn) / sizeof(valoriIn[0]) - 1; i++) //genero le sottostringhe
      {
        int endVal  = packetIn.indexOf(";");
        valoriIn[i] = packetIn.substring(0, endVal);
#ifdef serial_debug
        Serial.print("valoriIn[");
        Serial.print(i);
        Serial.print("] :");
        Serial.println(valoriIn[i]);
#endif
        packetIn    = packetIn.substring(endVal + 1, packetIn.length());
      }
      valoriIn[sizeof(valoriIn) / sizeof(valoriIn[0]) - 1] = packetIn;

      //Update:
      TFT_Brightness             = valoriIn[0].toInt();
      TFT_Time                   = valoriIn[1];
      Tracking_type              = valoriIn[2].toInt();
      IS_MERIDIAN_FLIP_AUTOMATIC = valoriIn[3].toInt();
      Fan1_State                 = valoriIn[4];
      Fan2_State                 = valoriIn[5];
      IS_SOUND_ON                = valoriIn[6].toInt();
      IS_STEPPERS_ON             = valoriIn[7].toInt();

      analogWrite(TFTBright, TFT_Brightness);

      if (Tracking_type == 0) Tracking_Mode = "Lunar";
      else if (Tracking_type == 2) Tracking_Mode = "Solar";
      else
      {
        Tracking_type = 1;
        Tracking_Mode = "Celest";
      }

      if      (TFT_Time.equals("30 s"  )) TFT_timeout = 30000;
      else if (TFT_Time.equals("60 s"  )) TFT_timeout = 60000;
      else if (TFT_Time.equals("2 min" )) TFT_timeout = 120000;
      else if (TFT_Time.equals("5 min" )) TFT_timeout = 300000;
      else if (TFT_Time.equals("10 min")) TFT_timeout = 600000;
      else
      {
        TFT_Time = "AL-ON";
        TFT_timeout = 0;
      }

#ifdef serial_debug
      Serial.print("TFT_timeout = ");
      Serial.println(TFT_timeout);
#endif

      if (IS_MERIDIAN_FLIP_AUTOMATIC) Mer_Flip_State = "AUTO";
      else
      {
        IS_MERIDIAN_FLIP_AUTOMATIC = false;
        Mer_Flip_State = "OFF";
      }

      if (Fan1_State.equals("OFF"))
        IS_FAN1_ON = false;
      else
      {
        Fan1_State = "ON";
        IS_FAN1_ON = true;
      }
      if (Fan2_State.equals("OFF"))
        IS_FAN2_ON = false;
      else
      {
        Fan2_State = "ON";
        IS_FAN2_ON = true;
      }

      if (IS_SOUND_ON)
      {
        Sound_State = "ON";
      }
      else
      {
        IS_SOUND_ON = false;
        Sound_State = "OFF";
      }

      if (IS_STEPPERS_ON)
        Stepper_State = "ON";
      else
      {
        IS_STEPPERS_ON = false;
        Stepper_State = "OFF";
      }
    }

    dataFile.close();
  }
}

double J2000(double yy, double m, double dd, double hh, double mm)
{
  double DD = 0; // use 0 for J2000
  double d = (double)367 * yy - (double)7 * ((double)yy + ((double)m + (double)9) / (double)12.0) / (double)4.0 + (double)275 * m / (double)9.0 + DD + (double)dd - (double)730531; // days since J2000
  d = floor(d) + hh / 24.0 + mm / (24.0 * 60.0);
  return d;
}

double ipart(double xx)
{
  double sgn;

  if (xx < 0)
  {
    sgn = -1.0;
  }

  else if (xx == 0)
  {
    sgn = 0.0;
  }

  else if (xx > 0)
  {
    sgn = 1.0;
  }
  double ret = sgn * ((int)fabs(xx));

  return ret;
}


double FNdegmin(double xx)
{
  double a = ipart(xx) ;
  double b = xx - a ;
  double e = ipart(60 * b) ;
  //   deal with carry on minutes
  if ( e >= 60 )
  {
    e = 0 ;
    a = a + 1 ;
  }
  return (a + (e / 100) );
}

double dayno(int dx, int mx, int yx, double fx)
{
  dno = (367 * yx) -  (int)(7 * (yx + (int)((mx + 9) / 12)) / 4) + (int)(275 * mx / 9) + dx - 730531.5 + fx;
  dno -= 4975.5;
  return dno;
}

double frange(double x)
{
  x = x / (2 * M_PI);
  x = (2 * M_PI) * (x - ipart(x));
  if (x < 0) x = x + (2 * M_PI);
  return x;
}

double fkep( double m, double ecc)
{
  double e = ecc;
  double v = m + (2 * e - 0.25 * pow(e, 3) + 5 / 96 * pow(e, 5)) * sin(m) + (1.25 * pow(e, 2) - 11 / 24 * pow(e, 4)) * sin(2 * m) + (13 / 12 * pow(e, 3) - 43 / 64 * pow(e, 5)) * sin(3 * m) + 103 / 96 * pow(e, 4) * sin(4 * m) + 1097 / 960 * pow(e, 5) * sin(5 * m);

  if (v < 0)  v = v + (2 * PI);
  return v;
}

double fnatan(double x, double y)
{
  double a = atan(y / x);
  if (x < 0)
    a = a + PI;
  if ((y < 0) && (x > 0))
    a = a + (2 * PI);
  return a;
}

double JulianDay (int j_date, int j_month, int j_year, double UT)
{
  if (j_month <= 2) {
    j_month = j_month + 12;
    j_year = j_year - 1;
  }
  return (int)(365.25 * j_year) + (int)(30.6001 * (j_month + 1)) - 15 + 1720996.5 + j_date + UT / 24.0;
}

void drawBatteryLevel(int x, int y, int level)
{
  tft.drawRect(x,    y,   43, 22, BLACK);
  tft.drawRect(x + 41, y + 6,  4,  10, title_bg); // Battery Tip

  tft.fillRect(x + 1, y + 1, 40, 20, title_bg); //background for empty battery >> Orange

  if (level == -1)  //USB as power supply
  {
    tft.setCursor(x + 12, y + 8); // 12 & 8 are accurate
    tft.setTextSize(1);
    tft.setTextColor(btn_d_text);
    tft.print("USB");
  }
  else if (level > 20)  tft.fillRect(x + 2, y + 2, round((double)level / 5) - 1, 8, BLACK);
  else if (IS_NIGHTMODE)  tft.fillRect(x + 2, y + 2, round((double)level / 5) - 1, 8, btn_d_text);
  else              tft.fillRect(x + 2, y + 2, round((double)level / 5), 8, RED);
}

int calculateBatteryLevel()
{
  int voltageLevel = 0;
  int pmin = -1;

#ifdef use_battery_level
  analogReadResolution(12);
  voltageLevel = analogRead(A2);
  voltageLevel += 270;  //To compensate OPAMP OFFSET (do I need to use an automatic routine??)
  analogReadResolution(10);

  /*
     Since SLA batteries have very non linear voltage to capacity response I'm approximating the function that describes the discharge of
     such batteries. To do so I have:
     1 - Measured battery voltages on known capacities.
     2 - Collected data on a table.
     3 - Find the best curve function that fit those data: V = a*exp(b*x)+c*exp(d*x), x is the left-charge percentage.
     4 - Solve the equation to find the value x that gives me the battery voltage V. I have percentage p = x.
  */

  // Data from SLA curve fitting.. Vedi file di Numbers: battery_calc.numbers
  double a = 3350;
  double b = 0.001042;
  double c = -271.6;
  double d = -0.129;

  double minim = 100;

  // This is a very simple way to solve the following equation: a*exp(b*x)+c*exp(d*x) since it can't be solved analitically.
  // Since I will probably have a fully charged battery most of the times I start solving the equations from 100% battery level.
  for (int p = 100; p > 0; p--)
  {
    double vq = a * exp(b * p) + c * exp(d * p);
    int val = abs(vq - voltageLevel);
    if (val < 15)  break; // Is 15 a good threshold? Decrese to increase accuracy (may fail?)
    else if (val < minim)
    {
      minim = val;
      pmin = p;
    }
  }

#ifdef serial_debug
  Serial.print("batteryLevel = ");
  Serial.print(pmin);
  Serial.println(" %");
#endif
#endif
  /*
    double voltageLevel = 0;
    #ifdef use_battery_level
    analogReadResolution(12);
    voltageLevel = analogRead(A2);
    #ifdef serial_debug
    Serial.print("voltageLevel = ");
    Serial.println(voltageLevel);
    #endif
    //voltageLevel *= 0.0243956;
    voltageLevel = (voltageLevel * 0.07326007326 - 200);
    #ifdef serial_debug
    Serial.print("voltageLevel1 = ");
    Serial.println(voltageLevel);
    #endif
    if(voltageLevel > 100)      voltageLevel = 100;
    else if (voltageLevel < 0)
    {
    //I'm using USB to power device
    voltageLevel = -1;
    }
    #endif */
  return pmin;
}
