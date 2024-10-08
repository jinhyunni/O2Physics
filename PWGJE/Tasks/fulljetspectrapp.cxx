// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

// FullJet Spectra in pp
//
// TO DO:
// 1. implement HadCorr and NEF for matched jets
//
/// \author Archita Rani Dash <archita.rani.dash@cern.ch>
#include <vector>
#include <iostream>
#include <utility>

#include "CommonConstants/PhysicsConstants.h"
#include "Framework/ASoA.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/AnalysisTask.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/O2DatabasePDGPlugin.h"
#include "Framework/runDataProcessing.h"
#include "Framework/RunningWorkflowInfo.h"

#include "Common/Core/TrackSelection.h"
#include "Common/Core/TrackSelectionDefaults.h"
#include "Common/DataModel/EventSelection.h"
#include "Common/DataModel/PIDResponse.h"
#include "Common/DataModel/TrackSelectionTables.h"

#include "PWGHF/Core/HfHelper.h"

#include "PWGJE/DataModel/Jet.h"
#include "PWGJE/DataModel/EMCALClusters.h"
#include "PWGJE/DataModel/EMCALMatchedCollisions.h"
#include "PWGJE/Core/JetFinder.h"
#include "PWGJE/Core/JetDerivedDataUtilities.h"
#include "PWGJE/Core/JetFindingUtilities.h"

#include "EventFiltering/filterTables.h"

using namespace std;
using namespace o2;
using namespace o2::analysis;
using namespace o2::framework;
using namespace o2::framework::expressions;

using EMCCollisions = o2::soa::Join<aod::JCollisions, aod::JEMCCollisionLbs>; // needed for the workaround to access EMCAL trigger bits

struct FullJetSpectrapp {

  HistogramRegistry registry;

  // Event configurables
  Configurable<float> VertexZCut{"VertexZCut", 10.0f, "Accepted z-vertex range"};
  Configurable<float> centralityMin{"centralityMin", -999.0, "minimum centrality"};
  Configurable<float> centralityMax{"centralityMax", 999.0, "maximum centrality"};
  Configurable<bool> doEMCALEventWorkaround{"doEMCALEventWorkaround", false, "apply the workaround to read the EMC trigger bit by requiring a cell content in the EMCAL"};
  Configurable<bool> doMBGapTrigger{"doMBGapTrigger", false, "set to true only when using MB-Gap Trigger JJ MC"};

  // Jet configurables
  Configurable<float> selectedJetsRadius{"selectedJetsRadius", 0.4, "resolution parameter for histograms without radius"};
  Configurable<std::vector<double>> jetRadii{"jetRadii", std::vector<double>{0.4}, "jet resolution parameters"};
  Configurable<float> jetpTMin{"jetpTMin", 10.0, "minimum jet pT"};
  Configurable<float> jetpTMax{"jetpTMax", 350., "maximum jet pT"};
  Configurable<float> jetEtaMin{"jetEtaMin", -1.0, "minimum jet eta"};
  Configurable<float> jetEtaMax{"jetEtaMax", 0.3, "maximum jet eta"};  // for now just hard-coding this value for R = 0.4 (EMCAL eta acceptance: eta_jet = 0.7 - R)
  Configurable<float> jetPhiMin{"jetPhiMin", 1.79, "minimum jet phi"}; // phi_jet_min for R = 0.4 is 1.80
  Configurable<float> jetPhiMax{"jetPhiMax", 2.87, "maximum jet phi"}; // phi_jet_min for R = 0.4 is 2.86
  Configurable<float> jetAreaFractionMin{"jetAreaFractionMin", -99.0, "used to make a cut on the jet areas"};
  Configurable<float> leadingConstituentPtMin{"leadingConstituentPtMin", -99.0, "minimum pT selection on jet constituent"};

  // Track configurables
  Configurable<float> trackpTMin{"trackpTMin", 0.15, "minimum track pT"};
  Configurable<float> trackpTMax{"trackpTMax", 350., "maximum track pT"};
  Configurable<float> trackEtaMin{"trackEtaMin", -1.0, "minimum track eta"};
  Configurable<float> trackEtaMax{"trackEtaMax", 0.70, "maximum track eta"}; // emcal eta_track cut
  Configurable<float> trackPhiMin{"trackPhiMin", 0., "minimum track phi"};
  Configurable<float> trackPhiMax{"trackPhiMax", 7., "maximum track phi"};
  Configurable<std::string> trackSelections{"trackSelections", "globalTracks", "set track selections"};
  Configurable<std::string> eventSelections{"eventSelections", "sel8Full", "choose event selection"};
  Configurable<std::string> particleSelections{"particleSelections", "PhysicalPrimary", "set particle selections"};

  // Cluster configurables

