#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"
#include "TBits.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLatex.h"
#include "TLine.h"
#include "TSystem.h"

TTree* eventTree = 0x0; // tree
// TTree variables (globals)
TObjString*         fClassesFired = 0x0; // list of fired trigger classes
UInt_t              fPhysSelDecision; // AliPhysicsSelection decision
Bool_t              fPhysSelPassed; // AliPhysicsSelection decision (pass/reject)
Bool_t              fEventCutsPassed; // AliEventCuts selection decision (pass/reject)
Int_t               fEventInFile; // current event number in the file
Int_t               fRunNumber; // run number
UInt_t              fPeriod; // run period
UInt_t              fOrbit; // orbit number
UShort_t            fBC; // bunch cross (BX) number
UInt_t              fL0inputs; // L0 trigger inputs
UInt_t              fL1inputs; // L1 trigger inputs
TObjString*         fFiredTriggerInputs; // list of fired trigger inputs
TBits*              fIR1 = 0x0; // interaction map for INT1 events (normally V0A&V0C) near the event, that's Int1Id-EventId within -90 +90 BXs
TBits*              fIR2 = 0x0; // map of the INT2 events (normally 0TVX) near the event, that's Int2Id-EventId within -90 +90 BXs
Int_t               fNumContrSPD; // number of contributors to SPD vertex
Int_t               fNumTracklets; // number of tracklets
Int_t               fNumTracks; // number of tracks
Int_t               fNumTracksRefMult08; //
TBits*              fFiredChipMap = 0x0; // map of fired chips (at least one cluster)
TBits*              fFiredChipMapFO = 0x0; // map of fired FastOr chips
Int_t               fNumITSCls[6]; // number of ITS clusters per layer
Int_t               fTriggerMaskTOF[72]; // TOF trigger mask array
Int_t               fNumberTOFcls; // number of reconstructed TOF clusters
Int_t               fNumberTOFtrgPads; // number of reconstructed trigger pads
Int_t               fNumberTOFmaxipads; // number of TOF trigger pads on
Float_t             fV0ATotMult; // total multiplicity in V0A
Float_t             fV0CTotMult; // total multiplicity in V0C
UShort_t            fV0ATriggerCharge; // online (trigger) charge in V0A
UShort_t            fV0CTriggerCharge; // online (trigger) charge in V0C
// Float_t             fV0AMult[32]; // multiplicity in V0A cells
// Float_t             fV0CMult[32];  // multiplicity in V0C cells
Float_t             fV0ATime; // average time in V0A
Float_t             fV0CTime; // average time in V0C
Bool_t              fV0PastFutureFilled; // flag for AliVZERO::kPastFutureFlagsFilled bit
Bool_t              fV0PastFuturePileUp; // flag for V0 past-future protection (true if pileup)
// Bool_t              fV0ATriggerBB[32]; // offline beam-beam flag in V0A cells
// Bool_t              fV0CTriggerBB[32]; // ffline beam-beam flag in V0C cells
// Bool_t              fV0ATriggerBG[32]; // offline beam-gas flag in V0A cells
// Bool_t              fV0CTriggerBG[32]; // offline beam-gas flag in V0C cells
UInt_t              fV0AFlagsBB; // Number of total online beam-beam flags in V0A
UInt_t              fV0CFlagsBB; // Number of total online beam-beam flags in V0C
UInt_t              fV0AFlagsBG; // Number of total online beam-gas flags in V0A
UInt_t              fV0CFlagsBG; // Number of total online beam-gas flags in V0C
Char_t              fV0ADecision; // final V0A decision
Char_t              fV0CDecision;  // final V0C decision
Double_t            fVtxX; // primary vertex x-coordinate
Double_t            fVtxY; // primary vertex y-coordinate
Double_t            fVtxZ; // primary vertex z-coordinate
Bool_t              fVtxTPC; // primary vertex reconstructed with TPC (not SPDVertex)

Int_t iNumFOonline = 0;
Int_t iNumFOoffline = 0;


