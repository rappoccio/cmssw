#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/interface/TrackerShapeToBounds.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/DDCMS/interface/DDFilteredView.h"
#include "CondFormats/GeometryObjects/interface/PGeometricDet.h"

#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "DataFormats/Math/interface/GeantUnits.h"

#include <cfloat>
#include <string>

namespace {

  const std::string strue("true");

  template <typename DDView>
  double getDouble(const char* s, DDView const& ev) {
    DDValue val(s);
    std::vector<const DDsvalues_type*> result;
    ev.specificsV(result);
    std::vector<const DDsvalues_type*>::iterator it = result.begin();
    bool foundIt = false;
    for (; it != result.end(); ++it) {
      foundIt = DDfetch(*it, val);
      if (foundIt)
        break;
    }
    if (foundIt) {
      const std::vector<std::string>& temp = val.strings();
      if (temp.size() != 1) {
        throw cms::Exception("Configuration") << "I need 1 " << s << " tags";
      }
      return double(::atof(temp[0].c_str()));
    }
    return 0;
  }

  template <typename DDView>
  std::string getString(const char* s, DDView const& ev) {
    DDValue val(s);
    std::vector<const DDsvalues_type*> result;
    ev.specificsV(result);
    std::vector<const DDsvalues_type*>::iterator it = result.begin();
    bool foundIt = false;
    for (; it != result.end(); ++it) {
      foundIt = DDfetch(*it, val);
      if (foundIt)
        break;
    }
    if (foundIt) {
      const std::vector<std::string>& temp = val.strings();
      if (temp.size() != 1) {
        throw cms::Exception("Configuration") << "I need 1 " << s << " tags";
      }
      return temp[0];
    }
    return "NotFound";
  }
}  // namespace

/**
 * What to do in the destructor?
 * destroy all the daughters!
 */
GeometricDet::~GeometricDet() { deleteComponents(); }

/*
  Constructor from old DD Filtered view.
 */
GeometricDet::GeometricDet(DDFilteredView* fv, GeometricEnumType type)
    : _trans(fv->translation()),
      _phi(_trans.Phi()),
      _rho(_trans.Rho()),
      _rot(fv->rotation()),
      _shape(cms::dd::name_from_value(cms::LegacySolidShapeMap, fv->shape())),
      _ddname(fv->name()),
      _type(type),
      _params(fv->parameters()),
      _radLength(getDouble("TrackerRadLength", *fv)),
      _xi(getDouble("TrackerXi", *fv)),
      _pixROCRows(getDouble("PixelROCRows", *fv)),
      _pixROCCols(getDouble("PixelROCCols", *fv)),
      _pixROCx(getDouble("PixelROC_X", *fv)),
      _pixROCy(getDouble("PixelROC_Y", *fv)),
      _stereo(getString("TrackerStereoDetectors", *fv) == strue),
      _isLowerSensor(getString("TrackerLowerDetectors", *fv) == strue),
      _isUpperSensor(getString("TrackerUpperDetectors", *fv) == strue),
      _siliconAPVNum(getDouble("SiliconAPVNumber", *fv)),
      _isFromDD4hep(false)
{
  //  workaround instead of this at initialization _ddd(fv->navPos().begin(),fv->navPos().end()),
  const DDFilteredView::nav_type& nt = fv->navPos();
  _ddd = nav_type(nt.begin(), nt.end());
}

/*
  Constructor from DD4HEP Filtered view.
 */
GeometricDet::GeometricDet(cms::DDFilteredView* fv, GeometricEnumType type)
  :
  _trans(geant_units::operators::convertCmToMm(fv->translation())),
  _phi(_trans.Phi()),
  _rho(_trans.Rho()),
  _rot(fv->rotation()),
  _shape(fv->shape()),
  _ddd(fv->navPos()), // To be studied
  _ddname(dd4hep::dd::noNamespace(fv->name())),
  _type(type),
  _params(computeLegacyShapeParameters(_shape, fv->solid())),
  _radLength(fv->get<double>("TrackerRadLength")), // To be studied
  _xi(fv->get<double>("TrackerXi")), // To be studied
  _pixROCRows(fv->get<double>("PixelROCRows")),
  _pixROCCols(fv->get<double>("PixelROCCols")),
  _pixROCx(fv->get<double>("PixelROC_X")),
  _pixROCy(fv->get<double>("PixelROC_Y")),
  _stereo(fv->get<std::string_view>("TrackerStereoDetectors") == strue),
  _isLowerSensor(fv->get<std::string_view>("TrackerLowerDetectors") == strue),
  _isUpperSensor(fv->get<std::string_view>("TrackerUpperDetectors") == strue),
  _siliconAPVNum(fv->get<double>("SiliconAPVNumber")),
  _isFromDD4hep(true)
{}

