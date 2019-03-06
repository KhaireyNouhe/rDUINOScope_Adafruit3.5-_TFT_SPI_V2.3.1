#define BLACK           0x0000      /*   0,   0,   0 */
#define NAVY            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define GREEN           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define RED             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define YELLOW          0xFFE0      /* 255, 255,   0 */
#define WHITE           0xFFFF      /* 255, 255, 255 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */
#define Pink            0xF81F

#define  note_c     3830    // 261 Hz
#define  note_cb    3610    // 261 Hz
#define  note_d     3400    // 294 Hz
#define  note_e     3038    // 329 Hz
#define  note_f     2864    // 349 Hz
#define  note_g     2550    // 392 Hz
#define  note_gb    2411    // 392 Hz
#define  note_a     2272    // 440 Hz
#define  note_b     2028    // 493 Hz
#define  note_C     1912    // 523 Hz
#define  note_X     1650    //

const String ss_planet_names[11] = {"SUN", "MERCURY", "VENUS", "EARTH", "MARS", "JUPITER", "SATURN", "URANUS", "NEPTUNE", "PLUTO", "MOON"};

double i[10] = {0.0, 7.0052, 3.3949, 0.0, 1.8496, 1.3033, 2.4869, 0.7728, 1.7692, 17.1695};
double o[10] = {0.0, 48.493, 76.804, 0.0, 49.668, 100.629, 113.732, 73.989, 131.946, 110.469};
double pl[10] = {0.0, 77.669, 131.99, 103.147, 336.322, 14.556, 91.500,  169.602, 6.152, 223.486};
double a[10] = {0.0, 0.387098, 0.723327, 1.0000, 1.523762, 5.20245, 9.52450, 19.1882, 29.9987, 39.2766};
double n[10] = {0.0, 4.09235, 1.60215, 0.985611, 0.523998, 0.083099, 0.033551, 0.011733, 0.006002, 0.004006};
double e[10] = {0.0, 0.205645 , 0.006769, 0.016679, 0.093346, 0.048892, 0.055724, 0.047874, 0.009816, 0.246211};
double L[10] = {0.0, 93.8725, 233.5729, 324.5489, 82.9625, 87.9728, 216.6279, 11.9756, 335.0233, 258.8717};
double M[10], v[10], r[10], x[10], y[10], z[10], X[10], Y[10], Z[10], Xq[10], Yq[10], Zq[10], ra[10], dec[10];
double d, dfrac, dno;
int dd, mm, yy, hh, mu;
const double rads = (M_PI / 180.0);
const double degs = 180.0 / (M_PI);

//Don't alter or you're screwed!!
const volatile double tab[13][6] =
{
  {0, 0,  1, 0, 6288774, -20905355},
  {2, 0, -1, 0, 1274027,  -3699111},
  {2, 0,  0, 0,  658314,  -2955968},
  {0, 0,  2, 0,  213618,   -569925},
  {0, 1,  0, 0, -185116,     48888},
  {0, 0,  0, 2, -114332,     -3149},
  {2, 0, -2, 0,  58793,     246158},
  {2, -1, -1, 0,  57066,    -152138},
  {2, 0,  1, 0,  53322,    -170733},
  {2, -1, 0, 0,  45758,    -204586},
  {0, 1, -1, 0, -40923,    -129620},
  {1, 0,  0, 0, -34720,     108743},
  {0, 1,  1, 0, -30383,     104755}
};
const volatile double tabb[15][5]
{
  {0, 0,  0,  1, 5128122},
  {0, 0,  1,  1,  280602},
  {0, 0,  1, -1,  277693},
  {2, 0,  0, -1,  173237},
  {2, 0, -1,  1,   55413},
  {2, 0, -1, -1,   46271},
  {2, 0,  0,  1,   32573},
  {0, 0,  2,  1,   17198},
  {2, 0,  1, -1,    9266},
  {0, 0,  2, -1,    8822},
  {2, -1,  0, -1,    8216},
  {2, 0, -2, -1,    4324},
  {2, 0,  1,  1,    4200},
  {2, 1,  0, -1,   -3359},
  {2, -1, -1,  1,    2463},
};