const Int_t iNumTypes = 5;
TString sTypeLabels[iNumTypes] = {"ALL", "CINT7", "CVHMSH2", "CINT7_PhysSel", "CVHMSH2_PhysSel"};
enum eEventType { kAll = 0, kCINT7, kCVHMSH2, kCINT7_PhysSel, kCVHMSH2_PhysSel};

TList* listOut[iNumTypes];

TH1D* hEventCounter;
TH1D* hPFPUCounter;
TH2D* hEventPerRunCounter;

TH1D* hTriggerClasses[iNumTypes];
TH1D* hDistNtrklets[iNumTypes];
TH1D* hDistNtrks[iNumTypes];
TH1D* hDistRefMult08[iNumTypes];
TH1D* hDistFOonline[iNumTypes];
TH1D* hDistFOoffline[iNumTypes];
TH2D* hDistFOonlinePerRun[iNumTypes];
TH2D* hDistFOofflinePerRun[iNumTypes];

// 2D mult correlations
TH2D* h2_FOonline_Ntrklets[iNumTypes];
TH2D* h2_FOonline_Ntrks[iNumTypes];
TH2D* h2_FOonline_RefMult08[iNumTypes];

Bool_t CheckTriggerConsistency();
void CreateHistos();
void FillCommonHistos(eEventType type);
Bool_t LoadTTreeVars();