// PGeometricDet is persistent version... make it... then come back here and make the
// constructor.
GeometricDet::GeometricDet(const PGeometricDet::Item& onePGD, GeometricEnumType type)
    : _trans(onePGD._x, onePGD._y, onePGD._z),
      _phi(onePGD._phi),  //_trans.Phi()),
      _rho(onePGD._rho),  //_trans.Rho()),
      _rot(onePGD._a11,
           onePGD._a12,
           onePGD._a13,
           onePGD._a21,
           onePGD._a22,
           onePGD._a23,
           onePGD._a31,
           onePGD._a32,
           onePGD._a33),
      _shape(cms::dd::name_from_value(cms::LegacySolidShapeMap, static_cast<LegacySolidShape>(onePGD._shape))),
      _ddd(),
      _ddname(onePGD._name),
      _type(type),
      _params(),
      _geographicalID(onePGD._geographicalID),
      _radLength(onePGD._radLength),
      _xi(onePGD._xi),
      _pixROCRows(onePGD._pixROCRows),
      _pixROCCols(onePGD._pixROCCols),
      _pixROCx(onePGD._pixROCx),
      _pixROCy(onePGD._pixROCy),
      _stereo(onePGD._stereo),
      _siliconAPVNum(onePGD._siliconAPVNum) {
  if (onePGD._shape == 1 || onePGD._shape == 3) {  //The parms vector is neede only in the case of box or trap shape
    _params.reserve(11);
    _params.emplace_back(onePGD._params0);
    _params.emplace_back(onePGD._params1);
    _params.emplace_back(onePGD._params2);
    _params.emplace_back(onePGD._params3);
    _params.emplace_back(onePGD._params4);
    _params.emplace_back(onePGD._params5);
    _params.emplace_back(onePGD._params6);
    _params.emplace_back(onePGD._params7);
    _params.emplace_back(onePGD._params8);
    _params.emplace_back(onePGD._params9);
    _params.emplace_back(onePGD._params10);
  }

  _ddd.reserve(onePGD._numnt);
  _ddd.emplace_back(onePGD._nt0);
  _ddd.emplace_back(onePGD._nt1);
  _ddd.emplace_back(onePGD._nt2);
  _ddd.emplace_back(onePGD._nt3);
  if (onePGD._numnt > 4) {
    _ddd.emplace_back(onePGD._nt4);
    if (onePGD._numnt > 5) {
      _ddd.emplace_back(onePGD._nt5);
      if (onePGD._numnt > 6) {
        _ddd.emplace_back(onePGD._nt6);
        if (onePGD._numnt > 7) {
          _ddd.emplace_back(onePGD._nt7);
          if (onePGD._numnt > 8) {
            _ddd.emplace_back(onePGD._nt8);
            if (onePGD._numnt > 9) {
              _ddd.emplace_back(onePGD._nt9);
              if (onePGD._numnt > 10) {
                _ddd.emplace_back(onePGD._nt10);
              }
            }
          }
        }
      }
    }
  }
}

GeometricDet::ConstGeometricDetContainer GeometricDet::deepComponents() const {
  //
  // iterate on all the components ;)
  //
  ConstGeometricDetContainer _temp;
  deepComponents(_temp);
  return _temp;
}

void GeometricDet::deepComponents(ConstGeometricDetContainer& cont) const {
  if (isLeaf())
    cont.emplace_back(this);
  else
    std::for_each(_container.begin(), _container.end(), [&](const GeometricDet* iDet) { iDet->deepComponents(cont); });
}

void GeometricDet::addComponents(GeometricDetContainer const& cont) {
  _container.reserve(_container.size() + cont.size());
  std::copy(cont.begin(), cont.end(), back_inserter(_container));
}

void GeometricDet::addComponents(ConstGeometricDetContainer const& cont) {
  _container.reserve(_container.size() + cont.size());
  std::copy(cont.begin(), cont.end(), back_inserter(_container));
}

