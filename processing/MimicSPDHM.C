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
Int_t               fNumContrSPD; // number of contributors to SPD vertex
Int_t               fNumTracklets; // number of tracklets
Int_t               fNumTracks; // number of tracks
Int_t               fNumTracksRefMult08; //
Int_t               fNumTracksMultKatarina; //
TBits*              fFiredChipMap = 0x0; // map of fired chips (at least one cluster)
TBits*              fFiredChipMapFO = 0x0; // map of fired FastOr chips
Int_t               fNumITSCls[6]; // number of ITS clusters per layer
Int_t               fTriggerMaskTOF[72]; // TOF trigger mask array
Float_t             fV0ATotMult; // total multiplicity in V0A
Float_t             fV0CTotMult; // total multiplicity in V0C
UShort_t            fV0ATriggerCharge; // online (trigger) charge in V0A
UShort_t            fV0CTriggerCharge; // online (trigger) charge in V0C
Float_t             fV0AMult[32]; // multiplicity in V0A cells
Float_t             fV0CMult[32];  // multiplicity in V0C cells
Float_t             fV0ATime; // average time in V0A
Float_t             fV0CTime; // average time in V0C
Bool_t              fV0PastFutureFilled; // flag for AliVZERO::kPastFutureFlagsFilled bit
Bool_t              fV0PastFuturePileUp; // flag for V0 past-future protection (true if pileup)
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
TArrayD*            fTracksPt = 0x0; // binned tracks pt

const Int_t iNumTypes = 5;
TString sTypeLabels[iNumTypes] = {"ALL", "MB", "SPDHM", "MB_PhysSel", "SPDHM_PhysSel"};
enum eEventType { kAll = 0, kCINT7, kCVHMSH2, kCINT7_PhysSel, kCVHMSH2_PhysSel};

TList* listOut[iNumTypes];

TH1D* hEventCounter;
TH1D* hDistFOonline[iNumTypes];
TH1D* hDistFOoffline[iNumTypes];

// 2D mult correlations
TH2D* h2_FOonline_Ntrklets[iNumTypes];
TH2D* h2_FOonline_Ntrks[iNumTypes];
TH2D* h2_FOonline_Ntrks08pt[iNumTypes];
TH2D* h2_FOonline_RefMult08[iNumTypes];

Int_t iNumFOonline = 0;
Int_t iNumFOoffline = 0;



Bool_t CheckTriggerConsistency();
void CreateHistos();
void FillCommonHistos(eEventType type);
Bool_t LoadTTreeVars();

