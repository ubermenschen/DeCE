/******************************************************************************/
/**     DeCE OUTPUT                                                          **/
/******************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

#include "dece.h"

static void dictsort         (ENDFDict *);
static void fixdictionary    (ifstream *, ENDFDict *);
static void makeMF1          (ifstream *, ENDFDict *);
static inline void swap      (int *, int *);

static string blank = "           ";


/**********************************************************/
/*      Put All Temporal Data in a File                   */
/**********************************************************/
void DeceOutput(ifstream *fpin, ENDFDict *dict, ENDF *lib[])
{
  /*** sort dictionary */
  dictsort(dict);

  /*** tape ID */
  cout << left << setw(66) << dict->tpid << "   1 0  0" << endl;
  cout << right;

  /*** for all MF */
  for(int mf=1 ; mf<=40 ; mf++){

    /*** check if MF is given */
    bool given = false;
    for(int i=0 ; i<dict->sec ; i++) if(dict->mf[i] == mf) given = true;
    if(!given) continue;

    /*** copy one section from original or replaced */
    DeceExtract(dict,lib,fpin,mf,0);

    /*** insert SEND recort */
    ENDFWriteFEND(dict->mat);
  }
}


/**********************************************************/
/*      Sort Dictionary                                   */
/**********************************************************/
void dictsort(ENDFDict *dict)
{
  for(int j=0 ; j<dict->sec ; j++){
    int k = j;
    for(int i=j ; i<dict->sec ; i++){
      if( (dict->mf[i]<=dict->mf[k]) && (dict->mt[i]<dict->mt[k]) ) k = i;
    }
    swap(&dict->mf[j], &dict->mf[k]);
    swap(&dict->mt[j], &dict->mt[k]);
    swap(&dict->nc[j], &dict->nc[k]);
    swap(&dict->id[j], &dict->id[k]);
  }
}

inline void swap(int *a, int *b)
{
  int tmp = *a;  *a = *b;  *b = tmp;
}


/**********************************************************/
/*      Clean-up, Renumber and Fix Dictionary             */
/**********************************************************/
void DeceRenumber(string fin, string fout, ENDFDict *dict)
{
  ifstream  fpin;
  streambuf *save = cout.rdbuf();
  ofstream  fpout;
  string    line;
  Record    head;

  /*** rescan temp file */
  head = dict->head;
  ENDFScanLibrary(fin,dict);
  dict->head = head;

  fpin.open(&fin[0]);
  fpout.open(&fout[0]);
  cout.rdbuf(fpout.rdbuf());

  /*** recalculate NWD and NXC from temp file */
  fixdictionary(&fpin,dict);

  /*** tape ID */
  fpin.seekg(0,ios_base::beg);
  getline(fpin,line);
  cout << left << setw(66) << dict->tpid << "   1 0  0    0" << endl;
  cout << right;

  /*** MF1 */
  makeMF1(&fpin,dict);

  /*** all other MFs, copy from temp file */
  for(int mf=2 ; mf<=40 ; mf++){

    /*** check if MF is given */
    bool given = false;
    for(int i=0 ; i<dict->sec ; i++) if(dict->mf[i] == mf) given = true;
    if(!given) continue;

    for(int i=0 ; i<dict->sec ; i++){
      if(mf == dict->mf[i]) ENDFExtract(&fpin,mf,dict->mt[i]);
    }

    /*** insert SEND recort */
    ENDFWriteFEND(dict->mat);
  }

  /*** write MEND */
  ENDFWriteFEND(0);

  /*** write TEND */
  ENDFWriteFEND(-1);

  fpin.close();
  fpout.close();
  cout.rdbuf(save);
}


/**********************************************************/
/*      Fix Dictionary                                    */
/**********************************************************/
void fixdictionary(ifstream *fp, ENDFDict *dict)
{
  string s1,s2,s3,s4,line;
  int nwd = 0, mf0,mt0,mf,mt;

  /*** skip tape ID, HEAD, and 3 CONTs */
  fp->seekg(0,ios_base::beg);
  for(int i=0 ; i<5 ; i++) getline(*fp,line);

  nwd = 0;
  bool nodic = false;
  while( getline(*fp,line) ){
    s1 = line.substr( 0,11);
    s2 = line.substr(11,11);
    s3 = line.substr(22,11);  mf = atoi(s3.c_str());
    s4 = line.substr(33,11);  mt = atoi(s4.c_str());

    s3 = line.substr(70, 2);  mf0 = atoi(s3.c_str());
    s4 = line.substr(72, 3);  mt0 = atoi(s4.c_str());

    /*** dictionary starts at two blanks and two integers line */
    if( (s1==blank) && (s2==blank) && (mf==1) && (mt==451) ){
      break;
    }
    /*** in case no dictionary */
    else if( (mf0==1) && (mt0==0) ){
      nodic = true;
      break;
    }

    nwd ++;
  }

  if(!nodic){
    int mod = 0;
    do{
      s1 = line.substr(55,11);  mod = atoi(s1.c_str());

      s3 = line.substr(22,11);  mf  = atoi(s3.c_str());
      s4 = line.substr(33,11);  mt  = atoi(s4.c_str());

      s3 = line.substr(70, 2);  mf0 = atoi(s3.c_str());
      s4 = line.substr(72, 3);  mt0 = atoi(s4.c_str());

      if( (mf0==1) && (mt0==0) )  break;

      int k = dict->scanDict(mf,mt);
      if(k>=0)  dict->mod[k] = mod;
    }while( getline(*fp,line) );
  }

  dict->setNWD(nwd);
  dict->setNXC(dict->sec);  // NXC should be the same as Nsec

  /*** the number of cards in the MF=1/MT=451 = NWD + NXC + Header */
  dict->nc[0] = nwd + dict->sec + 4;
}



/**********************************************************/
/*      Generate Comment and Dictionary Sections          */
/**********************************************************/
void makeMF1(ifstream *fpin, ENDFDict *dict)
{
  string    line;
  ENDF      lib(S);
  int       mf = 1, mt = 451;

  /*** header part */
  lib.setENDFmat(dict->mat);
  lib.setENDFmf(mf);
  lib.setENDFmt(mt);
  lib.setENDFhead(dict->head);
  getline(*fpin,line);
  ENDFWriteHEAD(&lib);

  for(int i=0 ; i<3 ; i++){
    getline(*fpin,line);
    lib.rdata[i] = dict->cont[i];
    ENDFWriteCONT(&lib);
  }

  /*** copy NWD lines from temp file */
  for(int i=0 ; i<dict->getNWD() ; i++){
    getline(*fpin,line);
    ENDFWriteTEXT(&lib,line);
  }

  /*** generate dictionary */
  for(int i=0 ; i<dict->sec ; i++){
    ENDFWriteDICT(&lib,dict->mf[i],dict->mt[i],dict->nc[i],dict->mod[i]);
  }

  ENDFWriteSEND(&lib);

  /*** MF1 MT452, 455, 456, and 458 */
  mt = 452; if( dict->scanDict(1,mt)>=0 ) ENDFExtract(fpin,1,mt);
  mt = 455; if( dict->scanDict(1,mt)>=0 ) ENDFExtract(fpin,1,mt);
  mt = 456; if( dict->scanDict(1,mt)>=0 ) ENDFExtract(fpin,1,mt);
  mt = 458; if( dict->scanDict(1,mt)>=0 ) ENDFExtract(fpin,1,mt);
  mt = 460; if( dict->scanDict(1,mt)>=0 ) ENDFExtract(fpin,1,mt);

  ENDFWriteFEND(dict->mat);
}
