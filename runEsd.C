Bool_t local = kTRUE;

// periods LHC 2016 datasets pp 13 TeV
// LHC16k:  RunList_LHC16k_pass1_CentralBarrelTracking_electronPID_20170217_v2.txt [194 runs]
TString dataDir = "/alice/data/2016/LHC16k";
TString dataPattern = "/pass1/*/AliESDs.root";
TString workingDir = "hmtrigger_16k_test";
Int_t nRuns = 10; // 194;
Int_t runList[] = {
  258537, 258499, 258477, 258456, 258454, 258452, 258426, 258393, 258391, 258387,
  258359, 258336, 258332, 258307, 258306, 258303, 258302, 258301, 258299, 258278,
  258274, 258273, 258271, 258270, 258258, 258257, 258256, 258204, 258203, 258202,
  258198, 258197, 258178, 258117, 258114, 258113, 258109, 258108, 258107, 258063,
  258062, 258060, 258059, 258053, 258049, 258045, 258042, 258041, 258039, 258019,
  258017, 258014, 258012, 258008, 258003, 257992, 257989, 257986, 257979, 257963,
  257960, 257957, 257939, 257937, 257936, 257892, 257855, 257853, 257851, 257850,
  257804, 257803, 257800, 257799, 257798, 257797, 257773, 257765, 257757, 257754,
  257737, 257735, 257734, 257733, 257727, 257725, 257724, 257697, 257694, 257692,
  257691, 257689, 257688, 257687, 257685, 257684, 257682, 257644, 257642, 257636,
  257635, 257632, 257630, 257606, 257605, 257604, 257601, 257595, 257594, 257592,
  257590, 257588, 257587, 257566, 257562, 257561, 257560, 257541, 257540, 257539,
  257537, 257531, 257530, 257492, 257491, 257490, 257488, 257487, 257474, 257468,
  257457, 257433, 257364, 257358, 257330, 257322, 257320, 257318, 257260, 257224,
  257209, 257206, 257204, 257144, 257141, 257139, 257138, 257137, 257136, 257100,
  257095, 257092, 257086, 257084, 257082, 257080, 257077, 257028, 257026, 257021,
  257012, 257011, 256944, 256942, 256941, 256697, 256695, 256694, 256692, 256691,
  256684, 256681, 256677, 256676, 256658, 256620, 256619, 256592, 256591, 256589,
  256567, 256565, 256564, 256562, 256560, 256557, 256556, 256554, 256552, 256514,
  256512, 256510, 256506, 256504
};

void runEsd(){
  gSystem->AddIncludePath("-I. -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");

  if (!TGrid::Connect("alien://")) return;

  AliAnalysisManager *mgr = new AliAnalysisManager("Analysis");
//  AliAODInputHandler* handler = new AliAODInputHandler();
  AliESDInputHandler* handler = new AliESDInputHandler();
  // handler->SetReadFriends(kTRUE);
  mgr->SetInputEventHandler(handler);


  gROOT->LoadMacro("AliAnalysisTaskTrigHMTF.cxx+g");
  AliAnalysisTaskTrigHMTF *ana = new AliAnalysisTaskTrigHMTF();
  mgr->AddTask(ana);
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("events", TTree::Class(),AliAnalysisManager::kOutputContainer,"AnalysisResults.root");
  mgr->ConnectInput  (ana, 0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput (ana, 1, coutput1);

  if (!mgr->InitAnalysis()) return;
  if(local)
  {
    printf("----- Running locally ------\n");
    TChain* chain = new TChain("esdTree");
    chain->Add("~/NBI/triggerHMstudies/alice/data/2016/LHC16k/000258537/pass1/16000258537020.603/AliESDs.root");
    mgr->SetUseProgressBar(1, 500);
    mgr->StartAnalysis("local", chain); // start the analysis locally, reading the events from the TChain
  }
  else
  {
    printf("----- Running on grid ------\n");
    AliAnalysisAlien *plugin = new AliAnalysisAlien();
    plugin->SetRunMode("full");
    plugin->SetNtestFiles(2);
    plugin->SetAPIVersion("V1.1x");
    plugin->SetAliPhysicsVersion("vAN-20170411-1");
    plugin->SetGridDataDir(dataDir.Data());
    plugin->SetDataPattern(dataPattern.Data());
    plugin->SetGridWorkingDir(workingDir.Data());
    plugin->SetRunPrefix("000");
    for (Int_t i=0;i<nRuns;i++)  plugin->AddRunNumber(runList[i]);
    plugin->SetGridOutputDir("output");
    plugin->SetAnalysisSource("AliAnalysisTaskTrigHMTF.cxx");
    plugin->SetAdditionalLibs("AliAnalysisTaskTrigHMTF.h AliAnalysisTaskTrigHMTF.cxx");
    plugin->SetNrunsPerMaster(1);
    plugin->SetSplitMaxInputFileNumber(300);
    plugin->SetMergeViaJDL();
    plugin->SetMaxMergeStages(1);
    plugin->SetOutputToRunNo(1);
    plugin->SetTTL(86400);

    mgr->SetGridHandler(plugin);
    mgr->PrintStatus();
    mgr->StartAnalysis("grid");
    //mgr->StartAnalysis("full");
  }
}
