#include "AliAnalysisTaskFilterTrigHMSPD.h"
#include "AliEventCuts.h"
#include "AliAODInputHandler.h"
#include "AliESDEvent.h"
#include "AliESDVZERO.h"
#include "AliESDVZEROfriend.h"
#include "AliESDTZERO.h"
#include "AliVVZERO.h"
#include "AliVAD.h"


// root
#include "TChain.h"
#include "TList.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TDirectory.h"

ClassImp(AliAnalysisTaskFilterTrigHMSPD)

//-----------------------------------------------------------------------------
AliAnalysisTaskFilterTrigHMSPD::AliAnalysisTaskFilterTrigHMSPD(const char* name) :
  AliAnalysisTaskSE(name),
  fEventCuts(0),
  fTree(0x0),
  fPhysSelDecision(0),
  fPhysSelPassed(0),
  fIsINT7(kFALSE),
  fIsSH2(kFALSE),
  fEventCutsPassed(kFALSE),
  fNumTracklets(0)
{
  DefineInput(0,TChain::Class());
  DefineOutput(1,TTree::Class());
}
//-----------------------------------------------------------------------------
void AliAnalysisTaskFilterTrigHMSPD::UserCreateOutputObjects()
{
  fTree = new TTree("events","events");
  fTree->Branch("fPhysSelDecision",&fPhysSelDecision);
  fTree->Branch("fPhysSelPassed",&fPhysSelPassed);
  fTree->Branch("fIsINT7",&fIsINT7);
  fTree->Branch("fIsSH2",&fIsSH2);
  fTree->Branch("fEventCutsPassed",&fEventCutsPassed);
  fTree->Branch("fNumTracklets",&fNumTracklets);
  // fTree->Branch("fIsIncomplete",&fIsIncomplete);

  PostData(1,fTree);
}
//-----------------------------------------------------------------------------
void AliAnalysisTaskFilterTrigHMSPD::UserExec(Option_t *)
{
  // Events

  fPhysSelDecision = fInputHandler->IsEventSelected();
  fPhysSelPassed = fPhysSelDecision ? kTRUE : kFALSE;
  fIsINT7 = fPhysSelDecision& AliVEvent::kINT7;
  fIsSH2 = fPhysSelDecision& AliVEvent::kHighMultSPD;
  fEventCutsPassed = fEventCuts.AcceptEvent(InputEvent());

  fNumTracklets = fInputEvent->GetMultiplicity()->GetNumberOfTracklets();

  fTree->Fill();
  PostData(1,fTree);
}
//-----------------------------------------------------------------------------
void AliAnalysisTaskFilterTrigHMSPD::Terminate(Option_t *)
{

}
