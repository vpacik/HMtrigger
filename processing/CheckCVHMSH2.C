#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"
#include "TBits.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TColor.h"


const Int_t iNumTypes = 5;
enum eEventType { kAll = 0, kCINT7, kCVHMSH2, kCINT7_PhysSel, kCVHMSH2_PhysSel};
TString sTypeLabels[iNumTypes] = {"ALL", "CINT7", "CVHMSH2", "CINT7_PhysSel", "CVHMSH2_PhysSel"};

const Int_t iNumMult = 2;
enum eMult { kNtrklets=0, kRefMult08};
TString sMultLabels[iNumMult] = {"Ntrklets", "RefMult08"};

Int_t iCutOFonline[] = {-1};
Double_t dCutOFonline[] = {-1};
const Int_t iNumCutOFonline = sizeof(iCutOFonline)/sizeof(iCutOFonline[0]);
Color_t colorsCutOFonline[] = {kBlue+2, kRed, kGreen+2, kOrange+2, kMagenta+2, kCyan+2, kRed+1};

Color_t colorMB = kBlue;
Color_t colorHM = kRed;

TList* listOut[iNumTypes];
TH1D* hEventCounter;

TH1D* hDistFOonline[iNumTypes];
TH1D* hDistFOoffline[iNumTypes];

TH1D* hDist[iNumMult][iNumTypes];
TH2D* h2_FOonline[iNumMult][iNumTypes];

void SetCustomPalette();
void LoadHistos(TFile* file);
TH1D* ProjectDist(TH2D* hist, Int_t iCut);
TH1D* GetEfficiency(TH1D* trigger, TH1D* mb);
Double_t GetTurnOn(TH1D* mult, Int_t threshold);

