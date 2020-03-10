/* \class PFClusterJetSelector
 *
 * Selects track with a configurable string-based cut.
 * Saves clones of the selected tracks 
 *
 * \author: Lauren Hay
 *
 * usage:
 *
 * module bestPFClusterJets = PFClusterJetSelector {
 *   src = ktJets
 *   string cut = "pt > 20 & abs( eta ) < 2"
 * }
 *
 * for more details about the cut syntax, see the documentation
 * page below:
 *
 *   https://twiki.cern.ch/twiki/bin/view/CMS/SWGuidePhysicsCutParser
 *
 *
 */

#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "DataFormats/JetReco/interface/PFClusterJet.h"
#include "DataFormats/JetReco/interface/PFClusterJetCollection.h"

typedef SingleObjectSelector<reco::PFClusterJetCollection, StringCutObjectSelector<reco::PFClusterJet> > PFClusterJetSelector;

DEFINE_FWK_MODULE(PFClusterJetSelector);