  Configurable<std::string> clusterDefinitionS{"clusterDefinition", "kV3Default", "cluster definition to be selected, e.g. V3Default"};
  Configurable<float> clusterEtaMin{"clusterEtaMin", -0.7, "minimum cluster eta"};
  Configurable<float> clusterEtaMax{"clusterEtaMax", 0.7, "maximum cluster eta"};
  Configurable<float> clusterPhiMin{"clusterPhiMin", 1.39, "minimum cluster phi"};
  Configurable<float> clusterPhiMax{"clusterPhiMax", 3.27, "maximum cluster phi"};
  Configurable<float> clusterEnergyMin{"clusterEnergyMin", 0.3, "minimum cluster energy in EMCAL (GeV)"};
  Configurable<float> clusterTimeMin{"clusterTimeMin", -20., "minimum cluster time (ns)"};
  Configurable<float> clusterTimeMax{"clusterTimeMax", 15., "maximum cluster time (ns)"};
  Configurable<bool> clusterRejectExotics{"clusterRejectExotics", true, "Reject exotic clusters"};

  Configurable<float> pTHatMaxMCD{"pTHatMaxMCD", 999.0, "maximum fraction of hard scattering for jet acceptance in detector MC"};
  Configurable<float> pTHatMaxMCP{"pTHatMaxMCP", 999.0, "maximum fraction of hard scattering for jet acceptance in particle MC"};
  Configurable<float> pTHatExponent{"pTHatExponent", 6.0, "exponent of the event weight for the calculation of pTHat"};

  int trackSelection = -1;
  int eventSelection = -1;
  std::vector<bool> filledJetR;
  std::vector<double> jetRadiiValues;

  std::string particleSelection;

  Service<o2::framework::O2DatabasePDG> pdgDatabase;

  // Add Collision Histograms' Bin Labels for clarity
  void labelCollisionHistograms(HistogramRegistry& registry)
  {
    auto h_collisions_unweighted = registry.get<TH1>(HIST("h_collisions_unweighted"));
    h_collisions_unweighted->GetXaxis()->SetBinLabel(2, "total events");
    h_collisions_unweighted->GetXaxis()->SetBinLabel(3, "EMC events with kTVXinEMC");
    h_collisions_unweighted->GetXaxis()->SetBinLabel(4, "EMC events w/o kTVXinEMC");

    if (doprocessTracksWeighted) {
      auto h_collisions_weighted = registry.get<TH1>(HIST("h_collisions_weighted"));
      h_collisions_weighted->GetXaxis()->SetBinLabel(2, "total events");
      h_collisions_weighted->GetXaxis()->SetBinLabel(3, "EMC events with kTVXinEMC");
      h_collisions_weighted->GetXaxis()->SetBinLabel(4, "EMC events w/o kTVXinEMC");
    }
  }

