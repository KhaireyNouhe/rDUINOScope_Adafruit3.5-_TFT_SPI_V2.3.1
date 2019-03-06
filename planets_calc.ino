/*
   There is some mess with the reference day when calculating coordinates.
   I'm using Julius Day for planets and J2000 reference frame for Moon.
   I will certainly unify the reference day in the future to save space (~33% used) and speed up the calculations.
   Planets calculations are nicely refined and I can get very small errors. Moon's calculation are going to get more and more errors by time.
   In the future I will switch to a different alghoritm for Moon's positioning.

   For help or suggesions -> e-mail: giacomo.mammarella@student.univaq.it
*/

void planet_pos(int pno)
{
  if (pno > 10) pno = 10;
  yy = year();
  mu = month();
  dd = day();
  hh = hour();
  mm = minute();

  OBJECT_NAME = ss_planet_names[pno];

#ifdef serial_debug
  Serial.print("pno = ");
  Serial.println(pno);
  Serial.print(hour());
  Serial.print(":");
  Serial.println(minute());
#endif

  double OBJ_RA = 0;
  double OBJ_DEC = 0;

  dfrac = (hh + (mm / 60)) / 24;
  d = dayno(dd, mu, yy, dfrac);

#ifdef serial_debug
  Serial.print("dayno : ");
  Serial.println(d);
#endif

  earth();  //Calculates the position of the Earth in the coordinate system used

  //Since Earth coordinates have been already calculated I need to calculate some parameter for the requested planet.
  if (pno != 3 && pno != 10)
  {
    TRACKING_MOON = false;
    M[pno] = (n[pno] * rads * (d)) + (L[pno] - pl[pno]) * rads;
    M[pno] = frange(M[pno]);
    v[pno] = fkep(M[pno], e[pno]);

    r[pno] = a[pno] * ((1 - pow(e[pno], 2)) / (1 + (e[pno] * cos(v[pno]))));
    x[pno] = r[pno] * (cos(o[pno] * rads) * cos(v[pno] + pl[pno] * rads - o[pno] * rads) - sin(o[pno] * rads) * sin(v[pno] + pl[pno] * rads - o[pno] * rads) * cos(i[pno] * rads));
    y[pno] = r[pno] * (sin(o[pno] * rads) * cos(v[pno] + pl[pno] * rads - o[pno] * rads) + cos(o[pno] * rads) * sin(v[pno] + pl[pno] * rads - o[pno] * rads) * cos(i[pno] * rads));
    z[pno] = r[pno] * (sin(v[pno] + pl[pno] * rads - o[pno] * rads)) * sin(i[pno] * rads);
    X[pno] = x[pno] - x[3];
    Y[pno] = y[pno] - y[3];
    Z[pno] = z[pno];

    double ec = 23.439292 * rads;
    Xq[pno] = X[pno];
    Yq[pno] = (Y[pno] * cos(ec)) - (Z[pno] * sin(ec));
    Zq[pno] = (Y[pno] * sin(ec)) + (Z[pno] * cos(ec));
    ra[pno] = fnatan(Xq[pno], Yq[pno]);
    dec[pno] = atan(Zq[pno] / sqrt(pow(Xq[pno], 2.0) + pow(Yq[pno], 2.0)));

    OBJ_RA  = FNdegmin((ra[pno] * degs) / 15);
    OBJ_DEC = FNdegmin(dec[pno] * degs);

    bool flag = false;  //I'm using this flag to invert the value of the OBJ_DEC if needed

    OBJECT_RA_H = floor(OBJ_RA);
    OBJECT_RA_M = (OBJ_RA - OBJECT_RA_H) * 100;

    if (OBJ_DEC < 0)
    {
      OBJ_DEC *= -1;
      flag = true;
    }

    OBJECT_DEC_D = floor(OBJ_DEC);
    OBJECT_DEC_M = (OBJ_DEC - OBJECT_DEC_D) * 100;

    if (flag)
    {
      OBJECT_DEC_D *= -1;
      flag = false;
    }

    //Some object details can be added in this code. Remember that pno is the number of the planet starting from 0 (Sun) to 9 (Pluto) with 10 (Moon)
    if (pno == 0)
      OBJECT_DETAILS = "The Sun is the star at the center of the Solar System";
    else
    {
      OBJECT_DETAILS = OBJECT_NAME + " is the " + pno;
      if (pno == 1)
        OBJECT_DETAILS += "st ";
      else if (pno == 2)
        OBJECT_DETAILS += "nd ";
      else
        OBJECT_DETAILS += "th ";
      OBJECT_DETAILS += "planet of our Solar System.";
      if (pno == 9)
        OBJECT_DETAILS += "\nFormally in 2006, during the 26th General Assembly ofInternational Astronomical Union, Pluto has been reclassified as dwarf planet.";
    }

#ifdef serial_debug
    Serial.print(OBJECT_NAME);
    Serial.println(" coordinates: ");
#endif
  }
  //If I selected Moon different calculations need to be done
  else if (pno == 10)
  {
    // CALCOLI PER LA LUNA
    TRACKING_MOON = true;
    double jd = J2000(yy, mu, dd, hh, mm);
    const double T = (jd + 0.5) / (double)36525;
    const double ec = (23.439291 - 0.013004 * T);

    const double T2 = T * T;
    const double T3 = T * T * T;
    const double T4 = T * T * T * T;

    double Lm = 218.3164591  + 481267.88134236 * T - 0.0013268 * T2 + T3 / 538841 + T4 / 65194000;
    while (Lm > 360) Lm -= 360;
    while (Lm < 0) Lm += 360;

    double D  = 297.8502042  + 445267.1115168 * T  - 0.0016300 * T2 + T3 / 545868 - T4 / 113065000;
    while (D > 360)  D -= 360;
    while (D < 0) D += 360;

    double Ms  = 357.5291092 + 35999.0502909 * T   - 0.0001536 * T2 + T3 / 24490000; //Sun's mean anomaly
    while (Ms > 360) Ms -= 360;
    while (Ms < 0) Ms += 360;

    double Mm  = 134.9634114 + 477198.8676313 * T  + 0.0089970 * T2 + T3 / 69699  - T4 / 14712000; //Moon's mean anomaly
    while (Mm > 360) Mm -= 360;
    while (Mm < 0) Mm += 360;

    double F   = 93.2720993 + 483202.0175273 * T   - 0.0034029 * T2 - T3 / 3526000 + T4 / 863310000;
    while (F > 360) F -= 360;
    while (F < 0) F += 360;

#ifdef serial_debug
    Serial.print("jd = ");
    Serial.println(jd);
    Serial.print("T = ");
    Serial.println(T);
    Serial.print("Lm = ");
    Serial.println(Lm);
    Serial.print("D = ");
    Serial.println(D);
    Serial.print("Ms = ");
    Serial.println(Ms);
    Serial.print("Mm = ");
    Serial.println(Mm);
    Serial.print("F = ");
    Serial.println(F);
#endif

    double A1 = 119.75 + 131.849 * T;
    double A2 = 53.09 + 479264.290 * T;
    double A3 = 313.45 + 481266.484 * T;

    double E = 1 - 0.002516 * T - 0.0000074 * T2;

    double sigma1 = 0;
    double sigmar = 0;
    double sigma2 = 0;
    for (int r = 0; r < 13; r++)
    {
      double mult = 1;

      if (abs(tab[r][1]) == 1)
      {
        mult = E;
      }
      else if (abs(tab[r][1]) == 2 )
      {
        mult = E * E;
      }
      else
      {
        mult = 1;
      }

      sigma1 += mult * tab[r][4] * sin((D * tab[r][0] + Ms * tab[r][1] + Mm * tab[r][2] + F * tab[r][3]) * rads);
      sigmar +=        tab[r][5] * cos((D * tab[r][0] + Ms * tab[r][1] + Mm * tab[r][2] + F * tab[r][3]) * rads);
    }
    for (int r = 0; r < 15; r++)
    {
      double mult = 1;

      if (abs(tabb[r][1]) == 1)
      {
        mult = E;
      }
      else if (abs(tabb[r][1]) == 2 )
      {
        mult = E * E;
      }
      else
      {
        mult = 1;
      }
      sigma2 += mult * tabb[r][4] * sin((D * tabb[r][0] + Ms * tabb[r][1] + Mm * tabb[r][2] + F * tabb[r][3]) * rads);
    }

    sigma1 = sigma1 + 3958 * sin(A1 * rads) + 1962 * sin((Lm - F) * rads) + 318 * sin(A2 * rads);
    sigma2 = sigma2 - 2235 * sin(Lm * rads) + 382 * sin(A3 * rads) + 175 * sin((A1 - F) * rads) + 175 * sin((A1 + F) * rads) + 127 * sin ((Lm - Mm) * rads) - 115 * sin((Lm + Mm) * rads);


#ifdef serial_debug
    Serial.print("sigma1 = ");
    Serial.println(sigma1);
    Serial.print("sigma2 = ");
    Serial.println(sigma2);
#endif

    double lambda = Lm + sigma1 / 1e6;
    double beta = sigma2 / 1e6;
    double r = 385000.56 + sigmar / 1000;

#ifdef serial_debug
    Serial.print("lambda = ");
    Serial.println(lambda);
    Serial.print("beta = ");
    Serial.println(beta);
#endif

    double alpha = atan2(sin(lambda * rads) * cos(ec * rads) - tan(beta * rads) * sin(ec * rads), cos(lambda * rads)) * degs;
    double delta = asin((sin(beta * rads)  * cos(ec * rads) + cos(beta * rads) * sin(ec * rads) * sin(lambda * rads))) * degs;

#ifdef serial_debug
    Serial.print("alpha = ");
    Serial.println(alpha);
    Serial.print("delta = ");
    Serial.println(delta);
    Serial.print("arg(asin) = ");
    Serial.println((sin(beta * rads)*cos(ec) + cos(beta * rads)*sin(ec)*sin(lambda * rads)));
    Serial.print("sin(beta*rads) = ");
    Serial.println(sin(beta * rads));
    Serial.print("cos(ec) = ");
    Serial.println(cos(ec));
    Serial.print("cos(beta*rads) = ");
    Serial.println(cos(beta * rads));
    Serial.print("sin(ec) = ");
    Serial.println(sin(ec));
    Serial.print("sin(lambda*rads) = ");
    Serial.println(sin(lambda * rads));
#endif

    alpha /= 15;  //to convert to degs

    OBJECT_RA_H = floor(alpha);
    OBJECT_RA_M = (alpha - floor(alpha)) * 60;
    OBJ_RA = OBJECT_RA_H + OBJECT_RA_M / 100;

    int mult = 1;
    if (delta < 0) mult = -1;
    OBJECT_DEC_D = floor(abs(delta)) * mult;
    OBJECT_DEC_M = (abs(delta) - abs(OBJECT_DEC_D)) * 6;
    OBJ_DEC = mult * (abs(OBJECT_DEC_D) + OBJECT_DEC_M / 100);

    //Calculation of the ILLUMINATED FRACTION OF MOON
    double i = 180 - D - 6.280 * sin(Mm * rads)
               + 2.100 * sin(Ms * rads)
               - 1.274 * sin((2 * D - Mm) * rads)
               - 0.658 * sin(2 * D * rads)
               - 0.214 * sin(2 * Mm * rads)
               - 0.111 * sin(D * rads);

    double k = round((1 + cos(i * rads)) / 2 * 10000) / 100.0;

    OBJECT_DETAILS = "The Moon is the natural satellite of our planet. Right now it is approximately ";
    OBJECT_DETAILS += floor(r);
    OBJECT_DETAILS += " km away from the Earth and the illuminated portion of the disk is equal to ";
    OBJECT_DETAILS += k;
    OBJECT_DETAILS += "%";

#ifdef serial_debug
    Serial.println("MOON coordinates: ");
#endif
  }

  OBJECT_DESCR = "";  //Still not using OBJECT_DESCR for Solar System planets. May be used in some way?

#ifdef serial_debug
  Serial.print("OBJ_RA = ");
  Serial.println(OBJ_RA);
  Serial.print("OBJ_DEC = ");
  Serial.println(OBJ_DEC);
#endif
}

void earth()
{
  M[3] = ((n[3] * rads) * (d)) + (L[3] - pl[3]) * rads;
  M[3] = frange(M[3]);
  v[3] = fkep(M[3], e[3]);
  r[3] = a[3] * ((1 - (pow(e[3], 2))) / (1 + (e[3] * cos(v[3]))));
  x[3] = r[3] * cos(v[3] + pl[3] * rads);
  y[3] = r[3] * sin(v[3] + pl[3] * rads);
  z[3] = 0;
}
