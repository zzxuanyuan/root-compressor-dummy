// @(#)root/test:$Id$
// Author: Rene Brun   19/08/96

#include "RVersion.h"
#include "TRandom.h"
#include "TDirectory.h"
#include "TProcessID.h"

#include "Dummy.h"
#include <iostream>

ClassImp(TDummy)

////////////////////////////////////////////////////////////////////////////////
/// Create an TDummy.
TDummy::TDummy(Int_t size)
{
   fSize = size;
   fDummy = new Float_t[fSize];
   for(int i=0;i<fSize;++i) {
      Dummy_t dummy;
      for(int offset = 0; offset < 4; ++offset) {
         Float_t tmp = Float_t(gRandom->Rndm(1));

         if(tmp < 0.4) {
            dummy.ch[offset] = 'a';
         } else if(tmp < 0.65) {
            dummy.ch[offset] = 'b';
         } else if(tmp < 0.9) {
            dummy.ch[offset] = 'c';
         } else {
            dummy.ch[offset] = 'd';
         }

/*
         if(tmp < 0.08167) {
            dummy.ch[offset] = 'a';
         } else if(tmp < 0.09659) {
            dummy.ch[offset] = 'b';
         } else if(tmp < 0.12441) {
            dummy.ch[offset] = 'c';
         } else if(tmp < 0.16694) {
            dummy.ch[offset] = 'd';
         } else if(tmp < 0.29396) {
            dummy.ch[offset] = 'e';
         } else if(tmp < 0.31624) {
            dummy.ch[offset] = 'f';
         } else if(tmp < 0.33639) {
            dummy.ch[offset] = 'g';
         } else if(tmp < 0.39733) {
            dummy.ch[offset] = 'h';
         } else if(tmp < 0.46699) {
            dummy.ch[offset] = 'i';
         } else if(tmp < 0.46852) {
            dummy.ch[offset] = 'j';
         } else if(tmp < 0.47624) {
            dummy.ch[offset] = 'k';
         } else if(tmp < 0.51649) {
            dummy.ch[offset] = 'l';
         } else if(tmp < 0.54055) {
            dummy.ch[offset] = 'm';
         } else if(tmp < 0.60804) {
            dummy.ch[offset] = 'n';
         } else if(tmp < 0.68311) {
            dummy.ch[offset] = 'o';
         } else if(tmp < 0.7024) {
            dummy.ch[offset] = 'p';
         } else if(tmp < 0.70335) {
            dummy.ch[offset] = 'q';
         } else if(tmp < 0.76322) {
            dummy.ch[offset] = 'r';
         } else if(tmp < 0.82649) {
            dummy.ch[offset] = 's';
         } else if(tmp < 0.91705) {
            dummy.ch[offset] = 't';
         } else if(tmp < 0.94463) {
            dummy.ch[offset] = 'u';
         } else if(tmp < 0.95441) {
            dummy.ch[offset] = 'v';
         } else if(tmp < 0.97801) {
            dummy.ch[offset] = 'w';
         } else if(tmp < 0.97951) {
            dummy.ch[offset] = 'x';
         } else if(tmp < 0.99925) {
            dummy.ch[offset] = 'y';
         } else {
            dummy.ch[offset] = 'z';
         }
*/
      }
      fDummy[i] = dummy.fp;
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Create an TDummy.
TDummy::TDummy(const TDummy& dummy) : TObject(dummy)
{
   Float_t *intermediate = dummy.GetDummy();
   Int_t size = dummy.GetSize();
   fDummy = new Float_t[size];
   for(int i=0;i<size;++i)
      fDummy[i] = intermediate[i];
}

////////////////////////////////////////////////////////////////////////////////

TDummy::~TDummy()
{
   Clear();
   delete fDummy;
   fSize = 0;
}

////////////////////////////////////////////////////////////////////////////////

void TDummy::Clear(Option_t * /*option*/)
{
   TObject::Clear();
   for(int i=0;i<fSize;++i)
      fDummy[i] = 0;
}

/////////////////////////////////////////////////////////////////////////////////

void TDummy::Build()
{
   for(int i=0;i<fSize;++i) {
      Dummy_t dummy;
      for(int offset = 0; offset < 4; ++offset) {
         Float_t tmp = Float_t(gRandom->Rndm(1));
         if(tmp < 0.4) {
            dummy.ch[offset] = 'a';
         } else if(tmp < 0.65) {
            dummy.ch[offset] = 'b';
         } else if(tmp < 0.9) {
            dummy.ch[offset] = 'c';
         } else {
            dummy.ch[offset] = 'd';
         }
/* This is another distribution which follows english letter frequency.
         Float_t tmp = Float_t(gRandom->Rndm(1));
         if(tmp < 0.08167) {
            dummy.ch[offset] = 'a';
         } else if(tmp < 0.09659) {
            dummy.ch[offset] = 'b';
         } else if(tmp < 0.12441) {
            dummy.ch[offset] = 'c';
         } else if(tmp < 0.16694) {
            dummy.ch[offset] = 'd';
         } else if(tmp < 0.29396) {
            dummy.ch[offset] = 'e';
         } else if(tmp < 0.31624) {
            dummy.ch[offset] = 'f';
         } else if(tmp < 0.33639) {
            dummy.ch[offset] = 'g';
         } else if(tmp < 0.39733) {
            dummy.ch[offset] = 'h';
         } else if(tmp < 0.46699) {
            dummy.ch[offset] = 'i';
         } else if(tmp < 0.46852) {
            dummy.ch[offset] = 'j';
         } else if(tmp < 0.47624) {
            dummy.ch[offset] = 'k';
         } else if(tmp < 0.51649) {
            dummy.ch[offset] = 'l';
         } else if(tmp < 0.54055) {
            dummy.ch[offset] = 'm';
         } else if(tmp < 0.60804) {
            dummy.ch[offset] = 'n';
         } else if(tmp < 0.68311) {
            dummy.ch[offset] = 'o';
         } else if(tmp < 0.7024) {
            dummy.ch[offset] = 'p';
         } else if(tmp < 0.70335) {
            dummy.ch[offset] = 'q';
         } else if(tmp < 0.76322) {
            dummy.ch[offset] = 'r';
         } else if(tmp < 0.82649) {
            dummy.ch[offset] = 's';
         } else if(tmp < 0.91705) {
            dummy.ch[offset] = 't';
         } else if(tmp < 0.94463) {
            dummy.ch[offset] = 'u';
         } else if(tmp < 0.95441) {
            dummy.ch[offset] = 'v';
         } else if(tmp < 0.97801) {
            dummy.ch[offset] = 'w';
         } else if(tmp < 0.97951) {
            dummy.ch[offset] = 'x';
         } else if(tmp < 0.99925) {
            dummy.ch[offset] = 'y';
         } else {
            dummy.ch[offset] = 'z';
         }
*/
      }
      fDummy[i] = dummy.fp;
   }
}