  void init(o2::framework::InitContext&)
  {
    trackSelection = jetderiveddatautilities::initialiseTrackSelection(static_cast<std::string>(trackSelections));
    eventSelection = jetderiveddatautilities::initialiseEventSelection(static_cast<std::string>(eventSelections));
    particleSelection = static_cast<std::string>(particleSelections);
    jetRadiiValues = (std::vector<double>)jetRadii;

    for (std::size_t iJetRadius = 0; iJetRadius < jetRadiiValues.size(); iJetRadius++) {
      filledJetR.push_back(0.0);
    }
    auto jetRadiiBins = (std::vector<double>)jetRadii;
    if (jetRadiiBins.size() > 1) {
      jetRadiiBins.push_back(jetRadiiBins[jetRadiiBins.size() - 1] + (TMath::Abs(jetRadiiBins[jetRadiiBins.size() - 1] - jetRadiiBins[jetRadiiBins.size() - 2])));
    } else {
      jetRadiiBins.push_back(jetRadiiBins[jetRadiiBins.size() - 1] + 0.1);
    }

    // JetTrack QA histograms
    if (doprocessTracks || doprocessTracksWeighted) {
      registry.add("h_collisions_unweighted", "event status; event status;entries", {HistType::kTH1F, {{4, 0., 4.0}}});

      registry.add("h_track_pt", "track pT;#it{p}_{T,track} (GeV/#it{c});entries", {HistType::kTH1F, {{350, 0., 350.}}});
      registry.add("h_track_eta", "track #eta;#eta_{track};entries", {HistType::kTH1F, {{100, -1., 1.}}});
      registry.add("h_track_phi", "track #varphi;#varphi_{track};entries", {HistType::kTH1F, {{160, 0., 7.}}});
      registry.add("h_track_energy", "track energy;Energy of tracks;entries", {HistType::kTH1F, {{400, 0., 400.}}});
      registry.add("h_track_energysum", "track energy sum;Sum of track energy per event;entries", {HistType::kTH1F, {{400, 0., 400.}}});

      // registry.add("h_gaptrig_track_pt", "gap triggered track pT;#it{p}_{T,track} (GeV/#it{c});entries", {HistType::kTH1F, {{350, 0., 350.}}});
      // registry.add("h_gaptrig_track_eta", "gap triggered track #eta;#eta_{track};entries", {HistType::kTH1F, {{100, -1., 1.}}});
      // registry.add("h_gaptrig_track_phi", "gap triggered track #varphi;#varphi_{track};entries", {HistType::kTH1F, {{160, 0., 7.}}});

      // Cluster QA histograms
      registry.add("h_cluster_pt", "cluster pT;#it{p}_{T_cluster} (GeV/#it{c});entries", {HistType::kTH1F, {{200, 0., 200.}}});
      registry.add("h_cluster_eta", "cluster #eta;#eta_{cluster};entries", {HistType::kTH1F, {{100, -1., 1.}}});
      registry.add("h_cluster_phi", "cluster #varphi;#varphi_{cluster};entries", {HistType::kTH1F, {{160, 0., 7.}}});
      registry.add("h_cluster_energy", "cluster energy;Energy of cluster;entries", {HistType::kTH1F, {{400, 0., 400.}}});
      registry.add("h_cluster_energysum", "cluster energy sum;Sum of cluster energy per event;entries", {HistType::kTH1F, {{400, 0., 400.}}});

      // registry.add("h_gaptrig_cluster_pt", "gap triggered cluster pT;#it{p}_{T_cluster} (GeV/#it{c});entries", {HistType::kTH1F, {{200, 0., 200.}}});
      // registry.add("h_gaptrig_cluster_eta", "gap triggered cluster #eta;#eta_{cluster};entries", {HistType::kTH1F, {{100, -1., 1.}}});
      // registry.add("h_gaptrig_cluster_phi", "gap triggered cluster #varphi;#varphi_{cluster};entries", {HistType::kTH1F, {{160, 0., 7.}}});
      // registry.add("h_gaptrig_cluster_energy", "gap triggered cluster #varphi;#varphi_{cluster};entries", {HistType::kTH1F, {{160, 0., 7.}}});

      if (doprocessTracksWeighted) {
        registry.add("h_collisions_weighted", "event status;event status;entries", {HistType::kTH1F, {{5, 0.0, 5.0}}});
        registry.add("h_gaptrig_collisions", "event status; event status; entries", {HistType::kTH1F, {{4, 0.0, 4.0}}});

        // registry.add("h_gaptrig_track_pt", "gap triggered track pT;#it{p}_{T,track} (GeV/#it{c});entries", {HistType::kTH1F, {{350, 0., 350.}}});
        // registry.add("h_gaptrig_track_eta", "gap triggered track #eta;#eta_{track};entries", {HistType::kTH1F, {{100, -1., 1.}}});
        // registry.add("h_gaptrig_track_phi", "gap triggered track #varphi;#varphi_{track};entries", {HistType::kTH1F, {{160, 0., 7.}}});
        //
        // registry.add("h_gaptrig_cluster_pt", "gap triggered cluster pT;#it{p}_{T_cluster} (GeV/#it{c});entries", {HistType::kTH1F, {{200, 0., 200.}}});
        // registry.add("h_gaptrig_cluster_eta", "gap triggered cluster #eta;#eta_{cluster};entries", {HistType::kTH1F, {{100, -1., 1.}}});
        // registry.add("h_gaptrig_cluster_phi", "gap triggered cluster #varphi;#varphi_{cluster};entries", {HistType::kTH1F, {{160, 0., 7.}}});
        // registry.add("h_gaptrig_cluster_energy", "gap triggered cluster #varphi;#varphi_{cluster};entries", {HistType::kTH1F, {{160, 0., 7.}}});
      }
    }

    // Jet QA histograms
    if (doprocessJetsData || doprocessJetsMCD || doprocessJetsMCDWeighted) {
      registry.add("h_full_jet_pt", "#it{p}_{T,jet};#it{p}_{T_jet} (GeV/#it{c});entries", {HistType::kTH1F, {{350, 0., 350.}}});
      registry.add("h_full_jet_eta", "jet #eta;#eta_{jet};entries", {HistType::kTH1F, {{100, -1., 1.}}});
      registry.add("h_full_jet_phi", "jet #varphi;#varphi_{jet};entries", {HistType::kTH1F, {{160, 0., 7.}}});
      registry.add("h2_full_jet_NEF", "#it{p}_{T,jet} vs NEF at Det Level; #it{p}_{T,jet} (GeV/#it{c});NEF", {HistType::kTH2F, {{350, 0., 350.}, {100, 0.0, 1.5}}});
      // registry.add("h_full_mcdjet_tablesize", "", {HistType::kTH1F, {{4, 0., 5.}}});
      // registry.add("h_full_mcdjet_ntracks", "", {HistType::kTH1F, {{200, -0.5, 200.}}});
      // registry.add("h_gaptrig_full_jet_pt", "gap triggered jet pT;#it{p}_{T_jet} (GeV/#it{c});entries", {HistType::kTH1F, {{350, 0., 350.}}});
      // registry.add("h_gaptrig_full_jet_eta", "gap triggered jet #eta;#eta_{jet};entries", {HistType::kTH1F, {{100, -1., 1.}}});
      // registry.add("h_gaptrig_full_jet_phi", "gap triggered jet #varphi;#varphi_{jet};entries", {HistType::kTH1F, {{160, 0., 7.}}});
    }
    if (doprocessJetsMCP || doprocessJetsMCPWeighted) {
      registry.add("h_full_mcpjet_tablesize", "", {HistType::kTH1F, {{4, 0., 5.}}});
      registry.add("h_full_mcpjet_ntracks", "", {HistType::kTH1F, {{200, -0.5, 200.}}});
      registry.add("h_full_jet_pt_part", "jet pT;#it{p}_{T_jet} (GeV/#it{c});entries", {HistType::kTH1F, {{350, 0., 350.}}});
      registry.add("h_full_jet_eta_part", "jet #eta;#eta_{jet};entries", {HistType::kTH1F, {{100, -1., 1.}}});
      registry.add("h_full_jet_phi_part", "jet #varphi;#varphi_{jet};entries", {HistType::kTH1F, {{160, 0., 7.}}});
      registry.add("h2_full_jet_NEF_part", "#it{p}_{T,jet} vs NEF at Part Level;#it{p}_{T,jet} (GeV/#it{c});NEF", {HistType::kTH2F, {{350, 0., 350.}, {100, 0.0, 1.5}}});

      // registry.add("h_gaptrig_full_mcpjet_tablesize", "", {HistType::kTH1F, {{4, 0., 5.}}});
      // registry.add("h_gaptrig_full_mcpjet_ntracks", "", {HistType::kTH1F, {{200, -0.5, 200.}}});
      // registry.add("h_gaptrig_full_jet_pt_part", "jet pT;#it{p}_{T_jet} (GeV/#it{c});entries", {HistType::kTH1F, {{350, 0., 350.}}});
      // registry.add("h_gaptrig_full_jet_eta_part", "jet #eta;#eta_{jet};entries", {HistType::kTH1F, {{100, -1., 1.}}});
      // registry.add("h_gaptrig_full_jet_phi_part", "jet #varphi;#varphi_{jet};entries", {HistType::kTH1F, {{160, 0., 7.}}});
    }

    if (doprocessJetsMCPMCDMatched || doprocessJetsMCPMCDMatchedWeighted) {
      registry.add("h_full_matchedmcdjet_tablesize", "", {HistType::kTH1F, {{4, 0., 5.}}});
      registry.add("h_full_matchedmcdjet_ntracks", "", {HistType::kTH1F, {{200, -0.5, 200.}}});
      registry.add("h_full_jet_energyscaleDet", "Jet Energy Scale (det); p_{T,det} (GeV/c); (p_{T,det} - p_{T,part})/p_{T,part}", {HistType::kTH2F, {{400, 0., 400.}, {200, -1., 1.}}});

      // registry.add("h_full_jet_energyscaleDetCharged", "Jet Energy Scale (det, charged part); p_{t,det} (GeV/c); (p_{t,det} - p_{t,part})/p_{t,part}", {HistType::kTH2F,{{400, 0., 400., 200, -1.,1.}}});
      // registry.add("h_full_jet_energyscaleDetNeutral", "Jet Energy Scale (det, neutral part); p_{t,det} (GeV/c); (p_{t,det} - p_{t,part})/p_{t,part}", {HistType::kTH2F,{{400, 0., 400., 200, -1.,1.}}});
      // registry.add("h_full_jet_energyscaleDetChargedVsFull", "Jet Energy Scale (det, charged part, vs. full jet pt); p_{t,det} (GeV/c); (p_{t,det} - p_{t,part})/p_{t,part}", {HistType::kTH2F,{{400, 0., 400., 200, -1.,1.}}});
      // registry.add("h_full_jet_energyscaleDetNeutralVsFull", "Jet Energy Scale (det, neutral part, vs. full jet pt); p_{t,det} (GeV/c); (p_{t,det} - p_{t,part})/p_{t,part}", {HistType::kTH2F,{{400, 0., 400., 200, -1.,1.}}});
      registry.add("h_full_jet_energyscalePart", "Jet Energy Scale (part); p_{T,part} (GeV/c); (p_{T,det} - p_{T,part})/p_{T,part}", {HistType::kTH2F, {{400, 0., 400.}, {200, -1., 1.}}});
      registry.add("h3_full_jet_energyscalePart", "R dependence of Jet Energy Scale (Part); #it{R}_{jet};p_{T,det} (GeV/c); (p_{T,det} - p_{T,part})/p_{T,part}", {HistType::kTH3F, {{jetRadiiBins, ""}, {400, 0., 400.}, {200, -1., 1.}}});

      // registry.add("h_full_jet_energyscaleCharged", "Jet Energy Scale (charged part); p_{t,part} (GeV/c); (p_{t,det} - p_{t,part})/p_{t,part}", {HistType::kTH2F,{{400, 0., 400., 200, -1.,1.}}});
      // registry.add("h_full_jet_energyscaleNeutral", "Jet Energy Scale (neutral part); p_{t,part} (GeV/c); (p_{t,det} - p_{t,part})/p_{t,part}", {HistType::kTH2F,{{400, 0., 400., 200, -1.,1.}}});
      // registry.add("h_full_jet_energyscaleChargedVsFull", "Jet Energy Scale (charged part, vs. full jet pt); p_{t,part} (GeV/c); (p_{t,det} - p_{t,part})/p_{t,part}", {HistType::kTH2F,{{400, 0., 400., 200, -1.,1.}}});
      // registry.add("h_full_jet_energyscaleNeutralVsFull", "Jet Energy Scale (neutral part, vs. full jet pt); p_{t,part} (GeV/c); (p_{t,det} - p_{t,part})/p_{t,part}", {HistType::kTH2F,{{400, 0., 400., 200, -1.,1.}}});

      // Response Matrix
      registry.add("h_full_jet_ResponseMatrix", "Full Jets Response Matrix; p_{T,det} (GeV/c); p_{T,part} (GeV/c)", {HistType::kTH2F, {{400, 0., 400.}, {400, 0., 400.}}});

      // registry.add("h_gaptrig_full_matchedmcdjet_tablesize", "", {HistType::kTH1F, {{4, 0., 5.}}});
      // registry.add("h_gaptrig_full_matchedmcdjet_ntracks", "", {HistType::kTH1F, {{200, -0.5, 200.}}});
      // registry.add("h_gaptrig_full_jet_energyscaleDet", "Jet Energy Scale (det); p_{T,det} (GeV/c); (p_{T,det} - p_{T,part})/p_{T,part}", {HistType::kTH2F,{{400, 0., 400.}, {200, -1.,1.}}});
      // registry.add("h_gaptrig_full_jet_energyscalePart", "Jet Energy Scale (part); p_{T,part} (GeV/c); (p_{T,det} - p_{T,part})/p_{T,part}", {HistType::kTH2F,{{400, 0., 400.}, {200, -1.,1.}}});
      // registry.add("h_gaptrig_full_jet_ResponseMatrix", "Full Jets Response Matrix; p_{T,det} (GeV/c); p_{T,part} (GeV/c)", {HistType::kTH2F,{{400, 0., 400.}, {400,0.,400.}}});
    }

    // Label the histograms
    labelCollisionHistograms(registry);

  } // init