void MimicSPDHM()
{
  Int_t iNumEventsToProcess = 1000000;
  TString sPath = "/Users/vpacik/NBI/ALICE/HMtrigger/running/16k-merged/";
  TString sInFileName = "AnalysisResults.root";
  TString sOutFileName = "Skimmed.root";

  // ===============================================================================

  CreateHistos();

  // openning input & output file
  TString sInputFile = sPath + sInFileName;
  TString sOutputFile = sPath + sOutFileName;

  TFile* fOutputFile = new TFile(sOutputFile.Data(),"RECREATE");
  if(!fOutputFile->IsOpen()) { printf(" ERROR: fOutputFile open!\n"); return; }

  TFile* fInputFile = new TFile(sInputFile.Data(),"READ");
  if(!fInputFile->IsOpen()) { printf(" ERROR: fInputFile not open!\n"); return; }

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
    hEventCounter->Fill("Input",1);

    // trigger part
    Bool_t bIsCINT7 = fClassesFired->String().Contains("CINT7-B");
    Bool_t bIsCVHMSH2 = fClassesFired->String().Contains("CVHMSH2-B");
    Bool_t bIsConsist = CheckTriggerConsistency();

    // number of online/oflline FAST-ORs
    iNumFOonline = fFiredChipMapFO->CountBits(400);
    iNumFOoffline = fFiredChipMap->CountBits(400);

    // filling common histos
    FillCommonHistos(kAll);
    if(bIsCINT7) { FillCommonHistos(kCINT7); }
    if(fPhysSelPassed && bIsCINT7)  { FillCommonHistos(kCINT7_PhysSel); }
    if(bIsCVHMSH2) { FillCommonHistos(kCVHMSH2); }
    if(fPhysSelPassed && kCVHMSH2_PhysSel)  { FillCommonHistos(kCVHMSH2_PhysSel); }

    // skipping events not marker as MB nor SPDHM (because we need only OFO > 70)
    if(!bIsCINT7) { continue; }
    if(!bIsCVHMSH2) { continue; }


  }

  fOutputFile->cd();
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

  for(Int_t iType(0); iType < iNumTypes; ++iType)
  {
    TString sLabel = sTypeLabels[iType];

    listOut[iType] = new TList();
    TList* list = listOut[iType];

    // Distributions
    hDistFOonline[iType] = new TH1D(Form("hDistFOonline_%s",sLabel.Data()), Form("hDistFOonline_%s; FO online; Counts",sLabel.Data()), 100,0,1000);
    list->Add(hDistFOonline[iType]);
    hDistFOoffline[iType] = new TH1D(Form("hDistFOoffline_%s",sLabel.Data()), Form("hDistFOoffline_%s; FO offline; Counts",sLabel.Data()), 100,0,1000);
    list->Add(hDistFOoffline[iType]);

    // 2D mult correlations
    h2_FOonline_Ntrklets[iType] = new TH2D(Form("h2_FOonline_Ntrklets_%s",sLabel.Data()), Form("h2_FOonline_Ntrklets_%s; N_{tracklets}; FO online",sLabel.Data()), 100,0,1000, 100,0,1000);
    list->Add(h2_FOonline_Ntrklets[iType]);
    h2_FOonline_Ntrks[iType] = new TH2D(Form("h2_FOonline_Ntrks_%s",sLabel.Data()), Form("h2_FOonline_Ntrks_%s; N_{tracks}; FO online",sLabel.Data()), 100,0,1000, 100,0,1000);
    list->Add(h2_FOonline_Ntrks[iType]);
    h2_FOonline_Ntrks08pt[iType] = new TH2D(Form("h2_FOonline_Ntrks08pt_%s",sLabel.Data()), Form("h2_FOonline_RefMult08_%s; N_{tracks}(|#eta|<0.8, 0.2 #it{p}_{T} < 3 GeV/#it{c}); FO online",sLabel.Data()), 100,0,1000, 100,0,1000);
    list->Add(h2_FOonline_Ntrks08pt[iType]);
    h2_FOonline_RefMult08[iType] = new TH2D(Form("h2_FOonline_RefMult08_%s",sLabel.Data()), Form("h2_FOonline_RefMult08_%s; RefMult08; FO online",sLabel.Data()), 100,0,1000, 100,0,1000);
    list->Add(h2_FOonline_RefMult08[iType]);
  }

  return;
}
// =====================================================================================================================
void FillCommonHistos(eEventType iType)
{
  hDistFOonline[iType]->Fill(iNumFOonline);
  hDistFOoffline[iType]->Fill(iNumFOoffline);

  // 2D mult correlations
  h2_FOonline_Ntrklets[iType]->Fill(iNumFOonline,fNumTracklets);
  h2_FOonline_Ntrks[iType]->Fill(iNumFOonline,fNumTracks);
  h2_FOonline_Ntrks08pt[iType]->Fill(iNumFOonline,fNumTracksMultKatarina);
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
  eventTree->SetBranchAddress("fChunkFileName",&fChunkFileName);
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
  eventTree->SetBranchAddress("fNumTracksMultKatarina",&fNumTracksMultKatarina);
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
  eventTree->SetBranchAddress("fV0AMult",fV0AMult);
  eventTree->SetBranchAddress("fV0CMult",fV0CMult);
  eventTree->SetBranchAddress("fV0PastFuturePileUp",&fV0PastFuturePileUp);
  eventTree->SetBranchAddress("fV0PastFutureFilled",&fV0PastFutureFilled);
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

  return kTRUE;
}