void GeometricDet::addComponent(GeometricDet* det) { _container.emplace_back(det); }

namespace {
  struct Deleter {
    void operator()(GeometricDet const* det) const { delete const_cast<GeometricDet*>(det); }
  };
}  // namespace

void GeometricDet::deleteComponents() {
  std::for_each(_container.begin(), _container.end(), Deleter());
  _container.clear();
}

GeometricDet::Position GeometricDet::positionBounds() const {
  Position _pos(float(_trans.x() / cm), float(_trans.y() / cm), float(_trans.z() / cm));
  return _pos;
}

GeometricDet::Rotation GeometricDet::rotationBounds() const {
  Translation x, y, z;
  _rot.GetComponents(x, y, z);
  Rotation _rotation(float(x.X()),
                     float(x.Y()),
                     float(x.Z()),
                     float(y.X()),
                     float(y.Y()),
                     float(y.Z()),
                     float(z.X()),
                     float(z.Y()),
                     float(z.Z()));
  return _rotation;
}

std::unique_ptr<Bounds> GeometricDet::bounds() const {
  const std::vector<double>& par = _params;
  TrackerShapeToBounds shapeToBounds;
  return std::unique_ptr<Bounds>(shapeToBounds.buildBounds(_shape, par));
}

/*

std::string GeometricDet::computeNameWithNoNamespace(std::string_view nameFromView) const {
  const auto& semiColonPos = nameFromView.find(":");
  const std::string name{(semiColonPos != std::string::npos ? nameFromView.substr(semiColonPos + 1) : nameFromView)};
  return name;
  }*/


/*
 * DD4hep.
 * Keep order and units of parameters as old DD to avoid numerous rgeressions.
 * Shape parameters to be stored in DB and of interest are only for boxes, trapezoids, and tubs.
 * params() will complain if parameters of any other shape are accessed.
 */
std::vector<double> GeometricDet::computeLegacyShapeParameters(const cms::DDSolidShape& mySolidShape,
							       const dd4hep::Solid& mySolid) const {
  std::vector<double> myOldDDShapeParameters;
  
  // Box
  if (mySolidShape == cms::DDSolidShape::ddbox) {
    const dd4hep::Box& myBox = dd4hep::Box(mySolid);
    myOldDDShapeParameters = { geant_units::operators::convertCmToMm(myBox.x() ),
			       geant_units::operators::convertCmToMm(myBox.y() ),
			       geant_units::operators::convertCmToMm(myBox.z() )
    };
  }
  
  // Trapezoid
  else if (mySolidShape == cms::DDSolidShape::ddtrap) {
    const dd4hep::Trap& myTrap = dd4hep::Trap(mySolid);
    myOldDDShapeParameters = {
      geant_units::operators::convertCmToMm(myTrap->GetDZ() ),
      static_cast<double>(angle_units::operators::convertDegToRad(myTrap->GetTheta())),
      static_cast<double>(angle_units::operators::convertDegToRad(myTrap->GetPhi())),
      geant_units::operators::convertCmToMm(myTrap->GetH1() ),
      geant_units::operators::convertCmToMm(myTrap->GetBl1() ),
      geant_units::operators::convertCmToMm(myTrap->GetTl1() ),
      static_cast<double>(angle_units::operators::convertDegToRad(myTrap->GetAlpha1())),
      geant_units::operators::convertCmToMm(myTrap->GetH2() ),
      geant_units::operators::convertCmToMm(myTrap->GetBl2() ),
      geant_units::operators::convertCmToMm(myTrap->GetTl2() ),		 
      static_cast<double>(angle_units::operators::convertDegToRad(myTrap->GetAlpha2()))
    }; 
  }

  // Tub
  else if (mySolidShape == cms::DDSolidShape::ddtubs) {
    const dd4hep::Tube& myTube = dd4hep::Tube(mySolid);
    myOldDDShapeParameters = { geant_units::operators::convertCmToMm(myTube->GetDz() ),
			       geant_units::operators::convertCmToMm(myTube->GetRmin() ),
			       geant_units::operators::convertCmToMm(myTube->GetRmax() ),
			       static_cast<double>(angle_units::operators::convertDegToRad(myTube->GetPhi1())),
			       static_cast<double>(angle_units::operators::convertDegToRad(myTube->GetPhi2() - myTube->GetPhi1()))
    };
  }

  return myOldDDShapeParameters;
}
