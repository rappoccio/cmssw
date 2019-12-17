#ifndef ParticleFlowCandidate_PFCandidateBase_h
#define ParticleFlowCandidate_PFCandidateBase_h
/** \class reco::PFCandidateBase
 *
 * particle candidate from particle flow
 *
 */

#if !defined(__CINT__) && !defined(__MAKECINT__) && !defined(__REFLEX__)
#include <atomic>
#endif
#include <iosfwd>
#include <array>

#include "DataFormats/Math/interface/Point3D.h"

#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateElectronExtraFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFDisplacedVertexFwd.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidatePhotonExtraFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateEGammaExtraFwd.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
namespace reco {
  /**\class PFCandidateBase
     \brief Base class for ParticleFlow so we can implement PUPPI as a PFCandidate + weight
          
     \author Salvatore Rappoccio, Lauren Hay
     \date   December 2019
  */
  class PFCandidateBase : public CompositeCandidate {
  public:
    /// particle types
    enum ParticleType {
      X = 0,     // undefined
      h,         // charged hadron
      e,         // electron
      mu,        // muon
      gamma,     // photon
      h0,        // neutral hadron
      h_HF,      // HF tower identified as a hadron
      egamma_HF  // HF tower identified as an EM particle
    };

    enum Flags {
      NORMAL = 0,
      E_PHI_SMODULES,
      E_ETA_0,
      E_ETA_MODULES,
      E_BARREL_ENDCAP,
      E_PRESHOWER_EDGE,
      E_PRESHOWER,
      E_ENDCAP_EDGE,
      H_ETA_0,
      H_BARREL_ENDCAP,
      H_ENDCAP_VFCAL,
      H_VFCAL_EDGE,
      T_TO_DISP,
      T_FROM_DISP,
      T_FROM_V0,
      T_FROM_GAMMACONV,
      GAMMA_TO_GAMMACONV
    };

    enum PFVertexType {
      kCandVertex = 0,
      kTrkVertex = 1,
      kComMuonVertex = 2,
      kSAMuonVertex = 3,
      kTrkMuonVertex = 4,
      kGSFVertex = 5,
      kTPFMSMuonVertex = 6,
      kPickyMuonVertex = 7,
      kDYTMuonVertex = 8
    };


    /// default constructor
    PFCandidateBase(){}

    /// copy constructor
    PFCandidateBase( const PFCandidateBase & iOther ): CompositeCandidate(iOther) {}

    /*     PFCandidateBase( Charge q,  */
    /*                  const LorentzVector & p4,  */
    /*                  ParticleType particleId,  */
    /*                  reco::PFBlockRef blockRef ); */
    PFCandidateBase(Charge q, const LorentzVector& p4, ParticleType particleId) : CompositeCandidate(q,p4) {}


    /// destructor
    ~PFCandidateBase() override {};

    virtual PFCandidateBase& operator=(PFCandidateBase const&iOther) {
      CompositeCandidate::operator=(iOther);
      return *this;
    }

    /// return a clone
    virtual PFCandidateBase* clone() const override = 0;

    /*    /// set source ref */
    /*     void setSourceRef(const PFCandidateBaseRef& ref) { sourceRef_ = ref; } */

    /*     size_type numberOfSourceCandidateRefs() const {return 1;} */

    /*     CandidateBaseRef sourceCandidateRef( size_type i ) const { */
    /*       return  CandidateBaseRef(sourceRef_); */
    /*     } */

    CandidatePtr sourceCandidatePtr(size_type i) const override = 0;

    /// returns the pdg id corresponding to the particle type.
    /// the particle type could be removed at some point to gain some space.
    /// low priority
    virtual int translateTypeToPdgId(ParticleType type) const = 0;
    virtual ParticleType translatePdgIdToType(int pdgid) const = 0;

    /// set Particle Type
    virtual void setParticleType(ParticleType type) = 0;

    /// add an element to the current PFCandidateBase
    /*     void addElement( const reco::PFBlockElement* element ); */

    /// add element in block
    virtual void addElementInBlock(const reco::PFBlockRef& blockref, unsigned elementIndex) = 0;

    /// set track reference
    virtual void setTrackRef(const reco::TrackRef& ref) = 0;

