/******************************************************************************/
/**     DeCE TABLE for MF2                                                   **/
/******************************************************************************/

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

#include "dece.h"
#include "decetable.h"
#include "terminate.h"

static int DeceTableMF2RRR(ENDF *, int, int);
static int DeceTableMF2RR7(ENDF *, int);
static int DeceTableMF2URR(ENDF *, int);


/**********************************************************/
/*      Process MF=2 Resonance Parameters                 */
/**********************************************************/
void DeceTableMF2(ENDF *lib)
{
  int idx = 0;
  Record cont = lib->rdata[idx];
  int lfw = cont.l2;
  int ner = cont.n1;
  idx++;

  cout << "# Resonance parameters" << endl;
  cout << "#          NER" << setw(14) << ner << "  number of resonance energy ranges" << endl;
  cout << "#          LFW" << setw(14) << lfw << "  average fission width given or not" << endl;

  for(int iner=0 ; iner<ner ; iner++){
    cont = lib->rdata[idx++];
    double el   = cont.c1;
    double eh   = cont.c2;
    int    lru  = cont.l1;
    int    lrf  = cont.l2;
    int    nro  = cont.n1;
    int    naps = cont.n2;
    cout << "#           EL"; outVal(el); cout << "  lower limit of this energy range" << endl;
    cout << "#           EH"; outVal(eh); cout << "  upper limit" << endl;
    cout << "#          LRU" << setw(14) << lru  << "  0:scattering radius only, 1:RRR, 2:URR" << endl;
    cout << "#          LRF" << setw(14) << lrf  << "  1:SLBW, 2:MLBW, 3:RM, 4:AA, 7:R-Matrix" << endl;
    cout << "#          NRO" << setw(14) << nro  << "  energy dependent scattering radius" << endl;
    cout << "#         NAPS" << setw(14) << naps << "  scattering radius control" << endl;

    /*** Resolved Resonance Region */
    if(lru == 1){
      if(lrf <= 3){
        cont = lib->rdata[idx++];
        double spi  = cont.c1;
        double ap   = cont.c2;
        int    nls  = cont.n1;

        cout << "#          SPI"; outVal(spi); cout << "  target spin" << endl;
        cout << "#           AP"; outVal(ap); cout << "  scattering radius" << endl;
        cout << "#          NLS" << setw(14) << nls << "  number of L-values" << endl;

        if(nro != 0) idx++;
        for(int inls=0 ; inls<nls ; inls++){
          cout << "# L       " << setw(4) << inls << endl;
          idx = DeceTableMF2RRR(lib,lrf,idx);
        }
      }
      else if(lrf == 7){
        idx = DeceTableMF2RR7(lib,idx);
      }
    }

    /*** Unresolved Resonance Region */
    else if(lru == 2){
      cont = lib->rdata[idx++];
      double spi  = cont.c1;
      double ap   = cont.c2;
      int    lssf = cont.l1;
      int    nls  = cont.n1;

      cout << "#          SPI"; outVal(spi); cout << "  target spin" << endl;
      cout << "#           AP"; outVal(ap); cout << "  scattering radius" << endl;
      cout << "#          NLS" << setw(14) << nls << "  number of L-values" << endl;
      cout << "#         LSSF" << setw(14) << lssf << "  FILE2 used for self-shielding only" << endl;

      if(lrf != 2){
        WarningMessage("LRF=0,1 cannot be processed");
        return;
      }
      for(int inls=0 ; inls<nls ; inls++){
        cout << "# L       " << setw(4) << inls << endl;
        idx = DeceTableMF2URR(lib,idx);
      }
    }
    else WarningMessage("this resonance parameters cannot be processed");
  }
}


/**********************************************************/
/*      Resolved Resonance Range                          */
/**********************************************************/
int DeceTableMF2RRR(ENDF *lib, int lrf, int idx)
{
  Record cont = lib->rdata[idx];
  int    nrs  = cont.n2;

  cout << "#          NRS" << setw(14) << nrs << "  number of resonances"<< endl;
  cout << "# E             J    ";
  if((lrf == 1) || (lrf == 2))
    cout << "G(total)      G(neutron)    G(gamma)      G(fission)" << endl;
  else if(lrf == 3)
    cout << "G(neutron)    G(gamma)      G(fissionA)   G(fissionB)" << endl;
  for(int i=0 ; i<nrs ; i++){
    int j = 6*i;
    outVal(lib->xptr[idx][j  ]);
    outVal(5,1,lib->xptr[idx][j+1]);
    outVal(lib->xptr[idx][j+2]);
    outVal(lib->xptr[idx][j+3]);
    outVal(lib->xptr[idx][j+4]);
    outVal(lib->xptr[idx][j+5]);
    cout << endl;
  }
  idx++;
  cout << endl;
  cout << endl;

  return(idx);
}


