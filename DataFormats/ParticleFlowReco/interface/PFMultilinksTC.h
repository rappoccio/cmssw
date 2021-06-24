#ifndef __PFMultilinksTC__
#define __PFMultilinksTC__

// Done by Glowinski & Gouzevitch

#include <vector>
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefVector.h"

namespace reco {

  /// \brief Abstract This class is used by the KDTree Track / Ecal Cluster
  /// linker to store all found links.
  ///
  struct PFMultilink {
    PFMultilink ( const reco::PFRecTrackRef & trackref, const reco::PFClusterRef & clusterref ) : 
       trackRef(trackref), clusterRef(clusterref), clusterRef2() {}
    PFMultilink ( const reco::PFClusterRef & clusterref1, const reco::PFClusterRef & clusterref2 ) : 
       trackRef(), clusterRef(clusterref1), clusterRef2(clusterref2) {}

    // Data are to link:
    // Track to ECAL (trackRef --> clusterRef)
    // Track to HCAL (trackRef --> clusterRef)
    // PS to ECAL (clusterRef --> clusterRef2)
    reco::PFRecTrackRef trackRef;
    reco::PFClusterRef clusterRef;
    reco::PFClusterRef clusterRef2;
  };
  /// collection of PFSuperCluster objects
  typedef edm::RefVector<std::vector<PFMultilink>> PFMultilinksType;
  class PFMultiLinksTC {
  public:
    bool isValid;
    PFMultilinksType linkedClusters;

  public:
    PFMultiLinksTC(bool isvalid = false) : isValid(isvalid) {}
  };
}  // namespace reco

#endif
