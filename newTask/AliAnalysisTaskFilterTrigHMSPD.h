#ifndef AliAnalysisTaskFilterTrigHMSPD_h
#define AliAnalysisTaskFilterTrigHMSPD_h

#include "AliAnalysisTaskSE.h"
#include "AliEventCuts.h"


class AliAnalysisTaskFilterTrigHMSPD : public AliAnalysisTaskSE {
  public:
                        AliAnalysisTaskFilterTrigHMSPD(const char* name="AliAnalysisTaskFilterTrigHMSPD");
    virtual             ~AliAnalysisTaskFilterTrigHMSPD();
    virtual void        UserCreateOutputObjects();
    virtual void        UserExec(Option_t *option);
    virtual void        Terminate(Option_t *option);

    AliEventCuts        fEventCuts; /// Event cuts
  protected:

    Short_t             GetPtBinIndex(Double_t pt); // return pt bin given the set binning

    Short_t             fTracksPtNumBins; // number of pt bins in binned pt dist.
    Float_t             fTracksPtLowEdge; // low edge of pt (binned) dist
    Float_t             fTracksPtUpEdge; // upper edge of pt (binned) dist

    TList*              fList; //! output TList
    TH1D*               fhEventCounter; //! Event counter
    TH1D*               fhTrackPt; //! Track pt dist


    TTree*              fTree; //! output TTree
    TObjString          fClassesFired; // list of fired trigger classes
    UInt_t              fPhysSelDecision; // AliPhysicsSelection decision
    Bool_t              fPhysSelPassed; // AliPhysicsSelection decision (pass/reject)
    // Bool_t              fIsINT7; // passes kINT7
    // Bool_t              fIsSH2; // passes kSH2
    Bool_t              fEventCutsPassed; // AliEventCuts selection decision (pass/reject)
    TObjString*         fChunkFileName; // current file name
    Int_t               fEventInFile; // current event number in the file
    Int_t               fRunNumber; // run number
    UInt_t              fPeriod; // run period
    UInt_t              fOrbit; // orbit number
    UShort_t            fBC; // bunch cross (BX) number
    UInt_t              fL0inputs; // L0 trigger inputs
    UInt_t              fL1inputs; // L1 trigger inputs
    TBits               fIR1; // interaction map for INT1 events (normally V0A&V0C) near the event, that's Int1Id-EventId within -90 +90 BXs
    TBits               fIR2; // map of the INT2 events (normally 0TVX) near the event, that's Int2Id-EventId within -90 +90 BXs

    Int_t               fNumTracklets; // number of tracklets
    TBits               fFiredChipMap; // map of fired chips (at least one cluster)
    TBits               fFiredChipMapFO; // map of fired FastOr chips
    Int_t               fNumITSCls[6]; // number of ITS clusters per layer
    UInt_t              fTriggerMaskTOF[72]; // TOF trigger mask array
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
    TArrayD             fTracksPt; // (binned) pt distribution of tracks


  ClassDef(AliAnalysisTaskFilterTrigHMSPD,1);
};
#endif