  using FullJetTableDataJoined = soa::Join<aod::FullJets, aod::FullJetConstituents>;
  using JetTableMCDJoined = soa::Join<aod::FullMCDetectorLevelJets, aod::FullMCDetectorLevelJetConstituents>;
  using JetTableMCDWeightedJoined = soa::Join<aod::FullMCDetectorLevelJets, aod::FullMCDetectorLevelJetConstituents, aod::FullMCDetectorLevelJetEventWeights>;
  using JetTableMCPJoined = soa::Join<aod::FullMCParticleLevelJets, aod::FullMCParticleLevelJetConstituents>;
  using JetTableMCPWeightedJoined = soa::Join<aod::FullMCParticleLevelJets, aod::FullMCParticleLevelJetConstituents, aod::FullMCParticleLevelJetEventWeights>;

  using JetTableMCDMatchedJoined = soa::Join<aod::FullMCDetectorLevelJets, aod::FullMCDetectorLevelJetConstituents, aod::FullMCDetectorLevelJetsMatchedToFullMCParticleLevelJets>;
  using JetTableMCPMatchedJoined = soa::Join<aod::FullMCParticleLevelJets, aod::FullMCParticleLevelJetConstituents, aod::FullMCParticleLevelJetsMatchedToFullMCDetectorLevelJets>;

