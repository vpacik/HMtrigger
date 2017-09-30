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


void AddArrToHist(TArrayD* arr, TH1D* hist);
void AddArrToHist2D(TArrayD* arr, TH2D* hist, Int_t mult);
TH1D* GetEfficiency(TH1D* trigger, TH1D* mb);
Double_t GetTurnOn(TH1D* mult, Int_t threshold);


void ProcessTrigger(Int_t iNumEventsToProcess = -1)
{
  // parameters
  // const char* sInputFile = "/Users/vpacik/NBI/triggerHMstudies/newTask/running/16k/AnalysisResults_BK.root";
  const char* sInputFile = "/Users/vpacik/NBI/triggerHMstudies/newTask/running/15l-2/AnalysisResults.root";
  // const char* sInputFile = "/Users/vpacik/NBI/triggerHMstudies/newTask/AnalysisResults.root";

  TString sOutputPath = "/Users/vpacik/NBI/triggerHMstudies/newTask/running/15l-2/";
  // TString sOutputPath = "/Users/vpacik/NBI/triggerHMstudies/newTask/";

  TString sOutFile = "Processed_V0PFP.root";

  Bool_t bPeriod15l = kTRUE; // LHC15l flag for checking 0SH2 && VHM trigger classes
  Bool_t bCheckVHMSPDconsistency = kTRUE; // if true fFiredTriggerInputs are checked (available in newer implementeation)

  // loading input (filtered) TTree & list with histos
  TFile* fInputFile = new TFile(sInputFile,"READ");
  if(!fInputFile->IsOpen()) return;

  fInputFile->cd("FilterTrig");

  TList* histos = (TList*) gDirectory->Get("histos");
  if (!histos) { printf(" ERROR: TList with histos is not there! \n"); }

  // TH1F* fhEventCounter = (TH1F*) histos->FindObject("fhEventCounter");
  // if(fhEventCounter) { fhEventCounter->Draw(); } else { printf(" ERROR: 'fhEventCounter' is not there! \n"); }

  TTree* eventTree = (TTree*) gDirectory->Get("events");
  if (!eventTree) { printf(" ERROR: Tree is not there! \n"); }

  // list of branches in TTree
  TObjString*         fClassesFired = 0x0; // list of fired trigger classes
  UInt_t              fPhysSelDecision; // AliPhysicsSelection decision
  Bool_t              fPhysSelPassed; // AliPhysicsSelection decision (pass/reject)
  Bool_t              fEventCutsPassed; // AliEventCuts selection decision (pass/reject)
  TObjString*         fChunkFileName = 0x0; // current file name
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
  Int_t               fNumTracklets; // number of tracklets
  TBits*              fFiredChipMap = 0x0; // map of fired chips (at least one cluster)
  TBits*              fFiredChipMapFO = 0x0; // map of fired FastOr chips
  Int_t               fNumITSCls[6]; // number of ITS clusters per layer
  Int_t               fTriggerMaskTOF[72]; // TOF trigger mask array
  Float_t             fV0ATotMult; // total multiplicity in V0A
  Float_t             fV0CTotMult; // total multiplicity in V0C
  Float_t             fV0AMult[32]; // multiplicity in V0A cells
  Float_t             fV0CMult[32];  // multiplicity in V0C cells
  Float_t             fV0ATime; // average time in V0A
  Float_t             fV0CTime; // average time in V0C
  Bool_t              fV0ATriggerBB[32]; // offline beam-beam flag in V0A cells
  Bool_t              fV0CTriggerBB[32]; // ffline beam-beam flag in V0C cells
  Bool_t              fV0ATriggerBG[32]; // offline beam-gas flag in V0A cells
  Bool_t              fV0CTriggerBG[32]; // offline beam-gas flag in V0C cells
  UInt_t              fV0AFlagsBB; // Number of total online beam-beam flags in V0A
  UInt_t              fV0CFlagsBB; // Number of total online beam-beam flags in V0C
  UInt_t              fV0AFlagsBG; // Number of total online beam-gas flags in V0A
  UInt_t              fV0CFlagsBG; // Number of total online beam-gas flags in V0C
  Bool_t              fV0FlagBB[64]; // online beam-beam flag in V0 (V0C 0-31, V0A 32-63) cells
  Bool_t              fV0FlagBG[64]; // online beam-gas flag in V0 (V0C 0-31, V0A 32-63) cells
  Bool_t              fV0FlagPFBB[64][21]; // beam-beam flag for V0 cells and clocks
  Bool_t              fV0FlagPFBG[64][21]; // beam-gas flag for V0 cells and clocks
  Char_t              fV0ADecision; // final V0A decision
  Char_t              fV0CDecision;  // final V0C decision
  Double_t            fVtxX; // primary vertex x-coordinate
  Double_t            fVtxY; // primary vertex y-coordinate
  Double_t            fVtxZ; // primary vertex z-coordinate
  Bool_t              fVtxTPC; // primary vertex reconstructed with TPC (not SPDVertex)
  Int_t               fNumTracks; // number of tracks
  TArrayD*            fTracksPt = 0x0; // binned tracks pt

  eventTree->SetBranchAddress("fClassesFired",&fClassesFired);
  eventTree->SetBranchAddress("fPhysSelDecision",&fPhysSelDecision);
  eventTree->SetBranchAddress("fPhysSelPassed",&fPhysSelPassed);
  eventTree->SetBranchAddress("fEventCutsPassed",&fEventCutsPassed);
  eventTree->SetBranchAddress("fChunkFileName",&fChunkFileName);
  eventTree->SetBranchAddress("fEventInFile",&fEventInFile);
  eventTree->SetBranchAddress("fRunNumber",&fRunNumber);
  eventTree->SetBranchAddress("fPeriod",&fPeriod);
  eventTree->SetBranchAddress("fOrbit",&fOrbit);
  eventTree->SetBranchAddress("fBC",&fBC);
  eventTree->SetBranchAddress("fL0inputs",&fL0inputs);
  eventTree->SetBranchAddress("fL1inputs",&fL1inputs);
  if(bCheckVHMSPDconsistency) eventTree->SetBranchAddress("fFiredTriggerInputs",&fFiredTriggerInputs);
  eventTree->SetBranchAddress("fIR1",&fIR1);
  eventTree->SetBranchAddress("fIR2",&fIR2);
  eventTree->SetBranchAddress("fNumTracklets",&fNumTracklets);
  eventTree->SetBranchAddress("fFiredChipMap",&fFiredChipMap);
  eventTree->SetBranchAddress("fFiredChipMapFO",&fFiredChipMapFO);
  eventTree->SetBranchAddress("fNumITSCls",fNumITSCls);
  eventTree->SetBranchAddress("fTriggerMaskTOF",fTriggerMaskTOF);
  eventTree->SetBranchAddress("fV0ATotMult",&fV0ATotMult);
  eventTree->SetBranchAddress("fV0CTotMult",&fV0CTotMult);
  eventTree->SetBranchAddress("fV0ATime",&fV0ATime);
  eventTree->SetBranchAddress("fV0CTime",&fV0CTime);
  eventTree->SetBranchAddress("fV0AMult",fV0AMult);
  eventTree->SetBranchAddress("fV0CMult",fV0CMult);
  eventTree->SetBranchAddress("fV0ATriggerBB",fV0ATriggerBB);
  eventTree->SetBranchAddress("fV0ATriggerBG",fV0ATriggerBG);
  eventTree->SetBranchAddress("fV0CTriggerBG",fV0ATriggerBG);
  eventTree->SetBranchAddress("fV0CTriggerBB",fV0ATriggerBB);
  eventTree->SetBranchAddress("fV0AFlagsBB",&fV0AFlagsBB);
  eventTree->SetBranchAddress("fV0CFlagsBB",&fV0CFlagsBB);
  eventTree->SetBranchAddress("fV0AFlagsBG",&fV0AFlagsBG);
  eventTree->SetBranchAddress("fV0CFlagsBG",&fV0CFlagsBG);
  eventTree->SetBranchAddress("fV0FlagBB",fV0FlagBB);
  eventTree->SetBranchAddress("fV0FlagBG",fV0FlagBG);
  eventTree->SetBranchAddress("fV0FlagPFBB",fV0FlagPFBB);
  eventTree->SetBranchAddress("fV0FlagPFBG",fV0FlagPFBG);
  eventTree->SetBranchAddress("fV0ADecision",&fV0ADecision);
  eventTree->SetBranchAddress("fV0CDecision",&fV0CDecision);
  eventTree->SetBranchAddress("fVtxX",&fVtxX);
  eventTree->SetBranchAddress("fVtxY",&fVtxY);
  eventTree->SetBranchAddress("fVtxZ",&fVtxZ);
  eventTree->SetBranchAddress("fVtxTPC",&fVtxTPC);
  eventTree->SetBranchAddress("fNumTracks",&fNumTracks);
  eventTree->SetBranchAddress("fTracksPt",&fTracksPt);

  // tree ready

  // ### Histograms ###

  TString sLabelEventCounter[] = {"Input", "INT7", "SH2", "VHM", "SH2 & VHM", "INT7 & SH2 & VHM", "VHMSH2", "INT7 & SH2 & VHM & !VHMSH2","INT7 & VHMSH2 & !(SH2 || VHM)"};
  Int_t iNumBinsEventCounter = sizeof(sLabelEventCounter)/sizeof(sLabelEventCounter[0]);

  // TH1D* hEventCounter = new TH1D("hEventCounter","Event counter",1,0,1);
  TH1D* hEventCounter = new TH1D("hEventCounter","Event counter",iNumBinsEventCounter,0,iNumBinsEventCounter);
  for(Int_t i(0); i < iNumBinsEventCounter; i++) { hEventCounter->GetXaxis()->SetBinLabel(i+1,sLabelEventCounter[i].Data()); }
  TH2D* h2EventCounter = new TH2D("h2EventCounter","Event counter", 1,0,1, 1,0,1);
  // multiplicity dist
  TH1D* hEventMultINT7 = new TH1D("hEventMultINT7","Event multiplicity (INT7); # tracklets",200,0,200);
  TH1D* hEventMultINT7PhysSel = new TH1D("hEventMultINT7PhysSel","Event multiplicity (INT7 && PhysSel); # tracklets",200,0,200);
  TH1D* hEventMultINT7PhysSelEventCuts = new TH1D("hEventMultINT7PhysSelEventCuts","Event multiplicity (INT7 && PhysSel && EventCuts); # tracklets",200,0,200);
  TH1D* hEventMultCVHMSH2 = new TH1D("hEventMultCVHMSH2","Event multiplicity (CVHMSH2); # tracklets",200,0,200);
  TH1D* hEventMultCVHMSH2PhysSel = new TH1D("hEventMultCVHMSH2PhysSel","Event multiplicity (CVHMSH2 && PhysSel); # tracklets",200,0,200);
  TH1D* hEventMultCVHMSH2PhysSelEventCuts = new TH1D("hEventMultCVHMSH2PhysSelEventCuts","Event multiplicity (CVHMSH2 && PhysSel && EventCuts); # tracklets",200,0,200);
  // FastOR
  TH2D* h2FOROnOffINT7 = new TH2D("h2FOROnOffINT7","FastOr offline x online (INT7); FOR offline; FOR online", 800,0,800, 800,0,800);
  TH2D* h2FOROnOffINT7PhysSel = new TH2D("h2FOROnOffINT7PhysSel","FastOr offline x online (INT7 && PhysSel); FOR offline; FOR online", 800,0,800, 800,0,800);
  TH2D* h2FOROnOffVHMSH2 = new TH2D("h2FOROnOffVHMSH2","FastOr offline x online (VHMSH2); FOR offline; FOR online", 800,0,800, 800,0,800);
  TH2D* h2FOROnOffVHMSH2PhysSel = new TH2D("h2FOROnOffVHMSH2PhysSel","FastOr offline x online (VHMSH2 && PhysSel); FOR offline; FOR online", 800,0,800, 800,0,800);
  TH2D* h2FOROffTrackletsINT7PhysSel = new TH2D("h2FOROffTrackletsINT7PhysSel","tracklets x FastOR (INT7 && PhysSel); # tracklets; FOR offline", 200,0,200, 200,0,200);
  TH2D* h2FOROffTrackletsVHMSH2PhysSel = new TH2D("h2FOROffTrackletsVHMSH2PhysSel","tracklets x FastOR (VHMSH2 && PhysSel); # tracklets; FOR offline;", 200,0,200, 200,0,200);
  TH2D* h2TrackletsTracksINT7PhysSel = new TH2D("h2TrackletsTracksINT7PhysSel","tracklets x ESD tracks (INT7 && PhysSel); # tracklets; # ESD tracks", 200,0,200, 2000,0,2000);
  TH2D* h2TrackletsTracksVHMSH2PhysSel = new TH2D("h2TrackletsTracksVHMSH2PhysSel","tracklets x ESD tracks (VHMSH2 && PhysSel); # tracklets; # ESD tracks", 200,0,200, 2000,0,2000);
  // pt dist
  TH2D* h2PtINT7 = new TH2D("h2PtINT7","Pt dist (INT7 && PhysSel); track pt(GeV/c); # tracklets", 100,0,100, 200,0,200);
  TH2D* h2PtINT7PhysSel = new TH2D("h2PtINT7PhysSel","Pt dist (INT7 && PhysSel); track pt(GeV/c); # tracklets", 100,0,100, 200,0,200);
  TH2D* h2PtVHMSH2 = new TH2D("h2PtVHMSH2","Pt dist (VHMSH2 && PhysSel); track pt(GeV/c); # tracklets", 100,0,100, 200,0,200);
  TH2D* h2PtVHMSH2PhysSel = new TH2D("h2PtVHMSH2PhysSel","Pt dist (VHMSH2 && PhysSel); track pt(GeV/c); # tracklets", 100,0,100, 200,0,200);

  TH1D* hPtCINT7All = new TH1D("hPtCINT7All","Pt dist (CINT7 | all); track pt (GeV/c)",100,0,100);
  TH1D* hPtCINT7AllPhysSel = new TH1D("hPtCINT7AllPhysSel","Pt dist (CINT7 && PhysSel | all); track pt (GeV/c)",100,0,100);
  TH1D* hPtCINT7AllPhysSelEventCuts = new TH1D("hPtCINT7AllPhysSelEventCuts","Pt dist (CINT7 && PhysSel && EventCuts | all); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2All = new TH1D("hPtCVHMSH2All","Pt dist (CVHMSH2 | all); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2AllPhysSel = new TH1D("hPtCVHMSH2AllPhysSel","Pt dist (CVHMSH2 && PhysSel | all); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2AllPhysSelEventCuts = new TH1D("hPtCVHMSH2AllPhysSelEventCuts","Pt dist (CVHMSH2 && PhysSel && EventCuts | all); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs90 = new TH1D("hPtCVHMSH2Thrs90","Pt dist (CVHMSH2 | above 90%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs90PhysSel = new TH1D("hPtCVHMSH2Thrs90PhysSel","Pt dist (CVHMSH2 && PhysSel | above 90%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs90PhysSelEventCuts = new TH1D("hPtCVHMSH2Thrs90PhysSelEventCuts","Pt dist (CVHMSH2 && PhysSel && EventCuts | above 90%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs95 = new TH1D("hPtCVHMSH2Thrs95","Pt dist (CVHMSH2 | above 95%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs95PhysSel = new TH1D("hPtCVHMSH2Thrs95PhysSel","Pt dist (CVHMSH2 && PhysSel | above 95%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs95PhysSelEventCuts = new TH1D("hPtCVHMSH2Thrs95PhysSelEventCuts","Pt dist (CVHMSH2 && PhysSel && EventCuts | above 95%); track pt (GeV/c)",100,0,100);

  // variables inside loop
  const char* sRunNumber = "";

  Double_t dNumFORon = -1.;
  Double_t dNumFORof = -1.;

  Bool_t bIsCINT7 = kFALSE;
  Bool_t bIs0VHM = kFALSE;
  Bool_t bIs0SH2 = kFALSE;
  Bool_t bIsCVHMSH2 = kFALSE;

  // looping over entries
  Int_t numEvents = eventTree->GetEntriesFast();
  printf("Found %d events\nTo be processed: %d\n",numEvents, (iNumEventsToProcess ? iNumEventsToProcess : numEvents) );

  // Counters
  Int_t iNumEventsINT7 = 0;
  Int_t iNumEventsVHMSH2 = 0;
  Int_t iNumEventsINT7PhysSel = 0;
  Int_t iNumEventsVHMSH2PhysSel = 0;


  for (Int_t i(0); i < numEvents; i++)
  {
    // if(i > 5) break;
    if( iNumEventsToProcess >= 0 && i > iNumEventsToProcess) break;
    if( (i % 100000) == 0) printf("=== Procesed %d / %d events === \n",i,numEvents);

    eventTree->GetEvent(i);
    sRunNumber = Form("%d",fRunNumber);

    hEventCounter->Fill("Input",1);
    h2EventCounter->Fill(sRunNumber,"Input",1);

    dNumFORon = fFiredChipMapFO->CountBits(400);
    dNumFORof = fFiredChipMap->CountBits(400);


    if(bCheckVHMSPDconsistency)
    {
      bIs0SH2 = fFiredTriggerInputs->String().Contains("0SH2");
      bIs0VHM = fFiredTriggerInputs->String().Contains("0VHM");
    }

    // past-future protection (for lhc15l)

    Bool_t bV0PFPileup = kFALSE;

    if(bPeriod15l)
    {
      Int_t fVIRBBAflags = 10;
      Int_t fVIRBBCflags = 10;
      Int_t fVIRBGAflags = 33;
      Int_t fVIRBGCflags = 33;

      Bool_t vir[21] = {0};

      for (Int_t bc = 0; bc < 21; bc++)
      {
        UChar_t nBBA = 0;
        UChar_t nBBC = 0;
        UChar_t nBGA = 0;
        UChar_t nBGC = 0;

        if (fVIRBBAflags<33) for (Int_t j=0;j<32;j++) nBBA += fV0FlagPFBB[j+32][bc];
        if (fVIRBBCflags<33) for (Int_t j=0;j<32;j++) nBBC += fV0FlagPFBB[j][bc];
        if (fVIRBGAflags<33) for (Int_t j=0;j<32;j++) nBGA += fV0FlagPFBG[j+32][bc];
        if (fVIRBGCflags<33) for (Int_t j=0;j<32;j++) nBGC += fV0FlagPFBG[j][bc];

        vir[bc] |= nBBA>=fVIRBBAflags;
        vir[bc] |= nBBC>=fVIRBBCflags;
        vir[bc] |= nBGA>=fVIRBGAflags;
        vir[bc] |= nBGC>=fVIRBGCflags;
      }

      Int_t bcMin = 3; //10 - fNBCsFuture + bcMod4;
      Int_t bcMax = 17; //10 + fNBCsPast + bcMod4;
      for (Int_t bc=bcMin;bc<=bcMax;bc++)
      {
        if (bc==10) continue; // skip current bc
        if (bc < 0) continue;
        if (bc >20) continue;
        if (vir[bc]) bV0PFPileup = kTRUE;
      }
    }



    // trigger classes check
    bIsCINT7 = fClassesFired->String().Contains("CINT7-B");
    if(bPeriod15l) bIsCVHMSH2 = (bIs0VHM && bIs0SH2 && bIsCINT7);
    else bIsCVHMSH2 = fClassesFired->String().Contains("CVHMSH2-B");


    // if(bPeriod15l)
    // {
    //   // From Michele analysis.C macro : apparently definition changed from 15l -> 16k (not working)
    //   // bIs0VHM = fL0inputs & 1<<10;
    //   // bIs0SH2 = fL0inputs & 1<<13;
    // }

    // by hand implementation (true? apparently SH2 requires at least 1 BG flag)
    // bIs0VHM = (fV0AFlagsBB >=1 && fV0CFlagsBB >=1 && fV0AFlagsBG <=15 && fV0CFlagsBG <=3);
    // bIs0SH2 = (dNumFORon >= 70);

    if(bIsCINT7) { hEventCounter->Fill("INT7",1); iNumEventsINT7++; }
    if(bIs0SH2) { hEventCounter->Fill("SH2",1); }
    if(bIs0VHM) { hEventCounter->Fill("VHM",1); }
    if(bIs0SH2 && bIs0VHM) { hEventCounter->Fill("SH2 & VHM",1); }
    if(bIsCINT7 && bIs0SH2 && bIs0VHM) { hEventCounter->Fill("INT7 & SH2 & VHM",1); }
    if(bIsCVHMSH2) { hEventCounter->Fill("VHMSH2",1); iNumEventsVHMSH2++; }

    if(bCheckVHMSPDconsistency)
    {
      if(bIsCINT7 && bIs0SH2 && bIs0VHM && !bIsCVHMSH2)
      {
        hEventCounter->Fill("INT7 & SH2 & VHM & !VHMSH2",1);
        // printf(" !!! INT7 && !SH2: #FO %d (on) %d (off) | BB %d (A) %d (C) | BG %d (A) %d (C)\n",(Short_t)dNumFORon,(Short_t)dNumFORof,fV0AFlagsBB,fV0CFlagsBB,fV0AFlagsBG,fV0CFlagsBG);
        // printf(" L0 inputs: %d | SH2 %d | VHM %d | VHMSH2 %d | INT7 %d\n", fL0inputs, bIs0SH2, bIs0VHM, bIsCVHMSH2, bIsCINT7);
      }

      if(bIsCINT7 && bIsCVHMSH2 && !(bIs0SH2 || bIs0VHM))
      {
        hEventCounter->Fill("INT7 & VHMSH2 & !(SH2 || VHM)",1);
        // printf(" !!! INT7 && !SH2: #FO %d (on) %d (off) | BB %d (A) %d (C) | BG %d (A) %d (C)\n",(Short_t)dNumFORon,(Short_t)dNumFORof,fV0AFlagsBB,fV0CFlagsBB,fV0AFlagsBG,fV0CFlagsBG);
        // printf(" L0 inputs: %d | SH2 %d | VHM %d | VHMSH2 %d | INT7 %d\n", fL0inputs, bIs0SH2, bIs0VHM, bIsCVHMSH2, bIsCINT7);
      }
    }

    if(!bIsCINT7) continue; // SPDHM should be subset of INT7 (MB) logic


    // Purity(multiplicity) = #Events after Phys.Selection / #Events

    if(bIsCINT7)
    {
      h2EventCounter->Fill(sRunNumber,"INT7",1);
      hEventMultINT7->Fill(fNumTracklets);
      AddArrToHist2D(fTracksPt,h2PtINT7,fNumTracklets);
      // AddArrToHist(fTracksPt,hPtCINT7All);
      h2FOROnOffINT7->Fill(dNumFORof,dNumFORon);

      if(fPhysSelPassed)
      {
        h2EventCounter->Fill(sRunNumber,"INT7 && PhysSel",1);
        iNumEventsINT7PhysSel++;
        hEventMultINT7PhysSel->Fill(fNumTracklets);
        AddArrToHist2D(fTracksPt,h2PtINT7PhysSel,fNumTracklets);
        // AddArrToHist(fTracksPt,hPtCINT7AllPhysSel);
        h2FOROnOffINT7PhysSel->Fill(dNumFORof,dNumFORon);
        h2FOROffTrackletsINT7PhysSel->Fill(fNumTracklets,dNumFORof);
        h2TrackletsTracksINT7PhysSel->Fill(fNumTracklets,fNumTracks);

      }

      if(fPhysSelPassed && fEventCutsPassed)
      {
        h2EventCounter->Fill(sRunNumber,"INT7 && PhysSel && EventCuts",1);
        hEventMultINT7PhysSelEventCuts->Fill(fNumTracklets);
        // AddArrToHist(fTracksPt,hPtCINT7AllPhysSelEventCuts);
      }
    }

    if(bIsCVHMSH2)
    {
      h2EventCounter->Fill(sRunNumber,"VHMSH2",1);

      if(bV0PFPileup) continue;
      
      h2EventCounter->Fill(sRunNumber,"VHMSH2 (pass. V0PFP)",1);

      hEventMultCVHMSH2->Fill(fNumTracklets);
      AddArrToHist2D(fTracksPt,h2PtVHMSH2,fNumTracklets);
      // AddArrToHist(fTracksPt,hPtCVHMSH2All);
      h2FOROnOffVHMSH2->Fill(dNumFORof,dNumFORon);

      if(fPhysSelPassed)
      {
        h2EventCounter->Fill(sRunNumber,"VHMSH2 && PhysSel",1);
        iNumEventsVHMSH2PhysSel++;
        hEventMultCVHMSH2PhysSel->Fill(fNumTracklets);
        AddArrToHist2D(fTracksPt,h2PtVHMSH2PhysSel,fNumTracklets);
        h2FOROnOffVHMSH2PhysSel->Fill(dNumFORof,dNumFORon);
        h2FOROffTrackletsVHMSH2PhysSel->Fill(fNumTracklets,dNumFORof);
        h2TrackletsTracksVHMSH2PhysSel->Fill(fNumTracklets,fNumTracks);

        // AddArrToHist(fTracksPt,hPtCVHMSH2AllPhysSel);
      }

      if(fPhysSelPassed && fEventCutsPassed)
      {
        h2EventCounter->Fill(sRunNumber,"VHMSH2 && PhysSel && EventCuts",1);
        hEventMultCVHMSH2PhysSelEventCuts->Fill(fNumTracklets);
        // AddArrToHist(fTracksPt,hPtCVHMSH2AllPhysSelEventCuts);
      }
    }

  }

  // obtaining rejection factors : # triggered events / # all (INT7) events
  Double_t dRejectionFactor = -1.;
  Double_t dRejectionFactorPhysSel = -1.;
  if(iNumEventsINT7 > 0.) dRejectionFactor = 1. * iNumEventsVHMSH2 / iNumEventsINT7;
  if(iNumEventsINT7PhysSel > 0.) dRejectionFactorPhysSel = 1. * iNumEventsVHMSH2PhysSel / iNumEventsINT7PhysSel;
  printf("Rejection factor (w/o PhysSel): %g\n",dRejectionFactor);
  printf("Rejection factor (with PhysSel): %g\n",dRejectionFactorPhysSel);
  // printf("After PS: %d (SH2) / %d (MB)\n",iNumEventsVHMSH2PhysSel,iNumEventsINT7PhysSel);

  TH1D* hRejectionFactor = new TH1D("hRejectionFactor","Rejection factors; ; Rejection factor", 2,0,2);
  hRejectionFactor->GetXaxis()->SetBinLabel(1,"w/o PhysSel");
  hRejectionFactor->GetXaxis()->SetBinLabel(2,"with PhysSel");
  hRejectionFactor->SetBinContent(1,dRejectionFactor);
  hRejectionFactor->SetBinContent(2,dRejectionFactorPhysSel);

  // obtaining Purity
  TH1D* hPurityINT7 = (TH1D*) hEventMultINT7->Clone("hPurityINT7");
  hPurityINT7->Divide(hEventMultINT7PhysSel,hEventMultINT7,1.,1.);
  hPurityINT7->SetTitle("Purity (INT7); # tracklets; PhysSel / INT7");

  TH1D* hPurityCVHMSH2 = (TH1D*) hEventMultCVHMSH2->Clone("hPurityCVHMSH2");
  hPurityCVHMSH2->Divide(hEventMultCVHMSH2PhysSel,hEventMultCVHMSH2,1.,1.);
  hPurityCVHMSH2->SetTitle("Purity (CVHMSH2); # tracklets; PhysSel / CVHMSH2");

  // obtaining efficiency, threshold, turn-on
  TH1D* hEffPhysSel = GetEfficiency(hEventMultCVHMSH2PhysSel,hEventMultINT7PhysSel);
  hEffPhysSel->SetNameTitle("hEffPhysSel","Efficiency CVHMSH2 / CINT7 (PhysSel); # tracklets; efficiency");
  Int_t iThrs90 = hEffPhysSel->FindFirstBinAbove(0.9,1) - 1;
  Int_t iThrs95 = hEffPhysSel->FindFirstBinAbove(0.95,1) - 1;
  printf("Eff. threshold multiplicity: %d tracklets (90%%) | %d tracklets (95%%)\n",iThrs90,iThrs95);
  Double_t dTurnOn90 = GetTurnOn(hEventMultCVHMSH2PhysSel,iThrs90);
  Double_t dTurnOn95 = GetTurnOn(hEventMultCVHMSH2PhysSel,iThrs95);
  printf("Turn-on: %g (90%%) | %g (95%%)\n",dTurnOn90,dTurnOn95);

  TH1I* hThrsPhysSel = new TH1I("hThrsPhysSel","Threshold",2,0,2);
  hThrsPhysSel->GetXaxis()->SetBinLabel(1,"90%");
  hThrsPhysSel->GetXaxis()->SetBinLabel(2,"95%");
  hThrsPhysSel->SetBinContent(1,iThrs90);
  hThrsPhysSel->SetBinContent(2,iThrs95);

  TH1D* hTurnonPhysSel = new TH1D("hTurnonPhysSel","TurnOn",2,0,2);
  hTurnonPhysSel->GetXaxis()->SetBinLabel(1,"90%");
  hTurnonPhysSel->GetXaxis()->SetBinLabel(2,"95%");
  hTurnonPhysSel->SetBinContent(1,dTurnOn90);
  hTurnonPhysSel->SetBinContent(2,dTurnOn95);

  // obtaining pt dep. turn-on
  // (out of TH2D)
  TH1D* hPtDistINT7PhysSelAll = (TH1D*) h2PtINT7PhysSel->ProjectionX("hPtDistINT7PhysSelAll",0,-1);
  TH1D* hPtDistINT7PhysSelThrs90 = (TH1D*) h2PtINT7PhysSel->ProjectionX("hPtDistINT7PhysSelThrs90",iThrs90,-1);
  TH1D* hPtDistINT7PhysSelThrs95 = (TH1D*) h2PtINT7PhysSel->ProjectionX("hPtDistINT7PhysSelThrs95",iThrs95,-1);
  TH1D* hPtDistVHMSH2PhysSelAll = (TH1D*) h2PtVHMSH2PhysSel->ProjectionX("hPtDistVHMSH2PhysSelAll",0,-1);
  TH1D* hPtDistVHMSH2PhysSelThrs90 = (TH1D*) h2PtVHMSH2PhysSel->ProjectionX("hPtDistVHMSH2PhysSelThrs90",iThrs90,-1);
  TH1D* hPtDistVHMSH2PhysSelThrs95 = (TH1D*) h2PtVHMSH2PhysSel->ProjectionX("hPtDistVHMSH2PhysSelThrs95",iThrs95,-1);

  // printf("#Events: SH2 %d (all) %g (90%%) %g (95%%)\n INT7 %d (all) %g (90%%) %g (95%%)\n",iNumEventsVHMSH2PhysSel, iNumEventsVHMSH2PhysSel*dTurnOn90, iNumEventsVHMSH2PhysSel*dTurnOn95, iNumEventsINT7PhysSel, iNumEventsINT7PhysSel, iNumEventsINT7PhysSel);


  // hPtDistINT7PhysSelAll->Scale(1.);
  // hPtDistINT7PhysSelAll->Scale(1./iNumEventsINT7PhysSel);
  // hPtDistINT7PhysSelThrs90->Scale(1./iNumEventsINT7PhysSel);
  // hPtDistINT7PhysSelThrs95->Scale(1./iNumEventsINT7PhysSel);
  // hPtDistVHMSH2PhysSelAll->Scale(1.);
  // hPtDistVHMSH2PhysSelAll->Scale(1./iNumEventsVHMSH2PhysSel);
  // hPtDistVHMSH2PhysSelThrs90->Scale(1./iNumEventsVHMSH2PhysSel);
  // hPtDistVHMSH2PhysSelThrs95->Scale(1./iNumEventsVHMSH2PhysSel);

  TH1D* hPtTurnOnAll = (TH1D*) hPtDistVHMSH2PhysSelAll->Clone("hPtTurnOnAll");
  hPtTurnOnAll->Divide(hPtDistVHMSH2PhysSelAll,hPtDistINT7PhysSelAll,1.,1.);
  hPtTurnOnAll->SetTitle("Pt turn-on (all); track pt (GeV/c); trigger/INT7");

  TH1D* hPtTurnOnThrs90 = (TH1D*) hPtDistVHMSH2PhysSelThrs90->Clone("hPtTurnOnThrs90");
  hPtTurnOnThrs90->Divide(hPtDistVHMSH2PhysSelThrs90,hPtDistINT7PhysSelThrs90,1.,1.);
  hPtTurnOnThrs90->SetTitle("Pt turn-on (90%); track pt (GeV/c); trigger/INT7");

  TH1D* hPtTurnOnThrs95 = (TH1D*) hPtDistVHMSH2PhysSelThrs95->Clone("hPtTurnOnThrs95");
  hPtTurnOnThrs95->Divide(hPtDistVHMSH2PhysSelThrs95,hPtDistINT7PhysSelThrs95,1.,1.);
  hPtTurnOnThrs95->SetTitle("Pt turn-on (95%); track pt (GeV/c); trigger/INT7");


  // Writing to output file
  TFile* fOutputFile = new TFile(Form("%s%s",sOutputPath.Data(),sOutFile.Data()),"RECREATE");
  if(fOutputFile->IsOpen())
  {
    hEventCounter->Write();
    h2EventCounter->Write();

    hEventMultINT7->Write();
    hEventMultINT7PhysSel->Write();
    hEventMultINT7PhysSelEventCuts->Write();
    hEventMultCVHMSH2->Write();
    hEventMultCVHMSH2PhysSel->Write();
    hEventMultCVHMSH2PhysSelEventCuts->Write();

    h2FOROnOffINT7->Write();
    h2FOROnOffINT7PhysSel->Write();
    h2FOROnOffVHMSH2->Write();
    h2FOROnOffVHMSH2PhysSel->Write();
    h2FOROffTrackletsINT7PhysSel->Write();
    h2FOROffTrackletsVHMSH2PhysSel->Write();
    h2TrackletsTracksINT7PhysSel->Write();
    h2TrackletsTracksVHMSH2PhysSel->Write();

    hRejectionFactor->Write();

    hPurityINT7->Write();
    hPurityCVHMSH2->Write();

    hEffPhysSel->Write();
    hThrsPhysSel->Write();
    hTurnonPhysSel->Write();

    h2PtINT7->Write();
    h2PtINT7PhysSel->Write();
    h2PtVHMSH2->Write();
    h2PtVHMSH2PhysSel->Write();

    hPtDistINT7PhysSelAll->Write();
    hPtDistINT7PhysSelThrs90->Write();
    hPtDistINT7PhysSelThrs95->Write();
    hPtDistVHMSH2PhysSelAll->Write();
    hPtDistVHMSH2PhysSelThrs90->Write();
    hPtDistVHMSH2PhysSelThrs95->Write();

    hPtTurnOnAll->Write();
    hPtTurnOnThrs90->Write();
    hPtTurnOnThrs95->Write();
  }

  return;
}
//_____________________________________________________________________________
void AddArrToHist(TArrayD* arr, TH1D* hist)
{
  // Example of making histo out of TArray
  // for(Short_t j(0); j < 100; j++)
  // {
  //   hTrackPt2->SetBinContent(j+1,hTrackPt2->GetBinContent(j+1)+fTracksPt->GetAt(j) );
  // }

  // printf("AddArrToHist\n");
  if(!arr || !hist) { printf("ERROR: array or histo does not exists!\n"); return; }

  // printf("Name: %s Sum: %g\n",hist->GetName(), arr->GetSum());

  if(arr->GetSum() == 0) { printf("Nothing to add! Array empty\n"); return; }

  Short_t arrSize = arr->GetSize();
  Short_t histBins = hist->GetNbinsX();

  if(arrSize != histBins) { printf("ERROR: Array size is different from number of bins!\n"); return; }

  for(Short_t bin = 0; bin < arrSize; bin++)
  {
    // printf("bin %d = %g \n", bin, arr->GetAt(bin));
    hist->SetBinContent(bin+1, hist->GetBinContent(bin+1)+arr->GetAt(bin) );
  }

  // printf("seems fine\n");
  return;
}
//_____________________________________________________________________________
void AddArrToHist2D(TArrayD* arr, TH2D* hist, Int_t mult)
{
  // Example of making histo out of TArray
  // for(Short_t j(0); j < 100; j++)
  // {
  //   hTrackPt2->SetBinContent(j+1,hTrackPt2->GetBinContent(j+1)+fTracksPt->GetAt(j) );
  // }

  // printf("AddArrToHist\n");
  if(!arr || !hist) { printf("ERROR: array or histo does not exists!\n"); return; }
  if(mult < 0) { printf("ERROR: Negative multiplicity!\n"); return; }

  // printf("Name: %s Sum: %g\n",hist->GetName(), arr->GetSum());

  if(arr->GetSum() == 0) { printf("Nothing to add! Array empty\n"); return; }

  Short_t arrSize = arr->GetSize();
  Short_t histBins = hist->GetNbinsX();

  if(arrSize != histBins) { printf("ERROR: Array size is different from number of bins!\n"); return; }

  for(Short_t bin = 0; bin < arrSize; bin++)
  {
    // printf("bin %d = %g \n", bin, arr->GetAt(bin));
    hist->SetBinContent(bin+1, mult, hist->GetBinContent(bin+1, mult)+arr->GetAt(bin) );
  }

  // printf("seems fine\n");
  return;
}
//_____________________________________________________________________________
TH1D* GetEfficiency(TH1D* trigger, TH1D* mb)
{
  if(!trigger || !mb) { printf("ERROR: histos does not exists!\n"); return 0x0; }

  TH1D* hEfficiency = (TH1D*) trigger->Clone("hEfficiency");
  hEfficiency->Divide(trigger,mb,1.,1.);

  return hEfficiency;
}
//_____________________________________________________________________________
Double_t GetTurnOn(TH1D* mult, Int_t threshold)
{
  if(!mult) { printf("ERROR: Multiplicity histo does not exists!\n"); return -1.; }
  threshold++; // histo bin number offset : starting with 1 instead of 0

  Double_t dEntries = mult->GetEntries();
  Double_t dInt = mult->Integral(threshold,mult->GetNbinsX()+1);
  // printf("Histo: Entries %g | ntegral %g | single bin (thrs) %g \n",mult->GetEntries(), mult->Integral(threshold,mult->GetNbinsX()+1), mult->Integral(threshold,threshold));
  return dInt / dEntries;
}
//_____________________________________________________________________________
