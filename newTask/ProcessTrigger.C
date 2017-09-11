
void AddArrToHist(TArrayD* arr, TH1D* hist);
TH1D* GetEfficiency(TH1D* trigger, TH1D* mb);
Double_t GetTurnOn(TH1D* mult, Int_t threshold);

void ProcessTrigger()
{
  // parameters
  const char* sInputFile = "/Users/vpacik/NBI/triggerHMstudies/newTask/running/16k/AnalysisResults_BK.root";
  // const char* sInputFile = "/Users/vpacik/NBI/triggerHMstudies/newTask/AnalysisResults.root";
  TString sOutputPath = "/Users/vpacik/NBI/triggerHMstudies/newTask/";
  // Int_t iNumEventsToProcess = -1; // number of events to be processed; if -1 all are processed
  Int_t iNumEventsToProcess = 100000; // number of events to be processed; if -1 all are processed
  Bool_t bCheckVHMSPDconsistency = kFALSE; // if true fFiredTriggerInputs are checked (available in newer implementeation)

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
  TH1D* hEventCounter = new TH1D("hEventCounter","Event counter",1,0,1);
  // multiplicity dist
  TH1D* hEventMultAll = new TH1D("hEventMultAll","Event multiplicity (all events); n tracklets",200,0,200);
  TH1D* hEventMultAllPhysSel = new TH1D("hEventMultAllPhysSel","Event multiplicity (all events passing Phys. Selection); n tracklets",200,0,200);
  TH1D* hEventMultAllPhysSelEventCuts = new TH1D("hEventMultAllPhysSelEventCuts","Event multiplicity (all events passing Phys. Selection & EventCuts); n tracklets",200,0,200);
  TH1D* hEventMultINT7 = new TH1D("hEventMultINT7","Event multiplicity (INT7 events); n tracklets",200,0,200);
  TH1D* hEventMultINT7PhysSel = new TH1D("hEventMultINT7PhysSel","Event multiplicity (INT7 events passing Phys. Selection); n tracklets",200,0,200);
  TH1D* hEventMultINT7PhysSelEventCuts = new TH1D("hEventMultINT7PhysSelEventCuts","Event multiplicity (INT7 events passing Phys. Selection & EventCuts); n tracklets",200,0,200);
  TH1D* hEventMultCVHMSH2 = new TH1D("hEventMultCVHMSH2","Event multiplicity (CVHMSH2 events); n tracklets",200,0,200);
  TH1D* hEventMultCVHMSH2PhysSel = new TH1D("hEventMultCVHMSH2PhysSel","Event multiplicity (CVHMSH2 events passing Phys. Selection); n tracklets",200,0,200);
  TH1D* hEventMultCVHMSH2PhysSelEventCuts = new TH1D("hEventMultCVHMSH2PhysSelEventCuts","Event multiplicity (CVHMSH2 events passing Phys. Selection & EventCuts); n tracklets",200,0,200);
  // pt dist
  TH1D* hPtCINT7All = new TH1D("hPtCINT7All","Pt dist (CINT7); track pt (GeV/c)",100,0,100);
  TH1D* hPtCINT7AllPhysSel = new TH1D("hPtCINT7AllPhysSel","Pt dist (CINT7); track pt (GeV/c)",100,0,100);
  TH1D* hPtCINT7AllPhysSelEventCuts = new TH1D("hPtCINT7AllPhysSelEventCuts","Pt dist (CINT7); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2All = new TH1D("hPtCVHMSH2All","Pt dist (CVHMSH2); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2AllPhysSel = new TH1D("hPtCVHMSH2AllPhysSel","Pt dist (CVHMSH2); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2AllPhysSelEventCuts = new TH1D("hPtCVHMSH2AllPhysSelEventCuts","Pt dist (CVHMSH2); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs90 = new TH1D("hPtCVHMSH2Thrs90","Pt dist (CVHMSH2|above 90%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs90PhysSel = new TH1D("hPtCVHMSH2Thrs90PhysSel","Pt dist (CVHMSH2|above 90%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs90PhysSelEventCuts = new TH1D("hPtCVHMSH2Thrs90PhysSelEventCuts","Pt dist (CVHMSH2|above 90%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs95 = new TH1D("hPtCVHMSH2Thrs95","Pt dist (CVHMSH2|above 95%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs95PhysSel = new TH1D("hPtCVHMSH2Thrs95PhysSel","Pt dist (CVHMSH2|above 95%); track pt (GeV/c)",100,0,100);
  TH1D* hPtCVHMSH2Thrs95PhysSelEventCuts = new TH1D("hPtCVHMSH2Thrs95PhysSelEventCuts","Pt dist (CVHMSH2|above 95%); track pt (GeV/c)",100,0,100);



  // variables inside loop

  Double_t dNumFORon = -1.;
  Double_t dNumFORof = -1.;

  Bool_t bIsCINT7 = kFALSE;
  Bool_t bIs0VHM = kFALSE;
  Bool_t bIs0SH2 = kFALSE;
  Bool_t bIsCVHMSH2 = kFALSE;

  // looping over entries
  Int_t numEvents = eventTree->GetEntriesFast();
  printf("Found %d events\nTo be processed: %d\n",numEvents,iNumEventsToProcess);

  for (Int_t i(0); i < numEvents; i++)
  {
    // if(i > 5) break;
    if( iNumEventsToProcess >= 0 && i > iNumEventsToProcess) break;
    if( (i % 10000) == 0) printf("=== Procesed %d / %d events === \n",i,numEvents);
    eventTree->GetEvent(i);
    hEventCounter->Fill("Input",1);

    dNumFORon = fFiredChipMapFO->CountBits(400);
    dNumFORof = fFiredChipMap->CountBits(400);

    // trigger classes check
    bIsCINT7 = fClassesFired->String().Contains("CINT7");
    bIsCVHMSH2 = fClassesFired->String().Contains("CVHMSH2");

    if(bCheckVHMSPDconsistency)
    {
      bIs0SH2 = fFiredTriggerInputs->String().Contains("0SH2");
      bIs0VHM = fFiredTriggerInputs->String().Contains("0VHM");
    }

    // From Michele analysis.C macro : apparently definition changed from 15l -> 16k (not working)
    // bIs0VHM = fL0inputs & 1<<10;
    // bIs0SH2 = fL0inputs & 1<<13;

    // by hand implementation (true? apparently SH2 requires at least 1 BG flag)
    // bIs0VHM = (fV0AFlagsBB >=1 && fV0CFlagsBB >=1 && fV0AFlagsBG <=15 && fV0CFlagsBG <=3);
    // bIs0SH2 = (dNumFORon >= 70);

    if(bIsCINT7) { hEventCounter->Fill("INT7",1); }
    if(bIs0SH2) { hEventCounter->Fill("SH2",1); }
    if(bIs0VHM) { hEventCounter->Fill("VHM",1); }
    if(bIsCINT7 && bIs0SH2 && bIs0VHM) { hEventCounter->Fill("INT7 & SH2 & VHM",1); }
    if(bIsCVHMSH2) { hEventCounter->Fill("VHMSH2",1); }
    if(bCheckVHMSPDconsistency && bIsCINT7 && bIs0SH2 && bIs0VHM && !bIsCVHMSH2)
    {
      hEventCounter->Fill("INT7 && SH2 & VHM & !VHMSH2",1);
      printf(" !!! INT7 && !SH2: #FO %d (on) %d (off) | BB %d (A) %d (C) | BG %d (A) %d (C)\n",(Short_t)dNumFORon,(Short_t)dNumFORof,fV0AFlagsBB,fV0CFlagsBB,fV0AFlagsBG,fV0CFlagsBG);
      printf(" L0 inputs: %d | SH2 %d | VHM %d | VHMSH2 %d | INT7 %d\n", fL0inputs, bIs0SH2, bIs0VHM, bIsCVHMSH2, bIsCINT7);
    }

    if(!bIsCINT7) continue; // SPDHM should be subset of INT7 (MB) logic

    // Purity(multiplicity) = #Events after Phys.Selection / #Events

    hEventMultAll->Fill(fNumTracks);
    if(fPhysSelPassed) hEventMultAllPhysSel->Fill(fNumTracklets);
    if(fPhysSelPassed && fEventCutsPassed) hEventMultAllPhysSelEventCuts->Fill(fNumTracklets);

    if(bIsCINT7)
    {
      hEventMultINT7->Fill(fNumTracks);
      // AddArrToHist(fTracksPt,hPtCINT7All);

      if(fPhysSelPassed)
      {
        hEventMultINT7PhysSel->Fill(fNumTracklets);
        // AddArrToHist(fTracksPt,hPtCINT7AllPhysSel);
      }

      if(fPhysSelPassed && fEventCutsPassed)
      {
        hEventMultINT7PhysSelEventCuts->Fill(fNumTracklets);
        // AddArrToHist(fTracksPt,hPtCINT7AllPhysSelEventCuts);
      }
    }

    if(bIsCVHMSH2)
    {
      hEventMultCVHMSH2->Fill(fNumTracks);
      // AddArrToHist(fTracksPt,hPtCVHMSH2All);

      if(fPhysSelPassed)
      {
        hEventMultCVHMSH2PhysSel->Fill(fNumTracklets);
        // AddArrToHist(fTracksPt,hPtCVHMSH2AllPhysSel);
      }

      if(fPhysSelPassed && fEventCutsPassed)
      {
        hEventMultCVHMSH2PhysSelEventCuts->Fill(fNumTracklets);
        // AddArrToHist(fTracksPt,hPtCVHMSH2AllPhysSelEventCuts);
      }
    }

  }

  // obtaining Purity
  TH1D* hPurityAll = (TH1D*) hEventMultAll->Clone("hPurityAll");
  hPurityAll->Divide(hEventMultAllPhysSel,hEventMultAll,1.,1.);
  hPurityAll->SetTitle("Purity (all events); n tracklets; PhysSel / all");

  TH1D* hPurityINT7 = (TH1D*) hEventMultINT7->Clone("hPurityINT7");
  hPurityINT7->Divide(hEventMultINT7PhysSel,hEventMultINT7,1.,1.);
  hPurityINT7->SetTitle("Purity (INT7 events); n tracklets; PhysSel / INT7");

  TH1D* hPurityCVHMSH2 = (TH1D*) hEventMultCVHMSH2->Clone("hPurityCVHMSH2");
  hPurityCVHMSH2->Divide(hEventMultCVHMSH2PhysSel,hEventMultCVHMSH2,1.,1.);
  hPurityCVHMSH2->SetTitle("Purity (CVHMSH2 events); n tracklets; PhysSel / CVHMSH2");

  // obtaining efficiency, threshold, turn-on
  TH1D* hEffPhysSel = GetEfficiency(hEventMultCVHMSH2PhysSel,hEventMultINT7PhysSel);
  hEffPhysSel->SetNameTitle("hEffCVHMSH2CINT7PhysSel","Efficiency CVHMSH2 / CINT7 (after PhysSel); n tracklets; eff.");
  Int_t iThrs90 = hEffPhysSel->FindFirstBinAbove(0.9,1) - 1;
  Int_t iThrs95 = hEffPhysSel->FindFirstBinAbove(0.95,1) - 1;
  printf("Eff. threshold multiplicity: %d tracklets (90%%) | %d tracklets (95%%)\n",iThrs90,iThrs95);
  Double_t dTurnOn90 = GetTurnOn(hEventMultCVHMSH2PhysSel,iThrs90);
  Double_t dTurnOn95 = GetTurnOn(hEventMultCVHMSH2PhysSel,iThrs95);
  printf("Turn-on: %g (90%%) | %g (95%%)\n",dTurnOn90,dTurnOn95);

  // loop over events again for pt dist
  if(iThrs90 != -1 && iThrs95 != -1)
  {
    printf("Obtaining pt distributions\n");
    for (Int_t i(0); i < numEvents; i++)
    {
      if( iNumEventsToProcess >= 0 && i > iNumEventsToProcess) break;
      if( (i % 10000) == 0) printf("=== Procesed %d / %d events === \n",i,numEvents);
      eventTree->GetEvent(i);

      bIsCINT7 = fClassesFired->String().Contains("CINT7");
      bIsCVHMSH2 = fClassesFired->String().Contains("CVHMSH2");

      if(bIsCINT7)
      {
        AddArrToHist(fTracksPt,hPtCINT7All);

        if(fPhysSelPassed)
        {
          AddArrToHist(fTracksPt,hPtCINT7AllPhysSel);

          if(fEventCutsPassed)
          {
            AddArrToHist(fTracksPt,hPtCINT7AllPhysSelEventCuts);
          }
        }
      } // end of CINT7 cond


      if(bIsCVHMSH2)
      {
        AddArrToHist(fTracksPt,hPtCVHMSH2All);
        if(fPhysSelPassed)
        {
          AddArrToHist(fTracksPt,hPtCVHMSH2AllPhysSel);
          if(fNumTracklets >= iThrs90) AddArrToHist(fTracksPt,hPtCVHMSH2Thrs90PhysSel);
          if(fNumTracklets >= iThrs95) AddArrToHist(fTracksPt,hPtCVHMSH2Thrs95PhysSel);

          if(fEventCutsPassed)
          {
            AddArrToHist(fTracksPt,hPtCVHMSH2AllPhysSelEventCuts);
            if(fNumTracklets >= iThrs90) AddArrToHist(fTracksPt,hPtCVHMSH2Thrs90PhysSelEventCuts);
            if(fNumTracklets >= iThrs95) AddArrToHist(fTracksPt,hPtCVHMSH2Thrs95PhysSelEventCuts);
          }
        }
      } // end of CHVMSH2 cond
    } // end loop over events
  } // end of threshold cond

  // obtaining pt dep. turn-on
  TH1D* hPtTurnOnAll = (TH1D*) hPtCVHMSH2AllPhysSel->Clone("hPtTurnOnAll");
  hPtTurnOnAll->Divide(hPtCVHMSH2AllPhysSel,hPtCINT7AllPhysSel,1.,1.);
  hPtTurnOnAll->SetTitle("Pt turn-on (all); track pt (GeV/c); trigger/MB");

  TH1D* hPtTurnOnThrs90 = (TH1D*) hPtCVHMSH2Thrs90PhysSel->Clone("hPtTurnOnAll");
  hPtTurnOnThrs90->Divide(hPtCVHMSH2Thrs90PhysSel,hPtCINT7AllPhysSel,1.,1.);
  hPtTurnOnThrs90->SetTitle("Pt turn-on (90%); track pt (GeV/c); trigger/MB");

  TH1D* hPtTurnOnThrs95 = (TH1D*) hPtCVHMSH2Thrs95PhysSel->Clone("hPtTurnOnAll");
  hPtTurnOnThrs95->Divide(hPtCVHMSH2Thrs95PhysSel,hPtCINT7AllPhysSel,1.,1.);
  hPtTurnOnThrs95->SetTitle("Pt turn-on (95%); track pt (GeV/c); trigger/MB");

  // Writing to output file
  TFile* fOutputFile = new TFile(sOutputPath.Append("Processed.root").Data(),"RECREATE");
  if(fOutputFile->IsOpen())
  {
    hEventCounter->Write();
    hEventMultAll->Write();
    hEventMultAllPhysSel->Write();
    hEventMultAllPhysSelEventCuts->Write();
    hEventMultINT7->Write();
    hEventMultINT7PhysSel->Write();
    hEventMultINT7PhysSelEventCuts->Write();
    hEventMultCVHMSH2->Write();
    hEventMultCVHMSH2PhysSel->Write();
    hEventMultCVHMSH2PhysSelEventCuts->Write();

    hPtCINT7All->Write();
    hPtCINT7AllPhysSel->Write();
    hPtCINT7AllPhysSelEventCuts->Write();
    hPtCVHMSH2All->Write();
    hPtCVHMSH2AllPhysSel->Write();
    hPtCVHMSH2AllPhysSelEventCuts->Write();
    hPtCVHMSH2Thrs90->Write();
    hPtCVHMSH2Thrs90PhysSel->Write();
    hPtCVHMSH2Thrs90PhysSelEventCuts->Write();
    hPtCVHMSH2Thrs95->Write();
    hPtCVHMSH2Thrs95PhysSel->Write();
    hPtCVHMSH2Thrs95PhysSelEventCuts->Write();

    hPurityAll->Write();
    hPurityINT7->Write();
    hPurityCVHMSH2->Write();

    hEffPhysSel->Write();

    hPtTurnOnAll->Write();
    hPtTurnOnThrs90->Write();
    hPtTurnOnThrs95->Write();
  }

  // Drawing stuff

  hEventMultAllPhysSel->SetLineColor(kRed);
  hEventMultINT7PhysSel->SetLineColor(kRed);
  hEventMultCVHMSH2PhysSel->SetLineColor(kRed);

  TCanvas* canPurity = new TCanvas("canPurity","canPurity",1500,1000);
  canPurity->Divide(3,2);
  canPurity->cd(1);
  gPad->SetLogy();
  hEventMultAllPhysSel->Draw();
  hEventMultAll->Draw("same");

  canPurity->cd(2);
  gPad->SetLogy();
  hEventMultINT7PhysSel->Draw();
  hEventMultINT7->Draw("same");

  canPurity->cd(3);
  gPad->SetLogy();
  hEventMultCVHMSH2PhysSel->Draw();
  hEventMultCVHMSH2->Draw("same");

  canPurity->cd(4);
  hPurityAll->Draw();

  canPurity->cd(5);
  hPurityINT7->Draw();

  canPurity->cd(6);
  hPurityCVHMSH2->Draw();

  // efficiency
  TLine* lineThrs90 = new TLine();
  lineThrs90->SetLineColor(kRed);
  lineThrs90->SetLineWidth(2);
  TLine* lineThrs95 = new TLine();
  lineThrs95->SetLineColor(kGreen+2);
  lineThrs95->SetLineWidth(2);
  TLatex* latexThrs90 = new TLatex();
  latexThrs90->SetNDC();
  latexThrs90->SetTextColor(kRed);
  TLatex* latexThrs95 = new TLatex();
  latexThrs95->SetNDC();
  latexThrs95->SetTextColor(kGreen+2);

  TCanvas* canEff = new TCanvas("canEff","canEff");
  canEff->Divide(2,1);
  canEff->cd(1);
  gPad->SetLogy();
  hEventMultINT7PhysSel->Draw();
  hEventMultCVHMSH2PhysSel->Draw("same");
  lineThrs90->DrawLine(iThrs90,0.,iThrs90,hEventMultINT7PhysSel->GetMaximum());
  lineThrs95->DrawLine(iThrs95,0.,iThrs95,hEventMultINT7PhysSel->GetMaximum());
  latexThrs90->DrawLatex(0.5,0.65, TString::Format("90%%: %g %%",dTurnOn90).Data());
  latexThrs95->DrawLatex(0.5,0.7, TString::Format("95%%: %g %%",dTurnOn95).Data());
  canEff->cd(2);
  hEffPhysSel->Draw();
  lineThrs90->DrawLine(iThrs90,0.,iThrs90,1.);
  lineThrs95->DrawLine(iThrs95,0.,iThrs95,1.);

  // pt turn on
  TCanvas* canPtTurn = new TCanvas("canPtTurn","canPtTurn",1500,500);
  canPtTurn->Divide(3,1);
  canPtTurn->cd(1);
  hPtTurnOnAll->Draw();
  canPtTurn->cd(2);
  hPtTurnOnThrs90->Draw();
  canPtTurn->cd(3);
  hPtTurnOnThrs95->Draw();



  return;
}

TH1D* AddArrToHist(TArrayD* arr, TH1D* hist)
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

TH1D* GetEfficiency(TH1D* trigger, TH1D* mb)
{
  if(!trigger || !mb) { printf("ERROR: histos does not exists!\n"); return 0x0; }

  TH1D* hEfficiency = (TH1D*) trigger->Clone("hEfficiency");
  hEfficiency->Divide(trigger,mb,1.,1.);

  return hEfficiency;
}

Double_t GetTurnOn(TH1D* mult, Int_t threshold)
{
  if(!mult) { printf("ERROR: Multiplicity histo does not exists!\n"); return -1.; }
  threshold++; // histo bin number offset : starting with 1 instead of 0

  Double_t dEntries = mult->GetEntries();
  Double_t dInt = mult->Integral(threshold,mult->GetNbinsX()+1);
  // printf("Histo: Entries %g | ntegral %g | single bin (thrs) %g \n",mult->GetEntries(), mult->Integral(threshold,mult->GetNbinsX()+1), mult->Integral(threshold,threshold));
  return dInt / dEntries;
}