  using JetTableMCDMatchedWeightedJoined = soa::Join<aod::FullMCDetectorLevelJets, aod::FullMCDetectorLevelJetConstituents, aod::FullMCDetectorLevelJetsMatchedToFullMCParticleLevelJets, aod::FullMCDetectorLevelJetEventWeights>;
  using JetTableMCPMatchedWeightedJoined = soa::Join<aod::FullMCParticleLevelJets, aod::FullMCParticleLevelJetConstituents, aod::FullMCParticleLevelJetsMatchedToFullMCDetectorLevelJets, aod::FullMCParticleLevelJetEventWeights>;

  // Applying some cuts(filters) on collisions, tracks, clusters

  Filter eventCuts = (nabs(aod::jcollision::posZ) < VertexZCut && aod::jcollision::centrality >= centralityMin && aod::jcollision::centrality < centralityMax);
  // Filter EMCeventCuts = (nabs(aod::collision::posZ) < VertexZCut && aod::collision::centrality >= centralityMin && aod::collision::centrality < centralityMax);
  Filter trackCuts = (aod::jtrack::pt >= trackpTMin && aod::jtrack::pt < trackpTMax && aod::jtrack::eta > trackEtaMin && aod::jtrack::eta < trackEtaMax && aod::jtrack::phi >= trackPhiMin && aod::jtrack::phi <= trackPhiMax);
  aod::EMCALClusterDefinition clusterDefinition = aod::emcalcluster::getClusterDefinitionFromString(clusterDefinitionS.value);
  Filter clusterFilter = (aod::jcluster::definition == static_cast<int>(clusterDefinition) && aod::jcluster::eta > clusterEtaMin && aod::jcluster::eta < clusterEtaMax && aod::jcluster::phi >= clusterPhiMin && aod::jcluster::phi <= clusterPhiMax && aod::jcluster::energy >= clusterEnergyMin && aod::jcluster::time > clusterTimeMin && aod::jcluster::time < clusterTimeMax && (clusterRejectExotics && aod::jcluster::isExotic != true));

