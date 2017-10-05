Bool_t local = 0;
Bool_t gridTest = 0;

// periods LHC 2016 datasets pp 13 TeV
// LHC16k:  RunList_LHC16k_pass1_CentralBarrelTracking_electronPID_20170217_v2.txt [194 runs]
TString dataDir = "/alice/data/2016/LHC16p";
TString dataPattern = "/pass1/*/AliESDs.root";
TString workingDir = "hm-16p";

// LHC17m // ITS QA OK
Int_t runList[] = {
  278914, 278915, 278936, 278939, 278941, 278959, 278960, 278963, 278964, 278999,
  279000, 279005, 279007, 279008, 279035, 279036, 279041, 279043, 279044, 279068,
  279069, 279073, 279074, 279075, 279106, 279107, 279117, 279118, 279122, 279123,
  279130, 279155, 279157, 279199, 289201, 279207, 279208, 279232, 279234, 279235,
  279238, 279242, 279264, 279265, 279267, 279268, 279270, 279273, 279274, 279309,
  279349, 279354, 279355
}

// LHC16k
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

// LHC16o
Int_t runList[] = {
  264035, 264033, 263985, 263984, 263981, 263978, 263977, 263923, 263920, 263917,
  263916, 263905, 263866, 263863, 263810, 263803, 263793, 263792, 263790, 263787,
  263786, 263785, 263784, 263744, 263743, 263741, 263739, 263738, 263737, 263691,
  263690, 263682, 263663, 263662, 263657, 263654, 263652, 263647, 263529, 263497,
  263496, 263490, 263487, 263332, 263331, 262858, 262855, 262853, 262849, 262847,
  262844, 262842, 262841, 262778, 262777, 262776, 262768, 262760, 262727, 262725,
  262723, 262719, 262717, 262713, 262708, 262706, 262705, 262428, 262426, 262425,
  262424
};

// LHC16p
Int_t runList[] = {
  264347, 264346, 264345, 264341, 264336, 264312, 264306, 264305, 264281, 264279,
  264277, 264273, 264267, 264266, 264265, 264264, 264262, 264261, 264260, 264259,
  264238, 264235, 264233, 264232, 264198, 264197, 264194, 264190, 264188, 264168,
  264164, 264139, 264138, 264137, 264129, 264110, 264109, 264086, 264085, 264082,
  264078, 264076
};

//LHC15l
Int_t runList[] = {
  241544, 241542, 241531, 241523, 241521, 241513, 241511, 241508, 241412, 241407,
  241361, 241360, 241354, 241296, 241295, 241288, 241281, 241268, 241267, 241263,
  241261, 241257, 241144, 241141, 241075, 241069, 241062, 241057, 241055, 241054,
  241050, 241047, 241010, 241001, 240874, 240872, 240860, 240854, 240845, 240612,
  240610, 240609, 240452, 240450, 240312, 240303, 240293, 240274, 240271, 240265,
  240262, 240241, 240069, 239519, 239319
};

Int_t nRuns = 50; // 194;
// Int_t nRuns = sizeof(runList) / sizeof(runList[0]);

