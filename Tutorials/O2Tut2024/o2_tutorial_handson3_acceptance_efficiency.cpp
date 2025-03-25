void o2_tutorial_handson3_acceptance_efficiency()
{
	TFile* infile = new TFile("AnalysisResults-handson3.root", "read");

	const int PIDNUM = 3;
	const int REBIN = 2;

	TH1F* Reco[PIDNUM];
	TH1F* Gene[PIDNUM];
	TH1F* AccEff[PIDNUM];
	TString particle[PIDNUM] = {"Pion", "Kaon", "Proton"};
	
	for( int i=0; i<PIDNUM; i++ )
	{
		TString inputhis_reco = "Pt"+particle[i]+"Track";
		TString inputhis_gene= "Pt"+particle[i]+"Gen";
		TString copyname = particle[i]+"AccEff";
		
		Reco[i] = (TH1F*)((TDirectory*)infile->Get("handson3")) -> Get(inputhis_reco);
		Gene[i] = (TH1F*)((TDirectory*)infile->Get("handson3")) -> Get(inputhis_gene);

		Reco[i] -> Sumw2();
		Gene[i] -> Sumw2();

		Reco[i] -> Rebin(REBIN);
		Gene[i] -> Rebin(REBIN);

		AccEff[i] = (TH1F*)Reco[i] -> Clone(copyname);
		AccEff[i] -> Divide(Gene[i]);
	}

	// Draw Acc x Eff to canvas
	gStyle -> SetOptStat(0);

	TCanvas* cn = new TCanvas("cn", "", 800, 600);
	cn -> SetTicks(1, 1);
	cn -> SetMargin(0.12, 0.12, 0.12, 0.12);

	TH1F* htmp = (TH1F*)gPad -> DrawFrame(0, 0, 10, 1.5);
	htmp -> GetXaxis() -> SetTitle("p_{T}(GeV/c)");
	htmp -> GetYaxis() -> SetTitle("Acceptance x efficiency");
	
	htmp -> GetXaxis() -> SetRangeUser(0, 4);

	AccEff[0] -> SetLineColor(kGreen+1);
	AccEff[0] -> SetMarkerColor(kGreen+1);
	AccEff[0] -> SetMarkerStyle(20);
	AccEff[0] -> SetMarkerSize(0.668);
	AccEff[0] -> Draw("p same");

	AccEff[1] -> SetLineColor(kRed+1);
	AccEff[1] -> SetMarkerColor(kRed+1);
	AccEff[1] -> SetMarkerStyle(20);
	AccEff[1] -> SetMarkerSize(0.668);
	AccEff[1] -> Draw("p same");

	AccEff[2] -> SetLineColor(kBlue+1);
	AccEff[2] -> SetMarkerColor(kBlue+1);
	AccEff[2] -> SetMarkerStyle(20);
	AccEff[2] -> SetMarkerSize(0.668);
	AccEff[2] -> Draw("p same");

	TLegend* l1 = new TLegend(0.55, 0.23, 0.84, 0.38);
	l1 -> SetBorderSize(0);
	l1 -> SetTextSize(0.035);
	l1 -> AddEntry(AccEff[0], "#pi", "l");
	l1 -> AddEntry(AccEff[1], "K", "l");
	l1 -> AddEntry(AccEff[2], "p", "l");
	l1 -> Draw("same");

}