int DeceTableMF2RR7(ENDF *lib, int idx)
{
  Record cont = lib->rdata[idx];
  int    ifg  = cont.l1;
  int    krm  = cont.l2;
  int    njs  = cont.n1;
  int    krl  = cont.n2;

  cout << endl;
  cout << "#          IFG" << setw(14) << ifg << "  0: GAM in eV, 1: GAM in sqrt(eV)" << endl;
  cout << "#          KRM" << setw(14) << krm << "  1:SLBW, 2:MLBW, 3:RM, 4:R-Matrix" << endl;
  cout << "#          NJS" << setw(14) << njs << "  number of J-pi values" << endl;
  cout << "#          KRL" << setw(14) << krl << "  0: non-relativistic, 1: relativistic" << endl;
  idx ++;

  cont = lib->rdata[idx];
  int    npp  = cont.l1;
  cout << "#          NPP" << setw(14) << npp << "  number of pairs"<< endl;

  for(int ipp=0 ; ipp<npp ; ipp++){
    double ma  = lib->xptr[idx][ipp*12];
    double mb  = lib->xptr[idx][ipp*12+1];
    double za  = lib->xptr[idx][ipp*12+2];
    double zb  = lib->xptr[idx][ipp*12+3];
    double ia  = lib->xptr[idx][ipp*12+4];
    double ib  = lib->xptr[idx][ipp*12+5];
    double q   = lib->xptr[idx][ipp*12+6];
    double pnt = lib->xptr[idx][ipp*12+7];
    double shf = lib->xptr[idx][ipp*12+8];
    double mt  = lib->xptr[idx][ipp*12+9];
    double pa  = lib->xptr[idx][ipp*12+10];
    double pb  = lib->xptr[idx][ipp*12+11];

    cout << "# MT" << setw(5) << (int)mt << "  Q-value = "; outVal(11,5,q);
    cout << endl;
    cout << "#              PNT = " << (int)pnt << "   SHF = " << (int) shf << endl;
    cout << "# Pair               Mass     Charge spin  par" << endl;
    cout << "              "; outVal(11,4,ma); outVal(11,4,za); outVal(5,1,ia); outVal(5,1,pa);
    cout << endl;
    cout << "              "; outVal(11,4,mb); outVal(11,4,zb); outVal(5,1,ib); outVal(5,1,pb);
    cout << endl;
  }
  idx++;

  for(int ijs=0 ; ijs<njs ; ijs++){
    cont = lib->rdata[idx];
    double aj   = cont.c1;
    double pj   = cont.c2;
    int    kbk  = cont.l1;
    int    kps  = cont.l2;
    int    nch  = cont.n2;

    int parity = (aj < 0.0) ? -1 : 1;
    if(aj == 0.0) parity = (int)pj;

    cout << "# "; outVal(4,1,abs(aj)); cout << ((parity < 0) ? "(-)" : "(+)") << endl;
    cout << "#          NBK" << setw(14) << kbk << "  non-zero if background R-matrix exist" << endl;
    cout << "#          KPS" << setw(14) << kps << "  non-zero if non-hard-sphere specified" << endl;
    cout << "#          NCH" << setw(14) << nch << "  number of channels"<< endl;

    int m = (nch+1)/6+1; if((nch+1)%6 == 0) m--;

    for(int ich=0 ; ich<nch ; ich++){
      int ppi = (int)lib->xptr[idx][ich*6];
      int l   = (int)lib->xptr[idx][ich*6+1];
      double sch =   lib->xptr[idx][ich*6+2];
      double bnd =   lib->xptr[idx][ich*6+3];
      double ape =   lib->xptr[idx][ich*6+4];
      double apt =   lib->xptr[idx][ich*6+5];

      cout << "#          PPI" << setw(14) << ppi << "  pair index" << endl;
      cout << "#            L" << setw(14) << l   << "  L-value" << endl;
      cout << "#          SCH"; outVal(sch); cout << "  channel spin" << endl;
      cout << "#          BND"; outVal(bnd); cout << "  boundary condition" << endl;
      cout << "#          APE"; outVal(ape); cout << "  effective channel radius" << endl;
      cout << "#          APT"; outVal(apt); cout << "  true channel radius" << endl;
    }
    idx++;

    cont = lib->rdata[idx];
    int    nrs  = cont.l2;
    cout << "#          NRS" << setw(14) << nrs << "  number of resonances" << endl;


    for(int irs=0 ; irs<nrs ; irs++){
      int i0 = irs*m*6;
      outVal(lib->xptr[idx][i0]);
      for(int ich=0 ; ich<nch ; ich++) outVal(lib->xptr[idx][i0+ich+1]);
      cout << endl;
    }
    idx++;
  }

  return(idx);
}


/**********************************************************/
/*      Unresolved Resonance Range                        */
/**********************************************************/
int DeceTableMF2URR(ENDF *lib, int idx)
{
  Record cont = lib->rdata[idx];
  int njs = cont.n1;
  idx++;

  cout << "#          NJS" << setw(14) << njs << "  number of J-values"<< endl;

  for(int injs=0 ; injs<njs ; injs++){
    cont = lib->rdata[idx];
    double aj   = cont.c1;
    int    ne   = cont.n2;

    cout << "# J       "; outVal(4,1,aj); cout << endl;
    cout << "#           NE" << setw(14) << ne-1 << "  nubmber of energy points" << endl;
    cout << "# Deg. Freedom  other         neutron       gamma"<<endl;
    outVal(lib->xptr[idx][0]);
    outVal(lib->xptr[idx][1]);
    outVal(lib->xptr[idx][2]);
    outVal(lib->xptr[idx][3]);
    outVal(lib->xptr[idx][4]);
    outVal(lib->xptr[idx][5]);
    cout << endl;

    cout << "# E             D             G(other)      G(neutron)";
    cout << "    G(gamma)      G(fission)" << endl;

    for(int i=1 ; i<=ne ; i++){
      int j = 6*i;
      outVal(lib->xptr[idx][j  ]);
      outVal(lib->xptr[idx][j+1]);
      outVal(lib->xptr[idx][j+2]);
      outVal(lib->xptr[idx][j+3]);
      outVal(lib->xptr[idx][j+4]);
      outVal(lib->xptr[idx][j+5]);
      cout << endl;
    }
    idx ++;

    cout << endl;
    cout << endl;
  }
  return(idx);
}