void SkimForSPDHM()
{
  Int_t iNumEventsToProcess = 0;
  // Int_t iNumEventsToProcess = 1000000;
  // Int_t iNumEventsToProcess = 200000;

  // TString sPath = "/Users/vpacik/Codes/ALICE/HMtrigger/running/17o/";
  // TString sPath = "/Users/vpacik/Codes/ALICE/HMtrigger/running/16k-merged/";
  // TString sPath = "/Users/vpacik/Codes/ALICE/HMtrigger/running/18f-muon_calo/";
  // TString sPath = "/Users/vpacik/Codes/ALICE/HMtrigger/running/18f-pass1_uncal/";
  TString sPath = "/Users/vpacik/Codes/ALICE/HMtrigger/running/18m-muon_calo/";
  // TString sPath = "/Users/vpacik/Codes/ALICE/HMtrigger/running/18l-muon_calo/";

  // TString sPath = "/Users/vpacik/Codes/ALICE/HMtrigger/newTask/";
  // TString sInFileName = "AnalysisResults_from_291285.root";
  // TString sOutFileName = "Skimmed_from_291285.root";
  // TString sInFileName = "AnalysisResults_290399_291284.root";
  // TString sOutFileName = "Skimmed_290411_290501.root";
  TString sInFileName = "AnalysisResults_290399_291284.root";
  TString sOutFileName = "Skimmed_290538_291284.root";
  // TString sOutFileName = "Skimmed.root";
  // TString sOutFileName = "Skimmed_noPFPU.root";
  // TString sOutFileName = "Skimmed_HMwithPFPU_wo287064.root";
  // TString sOutFileName = "Skimmed_HMwithPFPU.root";
  // TString sOutFileName = "Skimmed_test.root";
  // TString sOutFileName = "Skimmed_987b.root";
  // TString sOutFileName = "Skimmed_2556b.root";

  // ===============================================================================

  // openning input & output file
  TString sInputFile = sPath + sInFileName;
  TString sOutputFile = sPath + sOutFileName;

  TFile* fOutputFile = new TFile(sOutputFile.Data(),"RECREATE");
  if(!fOutputFile->IsOpen()) { printf(" ERROR: fOutputFile open!\n"); return; }

  TFile* fInputFile = new TFile(sInputFile.Data(),"READ");
  if(!fInputFile->IsOpen()) { printf(" ERROR: fInputFile not open!\n"); return; }

  CreateHistos();

  // loading TTree variables
  fInputFile->cd("FilterTrig");

  TList* histos = (TList*) gDirectory->Get("histos");
  if (!histos) { printf(" ERROR: TList with histos is not there! \n"); }

  // TH1F* fhEventCounter = (TH1F*) histos->FindObject("fhEventCounter");
  // if(fhEventCounter) { fhEventCounter->Draw(); } else { printf(" ERROR: 'fhEventCounter' is not there! \n"); }

  eventTree = (TTree*) gDirectory->Get("events");
  if (!eventTree) { printf(" ERROR: Tree is not there! \n"); return; }

  if(!LoadTTreeVars()) { return; }

  // looping over entries
  Int_t numEvents = eventTree->GetEntriesFast();
  printf("Found %d events\nTo be processed: %d\n",numEvents, (iNumEventsToProcess ? iNumEventsToProcess : numEvents) );
  if(iNumEventsToProcess > 0 && iNumEventsToProcess < numEvents) { numEvents = iNumEventsToProcess; }

  for (Int_t i(0); i < numEvents; i++)
  {
    if( (i % 100000) == 0) printf("=== Procesed %d out of %d events === \n",i,numEvents);
    eventTree->GetEvent(i);

    // issue in 18f with OFO bellow 85
    if(fRunNumber == 287064) { continue; }

    // trigger setting: pp2018_HighRateTest (v9) - not relevant
    if(fRunNumber == 287616) { continue; }

    // pp2018_Rare_987b
    // if(fRunNumber < 287323) continue;

    // pp2018_Rare_2556b
    if(fRunNumber == 287283) continue;
    if(fRunNumber > 287283) continue;

    // 18m/muon_calo : fishy runs with offline FO shifted
    if(fRunNumber >= 290611 && fRunNumber <= 290615) { continue; }
    if(fRunNumber == 291262) { continue; }

    hEventCounter->Fill("Input",1);
    hEventPerRunCounter->Fill(Form("%d",fRunNumber),"Input",1);

    // trigger part
    Bool_t bIsCINT7 = fClassesFired->String().Contains("CINT7-B-NOPF-CENT");
    Bool_t bIsCVHMSH2 = fClassesFired->String().Contains("CVHMSH2-B");
    Bool_t bIsConsist = CheckTriggerConsistency();
    Bool_t bPFPUFilled = fV0PastFutureFilled;
    Bool_t bPFPU = fV0PastFuturePileUp;

    if(bIsCINT7)
    {
      hEventPerRunCounter->Fill(Form("%d",fRunNumber),"CINT7-B",1);
      if(fClassesFired->String().Contains("CENTNOTRD")) { hTriggerClasses[kCINT7]->Fill("CENTNOTRD",1); }
      else if(fClassesFired->String().Contains("CENT")) { hTriggerClasses[kCINT7]->Fill("CENT",1); }
      if(fClassesFired->String().Contains("MUFAST")) hTriggerClasses[kCINT7]->Fill("MUFAST",1);
    }
    if(bIsCVHMSH2)
    {
      hPFPUCounter->Fill("CVHMSH2-B",1);
      hEventPerRunCounter->Fill(Form("%d",fRunNumber),"CVHMSH2",1);
      if(bPFPU && bPFPUFilled) { hPFPUCounter->Fill("CVHMSH2 && PFPU && filled",1); }
    }

    bIsCVHMSH2 = bIsCVHMSH2 && !bPFPU;

    // number of online/oflline FAST-ORs
    iNumFOonline = fFiredChipMapFO->CountBits(400);
    iNumFOoffline = fFiredChipMap->CountBits(400);

    // filling common histos
    FillCommonHistos(kAll);
    if(bIsCINT7) { FillCommonHistos(kCINT7); }
    if(fPhysSelPassed && bIsCINT7)  { FillCommonHistos(kCINT7_PhysSel); }
    if(bIsCVHMSH2) { FillCommonHistos(kCVHMSH2); }
    if(fPhysSelPassed && bIsCVHMSH2)  { FillCommonHistos(kCVHMSH2_PhysSel); }

    // skipping events not marker as MB
    if(!bIsCINT7) { continue; }
    if(!bIsCVHMSH2) { continue; }
  }

  fOutputFile->cd();
  hEventCounter->Write();
  hEventPerRunCounter->Write();
  hPFPUCounter->Write();
  for(Int_t iType(0); iType < iNumTypes; ++iType) { listOut[iType]->Write(Form("list_%s",sTypeLabels[iType].Data()),TObject::kSingleKey); }

  return;
}
// =====================================================================================================================
Bool_t CheckTriggerConsistency()
{
  Bool_t bIsCINT7 = fClassesFired->String().Contains("CINT7-B");
  Bool_t bIsCVHMSH2 = fClassesFired->String().Contains("CVHMSH2-B");

  Bool_t bIs0SH2 = fFiredTriggerInputs->String().Contains("0SH2");
  Bool_t bIs0VHM = fFiredTriggerInputs->String().Contains("0VHM");


  Bool_t bConsistent = kFALSE;
  if(bIsCVHMSH2 && bIs0VHM && bIs0SH2) { bConsistent = kTRUE; }

  if(bIsCVHMSH2 && !bConsistent)
  {
    printf("(CVHMSH2 %d | CINT7 %d | 0SH2 %d | 0VHM %d) %d \n",bIsCVHMSH2,bIsCINT7,bIs0SH2,bIs0VHM,bConsistent);
  }

  if(bIsCINT7) { hEventCounter->Fill("CINT7-B",1); }
  if(bIsCVHMSH2) { hEventCounter->Fill("CVHMSH2-B",1); }
  if(bIs0VHM) { hEventCounter->Fill("0VHM",1); }
  if(bIs0SH2) { hEventCounter->Fill("0SH2",1); }
  if(bIs0SH2 && bIs0VHM) { hEventCounter->Fill("0SH2 && 0VHM",1); }
  if(bConsistent) { hEventCounter->Fill("Consistent",1); }

  return bConsistent;
}
// =====================================================================================================================
void CreateHistos()
{
  hEventCounter = new TH1D("hEventCounter", "hEventCounter", 1,0,1);
  hEventPerRunCounter = new TH2D("hEventPerRunCounter", "hEventPerRunCounter", 1,0,1, 1,0,1);
  hPFPUCounter = new TH1D("hPFPUCounter", "hPFPUCounter", 1,0,1);


  for(Int_t iType(0); iType < iNumTypes; ++iType)
  {
    TString sLabel = sTypeLabels[iType];

    listOut[iType] = new TList();
    TList* list = listOut[iType];

    // 2D mult correlations
    Int_t iFOonlineBins = 100; Int_t iFOonlineLow = 0; Int_t iFOonlineHigh = 1000;
    Int_t iRefMultBins = 100; Int_t iRefMultLow = 0; Int_t iRefMultHigh = 2000;
    Int_t iNtrkletsBins = 100; Int_t iNtrkletsLow = 0; Int_t iNtrkletsHigh = 4000;

    if(iType != kCINT7)
    {
      iFOonlineBins = 800; iFOonlineLow = 0; iFOonlineHigh = 800;
      iRefMultBins = 200;  iRefMultLow = 0;  iRefMultHigh = 200;
      iNtrkletsBins = 200; iNtrkletsLow = 0; iNtrkletsHigh = 200;
    }

    // trigger classes
    hTriggerClasses[iType] = new TH1D(Form("hTriggerClasses_%s",sLabel.Data()), Form("hTriggerClasses_%s;",sLabel.Data()), 1,0,1);
    list->Add(hTriggerClasses[iType]);
    // Distributions
    hDistFOonlinePerRun[iType] = new TH2D(Form("hDistFOonlinePerRun_%s",sLabel.Data()), Form("hDistFOonlinePerRun%s; FO online; Run",sLabel.Data()), iFOonlineBins,iFOonlineLow,iFOonlineHigh, 1,0,1);
    list->Add(hDistFOonlinePerRun[iType]);
    hDistFOofflinePerRun[iType] = new TH2D(Form("hDistFOofflinePerRun_%s",sLabel.Data()), Form("hDistFOofflinePerRun%s; FO offline; Run",sLabel.Data()), iFOonlineBins,iFOonlineLow,iFOonlineHigh, 1,0,1);
    list->Add(hDistFOofflinePerRun[iType]);
    hDistNtrklets[iType] = new TH1D(Form("hDistNtrklets_%s",sLabel.Data()), Form("hDistNtrklets_%s; N_{tracklets}",sLabel.Data()), iNtrkletsBins, iNtrkletsLow, iNtrkletsHigh);
    list->Add(hDistNtrklets[iType]);
    hDistNtrks[iType] = new TH1D(Form("hDistNtrks_%s",sLabel.Data()), Form("hDistNtrks_%s; N_{tracks};",sLabel.Data()), 100,0,5000);
    list->Add(hDistNtrks[iType]);
    hDistRefMult08[iType] = new TH1D(Form("hDistRefMult08_%s",sLabel.Data()), Form("hDistRefMult08_%s; RefMult08;",sLabel.Data()), iRefMultBins,iRefMultLow,iRefMultHigh);
    list->Add(hDistRefMult08[iType]);
    hDistFOonline[iType] = new TH1D(Form("hDistFOonline_%s",sLabel.Data()), Form("hDistFOonline_%s; FO online; Counts",sLabel.Data()), iFOonlineBins,iFOonlineLow,iFOonlineHigh);
    list->Add(hDistFOonline[iType]);
    hDistFOoffline[iType] = new TH1D(Form("hDistFOoffline_%s",sLabel.Data()), Form("hDistFOoffline_%s; FO offline; Counts",sLabel.Data()), iFOonlineBins,iFOonlineLow,iFOonlineHigh);
    list->Add(hDistFOoffline[iType]);


    h2_FOonline_Ntrklets[iType] = new TH2D(Form("h2_FOonline_Ntrklets_%s",sLabel.Data()), Form("h2_FOonline_Ntrklets_%s; FO online; N_{tracklets}",sLabel.Data()), iFOonlineBins,iFOonlineLow,iFOonlineHigh, iNtrkletsBins, iNtrkletsLow, iNtrkletsHigh);
    list->Add(h2_FOonline_Ntrklets[iType]);
    h2_FOonline_Ntrks[iType] = new TH2D(Form("h2_FOonline_Ntrks_%s",sLabel.Data()), Form("h2_FOonline_Ntrks_%s; FO online; N_{tracks}",sLabel.Data()), iFOonlineBins,iFOonlineLow,iFOonlineHigh, 100,0,5000);
    list->Add(h2_FOonline_Ntrks[iType]);
    h2_FOonline_RefMult08[iType] = new TH2D(Form("h2_FOonline_RefMult08_%s",sLabel.Data()), Form("h2_FOonline_RefMult08_%s; FO online; RefMult08",sLabel.Data()), iFOonlineBins,iFOonlineLow,iFOonlineHigh, iRefMultBins,iRefMultLow,iRefMultHigh);
    list->Add(h2_FOonline_RefMult08[iType]);

  }

  return;
}
// =====================================================================================================================
void FillCommonHistos(eEventType iType)
{
  hDistNtrklets[iType]->Fill(fNumTracklets);
  hDistNtrks[iType]->Fill(fNumTracks);
  hDistRefMult08[iType]->Fill(fNumTracksRefMult08);

  hDistFOonline[iType]->Fill(iNumFOonline);
  hDistFOoffline[iType]->Fill(iNumFOoffline);

  hDistFOonlinePerRun[iType]->Fill(iNumFOonline, Form("%d",fRunNumber), 1);
  hDistFOofflinePerRun[iType]->Fill(iNumFOoffline, Form("%d",fRunNumber), 1);

  // 2D mult correlations
  h2_FOonline_Ntrklets[iType]->Fill(iNumFOonline,fNumTracklets);
  h2_FOonline_Ntrks[iType]->Fill(iNumFOonline,fNumTracks);
  h2_FOonline_RefMult08[iType]->Fill(iNumFOonline,fNumTracksRefMult08);

  return;
}
// =====================================================================================================================
Bool_t LoadTTreeVars()
{
  if(!eventTree) { printf(" ERROR-LoadTTreeVars: no eventTree\n"); return kFALSE; }

  // list of branches in TTree
  eventTree->SetBranchAddress("fClassesFired",&fClassesFired);
  eventTree->SetBranchAddress("fPhysSelDecision",&fPhysSelDecision);
  eventTree->SetBranchAddress("fPhysSelPassed",&fPhysSelPassed);
  eventTree->SetBranchAddress("fEventCutsPassed",&fEventCutsPassed);
  eventTree->SetBranchAddress("fEventInFile",&fEventInFile);
  eventTree->SetBranchAddress("fRunNumber",&fRunNumber);
  eventTree->SetBranchAddress("fPeriod",&fPeriod);
  eventTree->SetBranchAddress("fOrbit",&fOrbit);
  eventTree->SetBranchAddress("fBC",&fBC);
  eventTree->SetBranchAddress("fL0inputs",&fL0inputs);
  eventTree->SetBranchAddress("fL1inputs",&fL1inputs);
  eventTree->SetBranchAddress("fFiredTriggerInputs",&fFiredTriggerInputs);
  eventTree->SetBranchAddress("fIR1",&fIR1);
  eventTree->SetBranchAddress("fIR2",&fIR2);
  eventTree->SetBranchAddress("fNumContrSPD",&fNumContrSPD);
  eventTree->SetBranchAddress("fNumTracklets",&fNumTracklets);
  eventTree->SetBranchAddress("fNumTracksRefMult08",&fNumTracksRefMult08);
  eventTree->SetBranchAddress("fFiredChipMap",&fFiredChipMap);
  eventTree->SetBranchAddress("fFiredChipMapFO",&fFiredChipMapFO);
  eventTree->SetBranchAddress("fNumITSCls",fNumITSCls);
  eventTree->SetBranchAddress("fTriggerMaskTOF",fTriggerMaskTOF);
  eventTree->SetBranchAddress("fV0ATotMult",&fV0ATotMult);
  eventTree->SetBranchAddress("fV0CTotMult",&fV0CTotMult);
  eventTree->SetBranchAddress("fV0ATriggerCharge",&fV0ATriggerCharge);
  eventTree->SetBranchAddress("fV0CTriggerCharge",&fV0CTriggerCharge);
  eventTree->SetBranchAddress("fV0ATime",&fV0ATime);
  eventTree->SetBranchAddress("fV0CTime",&fV0CTime);
  // eventTree->SetBranchAddress("fV0AMult",fV0AMult);
  // eventTree->SetBranchAddress("fV0CMult",fV0CMult);
  eventTree->SetBranchAddress("fV0PastFuturePileUp",&fV0PastFuturePileUp);
  eventTree->SetBranchAddress("fV0PastFutureFilled",&fV0PastFutureFilled);
  // eventTree->SetBranchAddress("fV0ATriggerBB",fV0ATriggerBB);
  // eventTree->SetBranchAddress("fV0ATriggerBG",fV0ATriggerBG);
  // eventTree->SetBranchAddress("fV0CTriggerBG",fV0ATriggerBG);
  // eventTree->SetBranchAddress("fV0CTriggerBB",fV0ATriggerBB);
  eventTree->SetBranchAddress("fV0AFlagsBB",&fV0AFlagsBB);
  eventTree->SetBranchAddress("fV0CFlagsBB",&fV0CFlagsBB);
  eventTree->SetBranchAddress("fV0AFlagsBG",&fV0AFlagsBG);
  eventTree->SetBranchAddress("fV0CFlagsBG",&fV0CFlagsBG);
  eventTree->SetBranchAddress("fV0ADecision",&fV0ADecision);
  eventTree->SetBranchAddress("fV0CDecision",&fV0CDecision);
  eventTree->SetBranchAddress("fVtxX",&fVtxX);
  eventTree->SetBranchAddress("fVtxY",&fVtxY);
  eventTree->SetBranchAddress("fVtxZ",&fVtxZ);
  eventTree->SetBranchAddress("fVtxTPC",&fVtxTPC);
  eventTree->SetBranchAddress("fNumTracks",&fNumTracks);

  return kTRUE;
}