const String Stars[] =
{
  // Definition of the ALIGNMENT Stars. Add as many stars as you like in here
  "And;Almaak;2h4m;42.3°",
  "And;Alpheratz;0h8m;29.1°",
  "And;Mirach;1h10m;35.6°",
  "Aql;Altair;19h51m;8.9°",
  "Aql;Althalimain;19h6m;-4.9°",
  "Aql;Deneb el Okab;19h5m;13.9°",
  "Aql;Tarazed;19h46m;10.6°",
  "Aqr;Sadalmelik;22h6m;-0.3°",
  "Aqr;Sadalsuud;21h32m;-5.6°",
  "Aqr;Skat;22h55m;-15.8°",
  "Ara;Choo;17h32m;-49.9°",
  "Ari;Hamal;2h7m;23.5°",
  "Ari;Sheratan;1h55m;20.8°",
  "Aur;Almaaz;5h2m;43.8°",
  "Aur;Capella;5h17m;46°",
  "Aur;Hassaleh;4h57m;33.2°",
  "Aur;Hoedus II;5h7m;41.2°",
  "Aur;Menkalinan;6h0m;44.9°",
  "Boo;Arcturus;14h16m;19.2°",
  "Boo;Izar;14h45m;27.1°",
  "Boo;Muphrid;13h55m;18.4°",
  "Boo;Nekkar;15h2m;40.4°",
  "Boo;Seginus;14h32m;38.3°",
  "Cap;Dabih;20h21m;-14.8°",
  "Cap;Deneb Algedi;21h47m;-16.1°",
  "Car;Aspidiske;9h17m;-59.3°",
  "Car;Avior;8h23m;-59.5°",
  "Car;Canopus;6h24m;-52.7°",
  "Car;Miaplacidus;9h13m;-69.7°",
  "Cas;Achird;0h49m;57.8°",
  "Cas;Caph;0h9m;59.2°",
  "Cas;Cih;0h57m;60.7°",
  "Cas;Ruchbah;1h26m;60.2°",
  "Cas;Schedar;0h41m;56.5°",
  "Cas;Segin;1h54m;63.7°",
  "Cen;Al Nair al Kent;13h56m;-47.3°",
  "Cen;Hadar;14h4m;-60.4°",
  "Cen;Ke Kwan;14h59m;-42.1°",
  "Cen;Menkent;14h7m;-36.4°",
  "Cen;Muhlifain;12h42m;-49°",
  "Cen;Rigil Kentaurus;14h40m;-60.8°",
  "Cep;Alderamin;21h19m;62.6°",
  "Cep;Alphirk;21h29m;70.6°",
  "Cep;Errai;23h39m;77.6°",
  "Cet;Dheneb;1h9m;-10.2°",
  "Cet;Diphda;0h44m;-18°",
  "Cet;Kaffaljidhma;2h43m;3.2°",
  "Cet;Menkar;3h2m;4.1°",
  "CMa;Adhara;6h59m;-29°",
  "CMa;Aludra;7h24m;-29.3°",
  "CMa;Mirzam;6h23m;-18°",
  "CMa;Phurad;6h20m;-30.1°",
  "CMa;Sirius;6h45m;-16.7°",
  "CMa;Wezen;7h8m;-26.4°",
  "CMi;Gomeisa;7h27m;8.3°",
  "CMi;Procyon;7h39m;5.2°",
  "Cnc;Tarf;8h17m;9.2°",
  "Col;Phact;5h40m;-34.1°",
  "Col;Wazn;5h51m;-35.8°",
  "CrB;Alphecca;15h35m;26.7°",
  "Cru;Acrux;12h27m;-63.1°",
  "Cru;Gacrux;12h31m;-57.1°",
  "Cru;Mimosa;2h48m;-59.7°",
  "Crv;Algorel;12h30m;-16.5°",
  "Crv;Gienah Ghurab;12h16m;-17.5°",
  "Crv;Kraz;12h34m;-23.4°",
  "Crv;Minkar;12h10m;-22.6°",
  "CVn;Cor Caroli;12h56m;38.3°",
  "Cyg;Albireo;19h31m;28°",
  "Cyg;Deneb;20h41m;45.3°",
  "Cyg;Gienah;20h46m;34°",
  "Cyg;Sadr;20h22m;40.3°",
  "Dra;Aldhibah;17h9m;65.7°",
  "Dra;Aldhibain;16h24m;61.5°",
  "Dra;Edasich;15h25m;59°",
  "Dra;Eltanin;17h57m;51.5°",
  "Dra;Rastaban;17h30m;52.3°",
  "Dra;Tais;19h13m;67.7°",
  "Eri;Acamar;2h58m;-40.3°",
  "Eri;Achernar;1h38m;-57.2°",
  "Eri;Kursa;5h8m;-5.1°",
  "Eri;Rana;3h43m;-9.8°",
  "Eri;Zaurak;3h58m;-13.5°",
  "Gem;Alhena;6h38m;16.4°",
  "Gem;Alzirr;6h45m;12.9°",
  "Gem;Castor;7h35m;31.9°",
  "Gem;Mebsuta;6h44m;25.1°",
  "Gem;Pollux;7h45m;28°",
  "Gem;Propus;6h15m;22.5°",
  "Gem;Tejat;6h23m;22.5°",
  "Gem;Wasat;7h20m;22°",
  "Gru;Al Dhanab;22h43m;-46.9°",
  "Gru;Alnair;22h8m;-47°",
  "Her;Kornephoros;16h30m;21.5°",
  "Her;Rasalgethi;17h15m;14.4°",
  "Her;Rutilicus;16h41m;31.6°",
  "Her;Sarin;17h15m;24.8°",
  "Hya;Alphard;9h28m;-8.7°",
  "Hyi;Head of Hydrus;1h59m;-61.6°",
  "Ind;Persian;20h38m;-47.3°",
  "Leo;Algieba;10h20m;19.8°",
  "Leo;Chort;11h14m;15.4°",
  "Leo;Denebola;11h49m;14.6°",
  "Leo;dhafera;10h17m;23.4°",
  "Leo;Ras Elased Austr;9h46m;23.8°",
  "Leo;Regulus;10h8m;12°",
  "Leo;Subra;9h41m;9.9°",
  "Leo;Zosma;11h14m;20.5°",
  "Lep;Arneb;5h33m;-17.8°",
  "Lep;Nihal;5h28m;-20.8°",
  "Lib;Brachium;15h04m;-25.3°",
  "Lib;Zubenelgenubi;14h51m;-16°",
  "Lib;Zubeneschamali;15h17m;-9.4°",
  "Lup;Ke Kouan;14h59m;-43.1°",
  "Lup;Men;14h42m;-47.4°",
  "Lyr;Sheliak;18h50m;33.4°",
  "Lyr;Sulaphat;18h59m;32.7°",
  "Lyr;Vega;18h37m;38.8°",
  "Oph;Cebalrai;17h43m;4.6°",
  "Oph;Han;16h37m;-10.6°",
  "Oph;Rasalhague;17h35m;12.6°",
  "Oph;Sabik;17h10m;-15.7°",
  "Oph;Yed Posterior;16h18m;-4.7°",
  "Oph;Yed Prior;16h14m;-3.7°",
  "Ori;Algjebbah;5h24m;-2.4°",
  "Ori;Alnilam;5h36m;-1.2°",
  "Ori;Alnitak;5h41m;-1.9°",
  "Ori;Bellatrix;5h25m;6.3°",
  "Ori;Betelgeuse;5h55m;7.41°",
  "Ori;Hatysa;5h35m;-5.9°",
  "Ori;Meissa;5h35m;9.9°",
  "Ori;Mintaka;5h32m;-0.3°",
  "Ori;Rigel;5h15m;-8.2°",
  "Ori;Saiph;5h48m;-9.7°",
  "Ori;Tabit;4h50m;7°",
  "Pav;Peacock;20h26m;-56.7°",
  "Peg;Algenib;0h13m;15.2°",
  "Peg;Baham;22h10m;6.2°",
  "Peg;Enif;21h44m;9.9°",
  "Peg;Homam;22h41m;10.8°",
  "Peg;Markab;23h5m;15.2°",
  "Peg;Matar;22h43m;30.2°",
  "Peg;Sadalbari;22h50m;24.6°",
  "Peg;Scheat;23h4m;28.1°",
  "Per;Algol;3h8m;41°",
  "Per;Gorgonea Tertia;3h5m;38.8°",
  "Per;Mirfak;3h24m;49.9°",
  "Phe;Ankaa;0h26m;-42.3°",
  "PsA;Fomalhaut;22h58m;-29.6°",
  "Pup;Asmidiske;7h49m;-24.9°",
  "Pup;Naos;8h4m;-40°",
  "Pup;Turais;8h8m;-24.3°",
  "Sco;Alniyat;16h21m;-25.6°",
  "Sco;Antares;16h29m;-26.4°",
  "Sco;Dschubba;16h0m;-22.6°",
  "Sco;Girtab;17h42m;-39°",
  "Sco;Graffias;16h5m;-19.8°",
  "Sco;Lesath;17h31m;-37.3°",
  "Sco;Sargas;17h37m;-43°",
  "Sco;Shaula;17h34m;-37.1°",
  "Sco;We;16h50m;-34.3°",
  "Ser;Alava;18h21m;-2.9°",
  "Ser;Unukalhai;15h44m;6.4°",
  "Sgr;Albaldah;9h10m;-21°",
  "Sgr;Alnasl;18h6m;-30.4°",
  "Sgr;Ascella;19h3m;-29.9°",
  "Sgr;Kaus Australis;18h24m;-34.4°",
  "Sgr;Kaus Borealis;18h28m;-25.4°",
  "Sgr;Kaus Meridionalis;18h21m;-29.8°",
  "Sgr;Nunki;18h55m;-26.3°",
  "Tau;Ain;4h29m;19.2°",
  "Tau;Alcyone;3h47m;24.1°",
  "Tau;Aldebaran;4h36m;16.5°",
  "Tau;Alheka;5h38m;21.1°",
  "Tau;Elnath;5h26m;28.6°",
  "TrA;Atria;16h49m;-69°",
  "Tri;Mothallah;1h53m;29.6°",
  "UMa;Al Haud;9h33m;51.7°",
  "UMa;Alioth;12h54m;56°",
  "UMa;Alkaid;13h48m;49.3°",
  "UMa;Alula Borealis;11h18m;33.1°",
  "UMa;Dubhe;11h4m;61.8°",
  "UMa;Megrez;12h15m;57°",
  "UMa;Merak;11h2m;56.4°",
  "UMa;Mizar;13h24m;54.9°",
  "UMa;Muscida;8h30m;60.7°",
  "UMa;Phecda;11h54m;53.7°",
  "UMa;Talita;8h59m;48°",
  "UMa;Tania Australis;10h22m;41.5°",
  "UMa;Tania Borealis;10h17m;42.9°",
  "UMi;Kochab;14h51m;74.2°",
  "UMi;Pherkad Major;15h21m;71.8°",
  "UMi;Polaris;2h32m;89.3°",
  "Vel;Koo She;8h45m;-54.7°",
  "Vel;Markeb;9h22m;-55°",
  "Vel;Regor;8h10m;-47.3°",
  "Vel;Suhail;9h8m;-43.4°",
  "Vel;Tseen Ke;9h57m;-54.6°",
  "Vir;Auva;12h56m;3.4°",
  "Vir;Heze;13h35m;-0.6°",
  "Vir;Porrima;12h42m;-1.5°",
  "Vir;Spica;13h25m;-11.2°",
  "Vir;Vindemiatrix;13h2m;11°"
};
