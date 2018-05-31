#ifndef AliAnalysisTaskFilterTrigHMSPD_h
#define AliAnalysisTaskFilterTrigHMSPD_h

#include "AliAnalysisTaskSE.h"
#include "AliEventCuts.h"
#include "AliESDtrackCuts.h"

class AliAnalysisTaskFilterTrigHMSPD : public AliAnalysisTaskSE {
  public:
                        AliAnalysisTaskFilterTrigHMSPD(const char* name="AliAnalysisTaskFilterTrigHMSPD");
    virtual             ~AliAnalysisTaskFilterTrigHMSPD();
    virtual void        UserCreateOutputObjects();
    virtual void        UserExec(Option_t *option);
    virtual void        Terminate(Option_t *option);

    AliEventCuts        fEventCuts; /// Event cuts
  protected:
    AliESDtrackCuts*    fESDtrackCuts; //!

    TList*              fList; //! output TList
    TH1D*               fhEventCounter; //! Event counter

    TTree*              fTree; //! output TTree
    TObjString*         fClassesFired; // list of fired trigger classes
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

    TBits               fIR1; // interaction map for INT1 events (normally V0A&V0C) near the event, that's Int1Id-EventId within -90 +90 BXs
    TBits               fIR2; // map of the INT2 events (normally 0TVX) near the event, that's Int2Id-EventId within -90 +90 BXs
    Int_t               fNumContrSPD; // number of contributors to SPD PV

    Int_t               fNumTracklets; // number of tracklets
    Int_t               fNumTracks; // number of tracks
    Int_t               fNumTracksRefMult08; // number of tracks in midrapidity |eta|<0.8 ala RefMult08 multiplicity estimator
    TBits               fFiredChipMap; // map of fired chips (at least one cluster)
    TBits               fFiredChipMapFO; // map of fired FastOr chips
    Int_t               fNumITSCls[6]; // number of ITS clusters per layer
    UInt_t              fTriggerMaskTOF[72]; // TOF trigger mask array
    Float_t             fV0ATotMult; // total multiplicity in V0A
    Float_t             fV0CTotMult; // total multiplicity in V0C
    UShort_t            fV0ATriggerCharge; // online (trigger) charge in V0A
    UShort_t            fV0CTriggerCharge; // online (trigger) charge in V0C
    Float_t             fV0ATime; // average time in V0A
    Float_t             fV0CTime; // average time in V0C
    Bool_t              fV0PastFutureFilled; // flag for AliVZERO::kPastFutureFlagsFilled bit
    Bool_t              fV0PastFuturePileUp; // flag for V0 past-future protection (true if pileup)
    // Float_t             fV0AMult[32]; // multiplicity in V0A cells
    // Float_t             fV0CMult[32];  // multiplicity in V0C cells
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

  private:
    Bool_t              PastFutureProtection(AliVVZERO* vzero);

  ClassDef(AliAnalysisTaskFilterTrigHMSPD,4);
};
#endif