  template <typename T, typename U>
  bool isAcceptedJet(U const& jet)
  {

    if (jetAreaFractionMin > -98.0) {
      if (jet.area() < jetAreaFractionMin * M_PI * (jet.r() / 100.0) * (jet.r() / 100.0)) {
        return false;
      }
    }
    if (leadingConstituentPtMin > -98.0) {
      bool isMinleadingConstituent = false;
      for (auto& constituent : jet.template tracks_as<T>()) {
        if (constituent.pt() >= leadingConstituentPtMin) {
          isMinleadingConstituent = true;
          break;
        }
      }

      if (!isMinleadingConstituent) {
        return false;
      }
    }
    return true;
  }
  template <typename T>
  void fillJetHistograms(T const& jet, float weight = 1.0)
  {
    float neutralEnergy = 0.0;
    if (jet.r() == round(selectedJetsRadius * 100.0f)) {
      registry.fill(HIST("h_full_jet_pt"), jet.pt(), weight);
      registry.fill(HIST("h_full_jet_eta"), jet.eta(), weight);
      registry.fill(HIST("h_full_jet_phi"), jet.phi(), weight);
      // registry.fill(HIST("h_full_mcdjet_tablesize"), jet.size(), weight);
      // registry.fill(HIST("h_full_mcdjet_ntracks"), jet.tracksIds().size(), weight);
      // registry.fill(HIST("h_full_jet_energyscaleDet"), jet.phi(), weight);
      // }
      for (auto& cluster : jet.template clusters_as<JetClusters>()) {
        neutralEnergy += cluster.energy();
      }
      auto NEF = neutralEnergy / jet.energy();
      registry.fill(HIST("h2_full_jet_NEF"), jet.pt(), NEF, weight);
    }
  }

  template <typename T>
  void fillMCPHistograms(T const& jet, float weight = 1.0)
  {
    float neutralEnergy = 0.0;
    if (jet.r() == round(selectedJetsRadius * 100.0f)) {
      registry.fill(HIST("h_full_mcpjet_tablesize"), jet.size(), weight);
      registry.fill(HIST("h_full_mcpjet_ntracks"), jet.tracksIds().size(), weight);
      registry.fill(HIST("h_full_jet_pt_part"), jet.pt(), weight);
      registry.fill(HIST("h_full_jet_eta_part"), jet.eta(), weight);
      registry.fill(HIST("h_full_jet_phi_part"), jet.phi(), weight);
      // registry.fill(HIST("h_full_jet_ntracks_part"), jet.tracksIds().size(), weight);
      // }
      for (auto& constituent : jet.template tracks_as<JetParticles>()) {
        auto pdgParticle = pdgDatabase->GetParticle(constituent.pdgCode());
        if (pdgParticle->Charge() == 0) {
          neutralEnergy += constituent.e();
        }
        // To Do: Fill particle level track histos
      }
      auto NEF = neutralEnergy / jet.energy();
      registry.fill(HIST("h2_full_jet_NEF_part"), jet.pt(), NEF, weight);
    }
  }

  template <typename T, typename U>
  void fillTrackHistograms(T const& tracks, U const& clusters, float weight = 1.0)
  {
    double sumtrackE = 0.0;
    for (auto const& track : tracks) {
      if (!jetderiveddatautilities::selectTrack(track, trackSelection)) {
        continue;
      }
      sumtrackE += track.energy();
      registry.fill(HIST("h_track_pt"), track.pt(), weight);
      registry.fill(HIST("h_track_eta"), track.eta(), weight);
      registry.fill(HIST("h_track_phi"), track.phi(), weight);
      registry.fill(HIST("h_track_energysum"), sumtrackE, weight);
    }
    double sumclusterE = 0.0;
    for (auto const& cluster : clusters) {
      double clusterpt = cluster.energy() / std::cosh(cluster.eta());
      sumclusterE += cluster.energy();

      registry.fill(HIST("h_cluster_pt"), clusterpt, weight);
      registry.fill(HIST("h_cluster_eta"), cluster.eta(), weight);
      registry.fill(HIST("h_cluster_phi"), cluster.phi(), weight);
      registry.fill(HIST("h_cluster_energy"), cluster.energy(), weight);
      registry.fill(HIST("h_cluster_energysum"), sumclusterE, weight);
    }
  }

  template <typename T, typename U>
  void fillMatchedHistograms(T const& jetBase, float weight = 1.0)
  {

    float pTHat = 10. / (std::pow(weight, 1.0 / pTHatExponent));
    if (jetBase.pt() > pTHatMaxMCD * pTHat) { // Here, jetBase = mcd jets and jetTag = mcp jets
      return;
    }

    if (jetBase.has_matchedJetGeo()) { // geometrical jet matching only needed for pp
      for (auto& jetTag : jetBase.template matchedJetGeo_as<std::decay_t<U>>()) {
        if (jetTag.pt() > pTHatMaxMCP * pTHat) {
          continue;
        }
        // std::cout << jetTag.pt() << endl;
        registry.fill(HIST("h_full_matchedmcdjet_tablesize"), jetBase.size(), weight);
        registry.fill(HIST("h_full_matchedmcdjet_ntracks"), jetBase.tracksIds().size(), weight);
        registry.fill(HIST("h_full_jet_energyscaleDet"), jetBase.pt(), (jetBase.pt() - jetTag.pt()) / jetTag.pt(), weight);
        registry.fill(HIST("h_full_jet_energyscalePart"), jetTag.pt(), (jetBase.pt() - jetTag.pt()) / jetTag.pt(), weight);

        // JES for different jet R values
        registry.fill(HIST("h3_full_jet_energyscalePart"), jetBase.r() / 100.0, jetTag.pt(), (jetBase.pt() - jetTag.pt()) / jetTag.pt(), weight);

        // Response Matrix
        registry.fill(HIST("h_full_jet_ResponseMatrix"), jetBase.pt(), jetTag.pt(), weight); // MCD vs MCP jet pT
      }
    }
  }