    /// return a reference to the corresponding track, if charged.
    /// otherwise, return a null reference
    virtual reco::TrackRef trackRef() const = 0;

    /// return a pointer to the best track, if available.
    /// otherwise, return a null pointer
    const reco::Track* bestTrack() const override = 0;

    /// uncertainty on dz
    float dzError() const override = 0;

    /// uncertainty on dxy
    float dxyError() const override = 0;

    /// set gsftrack reference
    virtual void setGsfTrackRef(const reco::GsfTrackRef& ref) = 0;

    /// return a reference to the corresponding GSF track, if an electron.
    /// otherwise, return a null reference
    virtual reco::GsfTrackRef gsfTrackRef() const = 0;

    /// set muon reference
    virtual void setMuonRef(const reco::MuonRef& ref) = 0;

    /// return a reference to the corresponding muon, if a muon.
    /// otherwise, return a null reference
    virtual reco::MuonRef muonRef() const = 0;

    /// set displaced vertex reference
    virtual void setDisplacedVertexRef(const reco::PFDisplacedVertexRef& ref, Flags flag) = 0;

    /// return a reference to the corresponding displaced vertex,
    /// otherwise, return a null reference
    virtual reco::PFDisplacedVertexRef displacedVertexRef(Flags type) const = 0;

    /// set ref to original reco conversion
    virtual void setConversionRef(const reco::ConversionRef& ref) = 0;

    /// return a reference to the original conversion
    virtual reco::ConversionRef conversionRef() const = 0;

    /// set ref to original reco conversion
    virtual void setV0Ref(const reco::VertexCompositeCandidateRef& ref) = 0;

    /// return a reference to the original conversion
    virtual reco::VertexCompositeCandidateRef v0Ref() const = 0;

    /// return a reference to the corresponding GsfElectron if any
    virtual reco::GsfElectronRef gsfElectronRef() const = 0;

    /// return a reference to the electron extra
    virtual reco::PFCandidateElectronExtraRef electronExtraRef() const = 0;

    /// set corrected Ecal energy
    virtual void setEcalEnergy(float eeRaw, float eeCorr) = 0;

    /// return corrected Ecal energy
    virtual double ecalEnergy() const = 0;

    /// return corrected Ecal energy
    virtual double rawEcalEnergy() const = 0;

    /// set corrected Hcal energy
    virtual void setHcalEnergy(float ehRaw, float ehCorr) = 0;

    /// return corrected Hcal energy
    virtual double hcalEnergy() const = 0;

    /// return raw Hcal energy
    virtual double rawHcalEnergy() const = 0;

    /// set corrected Hcal energy
    virtual void setHoEnergy(float eoRaw, float eoCorr) = 0;

    /// return corrected Hcal energy
    virtual double hoEnergy() const = 0;

    /// return raw Hcal energy
    virtual double rawHoEnergy() const = 0;

    /// set GsfElectronRef
    virtual void setGsfElectronRef(const reco::GsfElectronRef& ref) = 0;

    virtual void setSuperClusterRef(const reco::SuperClusterRef& scRef) = 0;

    /// return a reference to the corresponding SuperCluster if any
    virtual reco::SuperClusterRef superClusterRef() const = 0;

    /// set ref to the corresponding reco::Photon if any
    virtual void setPhotonRef(const reco::PhotonRef& phRef) = 0;

    /// return a reference to the corresponding Photon if any
    virtual reco::PhotonRef photonRef() const = 0;

    /// set the PF Photon Extra Ref
    virtual void setPFPhotonExtraRef(const reco::PFCandidatePhotonExtraRef& ref) = 0;

    /// return a reference to the photon extra
    virtual reco::PFCandidatePhotonExtraRef photonExtraRef() const = 0;

    /// set the PF EGamma Extra Ref
    virtual void setPFEGammaExtraRef(const reco::PFCandidateEGammaExtraRef& ref) = 0;

    /// return a reference to the EGamma extra
    virtual reco::PFCandidateEGammaExtraRef egammaExtraRef() const = 0;

    /// set corrected PS1 energy
    virtual void setPs1Energy(float e1) = 0;

