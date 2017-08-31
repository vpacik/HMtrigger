#ifndef AliAnalysisTaskFilterTrigHMSPD_h
#define AliAnalysisTaskFilterTrigHMSPD_h

#include "AliAnalysisTaskSE.h"
#include "AliEventCuts.h"


class AliAnalysisTaskFilterTrigHMSPD : public AliAnalysisTaskSE {
  public:
                        AliAnalysisTaskFilterTrigHMSPD(const char* name="AliAnalysisTaskTest");
    virtual             ~AliAnalysisTaskFilterTrigHMSPD() {};
    virtual void        UserCreateOutputObjects();
    virtual void        UserExec(Option_t *option);
    virtual void        Terminate(Option_t *option);

    AliEventCuts        fEventCuts; /// Event cuts
  protected:
    TTree*              fTree; //! output tree
    UInt_t              fPhysSelDecision; // AliPhysicsSelection decision
    Bool_t              fPhysSelPassed; // AliPhysicsSelection decision (pass/reject)
    Bool_t              fIsINT7; // passes kINT7
    Bool_t              fIsSH2; // passes kSH2
    Bool_t              fEventCutsPassed; // AliEventCuts selection decision (pass/reject)
    Int_t               fNumTracklets; // number of tracklets


  ClassDef(AliAnalysisTaskFilterTrigHMSPD,1);
};
#endif