  void processDummy(JetCollisions const&)
  {
  }
  PROCESS_SWITCH(FullJetSpectrapp, processDummy, "dummy task", true);

  void processJetsData(soa::Filtered<EMCCollisions>::iterator const&, FullJetTableDataJoined const& jets, JetTracks const&, JetClusters const&)
  {
    for (auto const& jet : jets) {
      if (!jetfindingutilities::isInEtaAcceptance(jet, jetEtaMin, jetEtaMax, trackEtaMin, trackEtaMax)) {
        continue;
      }
      if (!isAcceptedJet<JetTracks>(jet)) {
        continue;
      }
      fillJetHistograms(jet, 1.0);
    }
  }
  PROCESS_SWITCH(FullJetSpectrapp, processJetsData, "Full Jets Data", false);

  void processJetsMCD(soa::Filtered<EMCCollisions>::iterator const&, JetTableMCDJoined const& jets, JetTracks const&, JetClusters const&)
  {
    for (auto const& jet : jets) {
      if (!jetfindingutilities::isInEtaAcceptance(jet, jetEtaMin, jetEtaMax, trackEtaMin, trackEtaMax)) {
        continue;
      }
      if (!isAcceptedJet<JetTracks>(jet)) {
        continue;
      }
      fillJetHistograms(jet, 1.0);
    }
  }
  PROCESS_SWITCH(FullJetSpectrapp, processJetsMCD, "Full Jets at Detector Level", false);

  void processJetsMCDWeighted(soa::Filtered<EMCCollisions>::iterator const&, JetTableMCDWeightedJoined const& jets, JetTracks const&, JetClusters const&)
  {
    for (auto const& jet : jets) {
      if (!jetfindingutilities::isInEtaAcceptance(jet, jetEtaMin, jetEtaMax, trackEtaMin, trackEtaMax)) {
        continue;
      }
      if (!isAcceptedJet<JetTracks>(jet)) {
        continue;
      }
      fillJetHistograms(jet, jet.eventWeight());
    }
  }
  PROCESS_SWITCH(FullJetSpectrapp, processJetsMCDWeighted, "jet finder HF QA mcd on weighted events", false);

  void processJetsMCP(typename JetTableMCPJoined::iterator const& jet, JetParticles const&)
  {
    if (!jetfindingutilities::isInEtaAcceptance(jet, jetEtaMin, jetEtaMax, trackEtaMin, trackEtaMax)) {
      return;
    }
    if (!isAcceptedJet<JetParticles>(jet)) {
      return;
    }
    fillMCPHistograms(jet, 1.0);
  }
  PROCESS_SWITCH(FullJetSpectrapp, processJetsMCP, "Full Jets at Particle Level", false);

  void processJetsMCPWeighted(typename JetTableMCPWeightedJoined::iterator const& jet, JetParticles const&)
  {
    if (!jetfindingutilities::isInEtaAcceptance(jet, jetEtaMin, jetEtaMax, trackEtaMin, trackEtaMax)) {
      return;
    }
    if (!isAcceptedJet<JetParticles>(jet)) {
      return;
    }
    fillMCPHistograms(jet, jet.eventWeight());
  }
  PROCESS_SWITCH(FullJetSpectrapp, processJetsMCPWeighted, "jet finder HF QA mcp on weighted events", false);

