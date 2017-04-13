// @(#)root/tmva $Id$
// Author: Andreas Hoecker, Peter Speckmayer, Joerg Stelzer, Helge Voss, Jan Therhaag

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : ResultsMulticlass                                                     *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Peter Speckmayer <Peter.Speckmayer@cern.ch>  - CERN, Switzerland          *
 *      Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN, Switzerland              *
 *      Jan Therhaag       <Jan.Therhaag@cern.ch>    - U of Bonn, Germany         *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *                                                                                *
 * Copyright (c) 2006:                                                            *
 *      CERN, Switzerland                                                         *
 *      MPI-K Heidelberg, Germany                                                 *
 *      U. of Bonn, Germany                                                       *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

/*! \class TMVA::ResultsMulticlass
\ingroup TMVA
Class which takes the results of a multiclass classification
*/

#include "TMVA/ResultsMulticlass.h"

#include "TMVA/DataSet.h"
#include "TMVA/DataSetInfo.h"
#include "TMVA/GeneticAlgorithm.h"
#include "TMVA/GeneticFitter.h"
#include "TMVA/MsgLogger.h"
#include "TMVA/Results.h"
#include "TMVA/Tools.h"
#include "TMVA/Types.h"

#include "TH1F.h"

#include <limits>
#include <vector>


////////////////////////////////////////////////////////////////////////////////
/// constructor

TMVA::ResultsMulticlass::ResultsMulticlass( const DataSetInfo* dsi, TString resultsName  )
   : Results( dsi, resultsName  ),
     IFitterTarget(),
     fLogger( new MsgLogger(Form("ResultsMultiClass%s",resultsName.Data()) , kINFO) ),
     fClassToOptimize(0),
     fAchievableEff(dsi->GetNClasses()),
     fAchievablePur(dsi->GetNClasses()),
     fBestCuts(dsi->GetNClasses(),std::vector<Double_t>(dsi->GetNClasses()))
{
}

////////////////////////////////////////////////////////////////////////////////
/// destructor

TMVA::ResultsMulticlass::~ResultsMulticlass()
{
   delete fLogger;
}

////////////////////////////////////////////////////////////////////////////////

void TMVA::ResultsMulticlass::SetValue( std::vector<Float_t>& value, Int_t ievt )
{
   if (ievt >= (Int_t)fMultiClassValues.size()) fMultiClassValues.resize( ievt+1 );
   fMultiClassValues[ievt] = value;
}

////////////////////////////////////////////////////////////////////////////////

Double_t TMVA::ResultsMulticlass::EstimatorFunction( std::vector<Double_t> & cutvalues ){

   DataSet* ds = GetDataSet();
   ds->SetCurrentType( GetTreeType() );

   // Cache optimisation, count true and false positives with memory access
   // instead of code branch.
   Float_t positives[2] = {0, 0};

   for (Int_t ievt = 0; ievt < ds->GetNEvents(); ievt++) {
      UInt_t  evClass = fEventClasses[ievt];
      Float_t w       = fEventWeights[ievt];

      Bool_t break_outer_loop = false;
      for (UInt_t icls = 0; icls < cutvalues.size(); ++icls) {
         auto value    = fMultiClassValues[ievt][icls];
         auto cutvalue = cutvalues.at(icls);
         if (cutvalue < 0. ? (-value < cutvalue) : (+value <= cutvalue)) {
            break_outer_loop = true;
            break;
         }
      }

      if (break_outer_loop) {
         continue;
      }

      Bool_t isEvCurrClass = (evClass == fClassToOptimize);
      positives[isEvCurrClass] += w;
   }

   const Float_t truePositive  = positives[1];
   const Float_t falsePositive = positives[0];

   Float_t eff         = truePositive / fClassSumWeights[fClassToOptimize];
   Float_t pur         = truePositive / (truePositive + falsePositive);
   Float_t effTimesPur = eff*pur;

   Float_t toMinimize = std::numeric_limits<float>::max();
   if (effTimesPur > std::numeric_limits<float>::min())
      toMinimize = 1./(effTimesPur); // we want to minimize 1/efficiency*purity

   fAchievableEff.at(fClassToOptimize) = eff;
   fAchievablePur.at(fClassToOptimize) = pur;

   return toMinimize;
}

////////////////////////////////////////////////////////////////////////////////
///calculate the best working point (optimal cut values)
///for the multiclass classifier

