#include "DQM/RPCMonitorClient/interface/RPCChamberQuality.h"

#include "DQM/RPCMonitorClient/interface/RPCBookFolderStructure.h"
#include "DQM/RPCMonitorClient/interface/RPCRollMapHisto.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <DataFormats/MuonDetId/interface/RPCDetId.h>

#include <fmt/format.h>

const std::string RPCChamberQuality::xLabels_[7] = {
    "Good", "OFF", "Nois.St", "Nois.Ch", "Part.Dead", "Dead", "Bad.Shape"};
const std::string RPCChamberQuality::regions_[3] = {"EndcapNegative", "Barrel", "EndcapPositive"};

RPCChamberQuality::RPCChamberQuality(const edm::ParameterSet& ps) {
  edm::LogVerbatim("rpcchamberquality") << "[RPCChamberQuality]: Constructor";

  prescaleFactor_ = ps.getUntrackedParameter<int>("PrescaleFactor", 5);

  std::string subsystemFolder = ps.getUntrackedParameter<std::string>("RPCFolder", "RPC");
  std::string recHitTypeFolder = ps.getUntrackedParameter<std::string>("RecHitTypeFolder", "AllHits");
  std::string summaryFolder = ps.getUntrackedParameter<std::string>("SummaryFolder", "SummaryHistograms");

  summaryDir_ = subsystemFolder + "/" + recHitTypeFolder + "/" + summaryFolder;
  prefixDir_ = subsystemFolder + "/" + recHitTypeFolder;

  enableDQMClients_ = ps.getUntrackedParameter<bool>("EnableRPCDqmClient", true);

  minEvents = ps.getUntrackedParameter<int>("MinimumRPCEvents", 10000);
  numberOfDisks_ = ps.getUntrackedParameter<int>("NumberOfEndcapDisks", 4);
  useRollInfo_ = ps.getUntrackedParameter<bool>("UseRollInfo", false);
  offlineDQM_ = ps.getUntrackedParameter<bool>("OfflineDQM", true);

  init_ = false;
  lumiCounter_ = 0;
}

RPCChamberQuality::~RPCChamberQuality() { edm::LogVerbatim("rpcchamberquality") << "[RPCChamberQuality]: Destructor "; }

void RPCChamberQuality::beginJob() { edm::LogVerbatim("rpcchamberquality") << "[RPCChamberQuality]: Begin job "; }

void RPCChamberQuality::dqmEndLuminosityBlock(DQMStore::IBooker& ibooker,
                                              DQMStore::IGetter& igetter,
                                              edm::LuminosityBlock const& iLumi,
                                              edm::EventSetup const& iSetup) {
  edm::LogVerbatim("rpcchamberquality") << "[RPCChamberQuality]: End Lumi Block";

  if (enableDQMClients_ && !offlineDQM_) {
    edm::LogVerbatim("rpcchamberquality") << "[RPCChamberQuality]: Performing DQM client operation";

    if (lumiCounter_ == 0) {  //book histograms during at firt LB end
      this->myBooker(ibooker);
      lumiCounter_ = prescaleFactor_;
    }

    if (lumiCounter_ % prescaleFactor_ == 0) {  //Fill histograms every N=prescaleFactor_ LBs
      this->fillMonitorElements(igetter);
    }
  }
  lumiCounter_++;
}

void RPCChamberQuality::dqmEndJob(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter) {
  edm::LogVerbatim("rpcchamberquality") << "[RPCChamberQuality]: End Job";

  if (enableDQMClients_) {
    edm::LogVerbatim("rpcchamberquality") << "[RPCChamberQuality]: Performing DQM client operation";
    if (offlineDQM_ && !init_) {
      this->myBooker(ibooker);
    }
    if (init_) {
      this->fillMonitorElements(igetter);
    }
  }
}