  void processTracks(soa::Filtered<EMCCollisions>::iterator const& collision, soa::Filtered<JetTracks> const& tracks, soa::Filtered<JetClusters> const& clusters)
  {
    registry.fill(HIST("h_collisions_unweighted"), 1.0);
    bool eventAccepted = false;
    // needed for the workaround to access EMCAL trigger bits. - This is needed for the MC productions in which the EMC trigger bits are missing. (MB MC LHC24f3, for ex.)
    // It first requires for atleast a cell in EMCAL to have energy content.
    // Once it finds a cell content,
    // it then checks if the collision is not an ambiguous collision (i.e. it has to be a unique collision = no bunch pile up)
    // If all of these conditions are satisfied then it checks for the required trigger bit in EMCAL.
    // For LHC22o, since the EMCAL didn't have hardware triggers, one would only require MB trigger (kTVXinEMC) in the EMCAL.

    if (doEMCALEventWorkaround) {
      if (collision.isEmcalReadout() && !collision.isAmbiguous()) { // i.e. EMCAL has a cell content
        eventAccepted = true;
        if (collision.alias_bit(kTVXinEMC)) {
          registry.fill(HIST("h_collisions_unweighted"), 2.0);
        }
      }
    } else {
      // Check if EMCAL was readout with the MB trigger(kTVXinEMC) fired. If not then reject the event and exit the function.
      // This is the default check for the simulations with proper trigger flags not requiring the above workaround.
      if (!collision.isAmbiguous() && jetderiveddatautilities::eventEMCAL(collision) && collision.alias_bit(kTVXinEMC)) {
        eventAccepted = true;
        registry.fill(HIST("h_collisions_unweighted"), 2.0);
      }
    }

    if (!eventAccepted) {
      registry.fill(HIST("h_collisions_unweighted"), 3.0);
      return;
    }
    // Fill Accepted events histos
    fillTrackHistograms(tracks, clusters, 1.0);
  }
  PROCESS_SWITCH(FullJetSpectrapp, processTracks, "QA for fulljet tracks", false);

  void processJetsMCPMCDMatched(soa::Filtered<EMCCollisions>::iterator const&, JetTableMCDMatchedJoined const& mcdjets, JetTableMCPMatchedJoined const&, JetTracks const&, JetClusters const&, JetParticles const&)
  {
    for (const auto& mcdjet : mcdjets) {
      if (!jetfindingutilities::isInEtaAcceptance(mcdjet, jetEtaMin, jetEtaMax, trackEtaMin, trackEtaMax)) {
        continue;
      }
      if (!isAcceptedJet<JetTracks>(mcdjet)) {
        continue;
      }
      fillMatchedHistograms<typename JetTableMCDMatchedJoined::iterator, JetTableMCPMatchedJoined>(mcdjet);
    }
  }
  PROCESS_SWITCH(FullJetSpectrapp, processJetsMCPMCDMatched, "full jet finder MCP matched to MCD", false);

  void processJetsMCPMCDMatchedWeighted(soa::Filtered<EMCCollisions>::iterator const&, JetTableMCDMatchedWeightedJoined const& mcdjets, JetTableMCPMatchedWeightedJoined const&, JetTracks const&, JetClusters const&, JetParticles const&)
  {
    for (const auto& mcdjet : mcdjets) {
      if (!jetfindingutilities::isInEtaAcceptance(mcdjet, jetEtaMin, jetEtaMax, trackEtaMin, trackEtaMax)) {
        continue;
      }
      if (!isAcceptedJet<JetTracks>(mcdjet)) {
        continue;
      }
      fillMatchedHistograms<typename JetTableMCDMatchedWeightedJoined::iterator, JetTableMCPMatchedWeightedJoined>(mcdjet, mcdjet.eventWeight());
    }
  }
  PROCESS_SWITCH(FullJetSpectrapp, processJetsMCPMCDMatchedWeighted, "full jet finder MCP matched to MCD on weighted events", false);

  void processTracksWeighted(soa::Filtered<soa::Join<EMCCollisions, aod::JMcCollisionLbs>>::iterator const& collision,
                             aod::JMcCollisions const&,
                             soa::Filtered<JetTracks> const& tracks,
                             soa::Filtered<JetClusters> const& clusters)
  {
    bool eventAccepted = false;
    float eventWeight = collision.mcCollision().weight();
    // registry.fill(HIST("h_collisions_unweighted"), 1.0);
    registry.fill(HIST("h_collisions_weighted"), 1.0, eventWeight);

    // set "doMBGapTrigger" to true only if you are testing with MB Gap-triggers
    if (doMBGapTrigger && eventWeight == 1) {
      return;
    }

    if (doEMCALEventWorkaround) {
      if (collision.isEmcalReadout() && !collision.isAmbiguous()) { // i.e. EMCAL has a cell content
        eventAccepted = true;
        fillTrackHistograms(tracks, clusters, eventWeight);
        if (collision.alias_bit(kTVXinEMC)) {
          registry.fill(HIST("h_collisions_weighted"), 2.0, eventWeight);
        }
      }
    } else {
      // Check if EMCAL was readout with the MB trigger(kTVXinEMC) fired. If not then reject the event and exit the function.
      // This is the default check for the simulations with proper trigger flags not requiring the above workaround.
      if (!collision.isAmbiguous() && jetderiveddatautilities::eventEMCAL(collision) && collision.alias_bit(kTVXinEMC)) {
        eventAccepted = true;
        registry.fill(HIST("h_collisions_weighted"), 2.0, eventWeight);
      }
    }

    if (!eventAccepted) {
      registry.fill(HIST("h_collisions_weighted"), 3.0, eventWeight);
      return;
    }
    // registry.fill(HIST("h_gaptrig_collisions"), 1.0, eventWeight);
    fillTrackHistograms(tracks, clusters, eventWeight);
  }
  PROCESS_SWITCH(FullJetSpectrapp, processTracksWeighted, "QA for fulljet tracks weighted", false);

}; // struct

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<FullJetSpectrapp>(cfgc, TaskName{"full-jet-spectra-pp"})};
}