std::vector<Double_t> TMVA::ResultsMulticlass::GetBestMultiClassCuts(UInt_t targetClass){

   const DataSetInfo* dsi = GetDataSetInfo();
   Log() << kINFO << "Calculating best set of cuts for class "
         << dsi->GetClassInfo( targetClass )->GetName() << Endl;

   fClassToOptimize = targetClass;
   std::vector<Interval*> ranges(dsi->GetNClasses(), new Interval(-1,1));

   fClassSumWeights.clear();
   fEventWeights.clear();
   fEventClasses.clear();

   for (UInt_t icls = 0; icls < dsi->GetNClasses(); ++icls) {
      fClassSumWeights.push_back(0);
   }

   DataSet *ds = GetDataSet();
   for (Int_t ievt = 0; ievt < ds->GetNEvents(); ievt++) {
      const Event *ev = ds->GetEvent(ievt);
      fClassSumWeights[ev->GetClass()] += ev->GetWeight();
      fEventWeights.push_back(ev->GetWeight());
      fEventClasses.push_back(ev->GetClass());
   }

   const TString name( "MulticlassGA" );
   const TString opts( "PopSize=100:Steps=30" );
   GeneticFitter mg( *this, name, ranges, opts);

   std::vector<Double_t> result;
   mg.Run(result);

   fBestCuts.at(targetClass) = result;

   UInt_t n = 0;
   for( std::vector<Double_t>::iterator it = result.begin(); it<result.end(); it++ ){
      Log() << kINFO << "  cutValue[" <<dsi->GetClassInfo( n )->GetName()  << "] = " << (*it) << ";"<< Endl;
      n++;
   }

   return result;
}

////////////////////////////////////////////////////////////////////////////////
/// this function fills the mva response histos for multiclass classification

void  TMVA::ResultsMulticlass::CreateMulticlassHistos( TString prefix, Int_t nbins, Int_t /* nbins_high */ )
{
   Log() << kINFO << "Creating multiclass response histograms..." << Endl;

   DataSet* ds = GetDataSet();
   ds->SetCurrentType( GetTreeType() );
   const DataSetInfo* dsi = GetDataSetInfo();

   std::vector<std::vector<TH1F*> > histos;
   Float_t xmin = 0.-0.0002;
   Float_t xmax = 1.+0.0002;
   for (UInt_t iCls = 0; iCls < dsi->GetNClasses(); iCls++) {
      histos.push_back(std::vector<TH1F*>(0));
      for (UInt_t jCls = 0; jCls < dsi->GetNClasses(); jCls++) {
         TString name(Form("%s_%s_prob_for_%s",prefix.Data(),
                           dsi->GetClassInfo( jCls )->GetName(),
                           dsi->GetClassInfo( iCls )->GetName()));
         histos.at(iCls).push_back(new TH1F(name,name,nbins,xmin,xmax));
      }
   }

   for (Int_t ievt=0; ievt<ds->GetNEvents(); ievt++) {
      const Event* ev = ds->GetEvent(ievt);
      Int_t cls = ev->GetClass();
      Float_t w = ev->GetWeight();
      for (UInt_t jCls = 0; jCls < dsi->GetNClasses(); jCls++) {
         histos.at(cls).at(jCls)->Fill(fMultiClassValues[ievt][jCls],w);
      }
   }
   for (UInt_t iCls = 0; iCls < dsi->GetNClasses(); iCls++) {
      for (UInt_t jCls = 0; jCls < dsi->GetNClasses(); jCls++) {
         gTools().NormHist( histos.at(iCls).at(jCls) );
         Store(histos.at(iCls).at(jCls));
      }
   }

   /*
   //fill fine binned histos for testing
   if(prefix.Contains("Test")){
   std::vector<std::vector<TH1F*> > histos_highbin;
   for (UInt_t iCls = 0; iCls < dsi->GetNClasses(); iCls++) {
   histos_highbin.push_back(std::vector<TH1F*>(0));
   for (UInt_t jCls = 0; jCls < dsi->GetNClasses(); jCls++) {
   TString name(Form("%s_%s_prob_for_%s_HIGHBIN",prefix.Data(),
   dsi->GetClassInfo( jCls )->GetName().Data(),
   dsi->GetClassInfo( iCls )->GetName().Data()));
   histos_highbin.at(iCls).push_back(new TH1F(name,name,nbins_high,xmin,xmax));
   }
   }

   for (Int_t ievt=0; ievt<ds->GetNEvents(); ievt++) {
   const Event* ev = ds->GetEvent(ievt);
   Int_t cls = ev->GetClass();
   Float_t w = ev->GetWeight();
   for (UInt_t jCls = 0; jCls < dsi->GetNClasses(); jCls++) {
   histos_highbin.at(cls).at(jCls)->Fill(fMultiClassValues[ievt][jCls],w);
   }
   }
   for (UInt_t iCls = 0; iCls < dsi->GetNClasses(); iCls++) {
   for (UInt_t jCls = 0; jCls < dsi->GetNClasses(); jCls++) {
   gTools().NormHist( histos_highbin.at(iCls).at(jCls) );
   Store(histos_highbin.at(iCls).at(jCls));
   }
   }
   }
   */
}
