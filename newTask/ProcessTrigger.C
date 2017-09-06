void ProcessTrigger()
{
  // parameters
  const char* sInputFile = "/Users/vpacik/NBI/triggerHMstudies/newTask/AnalysisResults_large.root";

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
  TObjString*          fClassesFired = 0x0; // list of fired trigger classes
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
  TBits*               fIR1 = 0x0; // interaction map for INT1 events (normally V0A&V0C) near the event, that's Int1Id-EventId within -90 +90 BXs
  TBits*               fIR2 = 0x0; // map of the INT2 events (normally 0TVX) near the event, that's Int2Id-EventId within -90 +90 BXs
  Int_t               fNumTracklets; // number of tracklets
  TBits*               fFiredChipMap = 0x0; // map of fired chips (at least one cluster)
  TBits*               fFiredChipMapFO = 0x0; // map of fired FastOr chips
  Int_t               fNumITSCls[6]; // number of ITS clusters per layer
  Int_t              fTriggerMaskTOF[72]; // TOF trigger mask array
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
  TArrayF*             fTracksArr = 0x0; // tracks array

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
  eventTree->SetBranchAddress("fTracksArr",&fTracksArr);

  // tree ready

  // looping over entries
  Int_t numEvents = eventTree->GetEntriesFast();
  for (Int_t i(0); i < numEvents; i++)
  {
    if(i > 10) break;
    eventTree->GetEvent(i);

    printf("RunNumber: %d\n",fRunNumber);


   }



  return;
}