void CheckCVHMSH2()
{
  TString sPath = "/Users/vpacik/Codes/ALICE/HMtrigger/running/18f-pass1/";
  TString sInFileName = "Skimmed.root";
  TString sOutputPath = sPath + "/check/";

  // TString sPath = "/Users/vpacik/NBI/ALICE/HMtrigger/running/17o/";
  // TString sInFileName = "Skimmed_HMwithPFPU.root";
  // TString sOutputPath = sPath + "/check_HWwithPFPU/";

  // TString sInFileName = "Skimmed_287323_287325.root";
  // TString sOutputPath = sPath + "/check_287323_287325/";

  // TString sInFileName = "Skimmed_987b.root";
  // TString sOutputPath = sPath + "/check_987b/";
  // TString sInFileName = "Skimmed_2556b.root";
  // TString sOutputPath = sPath + "/check_2556b/";

  // Double_t dDownscaling = 1.0;
  Double_t dDownscaling = 0.295/0.0027;


  // ===============================================================================
  gSystem->mkdir(sOutputPath.Data(),1);

  // openning input & output file
  TString sInputFile = sPath + sInFileName;
  TFile* fInputFile = new TFile(sInputFile.Data(),"READ");
  if(!fInputFile->IsOpen()) { printf(" ERROR: fInputFile not open!\n"); return; }


  LoadHistos(fInputFile);
  SetCustomPalette();

  Int_t nPnt  = iNumCutOFonline;
  Int_t nnCol = gStyle->GetNumberOfColors();


  // ### Multiplicity vs OFO correlations
  TLine* line_cor_unity = new TLine();
  line_cor_unity->SetLineColor(kOrange+7);
  line_cor_unity->SetLineWidth(1.0);

  for(Int_t iMult(0); iMult < iNumMult; ++iMult)
  {
    TCanvas* canCorr = new TCanvas("canCorr", "canCorr", 1200,500);
    canCorr->Divide(2,1);
    canCorr->cd(1);
    gPad->SetLogz();
    h2_FOonline[iMult][kCVHMSH2]->GetXaxis()->SetRangeUser(0,250);
    h2_FOonline[iMult][kCVHMSH2]->SetStats(0);
    h2_FOonline[iMult][kCVHMSH2]->Draw("colz");
    line_cor_unity->DrawLine(0,0,200,200);
    canCorr->cd(2);
    gPad->SetLogz();
    h2_FOonline[iMult][kCVHMSH2_PhysSel]->GetXaxis()->SetRangeUser(0,250);
    h2_FOonline[iMult][kCVHMSH2_PhysSel]->SetStats(0);
    h2_FOonline[iMult][kCVHMSH2_PhysSel]->Draw("colz");
    line_cor_unity->DrawLine(0,0,200,200);
    canCorr->SaveAs(Form("%s/corr_%s.pdf",sOutputPath.Data(), sMultLabels[iMult].Data()),"pdf");
  }

  // ### Slicing distribution based on OFO cut
  printf("INFO : Slicing the distributions based on OFO threshold\n");
  TH1D* hDistMult[iNumMult][iNumTypes][iNumCutOFonline];

  for(Int_t iType(0); iType < iNumTypes; ++iType)
  {
    for(Int_t iMult(0); iMult < iNumMult; ++iMult)
    {
      for(Int_t iCut(0); iCut < iNumCutOFonline; ++iCut)
      {
        hDistMult[iMult][iType][iCut] = ProjectDist(h2_FOonline[iMult][iType],iCutOFonline[iCut]);

        Int_t idx = iCut * Float_t(nnCol-1) / (nPnt-1);
        // Int_t iColor = gStyle->GetColorPalette(idx);
        Int_t iColor = colorHM;
        hDistMult[iMult][iType][iCut]->SetLineColor(iColor);
        hDistMult[iMult][iType][iCut]->SetMarkerColor(iColor);
        hDistMult[iMult][iType][iCut]->SetMarkerStyle(kFullCircle);
        hDistMult[iMult][iType][iCut]->SetMarkerSize(0.4);
        if(iType == kCVHMSH2 || iType == kCVHMSH2_PhysSel) { hDistMult[iMult][iType][iCut]->Scale(1.0/dDownscaling); }
      }
    }
  }


  // ### Calculating efficiencies based on OFO cut
  printf("INFO : Calculating efficiencies\n");
  TH1D* hEff[iNumMult][iNumTypes][iNumCutOFonline];

  for(Int_t iMult(0); iMult < iNumMult; ++iMult)
  {
    Int_t iType = kCVHMSH2_PhysSel;
    TH1D* hDistMult_CINT7 = hDistMult[iMult][kCINT7_PhysSel][0];
    hDistMult_CINT7->SetLineColor(colorMB);

    for(Int_t iCut(0); iCut < iNumCutOFonline; ++iCut)
    {
      hEff[iMult][iType][iCut] = GetEfficiency(hDistMult[iMult][iType][iCut],hDistMult_CINT7);
    }

    // Getting thresholds & fractions of SH2 events
    TGraphErrors* graph_turnon = new TGraphErrors(iNumCutOFonline);
    graph_turnon->SetMarkerStyle(kFullCircle);
    graph_turnon->SetMarkerColor(kRed);

    TLegend* legThrs = new TLegend(0.12,0.7,0.4,0.88);
    legThrs->SetFillColorAlpha(0.0,0.0);
    legThrs->SetBorderSize(0);
    legThrs->SetHeader("Mult. thresholds");
    legThrs->SetTextSize(0.04);

    for(Int_t iCut(0); iCut < iNumCutOFonline; ++iCut)
    {
      Int_t iThrsOFObin = hEff[iMult][iType][iCut]->FindFirstBinAbove(0.95);
      Int_t iThrsOFO = hEff[iMult][iType][iCut]->GetBinCenter(iThrsOFObin);
      Double_t dTurnOn = GetTurnOn(hDistMult[iMult][iType][iCut], iThrsOFObin);
      printf("Cut %d | Thrs %d (bin %d) | TurnOn %f \n", iCutOFonline[iCut], iThrsOFO, iThrsOFObin, dTurnOn);
      graph_turnon->SetPoint(iCut, iCutOFonline[iCut], dTurnOn);
      legThrs->AddEntry(hDistMult[iMult][iType][iCut],Form("%d",iThrsOFO),"l");

    }

    TLegend* leg = new TLegend(0.4,0.7,0.88,0.88);
    leg->SetFillColorAlpha(0.0,0.0);
    leg->SetBorderSize(0);
    // leg->SetHeader("FO online thrs.");
    leg->SetTextSize(0.04);
    leg->AddEntry(hDistMult_CINT7,"CINT7-B-NOPF-CENT","l");


    TLine* line = new TLine();
    line->SetLineStyle(kDashed);
    line->SetLineColor(kRed);
    line->SetLineWidth(2.0);

    TCanvas* canEff = new TCanvas("canEff","canEff",1500,600);
    canEff->Divide(3,1);

    canEff->cd(1);
    gPad->SetLogy();
    hDistMult_CINT7->SetStats(0);
    hDistMult_CINT7->Draw();
    for(Int_t iCut(0); iCut < iNumCutOFonline; ++iCut)
    {
      hDistMult[iMult][iType][iCut]->Draw("same");
      leg->AddEntry(hDistMult[iMult][iType][iCut],"CVHMSH2-B","l");
    }

    leg->Draw();
    canEff->cd(2);
    TH1* frame_canEff_2 = (TH1*) gPad->DrawFrame(0,0.0,120,1.1);
    frame_canEff_2->SetTitle(Form("Efficiency: SPDHM / CINT7 (after Physics Selection); %s; Eff",sMultLabels[iMult].Data()));
    gPad->SetGridx();
    for(Int_t iCut(0); iCut < iNumCutOFonline; ++iCut)
    {
      Int_t idx = iCut * Float_t(nnCol-1) / (nPnt-1);
      // Int_t iColor = gStyle->GetColorPalette(idx);
      Int_t iColor = colorHM;
      hEff[iMult][iType][iCut]->SetLineColor(iColor);

      hEff[iMult][iType][iCut]->Draw("same");
    }
    line->DrawLine(0,0.95,120,0.95);
    legThrs->Draw();
    canEff->cd(3);
    TH1* frame_canEff_3 = (TH1*) gPad->DrawFrame(iCutOFonline[0]-5,0.0,iCutOFonline[iNumCutOFonline-1]+5,0.50);
    frame_canEff_3->SetTitle(Form("TurnOn (95%% thrs): %s (with Physics Selection); #FO online; TurnOn",sMultLabels[iMult].Data()));
    gPad->SetGridy();
    graph_turnon->Draw("same p");

    canEff->SaveAs(Form("%s/eff_%s_%s.pdf",sOutputPath.Data(), sTypeLabels[iType].Data(), sMultLabels[iMult].Data()),"pdf");
  }

  // ### Calculating purity
  for(Int_t iMult(0); iMult < iNumMult; ++iMult)
  {
    TLegend* leg = new TLegend(0.4,0.7,0.88,0.88);
    leg->SetFillColorAlpha(0.0,0.0);
    leg->SetBorderSize(0);
    // leg->SetHeader("FO online thrs.");
    leg->SetTextSize(0.04);

    TCanvas* canPurity = new TCanvas("canPurity","canPurity",1500,600);
    canPurity->Divide(3,1);

    canPurity->cd(1);
    gPad->SetLogy();
    TH1* frame_canPurity_1 = (TH1*) gPad->DrawFrame(0.0,1,150,1e5);
    frame_canPurity_1->SetTitle(Form("Multiplicity distribution (CVHMSH2); %s;;", hDistMult[iMult][kCVHMSH2][0]->GetXaxis()->GetTitle()));
    leg->Draw();

    canPurity->cd(2);
    TH1* frame_canPurity_2 = (TH1*) gPad->DrawFrame(0.0,0.0,150,1.0);
    frame_canPurity_2->SetTitle(Form("Purity (CVHMSH2); %s; Purity;", hDistMult[iMult][kCVHMSH2][0]->GetXaxis()->GetTitle()));

    canPurity->cd(3);
    gPad->SetGridy();
    TH1* frame_canPurity_3 = (TH1*) gPad->DrawFrame(iCutOFonline[0]-5,0.5,iCutOFonline[iNumCutOFonline-1]+5,1.0);
    frame_canPurity_3->SetTitle(Form("Integrated purity (CVHMSH2); #FO online; Purity"));



    TH1D* hPurityCVHMSH2[iNumCutOFonline];
    Double_t dIntPurityCVHMSH2[iNumCutOFonline];

    TGraphErrors* graph_IntPurity_CVHMSH2 = new TGraphErrors(iNumCutOFonline);
    graph_IntPurity_CVHMSH2->SetMarkerStyle(kFullCircle);
    graph_IntPurity_CVHMSH2->SetMarkerColor(kRed);

    for(Int_t iCut(0); iCut < iNumCutOFonline; ++iCut)
    {
      leg->AddEntry(hDistMult[iMult][kCVHMSH2][iCut],"Before AliPhysics");
      leg->AddEntry(hDistMult[iMult][kCVHMSH2_PhysSel][iCut],"Passing AliPhysics");

      hPurityCVHMSH2[iCut] = (TH1D*) hDistMult[iMult][kCVHMSH2][iCut]->Clone(Form("purity_%s", hDistMult[iMult][kCVHMSH2][iCut]->GetName()));
      hPurityCVHMSH2[iCut]->Divide(hDistMult[iMult][kCVHMSH2_PhysSel][iCut],hDistMult[iMult][kCVHMSH2][iCut],1.,1.);
      // hPurityCVHMSH2[iCut]->SetTitle("Purity (INT7); # tracklets; PhysSel / INT7");

      dIntPurityCVHMSH2[iCut] = hDistMult[iMult][kCVHMSH2_PhysSel][iCut]->GetEntries() / hDistMult[iMult][kCVHMSH2][iCut]->GetEntries();
      graph_IntPurity_CVHMSH2->SetPoint(iCut, iCutOFonline[iCut], dIntPurityCVHMSH2[iCut]);

      canPurity->cd(1);
      hDistMult[iMult][kCVHMSH2][iCut]->SetLineColor(colorMB);
      hDistMult[iMult][kCVHMSH2][iCut]->SetMarkerColor(colorMB);
      hDistMult[iMult][kCVHMSH2][iCut]->Draw("same");
      hDistMult[iMult][kCVHMSH2_PhysSel][iCut]->SetLineColor(colorHM);
      hDistMult[iMult][kCVHMSH2_PhysSel][iCut]->SetMarkerColor(colorHM);
      hDistMult[iMult][kCVHMSH2_PhysSel][iCut]->Draw("same");

      canPurity->cd(2);
      hPurityCVHMSH2[iCut]->Draw("same");
    }

    canPurity->cd(3);
    graph_IntPurity_CVHMSH2->Draw("same p");

    canPurity->SaveAs(Form("%s/purity_%s_%s.pdf",sOutputPath.Data(), "CVHMSH2", sMultLabels[iMult].Data()),"pdf");
  }

  // ### Calculating rejection factors
  printf("INFO : Calculating rejection factors\n");
  TH1D* hRejectionFactor = new TH1D("hRejectionFactor","hRejectionFactor",iNumCutOFonline-1,dCutOFonline);
  hRejectionFactor->SetMarkerStyle(kFullCircle);
  hRejectionFactor->SetMarkerColor(kBlack);
  TGraphErrors* graph_RejectionFactor = new TGraphErrors(iNumCutOFonline);
  graph_RejectionFactor->SetMarkerStyle(kFullCircle);
  graph_RejectionFactor->SetMarkerColor(kBlack);

  Int_t iNumEvents_CINT7 = hDistFOonline[kCINT7]->GetEntries();
  Int_t iNumEvents_CVHSH2[iNumCutOFonline];
  Double_t dRejectionFactor[iNumCutOFonline];

  Int_t iType = kCVHMSH2;
  Int_t iMult = kRefMult08;

  printf("=== Rejection factors (w/o PhysSel) =====\n");
  printf("CINT7 %d \n",iNumEvents_CINT7);
  for(Int_t iCut(0); iCut < iNumCutOFonline; ++iCut)
  {
    iNumEvents_CVHSH2[iCut] = hDistMult[iMult][iType][iCut]->GetEntries();
    dRejectionFactor[iCut] = ((Double_t) iNumEvents_CVHSH2[iCut]) / iNumEvents_CINT7 / dDownscaling;
    printf("%d: %d | rejection %f \n",iCutOFonline[iCut], iNumEvents_CVHSH2[iCut], dRejectionFactor[iCut]);

    // hRejectionFactor->SetBinContent(iCut+1, dRejectionFactor[iCut]);
    graph_RejectionFactor->SetPoint(iCut, iCutOFonline[iCut], dRejectionFactor[iCut]);
  }

  TCanvas* canReject = new TCanvas("canReject","canRejection",800,800);
  canReject->cd(1);
  gPad->SetLogy();
  TH1* frame_canReject_1 = (TH1*) gPad->DrawFrame(iCutOFonline[0]-5,1e-4,iCutOFonline[iNumCutOFonline-1]+5,0.1);
  frame_canReject_1->SetTitle(Form("Rejection factor (w/o Physics Selection); #FO online; Rejection"));
  gPad->SetGridy();

  hRejectionFactor->Draw("same p");
  graph_RejectionFactor->Draw("same p");
  canReject->SaveAs(Form("%s/rejection_%s.pdf",sOutputPath.Data(), sTypeLabels[iType].Data()),"pdf");

  return;
}
// =====================================================================================================================
TH1D* ProjectDist(TH2D* hist, Int_t iCut)
{
  if(!hist) { printf("ERROR-ProjectDist: no hist \n"); return 0x0; }

  // TH2D* hist_temp = (TH1D*) hist->Clone("_proj");
  // if(!hist_temp) { printf("ERROR-ProjectDist: not cloned \n"); return 0x0; }

  // hist_temp->GetXaxis()

  Int_t iLowBinIndex = hist->GetXaxis()->FindBin(iCut);

  TH1D* proj = (TH1D*) hist->ProjectionY(Form("%s_OFO%d",hist->GetName(), iCut), iLowBinIndex, -1);
  if(!proj) { printf("ERROR-ProjectDist: not projected \n"); return 0x0; }

  return proj;
}
// =====================================================================================================================
TH1D* GetEfficiency(TH1D* trigger, TH1D* mb)
{
  if(!trigger || !mb) { printf("ERROR: histos does not exists!\n"); return 0x0; }

  TH1D* hEfficiency = (TH1D*) trigger->Clone("hEfficiency");
  hEfficiency->Divide(trigger,mb,1.,1.);

  return hEfficiency;
}
// =====================================================================================================================
Double_t GetTurnOn(TH1D* mult, Int_t threshold)
{
  if(!mult) { printf("ERROR: Multiplicity histo does not exists!\n"); return -1.; }
  threshold++; // histo bin number offset : starting with 1 instead of 0

  // Double_t dEntries = mult->GetEntries();
  Double_t dInt = mult->Integral(threshold,mult->GetNbinsX()+1);
  Double_t dEntries = mult->Integral(0,mult->GetNbinsX()+1);
  // printf("Histo: Entries %g | ntegral %g | single bin (thrs) %g \n",mult->GetEntries(), mult->Integral(threshold,mult->GetNbinsX()+1), mult->Integral(threshold,threshold));

  // printf("INFO: GetTurnOn : %f / %f = %f (%d)\n",dInt, dEntries, dInt / dEntries, threshold);

  return dInt / dEntries;
}
// =====================================================================================================================
void LoadHistos(TFile* file)
{
  if(!file) { printf("ERROR-LoadHistos: no file!\n"); return; }

  // file->ls();

  // hEventCounter = new TH1D("hEventCounter", "hEventCounter", 1,0,1);

  for(Int_t iType(0); iType < iNumTypes; ++iType)
  {
    TString sLabel = sTypeLabels[iType];

    listOut[iType] = (TList*) file->Get(Form("list_%s",sLabel.Data()));
    TList* list = listOut[iType];
    if(!list) { printf("ERROR-LoadHistos: no list '%s' loaded!\n",sLabel.Data()); return; }
    // list->ls();

    hDistFOonline[iType] = (TH1D*) list->FindObject(Form("hDistFOonline_%s",sLabel.Data()));
    if(!hDistFOonline[iType]) { printf("ERROR-LoadHistos: no histo 'hDistFOonline_%s'\n",sLabel.Data()); return; }
    hDistFOoffline[iType] = (TH1D*) list->FindObject(Form("hDistFOoffline_%s",sLabel.Data()));
    if(!hDistFOoffline[iType]) { printf("ERROR-LoadHistos: no histo 'hDistFOoffline_%s'\n",sLabel.Data()); return; }


    for(Int_t iMult(0); iMult < iNumMult; ++iMult)
    {
      TString sMult = sMultLabels[iMult];

      hDist[iMult][iType] = (TH1D*) list->FindObject(Form("hDist%s_%s",sMult.Data(),sLabel.Data()));
      if(!hDist[iMult][iType]) { printf("ERROR-LoadHistos: no histo 'hDist%s_%s'\n",sMult.Data(), sLabel.Data()); return; }

      h2_FOonline[iMult][iType] = (TH2D*) list->FindObject(Form("h2_FOonline_%s_%s",sMult.Data(), sLabel.Data()));
      if(!h2_FOonline[iMult][iType]) { printf("ERROR-LoadHistos: no histo 'h2_FOonline_%s_%s'\n",sMult.Data(), sLabel.Data()); return; }


      // // Distributions
      // hDistNtrklets[iType] = (TH1D*) list->FindObject(Form("hDistNtrklets_%s",sLabel.Data()));
      // if(!hDistNtrklets[iType]) { printf("ERROR-LoadHistos: no histo 'hDistNtrklets_%s'\n",sLabel.Data()); return; }
      // hDistNtrks[iType] = (TH1D*) list->FindObject(Form("hDistNtrks_%s",sLabel.Data()));
      // if(!hDistNtrks[iType]) { printf("ERROR-LoadHistos: no histo 'hDistNtrks_%s'\n",sLabel.Data()); return; }
      // hDistNtrks08pt[iType] = (TH1D*) list->FindObject(Form("hDistNtrks08pt_%s",sLabel.Data()));
      // if(!hDistNtrks08pt[iType]) { printf("ERROR-LoadHistos: no histo 'hDistNtrks08pt_%s'\n",sLabel.Data()); return; }
      // hDistRefMult08[iType] = (TH1D*) list->FindObject(Form("hDistRefMult08_%s",sLabel.Data()));
      // if(!hDistRefMult08[iType]) { printf("ERROR-LoadHistos: no histo 'hDistRefMult08_%s'\n",sLabel.Data()); return; }

      // 2D mult correlations
      // h2_FOonline_Ntrklets[iType] = (TH2D*) list->FindObject(Form("h2_FOonline_Ntrklets_%s",sLabel.Data()));
      // if(!h2_FOonline_Ntrklets[iType]) { printf("ERROR-LoadHistos: no histo 'h2_FOonline_Ntrklets_%s'\n",sLabel.Data()); return; }
      // h2_FOonline_Ntrks[iType] = (TH2D*) list->FindObject(Form("h2_FOonline_Ntrks_%s",sLabel.Data()));
      // if(!h2_FOonline_Ntrks[iType]) { printf("ERROR-LoadHistos: no histo 'h2_FOonline_Ntrks_%s'\n",sLabel.Data()); return; }
      // h2_FOonline_Ntrks08pt[iType] = (TH2D*) list->FindObject(Form("h2_FOonline_Ntrks08pt_%s",sLabel.Data()));
      // if(!h2_FOonline_Ntrks08pt[iType]) { printf("ERROR-LoadHistos: no histo 'h2_FOonline_Ntrks08pt_%s'\n",sLabel.Data()); return; }
      // h2_FOonline_RefMult08[iType] = (TH2D*) list->FindObject(Form("h2_FOonline_RefMult08_%s",sLabel.Data()));
      // if(!h2_FOonline_RefMult08[iType]) { printf("ERROR-LoadHistos: no histo 'h2_FOonline_RefMult08_%s'\n",sLabel.Data()); return; }
    }
  }

  printf("INFO-LoadHistos : Loading successful\n");

  return;
}
// ==================================================================================================================
void SetCustomPalette()
{
  Double_t stops[9] = { 0.0000, 0.1250, 0.2500, 0.3750, 0.5000,	0.6250, 0.7500, 0.8750, 1.0000};


  // Rain Bow
  // case 55:
  Double_t red[9]   = {  0./255.,   5./255.,  15./255.,  35./255., 102./255., 196./255., 208./255., 199./255., 110./255.};
  Double_t green[9] = {  0./255.,  48./255., 124./255., 192./255., 206./255., 226./255.,  97./255.,  16./255.,   0./255.};
  Double_t blue[9]  = { 99./255., 142./255., 198./255., 201./255.,  90./255.,  22./255.,  13./255.,   8./255.,   2./255.};

  // Bird
  //case 57:
  // Double_t red[9]   = { 0.2082, 0.0592, 0.0780, 0.0232, 0.1802, 0.5301, 0.8186, 0.9956, 0.9764};
  // Double_t green[9] = { 0.1664, 0.3599, 0.5041, 0.6419, 0.7178, 0.7492, 0.7328, 0.7862, 0.9832};
  // Double_t blue[9]  = { 0.5293, 0.8684, 0.8385, 0.7914, 0.6425, 0.4662, 0.3499, 0.1968, 0.0539};

  // Blue Green Yellow
  //case 71:
  // Double_t red[9]   = { 22./255., 19./255.,  19./255.,  25./255.,  35./255.,  53./255.,  88./255., 139./255., 210./255.};
  // Double_t green[9] = {  0./255., 32./255.,  69./255., 108./255., 135./255., 159./255., 183./255., 198./255., 215./255.};
  // Double_t blue[9]  = { 77./255., 96./255., 110./255., 116./255., 110./255., 100./255.,  90./255.,  78./255.,  70./255.};

  // Solar
  // case 100:
  // Double_t red[9]   = { 99./255., 116./255., 154./255., 174./255., 200./255., 196./255., 201./255., 201./255., 230./255.};
  // Double_t green[9] = {  0./255.,   0./255.,   8./255.,  32./255.,  58./255.,  83./255., 119./255., 136./255., 173./255.};
  // Double_t blue[9]  = {  5./255.,   6./255.,   7./255.,   9./255.,   9./255.,  14./255.,  17./255.,  19./255.,  24./255.};

  // Viridis
  // case 112:
  // Double_t red[9]   = { 26./255., 51./255.,  43./255.,  33./255.,  28./255.,  35./255.,  74./255., 144./255., 246./255.};
  // Double_t green[9] = {  9./255., 24./255.,  55./255.,  87./255., 118./255., 150./255., 180./255., 200./255., 222./255.};
  // Double_t blue[9]  = { 30./255., 96./255., 112./255., 114./255., 112./255., 101./255.,  72./255.,  35./255.,   0./255.};

  // Cividis
  // case 113:
  // Double_t red[9]   = {  0./255.,   5./255.,  65./255.,  97./255., 124./255., 156./255., 189./255., 224./255., 255./255.};
  // Double_t green[9] = { 32./255.,  54./255.,  77./255., 100./255., 123./255., 148./255., 175./255., 203./255., 234./255.};
  // Double_t blue[9]  = { 77./255., 110./255., 107./255., 111./255., 120./255., 119./255., 111./255.,  94./255.,  70./255.};

  Int_t pal = TColor::CreateGradientColorTable(9, stops, red, green, blue, 255, 1);
  const Int_t nCol = 255;
  Int_t colors[nCol];
  for (int i=0; i<nCol; i++) colors[i] =pal+i;

  gStyle->SetPalette(nCol,colors);
}
// ==================================================================================================================
