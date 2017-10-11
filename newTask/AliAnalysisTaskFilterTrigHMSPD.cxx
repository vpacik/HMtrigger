#include "AliAnalysisTaskFilterTrigHMSPD.h"
// #include "AliEventCuts.h"
#include "AliESDInputHandler.h"
#include "AliESDEvent.h"
#include "AliESDtrack.h"
#include "AliESDVZERO.h"
#include "AliESDVZEROfriend.h"
#include "AliESDTZERO.h"
#include "AliVVZERO.h"

// root
#include "TChain.h"
#include "TList.h"
#include "TBits.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"

ClassImp(AliAnalysisTaskFilterTrigHMSPD)
//-----------------------------------------------------------------------------
AliAnalysisTaskFilterTrigHMSPD::AliAnalysisTaskFilterTrigHMSPD(const char* name) :
  AliAnalysisTaskSE(name),
  fEventCuts(),
  fTracksPtNumBins(100),
  fTracksPtLowEdge(0.),
  fTracksPtUpEdge(100.),
  fList(0x0),
  fhEventCounter(0x0),
  fTree(0x0),
  fClassesFired(new TObjString()),
  fPhysSelDecision(0),
  fPhysSelPassed(kFALSE),
  fEventCutsPassed(kFALSE),
  fChunkFileName(new TObjString()),
  fEventInFile(-1),
  fRunNumber(-1),
  fPeriod(0),
  fOrbit(0),
  fBC(0),
  fL0inputs(0),
  fL1inputs(0),
  fFiredTriggerInputs(new TObjString()),
  fIR1(),
  fIR2(),
  fNumContrSPD(0),
  fNumTracklets(0),
  fFiredChipMap(),
  fFiredChipMapFO(),
  fNumITSCls(),
  fTriggerMaskTOF(),
  fV0PastFutureFilled(0),
  fV0PastFuturePileUp(0),
  fV0ATotMult(0),
  fV0CTotMult(0),
  fV0ATime(0),
  fV0CTime(0),
  fV0ATriggerCharge(0),
  fV0CTriggerCharge(0),
  fV0AMult(),
  fV0CMult(),
  fV0AFlagsBB(0),
  fV0CFlagsBB(0),
  fV0AFlagsBG(0),
  fV0CFlagsBG(0),
  fV0ATriggerBB(),
  fV0ATriggerBG(),
  fV0CTriggerBB(),
  fV0CTriggerBG(),
  fV0FlagBB(),
  fV0FlagBG(),
  fV0FlagPFBB(),
  fV0FlagPFBG(),
  fV0ADecision(),
  fV0CDecision(),
  fVtxX(0),
  fVtxY(0),
  fVtxZ(0),
  fVtxTPC(kFALSE),
  fNumTracks(-1),
  fTracksPt(0x0)
{
  DefineInput(0,TChain::Class());
  DefineOutput(1,TList::Class());
  DefineOutput(2,TTree::Class());
}
//-----------------------------------------------------------------------------
AliAnalysisTaskFilterTrigHMSPD::~AliAnalysisTaskFilterTrigHMSPD()
{
  if(fList) delete fList;
  if(fTree) delete fTree;
}
//-----------------------------------------------------------------------------
void AliAnalysisTaskFilterTrigHMSPD::UserCreateOutputObjects()
{
  fTracksPt = new TArrayD(fTracksPtNumBins);

  fList = new TList();
  fList->SetOwner(kTRUE);
  fhEventCounter = new TH1D("fhEventCounter","fhEventCounter",1,0,1);
  fList->Add(fhEventCounter);

  fTree = new TTree("events","events");
  fTree->Branch("fClassesFired",&fClassesFired);
  fTree->Branch("fPhysSelDecision",&fPhysSelDecision);
  fTree->Branch("fPhysSelPassed",&fPhysSelPassed);
  fTree->Branch("fEventCutsPassed",&fEventCutsPassed);
  fTree->Branch("fChunkFileName",&fChunkFileName);
  fTree->Branch("fEventInFile",&fEventInFile);
  fTree->Branch("fRunNumber",&fRunNumber);
  fTree->Branch("fPeriod",&fPeriod);
  fTree->Branch("fOrbit",&fOrbit);
  fTree->Branch("fBC",&fBC);
  fTree->Branch("fL0inputs",&fL0inputs);
  fTree->Branch("fL1inputs",&fL1inputs);
  fTree->Branch("fFiredTriggerInputs",&fFiredTriggerInputs);
  fTree->Branch("fIR1",&fIR1);
  fTree->Branch("fIR2",&fIR2);
  fTree->Branch("fNumContrSPD",&fNumContrSPD);
  fTree->Branch("fFiredChipMap",&fFiredChipMap);
  fTree->Branch("fFiredChipMapFO",&fFiredChipMapFO);
  fTree->Branch("fNumITSCls",&fNumITSCls,"fNumITSCls[6]/I");
  fTree->Branch("fTriggerMaskTOF",&fTriggerMaskTOF,"fTriggerMask[72]/I");
  fTree->Branch("fV0PastFutureFilled",&fV0PastFutureFilled);
  fTree->Branch("fV0PastFuturePileUp",&fV0PastFuturePileUp);
  fTree->Branch("fV0ATotMult",&fV0ATotMult);
  fTree->Branch("fV0CTotMult",&fV0CTotMult);
  fTree->Branch("fV0ATriggerCharge",&fV0ATriggerCharge);
  fTree->Branch("fV0CTriggerCharge",&fV0CTriggerCharge);
  fTree->Branch("fV0ATime",&fV0ATime);
  fTree->Branch("fV0CTime",&fV0CTime);
  fTree->Branch("fV0AMult",&fV0AMult,"fV0AMult[32]/F");
  fTree->Branch("fV0CMult",&fV0CMult,"fV0CMult[32]/F");
  fTree->Branch("fV0ATriggerBB",&fV0ATriggerBB,"fV0ATriggerBB[32]/O");
  fTree->Branch("fV0ATriggerBG",&fV0ATriggerBG,"fV0ATriggerBG[32]/O");
  fTree->Branch("fV0CTriggerBG",&fV0ATriggerBG,"fV0ATriggerBG[32]/O");
  fTree->Branch("fV0CTriggerBB",&fV0ATriggerBB,"fV0ATriggerBB[32]/O");
  fTree->Branch("fV0AFlagsBB",&fV0AFlagsBB);
  fTree->Branch("fV0CFlagsBB",&fV0CFlagsBB);
  fTree->Branch("fV0AFlagsBG",&fV0AFlagsBG);
  fTree->Branch("fV0CFlagsBG",&fV0CFlagsBG);
  fTree->Branch("fV0FlagBB",&fV0FlagBB,"fV0FlagBB[64]/O");
  fTree->Branch("fV0FlagBG",&fV0FlagBG,"fV0FlagBG[64]/O");
  fTree->Branch("fV0FlagPFBB",&fV0FlagPFBB,"fV0FlagPFBB[64][21]/O");
  fTree->Branch("fV0FlagPFBG",&fV0FlagPFBG,"fV0FlagPFBG[64][21]/O");
  fTree->Branch("fV0ADecision",&fV0ADecision);
  fTree->Branch("fV0CDecision",&fV0CDecision);
  fTree->Branch("fVtxX",&fVtxX);
  fTree->Branch("fVtxY",&fVtxY);
  fTree->Branch("fVtxZ",&fVtxZ);
  fTree->Branch("fVtxTPC",&fVtxTPC);
  fTree->Branch("fNumTracklets",&fNumTracklets);
  fTree->Branch("fNumTracks",&fNumTracks);
  fTree->Branch("fTracksPt",&fTracksPt);

  PostData(1,fList);
  PostData(2,fTree);
}
//-----------------------------------------------------------------------------
void AliAnalysisTaskFilterTrigHMSPD::UserExec(Option_t *)
{
  if(!fInputEvent) return;

  fhEventCounter->Fill("Input",1);

  // Events
  fClassesFired->SetString(fInputEvent->GetFiredTriggerClasses());

  // skipping not interesting events (in this case: anything else than kINT7 and HM events)
  if (!fClassesFired->String().Contains("CINT7-B-NOPF-CENT") &&
      !fClassesFired->String().Contains("CINT7-B-NOPF-ALL") &&
      !fClassesFired->String().Contains("CINT7-I-NOPF-CENT") &&
      !fClassesFired->String().Contains("CVHMV0M-B") &&
      !fClassesFired->String().Contains("CVHMSH1-B") &&
      !fClassesFired->String().Contains("CVHMSH2-B")
  ) {
    PostData(1,fList);
    PostData(2,fTree);
    return;
  }
  fhEventCounter->Fill("Interesting",1);

  fPhysSelDecision = fInputHandler->IsEventSelected();
  if(fPhysSelDecision) fhEventCounter->Fill("PhysicsSelection",1);
  fPhysSelPassed = fPhysSelDecision ? kTRUE : kFALSE;
  // fIsINT7 = fPhysSelDecision& AliVEvent::kINT7;
  // fIsSH2 = fPhysSelDecision& AliVEvent::kHighMultSPD;

  fEventCutsPassed = fEventCuts.AcceptEvent(InputEvent());
  if(fEventCutsPassed) fhEventCounter->Fill("EventCuts",1);

  fChunkFileName->SetString(((TTree*) GetInputData(0))->GetCurrentFile()->GetName());
  fEventInFile = fInputEvent->GetEventNumberInFile();
  fRunNumber  = fInputEvent->GetRunNumber();
  fPeriod = fInputEvent->GetPeriodNumber();
  fOrbit = fInputEvent->GetOrbitNumber();
  fBC = fInputEvent->GetBunchCrossNumber();
  fL0inputs = fInputEvent->GetHeader()->GetL0TriggerInputs();
  fL1inputs = fInputEvent->GetHeader()->GetL1TriggerInputs();
  fIR1 = fInputEvent->GetHeader()->GetIRInt1InteractionMap();
  fIR2 = fInputEvent->GetHeader()->GetIRInt2InteractionMap();
  fNumContrSPD = fInputEvent->GetPrimaryVertexSPD()->GetNContributors();
  fFiredTriggerInputs->SetString( ( (AliESDHeader*)fInputEvent->GetHeader() )->GetFiredTriggerInputs() );

  // === do not know what exactly happens here ====
  // fIRproblem = 0;
  // Int_t nPast   = 90;
  // Int_t nFuture = 90;
  // Int_t nTotal   = nPast+nFuture+1;
  // ULong64_t id = fInputEvent->GetHeader()->GetEventIdAsLong();
  // if (fInputEvent->GetDataLayoutType()==AliVEvent::kESD) {
  //   fVBA.ResetAllBits();
  //   fVBC.ResetAllBits();
  //   fUBA.ResetAllBits();
  //   fUBC.ResetAllBits();
  //   fOM2.ResetAllBits();
  //   fOMU.ResetAllBits();
  //   fSTG.ResetAllBits();
  //   const AliESDHeader* header = dynamic_cast<const AliESDHeader*>(fInputEvent->GetHeader());
  //   Int_t nIRs = header->GetTriggerIREntries();
  //   if (nIRs!=10) fIRproblem = 1;
  //   for (Int_t i=0;i<nIRs;i++){
  //     const AliTriggerIR* ir = header->GetTriggerIR(i);
  //     if (ir->GetIncomplete() || ir->GetTransErr2()) fIRproblem = 1;
  //     UInt_t orbit = ir->GetOrbit();
  //     UShort_t* bcs   = ir->GetBC2s();
  //     ULong64_t* ints = ir->GetIntsRun2();
  //     Int_t nWords = ir->GetNWord2();
  //     if (nWords>509) fIRproblem = 1;
  //     for (UInt_t r=0;r<ir->GetNWord2();r++) {
  //       Int_t bc = nPast+(orbit-fOrbit)*3564+(bcs[r]-fBC);
  //       if (bc<0) continue;
  //       if (bc>nTotal) continue;
  //       if (!ints[r]) printf("bug\n");
  //       Bool_t vba = ints[r] & BIT( 5);
  //       Bool_t vbc = ints[r] & BIT( 6);
  //       Bool_t uba = ints[r] & BIT(39);
  //       Bool_t ubc = ints[r] & BIT(40);
  //       Bool_t om2 = ints[r] & BIT(30);
  //       Bool_t omu = ints[r] & BIT(32);
  //       Bool_t stg = ints[r] & BIT(25);
  //       if (vba) fVBA.SetBitNumber(bc);
  //       if (vbc) fVBC.SetBitNumber(bc);
  //       if (uba) fUBA.SetBitNumber(bc);
  //       if (ubc) fUBC.SetBitNumber(bc);
  //       if (om2) fOM2.SetBitNumber(bc);
  //       if (omu) fOMU.SetBitNumber(bc);
  //       if (stg) fSTG.SetBitNumber(bc);
  //
  //     }
  //   }
  //   //if (!fIRproblem) {
  //   //  for (Int_t i=0;i<nTotal;i++) if (fIR1.TestBitNumber(i) && !fVBA.TestBitNumber(i) && !fVBC.TestBitNumber(i)) printf("Unknown IR problem\n");
  //   //}
  //
  //   // for (Int_t i=0;i<nTotal;i++) printf("%i",fIR1.TestBitNumber(i)); printf("\n");
  // ==== end ====

  // ITS
  AliVMultiplicity* mult = fInputEvent->GetMultiplicity();
  fNumTracklets = mult ? mult->GetNumberOfTracklets() : 0;
  fFiredChipMapFO = mult ? mult->GetFastOrFiredChips() : 0;
  fFiredChipMap = mult ? mult->GetFiredChipMap() : 0;
  for(Int_t i = 0; i < 6; i++) { fNumITSCls[i] = fInputEvent->GetNumberOfITSClusters(i); }

  // TOF
  for(UInt_t k = 0; k < 72; k++) { fTriggerMaskTOF[k] = fInputEvent->GetTOFHeader()->GetTriggerMask()->GetTriggerMask(k); }

  // VZERO
  AliVVZERO* vzero = fInputEvent->GetVZEROData();
  fV0PastFutureFilled = vzero->TestBit(AliVVZERO::kPastFutureFlagsFilled);
  fV0ATriggerCharge = vzero->GetTriggerChargeA();
  fV0CTriggerCharge = vzero->GetTriggerChargeC();
  fV0ATotMult = vzero->GetMTotV0A();
  fV0CTotMult = vzero->GetMTotV0C();
  fV0ATime = vzero->GetV0ATime();
  fV0CTime = vzero->GetV0CTime();

  for (Int_t i = 0; i < 32; i++)
  {
    fV0AMult[i] = vzero->GetMultiplicityV0A(i);
    fV0CMult[i] = vzero->GetMultiplicityV0C(i);
    // offline BB/BG
    fV0ATriggerBB[i] = vzero->BBTriggerV0A(i);
    fV0ATriggerBG[i] = vzero->BGTriggerV0A(i);
    fV0CTriggerBB[i] = vzero->BBTriggerV0C(i);
    fV0CTriggerBG[i] = vzero->BGTriggerV0C(i);
  }

  // online BB/BG // past-future protection
  fV0PastFuturePileUp = kFALSE;
  fV0AFlagsBB = 0; fV0CFlagsBB = 0; fV0AFlagsBG = 0; fV0CFlagsBG = 0;
  Bool_t vir[21] = {0};

  for (Int_t i = 0; i < 64; i++)
  {
    fV0FlagBB[i] = vzero->GetBBFlag(i);
    fV0FlagBG[i] = vzero->GetBGFlag(i);

    if (i<32) { fV0CFlagsBB += fV0FlagBB[i]; fV0CFlagsBG += fV0FlagBG[i]; }
    else { fV0AFlagsBB += fV0FlagBB[i]; fV0AFlagsBG += fV0FlagBG[i]; }

    for (Int_t bc = 0; bc < 21; bc++)
    {
      fV0FlagPFBB[i][bc] = vzero->GetPFBBFlag(i,bc);
      fV0FlagPFBG[i][bc] = vzero->GetPFBGFlag(i,bc);
    }
  }

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

  Int_t fVIRBBAflags = 10;
  Int_t fVIRBBCflags = 10;
  Int_t fVIRBGAflags = 33;
  Int_t fVIRBGCflags = 33;

  Int_t bcMin = 3; //10 - fNBCsFuture + bcMod4;
  Int_t bcMax = 17; //10 + fNBCsPast + bcMod4;
  for (Int_t bc = bcMin; bc <= bcMax; bc++)
  {
    if (bc==10) continue; // skip current bc
    if (bc < 0) continue;
    if (bc >20) continue;
    if (vir[bc]) fV0PastFuturePileUp = kTRUE;
  }

  fV0ADecision = vzero->GetV0ADecision();
  fV0CDecision = vzero->GetV0CDecision();

  const AliVVertex* vertex  = fInputEvent->GetPrimaryVertex();
  if(vertex)
  {
    fVtxX   = vertex->GetX();
    fVtxY   = vertex->GetY();
    fVtxZ   = vertex->GetZ();
    fVtxTPC = TString(vertex->GetName()).CompareTo("PrimaryVertex") && TString(vertex->GetName()).CompareTo("SPDVertex");
  }

  fTracksPt->Reset();

  Short_t index = -1;
  AliESDtrack* track = 0x0;
  fNumTracks = fInputEvent->GetNumberOfTracks();
  for(Int_t i(0); i < fNumTracks; i++)
  {
    track = (AliESDtrack*) fInputEvent->GetTrack(i);
    if(!track) continue;

    index = GetPtBinIndex(track->Pt());
    if(index == -1) continue;

    fTracksPt->AddAt(fTracksPt->At(index)+1,index);

    //EXAMPLE: AliUpcParticle* part = new ((*fTracks)[fTracks->GetEntriesFast()]) Ali(pt,eta,phi,charge,label,21);
    // new( (*fTracks)[i] ) AliESDtrack(track); // working
  }

  fTree->Fill();
  PostData(1,fList);
  PostData(2,fTree);
}
//-----------------------------------------------------------------------------
void AliAnalysisTaskFilterTrigHMSPD::Terminate(Option_t *)
{

}
//-----------------------------------------------------------------------------
Short_t AliAnalysisTaskFilterTrigHMSPD::GetPtBinIndex(Double_t pt)
{
  if( pt < fTracksPtLowEdge || pt >= fTracksPtUpEdge ) return -1;
  Double_t dBinWidth = (fTracksPtUpEdge - fTracksPtLowEdge)/fTracksPtNumBins;
  return (Short_t) (pt / dBinWidth);
}
