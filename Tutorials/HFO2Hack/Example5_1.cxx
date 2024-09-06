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

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Common/DataModel/Multiplicity.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

constexpr int each = 10;

struct ExampleFive {
  HistogramRegistry registry{
    "registry",
    {
      {"hpt", " ; p_{T} (GeV/c)", {HistType::kTH1F, {{101, -0.05, 10.05}}}},                             //
      {"hptMC", " ; p_{T} (GeV/c)", {HistType::kTH1F, {{101, -0.05, 10.05}}}},                           //
      {"havpt", "; <p_{T}> (GeV/c) ; V0M", {HistType::kTH2F, {{21, -0.05, 2.05}, {201, -0.5, 200.5}}}},  //
      {"havptMC", "; <p_{T}> (GeV/c) ; V0M", {HistType::kTH2F, {{21, -0.05, 2.05}, {201, -0.5, 200.5}}}} //
    }                                                                                                    //
  };

  Configurable<float> etaCut{"etaCut", 0.8, "track eta cut"};
  Filter etaFilter = nabs(aod::track::eta) <= etaCut;
  Filter trackFilter = aod::track::trackType != (uint8_t)aod::track::TrackTypeEnum::Run2Tracklet;

  using MyTracks = soa::Filtered<aod::Tracks>;

  void process(soa::Join<aod::Collisions, aod::Mults>::iterator const& collision, MyTracks const& tracks)
  {
    auto avpt = 0.f;
    for (auto& track : tracks) {
      avpt += track.pt();
      registry.fill(HIST("hpt"), track.pt());
    }
    if (tracks.size() > 0) {
      avpt /= (float)tracks.size();
    }
    registry.fill(HIST("havpt"), avpt, collision.multFV0M());
    if (collision.index() % each == 0) {
      LOGP(info, "Collision {} has {} tracks, average pt = {}", collision.index(), tracks.size(), avpt);
    }
  }

  void processMC(aod::McCollision const& mccollision, aod::McParticles const& particles)
  {
    auto avpt = 0.f;
    auto count = 0;
    for (auto& particle : particles) {
      if (particle.isPhysicalPrimary()) {
        if (std::abs(particle.eta()) < etaCut) {
          count++;
          if (!isnan(particle.pt())) {
            avpt += particle.pt();
          }
          registry.fill(HIST("hptMC"), particle.pt());
        }
      }
    }
    if (count > 0) {
      avpt /= (float)count;
    }
    auto pcount = 0;
    for (auto& particle : particles) {
      if (particle.isPhysicalPrimary()) {
        if ((particle.eta() > 2.7f && particle.eta() < 5.1f) || (particle.eta() > -3.7f && particle.eta() < -1.7f)) {
          pcount++;
        }
      }
    }
    registry.fill(HIST("havptMC"), avpt, pcount);
    if (mccollision.index() % each == 0) {
      LOGP(info, "MC Collision {} has {} primary particles, average pt = {}", mccollision.index(), count, avpt);
    }
  }

  PROCESS_SWITCH(ExampleFive, processMC, "Process MC info", false);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{adaptAnalysisTask<ExampleFive>(cfgc)};
}