void runEsd(){
  gSystem->AddIncludePath("-I. -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");

  // Load common libraries
  gSystem->Load("libTree.so");
  gSystem->Load("libGeom.so");
  gSystem->Load("libVMC.so");

  // Use AliRoot includes to compile our task
  //  gROOT->ProcessLine(".include $ALICE_ROOT/include");
  gSystem->AddIncludePath("-I$ALICE_ROOT/include");
  gSystem->AddIncludePath("-I$ALICE_PHYSICS/include");
  gSystem->AddIncludePath("-I$ALICE_PHYSICS/../src/OADB/COMMON/MULTIPLICITY");
  gSystem->Load("libPhysics.so");
  gSystem->Load("libOADB.so");
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libANALYSISaliceBase");
  gSystem->Load("libCORRFW");


  // if (!TGrid::Connect("alien://")) return;

  AliAnalysisManager *mgr = new AliAnalysisManager("Analysis");
//  AliAODInputHandler* handler = new AliAODInputHandler();
  AliESDInputHandler* handler = new AliESDInputHandler();
  handler->SetReadFriends(kTRUE);
  mgr->SetInputEventHandler(handler);

  gROOT->ProcessLine(".L $ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
  AliPhysicsSelectionTask* physSelTask= AddTaskPhysicsSelection(kFALSE,kTRUE);

  gROOT->LoadMacro("AliAnalysisTaskFilterTrigHMSPD.cxx+g");
  gROOT->LoadMacro("AddTaskFilterTrigHMSPD.C");
  AliAnalysisTaskFilterTrigHMSPD *ana = AddTaskFilterTrigHMSPD();

  if (!mgr->InitAnalysis()) return;
  if(local)
  {
		gROOT->ProcessLine(".include $ROOTSYS/include");
		gROOT->ProcessLine(".include $ALICE_ROOT/include");

    printf("----- Running locally ------\n");
    TChain* chain = new TChain("esdTree");
    chain->Add("~/NBI/triggerHMstudies/alice/data/2016/LHC16k/000258537/pass1/16000258537020.603/AliESDs.root");
    mgr->SetUseProgressBar(1, 500);
    mgr->StartAnalysis("local", chain); // start the analysis locally, reading the events from the TChain
  }
  else
  {
    printf("----- Running on grid ------\n");
    // if we want to run on grid, we create and configure the plugin
    AliAnalysisAlien *alienHandler = new AliAnalysisAlien();
    // also specify the include (header) paths on grid
    alienHandler->AddIncludePath("-I$ROOTSYS/include -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
    // make sure your source files get copied to grid
    alienHandler->SetAdditionalLibs("AliAnalysisTaskFilterTrigHMSPD.cxx AliAnalysisTaskFilterTrigHMSPD.h");
    alienHandler->SetAnalysisSource("AliAnalysisTaskFilterTrigHMSPD.cxx");
    // select the aliphysics version. all other packages
    // are LOADED AUTOMATICALLY!
    alienHandler->SetAliPhysicsVersion("vAN-20170820-1");
    //alienHandler->SetAliPhysicsVersion("vAN-20160131-1");
    // select the input data
    alienHandler->SetGridDataDir(dataDir);
    alienHandler->SetDataPattern(dataPattern);
    // MC has no prefix, data has prefix 000
    alienHandler->SetRunPrefix("000");
    // runnumber

    for (Int_t i = 0; i < nRuns; i++) { alienHandler->AddRunNumber(runList[i]); }

    alienHandler->SetMasterResubmitThreshold(90);
    // number of files per subjob
    alienHandler->SetSplitMaxInputFileNumber(300);
    // number of runs per master
    alienHandler->SetNrunsPerMaster(1);

    // alienHandler->SetExecutable("FlowPID.sh");
    // specify how many seconds your job may take
    alienHandler->SetTTL(20000);
    // alienHandler->SetJDLName("FlowPID.jdl");
    alienHandler->SetPrice(1);
    alienHandler->SetOutputToRunNo(kTRUE);
    alienHandler->SetKeepLogs(kTRUE);

    alienHandler->SetMaxMergeStages(2);
    alienHandler->SetMergeViaJDL(kTRUE);

    // define the output folders
    alienHandler->SetGridWorkingDir(workingDir);
    alienHandler->SetGridOutputDir("output");

    // connect the alien plugin to the manager
    mgr->SetGridHandler(alienHandler);
    if(gridTest) {
        // speficy on how many files you want to run
        alienHandler->SetNtestFiles(2);
        // and launch the analysis
        alienHandler->SetRunMode("test");
        mgr->StartAnalysis("grid");
    } else {
        // else launch the full grid analysis
        alienHandler->SetRunMode("full");
        mgr->StartAnalysis("grid");
    }
  }
}