    /// return corrected PS1 energy
    virtual double pS1Energy() const = 0;

    /// set corrected PS2 energy
    virtual void setPs2Energy(float e2) = 0;

    /// return corrected PS2 energy
    virtual double pS2Energy() const = 0;

    /// particle momentum *= rescaleFactor
    virtual void rescaleMomentum(double rescaleFactor) = 0;

    /// set a given flag
    virtual void setFlag(Flags theFlag, bool value) = 0;

    /// return a given flag
    virtual bool flag(Flags theFlag) const = 0;

    /// set uncertainty on momentum
    virtual void setDeltaP(double dp) = 0;

    /// uncertainty on 3-momentum
    virtual double deltaP() const = 0;

    //  int pdgId() const { return translateTypeToPdgId( particleId_ ); }

    /// set mva for electron-pion discrimination.
    /// For charged particles, this variable is set
    ///   to 0 for particles that are not preided
    ///   to 1 otherwise
    /// For neutral particles, it is set to the default value

    virtual void set_mva_Isolated(float mvaI) = 0;
    // mva for isolated electrons
    virtual float mva_Isolated() const = 0;

    virtual void set_mva_e_pi(float mvaNI) = 0;
    /// mva for electron-pion discrimination
    virtual float mva_e_pi() const = 0;

    /// set mva for electron-muon discrimination
    virtual void set_mva_e_mu(float mva) = 0;

    /// mva for electron-muon discrimination
    virtual float mva_e_mu() const = 0;

    /// set mva for pi-muon discrimination
    virtual void set_mva_pi_mu(float mva) = 0;

    /// mva for pi-muon discrimination
    virtual float mva_pi_mu() const = 0;

    /// set mva for gamma detection
    virtual void set_mva_nothing_gamma(float mva) = 0;

    /// mva for gamma detection
    virtual float mva_nothing_gamma() const = 0;

    /// set mva for neutral hadron detection
    virtual void set_mva_nothing_nh(float mva) = 0;

    /// mva for neutral hadron detection
    virtual float mva_nothing_nh() const = 0;

    /// set mva for neutral hadron - gamma discrimination
    virtual void set_mva_gamma_nh(float mva) = 0;

    /// mva for neutral hadron - gamma discrimination
    virtual float mva_gamma_nh() const = 0;

    /// set position at ECAL entrance
    virtual void setPositionAtECALEntrance(const math::XYZPointF& pos) = 0;

    /// set the PF Electron Extra Ref
    virtual void setPFElectronExtraRef(const reco::PFCandidateElectronExtraRef& ref)= 0;

    /// set the Best Muon Track Ref
    virtual void setMuonTrackType(const reco::Muon::MuonTrackType& type) = 0;

    /// get the Best Muon Track Ref

    virtual const reco::Muon::MuonTrackType bestMuonTrackType() const= 0;

    /// \return position at ECAL entrance
    virtual const math::XYZPointF& positionAtECALEntrance() const = 0;

    /// particle identification code
    /// \todo use Particle::pdgId_ and remove this data member
    virtual ParticleType particleId() const = 0;

    virtual void setVertexSource(PFVertexType vt)= 0;

    virtual void setVertex(const math::XYZPoint& p) override = 0;

    const Point& vertex() const override = 0;
    double vx() const override = 0;
    double vy() const override = 0;
    double vz() const override = 0;

    /// do we have a valid time information
    virtual bool isTimeValid() const= 0;
    /// \return the timing
    virtual float time() const = 0;
    /// \return the timing uncertainty
    virtual float timeError() const = 0;
    /// \set the timing information
    virtual void setTime(float time, float timeError = 0.f) = 0;
    /// fraction of hcal energy at a given depth  (depth = 1 .. 7)
    virtual float hcalDepthEnergyFraction(unsigned int depth) const = 0;
    /// fraction of hcal energy at a given depth (index 0..6 for depth 1..7)
    virtual const std::array<float, 7>& hcalDepthEnergyFractions() const = 0; 
    /// set the fraction of hcal energy as function of depth (index 0..6 for depth 1..7)
    virtual void setHcalDepthEnergyFractions(const std::array<float, 7>& fracs) = 0;

  };

}  // namespace reco

#endif
