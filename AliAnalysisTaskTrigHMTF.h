#ifndef AliAnalysisTaskTrigHMTF_h
#define AliAnalysisTaskTrigHMTF_h

class TTree;
#include "AliAnalysisTaskSE.h"
#include "TBits.h"
#include "TObjString.h"

class AliAnalysisTaskTrigHMTF : public AliAnalysisTaskSE {
  public:
                        AliAnalysisTaskTrigHMTF(const char* name="AliAnalysisTaskTest");
    virtual             ~AliAnalysisTaskTrigHMTF(){};
    virtual void        UserCreateOutputObjects();
    virtual void        UserExec(Option_t *option);
    virtual void        Terminate(Option_t *option);
  protected:
    TTree*              fTree; //! output tree
    Bool_t              fIsFriend; // container for ESD friend flag
    Int_t               fRunNumber; // run number
    Bool_t              fIsIncomplete; // IsIncompleteDAQ() ?
    UShort_t            fBC; // Bunch crossing number from AliESDHeader
    TBits               fIR1; // map of INT1 events (V0A&V0C) within ± 90 BXs
    TBits               fIR2; // map of INT2 events (0TVX) within ± 90 BXs
    TObjString          fClassesFired; // list of fired trigger classes (TStrings)
    UInt_t              fInputsL0; // L0 trigger input mask

    // VZERO data
    Float_t             fV0ATime; // average time in V0A
    Float_t             fV0CTime; // average time in V0C
    Int_t               fV0ADecision; // final V0A decition based on average time of channels
    Int_t               fV0CDecision; // final V0C decision based on average time of channels
    Float_t             fMTotV0A; // total multiplicity in V0A
    Float_t             fMTotV0C; // total multiplicity in V0C
    Float_t             fMRingV0A[4]; // multiplicity in V0A rings
    Float_t             fMRingV0C[4]; // multiplicity in V0C rings
    UShort_t            fTriggerChargeA; // sum of the trigger (clock=10) charge on A side
    UShort_t            fTriggerChargeC; // sum of the trigger (clock=10) charge on C side
    Bool_t              fBBFlag[64]; // online beam-beam flag in V0
    Bool_t              fBGFlag[64]; // online beam-gas flag in V0
    ULong64_t           fBBFlagPF[21]; // 'Beam-Beam' flag for all channels and 21 clocks
    ULong64_t           fBGFlagPF[21]; // 'Beam-Gas' flag for all channels and 21 clocks

    // ITS data
    UInt_t              fNofTracklets; // number of tracklets (if not available: number of SPD tracklets on-fly)
    Int_t               fNofITSClusters[6]; // number of ITS clusters per layer
    TBits               fFOmap; // map of FastOr fired chips
    TBits               fFiredChipMap; // map of fired chips (at least one cluster)
    Float_t             fVz; // SPD vertex z-position
    Int_t               fVertexContributors; // number of SPD vertex estimate contributors
    Bool_t              fIsPileupSPD; // SPD pileup flag
    Int_t               fPileupContributors; // sum of all contributors to SPD pile up vertices

    // AD data
    Float_t             fADATime; // average time in ADA
    Float_t             fADCTime; // average time in ADC
    Int_t               fADADecision; // ADA final decision based on average time of channels
    Int_t               fADCDecision; // ADC final decision based on average time of channels
    Float_t             fMTotADA; // total multiplicity in ADA
    Float_t             fMTotADC; // total multiplicity in ADC
    UShort_t            fTriggerChargeADA; // sum of the trigger (clock=10) charge on A side
    UShort_t            fTriggerChargeADC; // sum of the trigger (clock=10) charge on C side

    // TO data
    Float_t             fT0A[5]; // hardware OrA centred around 0 [ns]
    Float_t             fT0C[5]; // hardware OrC centred around 0 [ns]
    Float_t             fTVX[5]; // hardware TVDC centred around 0 [ns]

    // inside track loop: track status is AliESDtrack::kITSpureSA && AliESDtrack::kITSrefit
    UInt_t              fNITSsaTracks; // number of ITS standalone tracks in given event
    UInt_t              fNITSsaTracksHits[6]; // Number of tracks having hits in layer x in given event

  ClassDef(AliAnalysisTaskTrigHMTF,1);
};
#endif
