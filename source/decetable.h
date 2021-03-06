/*
   decetable.h : 
        prototype of functions to tabulate data
 */

/**************************************/
/*      decetable.cpp                 */
/**************************************/

void   DeceLibToTable    (ENDF *, ENDF *, int);
void   DeceTableMF1      (ENDF *);
void   DeceTableMF2      (ENDF *);
void   DeceTableMF3      (ENDF *);
void   DeceTableMF4      (ENDF *, int);
void   DeceTableMF5      (ENDF *);
void   DeceTableMF6      (ENDF *, ENDF *);
void   DeceTableMF8      (ENDF *);
void   DeceTableMF9      (ENDF *);
void   DeceTableMF10     (ENDF *);
void   DeceTableMF12     (ENDF *);
void   DeceTableMF13     (ENDF *);
void   DeceTableMF15     (ENDF *);
void   DeceTableMF33     (ENDF *);

static inline void outVal(int x)
{ cout << setw(10) << x << "    "; }

static inline void outVal(double x)
{ cout.setf(ios::scientific, ios::floatfield);
  cout << setprecision(7) << setw(14) << x; }

static inline void outVal(int w, int p, double x)
{ cout.setf(ios::fixed, ios::floatfield);
  cout << setw(w) << setprecision(p) << x; }

