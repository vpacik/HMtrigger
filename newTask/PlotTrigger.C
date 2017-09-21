void PlotTrigger()
{
  const char* sInputPath = "/Users/vpacik/NBI/triggerHMstudies/newTask/running/15l-2/";
  // const char* sInputPath = "/Users/vpacik/NBI/triggerHMstudies/newTask/";
  TString sOutputPath = sInputPath;
  // TString sOutputPath = "/Users/vpacik/NBI/triggerHMstudies/newTask/running/16k/";


  // loading stuff
  TFile* fInputFile = TFile::Open(Form("%s/Processed.root",sInputPath),"READ");
  if(!fInputFile->IsOpen()) return;

  // fInputFile->ls();

  TH1D*	hEventCounter = (TH1D*) fInputFile->Get("hEventCounter");
  TH2D*	h2EventCounter = (TH2D*) fInputFile->Get("h2EventCounter");
  TH1D*	hEventMultINT7 = (TH1D*) fInputFile->Get("hEventMultINT7");
  TH1D*	hEventMultINT7PhysSel = (TH1D*) fInputFile->Get("hEventMultINT7PhysSel");
  TH1D*	hEventMultINT7PhysSelEventCuts = (TH1D*) fInputFile->Get("hEventMultINT7PhysSelEventCuts");
  TH1D*	hEventMultCVHMSH2 = (TH1D*) fInputFile->Get("hEventMultCVHMSH2");
  TH1D*	hEventMultCVHMSH2PhysSel = (TH1D*) fInputFile->Get("hEventMultCVHMSH2PhysSel");
  TH1D*	hEventMultCVHMSH2PhysSelEventCuts = (TH1D*) fInputFile->Get("hEventMultCVHMSH2PhysSelEventCuts");
  TH2D*	h2FOROnOffINT7 = (TH2D*) fInputFile->Get("h2FOROnOffINT7");
  TH2D*	h2FOROnOffINT7PhysSel = (TH2D*) fInputFile->Get("h2FOROnOffINT7PhysSel");
  TH2D*	h2FOROnOffVHMSH2 = (TH2D*) fInputFile->Get("h2FOROnOffVHMSH2");
  TH2D*	h2FOROnOffVHMSH2PhysSel = (TH2D*) fInputFile->Get("h2FOROnOffVHMSH2PhysSel");
  TH2D*	h2FOROffTrackletsINT7PhysSel = (TH2D*) fInputFile->Get("h2FOROffTrackletsINT7PhysSel");
  TH2D*	h2FOROffTrackletsVHMSH2PhysSel = (TH2D*) fInputFile->Get("h2FOROffTrackletsVHMSH2PhysSel");
  TH2D*	h2TrackletsTracksINT7PhysSel = (TH2D*) fInputFile->Get("h2TrackletsTracksINT7PhysSel");
  TH2D*	h2TrackletsTracksVHMSH2PhysSel = (TH2D*) fInputFile->Get("h2TrackletsTracksVHMSH2PhysSel");
  TH1D*	hRejectionFactor = (TH1D*) fInputFile->Get("hRejectionFactor");
  TH1D*	hPurityINT7 = (TH1D*) fInputFile->Get("hPurityINT7");
  TH1D*	hPurityCVHMSH2 = (TH1D*) fInputFile->Get("hPurityCVHMSH2");
  TH1D*	hEffPhysSel = (TH1D*) fInputFile->Get("hEffPhysSel");
  TH1I*	hThrsPhysSel = (TH1I*) fInputFile->Get("hThrsPhysSel");
  TH1D*	hTurnonPhysSel = (TH1D*) fInputFile->Get("hTurnonPhysSel");
  TH2D*	h2PtINT7 = (TH2D*) fInputFile->Get("h2PtINT7");
  TH2D*	h2PtINT7PhysSel = (TH2D*) fInputFile->Get("h2PtINT7PhysSel");
  TH2D*	h2PtVHMSH2 = (TH2D*) fInputFile->Get("h2PtVHMSH2");
  TH2D*	h2PtVHMSH2PhysSel = (TH2D*) fInputFile->Get("h2PtVHMSH2PhysSel");
  TH1D*	hPtDistINT7PhysSelAll = (TH1D*) fInputFile->Get("hPtDistINT7PhysSelAll");
  TH1D*	hPtDistINT7PhysSelThrs90 = (TH1D*) fInputFile->Get("hPtDistINT7PhysSelThrs90");
  TH1D*	hPtDistINT7PhysSelThrs95 = (TH1D*) fInputFile->Get("hPtDistINT7PhysSelThrs95");
  TH1D*	hPtDistVHMSH2PhysSelAll = (TH1D*) fInputFile->Get("hPtDistVHMSH2PhysSelAll");
  TH1D*	hPtDistVHMSH2PhysSelThrs90 = (TH1D*) fInputFile->Get("hPtDistVHMSH2PhysSelThrs90");
  TH1D*	hPtDistVHMSH2PhysSelThrs95 = (TH1D*) fInputFile->Get("hPtDistVHMSH2PhysSelThrs95");
  TH1D*	hPtTurnOnAll = (TH1D*) fInputFile->Get("hPtTurnOnAll");
  TH1D*	hPtTurnOnThrs90 = (TH1D*) fInputFile->Get("hPtTurnOnThrs90");
  TH1D*	hPtTurnOnThrs95 = (TH1D*) fInputFile->Get("hPtTurnOnThrs95");

  // Drawing stuff
  Int_t iThrs90 = hThrsPhysSel->GetBinContent(1);
  Int_t iThrs95 = hThrsPhysSel->GetBinContent(2);
  Double_t dTurnOn90 = hTurnonPhysSel->GetBinContent(1);
  Double_t dTurnOn95 = hTurnonPhysSel->GetBinContent(2);

  Double_t dPurityINT7 = hEventMultINT7PhysSel->GetEntries() / hEventMultINT7->GetEntries();
  Double_t dPurityVHMSH2 = hEventMultCVHMSH2PhysSel->GetEntries() / hEventMultCVHMSH2->GetEntries();

  TLatex* latexINT7 = new TLatex();
  latexINT7->SetNDC();
  latexINT7->SetTextColor(kBlue);
  TLatex* latexVHMSH2 = new TLatex();
  latexVHMSH2->SetNDC();
  latexVHMSH2->SetTextColor(kRed);

  TCanvas* canPurity = new TCanvas("canPurity","canPurity",1000,1000);
  canPurity->Divide(2,2);
  canPurity->cd(1);
  gPad->SetLogy();
  hEventMultINT7->SetLineColor(kBlue);
  hEventMultINT7->SetFillColor(kBlue);
  hEventMultINT7->SetFillStyle(3004);
  hEventMultINT7PhysSel->SetLineColor(kRed);
  hEventMultINT7PhysSel->SetFillColor(kRed);
  hEventMultINT7PhysSel->SetFillStyle(3005);
  hEventMultINT7PhysSelEventCuts->SetLineColor(kGreen+2);
  hEventMultINT7PhysSelEventCuts->SetFillColor(kGreen+2);
  hEventMultINT7PhysSelEventCuts->SetFillStyle(3013);
  hEventMultINT7->DrawCopy();
  hEventMultINT7PhysSel->DrawCopy("same");
  // hEventMultINT7PhysSelEventCuts->Draw("same");
  latexINT7->DrawLatex(0.5,0.65,"Before PhysSel");
  latexVHMSH2->DrawLatex(0.5,0.7,"After PhysSel");

  canPurity->cd(2);
  gPad->SetLogy();
  hEventMultCVHMSH2->SetLineColor(kBlue);
  hEventMultCVHMSH2->SetFillColor(kBlue);
  hEventMultCVHMSH2->SetFillStyle(3004);
  hEventMultCVHMSH2PhysSel->SetLineColor(kRed);
  hEventMultCVHMSH2PhysSel->SetFillColor(kRed);
  hEventMultCVHMSH2PhysSel->SetFillStyle(3005);
  hEventMultCVHMSH2PhysSelEventCuts->SetLineColor(kGreen+2);
  hEventMultCVHMSH2PhysSelEventCuts->SetFillColor(kGreen+2);
  hEventMultCVHMSH2PhysSelEventCuts->SetFillStyle(3013);
  hEventMultCVHMSH2->Draw();
  hEventMultCVHMSH2PhysSel->Draw("same");
  // hEventMultCVHMSH2PhysSelEventCuts->Draw("same");
  latexINT7->DrawLatex(0.5,0.65,"Before PhysSel");
  latexVHMSH2->DrawLatex(0.5,0.7,"After PhysSel");

  canPurity->cd(3);
  hPurityINT7->SetMarkerStyle(kOpenCircle);
  hPurityINT7->SetMarkerSize(0.8);
  hPurityINT7->SetMarkerColor(kRed);
  hPurityINT7->SetStats(0);
  hPurityINT7->SetMinimum(0);
  hPurityINT7->SetMaximum(1.05);
  hPurityINT7->Draw("p0");
  latexVHMSH2->DrawLatex(0.55,0.8,Form("Int. purity: %g",dPurityINT7));

  canPurity->cd(4);
  hPurityCVHMSH2->SetMarkerStyle(kOpenCircle);
  hPurityCVHMSH2->SetMarkerSize(0.8);
  hPurityCVHMSH2->SetMarkerColor(kRed);
  hPurityCVHMSH2->SetStats(0);
  hPurityCVHMSH2->SetMinimum(0);
  hPurityCVHMSH2->SetMaximum(1.05);
  hPurityCVHMSH2->Draw("p0");
  latexVHMSH2->DrawLatex(0.55,0.8,Form("Int. purity: %g",dPurityVHMSH2));
  canPurity->SaveAs(Form("%s/canPurity.pdf",sOutputPath.Data()),"pdf");

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


  // hEventMultINT7PhysSel->Scale(1/0.00162);
  // hEventMultCVHMSH2PhysSel->Scale(1/0.25);

  TCanvas* canEff = new TCanvas("canEff","canEff",1000,500);
  canEff->Divide(2,1);
  canEff->cd(1);
  gPad->SetLogy();
  hEventMultINT7PhysSel->SetLineColor(kBlue);
  hEventMultINT7PhysSel->SetFillColor(kBlue);
  hEventMultINT7PhysSel->SetFillStyle(3004);
  hEventMultINT7PhysSel->SetLineColor(kBlue);
  hEventMultINT7PhysSel->SetFillColor(kBlue);
  hEventMultINT7PhysSel->SetFillStyle(3004);
  hEventMultINT7PhysSel->Draw();
  hEventMultCVHMSH2PhysSel->Draw("same");
  lineThrs90->DrawLine(iThrs90,0.,iThrs90,hEventMultINT7PhysSel->GetMaximum());
  lineThrs95->DrawLine(iThrs95,0.,iThrs95,hEventMultINT7PhysSel->GetMaximum());
  latexThrs90->DrawLatex(0.46,0.65, TString::Format("90%% (%d): %g",iThrs90,dTurnOn90).Data());
  latexThrs95->DrawLatex(0.46,0.7, TString::Format("95%% (%d): %g",iThrs95,dTurnOn95).Data());
  canEff->cd(2);
  hEffPhysSel->SetStats(0);
  hEffPhysSel->Draw();
  lineThrs90->DrawLine(iThrs90,0.,iThrs90,1.);
  lineThrs95->DrawLine(iThrs95,0.,iThrs95,1.);
  canEff->SaveAs(Form("%s/canEff.pdf",sOutputPath.Data()),"pdf");


  // pt turn on

  // scaling distributions -> pdf
  hPtDistINT7PhysSelAll->Scale(1/hPtDistINT7PhysSelAll->Integral(1,hPtDistINT7PhysSelAll->GetNbinsX()));
  hPtDistVHMSH2PhysSelAll->Scale(1/hPtDistVHMSH2PhysSelAll->Integral(1,hPtDistVHMSH2PhysSelAll->GetNbinsX()));
  hPtDistINT7PhysSelThrs90->Scale(1/hPtDistINT7PhysSelThrs90->Integral(1,hPtDistINT7PhysSelThrs90->GetNbinsX()));
  hPtDistVHMSH2PhysSelThrs90->Scale(1/hPtDistVHMSH2PhysSelThrs90->Integral(1,hPtDistVHMSH2PhysSelThrs90->GetNbinsX()));
  hPtDistINT7PhysSelThrs95->Scale(1/hPtDistINT7PhysSelThrs95->Integral(1,hPtDistINT7PhysSelThrs95->GetNbinsX()));
  hPtDistVHMSH2PhysSelThrs95->Scale(1/hPtDistVHMSH2PhysSelThrs95->Integral(1,hPtDistVHMSH2PhysSelThrs95->GetNbinsX()));

  // recomputing turnon
  hPtTurnOnAll->Divide(hPtDistVHMSH2PhysSelAll,hPtDistINT7PhysSelAll);
  hPtTurnOnThrs90->Divide(hPtDistVHMSH2PhysSelThrs90,hPtDistINT7PhysSelThrs90);
  hPtTurnOnThrs95->Divide(hPtDistVHMSH2PhysSelThrs95,hPtDistINT7PhysSelThrs95);


  TCanvas* canPtTurn = new TCanvas("canPtTurn","canPtTurn",1500,1000);
  canPtTurn->Divide(3,2);
  canPtTurn->cd(1);
  gPad->SetLogy();
  hPtDistINT7PhysSelAll->SetLineColor(kBlue);
  hPtDistINT7PhysSelAll->SetFillColor(kBlue);
  hPtDistINT7PhysSelAll->SetFillStyle(3004);
  hPtDistVHMSH2PhysSelAll->SetLineColor(kRed);
  hPtDistVHMSH2PhysSelAll->SetFillColor(kRed);
  hPtDistVHMSH2PhysSelAll->SetFillStyle(3005);
  // hPtDistINT7PhysSelAll->SetMinimum(0.5);
  hPtDistINT7PhysSelAll->Draw();
  hPtDistVHMSH2PhysSelAll->Draw("same");
  latexINT7->DrawLatex(0.5,0.65,"INT7");
  latexVHMSH2->DrawLatex(0.5,0.7,"VHMSH2");

  canPtTurn->cd(2);
  gPad->SetLogy();
  hPtDistINT7PhysSelThrs90->SetLineColor(kBlue);
  hPtDistINT7PhysSelThrs90->SetFillColor(kBlue);
  hPtDistINT7PhysSelThrs90->SetFillStyle(3004);
  hPtDistVHMSH2PhysSelThrs90->SetLineColor(kRed);
  hPtDistVHMSH2PhysSelThrs90->SetFillColor(kRed);
  hPtDistVHMSH2PhysSelThrs90->SetFillStyle(3005);
  // hPtDistINT7PhysSelThrs90->SetMinimum(0.5);
  hPtDistINT7PhysSelThrs90->Draw();
  hPtDistVHMSH2PhysSelThrs90->Draw("same");
  latexINT7->DrawLatex(0.5,0.65,"INT7");
  latexVHMSH2->DrawLatex(0.5,0.7,"VHMSH2");

  canPtTurn->cd(3);
  gPad->SetLogy();
  hPtDistINT7PhysSelThrs95->SetLineColor(kBlue);
  hPtDistINT7PhysSelThrs95->SetFillColor(kBlue);
  hPtDistINT7PhysSelThrs95->SetFillStyle(3004);
  hPtDistVHMSH2PhysSelThrs95->SetLineColor(kRed);
  hPtDistVHMSH2PhysSelThrs95->SetFillColor(kRed);
  hPtDistVHMSH2PhysSelThrs95->SetFillStyle(3005);
  // hPtDistINT7PhysSelThrs95->SetMinimum(0.5);
  hPtDistINT7PhysSelThrs95->Draw();
  hPtDistVHMSH2PhysSelThrs95->Draw("same");
  latexINT7->DrawLatex(0.5,0.65,"INT7");
  latexVHMSH2->DrawLatex(0.5,0.7,"VHMSH2");

  canPtTurn->cd(4);
  hPtTurnOnAll->SetMarkerStyle(kOpenCircle);
  hPtTurnOnAll->SetMarkerSize(0.8);
  hPtTurnOnAll->SetMarkerColor(kRed);
  hPtTurnOnAll->SetMinimum(0.5);
  hPtTurnOnAll->SetMaximum(1.7);
  hPtTurnOnAll->SetStats(0);
  hPtTurnOnAll->Draw("p0");
  // hPtTurnOnAll_New->Draw("same p0");

  canPtTurn->cd(5);
  hPtTurnOnThrs90->SetMarkerStyle(kOpenCircle);
  hPtTurnOnThrs90->SetMarkerSize(0.8);
  hPtTurnOnThrs90->SetMarkerColor(kRed);
  hPtTurnOnThrs90->SetMinimum(0.5);
  hPtTurnOnThrs90->SetMaximum(1.5);
  hPtTurnOnThrs90->SetStats(0);
  hPtTurnOnThrs90->Draw("p0");

  canPtTurn->cd(6);
  hPtTurnOnThrs95->SetMarkerStyle(kOpenCircle);
  hPtTurnOnThrs95->SetMarkerSize(0.8);
  hPtTurnOnThrs95->SetMarkerColor(kRed);
  hPtTurnOnThrs95->SetMinimum(0.5);
  hPtTurnOnThrs95->SetMaximum(1.5);
  hPtTurnOnThrs95->SetStats(0);
  hPtTurnOnThrs95->Draw("p0");
  canPtTurn->SaveAs(Form("%s/canPtTurn.pdf",sOutputPath.Data()),"pdf");


  // mult & FOR
  TCanvas* canFOR = new TCanvas("canFOR","canFOR",1000,1000);
  canFOR->Divide(2,2);
  canFOR->cd(1);
  gPad->SetLogz();
  h2FOROnOffINT7->SetMinimum(1);
  h2FOROnOffINT7->SetStats(0);
  h2FOROnOffINT7->Draw("colz");
  canFOR->cd(2);
  gPad->SetLogz();
  h2FOROnOffVHMSH2->SetMinimum(1);
  h2FOROnOffVHMSH2->SetStats(0);
  h2FOROnOffVHMSH2->Draw("colz");
  canFOR->cd(3);
  gPad->SetLogz();
  h2FOROnOffINT7PhysSel->SetMinimum(1);
  h2FOROnOffINT7PhysSel->SetStats(0);
  h2FOROnOffINT7PhysSel->Draw("colz");
  canFOR->cd(4);
  gPad->SetLogz();
  h2FOROnOffVHMSH2PhysSel->SetMinimum(1);
  h2FOROnOffVHMSH2PhysSel->SetStats(0);
  h2FOROnOffVHMSH2PhysSel->Draw("colz");
  canFOR->SaveAs(Form("%s/canFOR.pdf",sOutputPath.Data()),"pdf");

  TCanvas* canFORtracklets = new TCanvas("canFORtracklets","canFORtracklets",1000,1000);
  canFORtracklets->Divide(2,2);
  canFORtracklets->cd(1);
  gPad->SetLogz();
  h2FOROffTrackletsINT7PhysSel->SetMinimum(1);
  h2FOROffTrackletsINT7PhysSel->SetStats(0);
  h2FOROffTrackletsINT7PhysSel->Draw("colz");
  canFORtracklets->cd(2);
  gPad->SetLogz();
  h2FOROffTrackletsVHMSH2PhysSel->SetMinimum(1);
  h2FOROffTrackletsVHMSH2PhysSel->SetStats(0);
  h2FOROffTrackletsVHMSH2PhysSel->Draw("colz");
  canFORtracklets->cd(3);
  gPad->SetLogz();
  h2TrackletsTracksINT7PhysSel->SetMinimum(1);
  h2TrackletsTracksINT7PhysSel->SetStats(0);
  h2TrackletsTracksINT7PhysSel->Draw("colz");
  canFORtracklets->cd(4);
  gPad->SetLogz();
  h2TrackletsTracksVHMSH2PhysSel->SetMinimum(1);
  h2TrackletsTracksVHMSH2PhysSel->SetStats(0);
  h2TrackletsTracksVHMSH2PhysSel->Draw("colz");
  canFORtracklets->SaveAs(Form("%s/canFORtracklets.pdf",sOutputPath.Data()),"pdf");

  return;
}