void RPCChamberQuality::myBooker(DQMStore::IBooker& ibooker) {
  init_ = true;

  ibooker.setCurrentFolder(summaryDir_);

  for (int r = 0; r < 3; r++) {
    const std::string histoName = fmt::format("RPCChamberQuality_{}", regions_[r]);
    MonitorElement* me = ibooker.book1D(histoName, histoName, 7, 0.5, 7.5);

    for (int x = 1; x < 8; x++) {
      me->setBinLabel(x, xLabels_[x - 1]);
    }
  }


  MonitorElement* me = ibooker.book2D("RPC_System_Quality_Overview", "RPC_System_Quality_Overview", 7, 0.5, 7.5, 3, 0.5, 3.5);
  me->setBinLabel(1, "E-", 2);
  me->setBinLabel(2, "B", 2);
  me->setBinLabel(3, "E+", 2);

  for (int x = 1; x < 8; x++) {
    me->setBinLabel(x, xLabels_[x - 1]);
  }

  for (int w = -2; w < 3; w++) {  //Loop on wheels

    const std::string histoName2D = fmt::format("RPCChamberQuality_Roll_vs_Sector_Wheel{}", w);
    auto * me2D = RPCRollMapHisto::bookBarrel(ibooker, w, histoName2D, histoName2D, useRollInfo_);

    const std::string histoName1D = fmt::format("RPCChamberQuality_Distribution_Wheel{}", w);
    MonitorElement* me1D = ibooker.book1D(histoName1D, histoName1D, 7, 0.5, 7.5);

    for (int x = 1; x < 8; x++) {
      me1D->setBinLabel(x, xLabels_[x - 1]);
    }
  }  //end loop on wheels

  for (int d = -numberOfDisks_; d <= numberOfDisks_; d++) {  // Loop on disk
    if (d == 0)
      continue;
    const std::string histoName2D = fmt::format("RPCChamberQuality_Ring_vs_Segment_Disk{}", d);  //  2D histo for RPC Qtest
    auto me2D = RPCRollMapHisto::bookEndcap(ibooker, d, histoName2D, histoName2D, useRollInfo_);

    const std::string histoName1D = fmt::format("RPCChamberQuality_Distribution_Disk{}", d);
    MonitorElement* me1D = ibooker.book1D(histoName1D, histoName1D, 7, 0.5, 7.5);

    for (int x = 1; x < 8; x++) {
      me1D->setBinLabel(x, xLabels_[x - 1]);
    }
  }
}

void RPCChamberQuality::fillMonitorElements(DQMStore::IGetter& igetter) {
  int rpcEvents = minEvents;
  RpcEvents = igetter.get(prefixDir_+"/RPCEvents");

  if (RpcEvents)
    rpcEvents = (int)RpcEvents->getBinContent(1);

  if (rpcEvents >= minEvents) {
    init_ = true;

    MonitorElement* summary[3];

    for (int r = 0; r < 3; r++) {
      const std::string meName = fmt::format("{}/RPCChamberQuality_{}", summaryDir_, RPCChamberQuality::regions_[r]);
      summary[r] = igetter.get(meName);

      if (summary[r] != nullptr)
        summary[r]->Reset();
    }

    //Barrel
    for (int wheel = -2; wheel < 3; wheel++) {  // loop by Wheels
      const std::string meName = fmt::format("Roll_vs_Sector_Wheel{}", wheel);

      this->performeClientOperation(meName, 0, summary[1], igetter);
    }  // loop by Wheels

    // Endcap
    for (int i = -numberOfDisks_; i <= numberOfDisks_; i++) {  //loop on Disks
      if (i == 0)
        continue;

      const std::string meName = fmt::format("Ring_vs_Segment_Disk{}", i);

      if (i < 0)
        this->performeClientOperation(meName, -1, summary[0], igetter);
      else
        this->performeClientOperation(meName, 1, summary[2], igetter);
    }  //loop on Disks

    MonitorElement* RpcOverview = igetter.get(summaryDir_+"/RPC_System_Quality_Overview");
    RpcOverview->Reset();

    if (RpcOverview) {  //Fill Overview ME
      for (int r = 0; r < 3; r++) {
        if (summary[r] == nullptr)
          continue;
        const double entries = summary[r]->getEntries();
        if (entries == 0)
          continue;
        for (int x = 1; x <= 7; x++) {
          RpcOverview->setBinContent(x, r + 1, (summary[r]->getBinContent(x) / entries));
        }
      }
    }  //loop by LimiBloks
  }
}

