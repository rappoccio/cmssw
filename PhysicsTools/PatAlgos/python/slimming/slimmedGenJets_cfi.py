import FWCore.ParameterSet.Config as cms

slimmedGenJets = cms.EDProducer("PATJetSlimmer",
   src = cms.InputTag("selectedPatGenJets"),
   packedGenParticles = cms.InputTag("packedGenParticles"),
   inputIsGenJets = cms.bool(True),
   dropJetVars = cms.string("1"),
   dropDaughters = cms.string("0"),
   rekeyDaughters = cms.string("1"),
   dropTrackRefs = cms.string("1"),
   dropSpecific = cms.string("0"),
   dropTagInfos = cms.string("1"),
   modifyJets = cms.bool(True),
   mixedDaughters = cms.bool(False),
   modifierConfig = cms.PSet( modifications = cms.VPSet() )
)
slimmedGenJetsAK8 = cms.EDProducer("PATJetSlimmer",
   src = cms.InputTag("packedPatGenJetsAK8"),
   packedGenParticles = cms.InputTag("packedGenParticles"),
   inputIsGenJets = cms.bool(True),
   dropJetVars = cms.string("1"),
   dropDaughters = cms.string("0"),
   rekeyDaughters = cms.string("1"),
   dropTrackRefs = cms.string("1"),
   dropSpecific = cms.string("0"),
   dropTagInfos = cms.string("1"),
   modifyJets = cms.bool(True),
   mixedDaughters = cms.bool(False),
   modifierConfig = cms.PSet( modifications = cms.VPSet() )
)

