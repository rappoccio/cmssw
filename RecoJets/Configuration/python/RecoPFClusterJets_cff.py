import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.ak4PFClusterJets_cfi import ak4PFClusterJets
from RecoJets.JetProducers.PFClustersForJets_cff import *
from RecoJets.JetProducers.fixedGridRhoProducerFastjet_cfi import fixedGridRhoFastjetAllPFCluster

recoPFClusterJetsTask   =cms.Task(pfClusterRefsForJetsHCAL,
                                  pfClusterRefsForJetsECAL,
                                  pfClusterRefsForJetsHF,
                                  pfClusterRefsForJetsHO,
                                  pfClusterRefsForJets,
                                  ak4PFClusterJets,
                                  fixedGridRhoFastjetAllPFCluster)
recoPFClusterJets   =cms.Sequence(recoPFClusterJetsTask)
recoAllPFClusterJets=cms.Sequence(recoPFClusterJetsTask)