void RPCChamberQuality::performeClientOperation(std::string MESufix,
                                                int region,
                                                MonitorElement* quality,
                                                DQMStore::IGetter& igetter) {
  MonitorElement* RCQ = igetter.get(summaryDir_+"/RPCChamberQuality_"+MESufix);
  //  if (RCQ)  RCQ->Reset();

  const int pos = MESufix.find_last_of('_');
  MonitorElement* RCQD = igetter.get(summaryDir_+"/RPCChamberQuality_Distribution"+MESufix.substr(pos));
  if (RCQD)
    RCQD->Reset();

  //get HV Histo
  MonitorElement* HV = igetter.get(summaryDir_+"/HVStatus_"+MESufix);
  //get LV Histo
  MonitorElement* LV = igetter.get(summaryDir_+"/LVStatus_"+MESufix);
  //Dead
  MonitorElement* DEAD = igetter.get(summaryDir_+"/DeadChannelFraction_"+MESufix);
  //ClusterSize
  MonitorElement* CLS = igetter.get(summaryDir_+"/ClusterSizeIn1Bin_"+MESufix);
  //NoisyStrips
  MonitorElement* NoisySt = igetter.get(summaryDir_+"/RPCNoisyStrips_"+MESufix);
  //Multiplicity
  MonitorElement* MULT = igetter.get(summaryDir_+"/NumberOfDigi_Mean_"+MESufix);
  //Asymetry
  MonitorElement* Chip = igetter.get(summaryDir_+"/AsymmetryLeftRight_"+MESufix);

  int xBinMax, yBinMax;

  if (region != 0)
    xBinMax = 37;
  else
    xBinMax = 13;

  for (int x = 1; x < xBinMax; x++) {
    if (region != 0) {
      yBinMax = 7;
    } else {
      if (x == 4)
        yBinMax = 22;
      else if (x == 9 || x == 11)
        yBinMax = 16;
      else
        yBinMax = 18;
    }
    for (int y = 1; y < yBinMax; y++) {
      int hv = 1;
      int lv = 1;
      float dead = 0;
      float firstbin = 0;
      float noisystrips = 0;
      float mult = 0;
      float asy = 0;
      chamberQualityState chamberState = GoodState;

      if (HV)
        hv = (int)HV->getBinContent(x, y);
      if (LV)
        lv = (int)LV->getBinContent(x, y);

      if (hv != 1 || lv != 1) {
        chamberState = OffState;
      } else {
        if (DEAD)
          dead = DEAD->getBinContent(x, y);
        if (dead >= 0.80) {
          chamberState = DeadState;
        } else if (0.33 <= dead && dead < 0.80) {
          chamberState = PartiallyDeadState;
        } else {
          if (CLS)
            firstbin = CLS->getBinContent(x, y);
          if (firstbin >= 0.88) {
            chamberState = NoisyStripState;
          } else {
            if (NoisySt)
              noisystrips = NoisySt->getBinContent(x, y);
            if (noisystrips > 0) {
              chamberState = NoisyStripState;
            } else {
              if (MULT)
                mult = MULT->getBinContent(x, y);
              if (mult >= 6) {
                chamberState = NoisyRollState;
              } else {
                if (Chip)
                  asy = Chip->getBinContent(x, y);
                if (asy > 0.35) {
                  chamberState = BadShapeState;
                } else {
                  chamberState = GoodState;
                }
              }
            }
          }
        }
      }
      if (RCQ)
        RCQ->setBinContent(x, y, chamberState);
      if (RCQD)
        RCQD->Fill(chamberState);
      if (quality)
        quality->Fill(chamberState);
    }
  }
  return;
}
