/* \class EtMinPFClusterJetSelector
 *
 * selects pf-jet above a minumum Et cut
 *
 * \author: Lauren Hay
 *
 */
#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/EtMinSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "DataFormats/JetReco/interface/PFClusterJetCollection.h"

typedef SingleObjectSelector<reco::PFClusterJetCollection, EtMinSelector> EtMinPFClusterJetSelector;

DEFINE_FWK_MODULE(EtMinPFClusterJetSelector);
