#ifndef ROOT_Localcompression
#define ROOT_Localcompression

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Local Compression                                                    //
//                                                                      //
// Description of the event and track parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TRef.h"
#include "TH1.h"
#include "TBits.h"
#include "TMath.h"

#define DUMMYSIZE 10
#define LARGESIZE 1000000
#define SMALLSIZE 1000
#define FLOATSIZE 6

union Dummy_t {
   char ch[4];
   Float_t fp;
};

class TDummy : public TObject {

private:
   Int_t         fSize;
   Float_t      *fDummy; //[fSize]

public:
   TDummy(Int_t size = DUMMYSIZE); 
   TDummy(const TDummy& dummy);
   virtual ~TDummy();
   TDummy &operator=(const TDummy &dummy);

   void          Clear(Option_t *option ="");
   void          Build();
   Int_t         GetSize() const { return fSize; }
   Float_t      *GetDummy() const { return fDummy; }

   ClassDef(TDummy,1)
};

#endif
