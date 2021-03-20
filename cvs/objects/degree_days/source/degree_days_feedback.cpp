#include "util/base/include/definitions.h"
#include <cassert>
#include <vector>
#include <cstdio>

#include "degree_days/include/degree_days_feedback.h"
#include "util/base/include/xml_helper.h"
#include "containers/include/scenario.h"
#include "util/base/include/model_time.h"

#include "util/base/include/gcam_fusion.hpp"
#include "util/base/include/gcam_data_containers.h"
#include "util/base/include/value.h"

using namespace std;
using namespace xercesc;

DegreeDaysFeedback::DegreeDaysFeedback()
  :mHDDCoef( 0 ),
   mCDDCoef( 0 ),
   mBaseYearValue( 0 )
{
}

DegreeDaysFeedback::~DegreeDaysFeedback() {
}

const string& DegreeDaysFeedback::getXMLNameStatic() {
  // This is the string you will use to refer to this object
  // in input files.
  const static string XML_NAME = "degree-day-feedback";
  return XML_NAME;
}

const string& DegreeDaysFeedback::getName() const {
  return mName;
}

bool DegreeDaysFeedback::XMLParse( const DOMNode* aNode ) {
  // Code to read the feedback object from XML inputs

  /*! \pre Make sure we were passed a valid node. */
  assert( aNode );

  // get the name attribute.
  mName = XMLHelper<string>::getAttr( aNode, XMLHelper<void>::name() );

  // get all child nodes.
  DOMNodeList* nodeList = aNode->getChildNodes();

  // loop through the child nodes.
  for( unsigned int i = 0; i < nodeList->getLength(); i++ ){
    DOMNode* curr = nodeList->item( i );
    string nodeName = XMLHelper<string>::safeTranscode( curr->getNodeName() );

    if( nodeName == XMLHelper<void>::text() ) {
      continue;
    }
    else if( nodeName == "hdd-coef" ) {
      mHDDCoef = XMLHelper<double>::getValue( curr );
    }
    else if( nodeName == "cdd-coef" ) {
      mCDDCoef = XMLHelper<double>::getValue( curr );
    }
    else {
      ILogger& mainLog = ILogger::getLogger( "main_log" );
      mainLog.setLevel( ILogger::ERROR );
      mainLog << "Unknown element " << nodeName << " encountered while parsing " << getXMLNameStatic() << endl;
    }
  }

  return true;
}

void DegreeDaysFeedback::toInputXML( ostream& aOut, Tabs* aTabs ) const {
  // Code to write the object's configuration as XML
  // (This is used when saving a configuration to be reread later)
  XMLWriteOpeningTag( getXMLNameStatic(), aOut, aTabs );

  XMLWriteElement( mHDDCoef, "hdd-coef", aOut, aTabs );
  XMLWriteElement( mCDDCoef, "cdd-coef", aOut, aTabs );

  XMLWriteClosingTag( getXMLNameStatic(), aOut, aTabs );
}

void DegreeDaysFeedback::calcFeedbacksBeforePeriod( Scenario* aScenario,
						    const IClimateModel* aClimateModel,
						    const int aPeriod )
{
  // code that gets called just before a period will begin to solve
  
  
  
}

void DegreeDaysFeedback::calcFeedbacksAfterPeriod( Scenario* aScenario,
						   const IClimateModel* aClimateModel,
						   const int aPeriod )
{

  const Modeltime* modeltime = aScenario->getModeltime();

  // code that gets called after a period is done solving,
  char buffer [200];
  sprintf(buffer, "%s%d%s", "world/region/sector/subsector/technology/period[YearFilter,IntLessThanEq,",
	  modeltime->getper_to_yr( aPeriod ), "]/ghg[NamedFilter,StringEquals,CO2]");
  vector<FilterStep*> emissFilterSteps = parseFilterString( buffer );

  // notice we can search by a year by using the YearFilter or by a GCAM model period by just using
  // an IndexFilter
  emissFilterSteps.push_back( new FilterStep( "emissions", new IndexFilter( new IntEquals( aPeriod ) ) ) );
  GatherEmiss gatherEmissProc;
  // note we are just using the default template flags here: just process data, not the steps
  GCAMFusion<GatherEmiss> gatherEmiss( gatherEmissProc, emissFilterSteps );
  // We must provide an object as the context to start the search, in this case we
  // start at the top with the Scenario object.
  gatherEmiss.startFilter( aScenario );
  // Results are not returned and instead the processData callback function of the
  // GatherEmiss class is called when a matching emissions value is found.

  // We can then retrieve the result to use it in our impact calculations
  double currGlobalEmiss = gatherEmissProc.mEmiss;
  cout << "Curr global emissions are " << currGlobalEmiss << " in period " << aPeriod << endl;


  if( aPeriod == modeltime->getFinalCalibrationPeriod() ) {
    // just store the base year value
    mBaseYearValue = currGlobalEmiss;
  }
  // scale heating and cooling degree days for the next period
  mCurrDDScaler = 1.0 / ( currGlobalEmiss / mBaseYearValue ) * mHDDCoef;


  // Note the actual services are "resid heating" or "comm cooling", etc so we
  // use regular expression partial matching so we do not have to spell it out.
  vector<FilterStep*> ddFilterSteps = parseFilterString( "world/region/consumer/nodeInput/nodeInput/nodeInput[NamedFilter,StringRegexMatches,heating]" );
  ddFilterSteps.push_back( new FilterStep( "degree-days", new IndexFilter( new IntEquals( aPeriod + 1 ) ) ) );
  GCAMFusion<DegreeDaysFeedback> scaleHDD( *this, ddFilterSteps );
  scaleHDD.startFilter( aScenario );

  mCurrDDScaler = ( currGlobalEmiss / mBaseYearValue ) * mCDDCoef;
  // only updating the service name filter of our query, we can keep the rest of it the same
  delete ddFilterSteps[ ddFilterSteps.size() - 2 ];
  ddFilterSteps[ ddFilterSteps.size() - 2 ] = new FilterStep( "nodeInput", new NamedFilter( new StringRegexMatches( "cooling" ) ) );
  GCAMFusion<DegreeDaysFeedback, true> scaleCDD( *this, ddFilterSteps );
  scaleCDD.startFilter( aScenario );

  // note we are still responsible for the memory we allocate even if it was done in the parseFilterString utility
  for( auto filterStep : ddFilterSteps ) {
    delete filterStep;
  }
  
}

template<typename T>
void DegreeDaysFeedback::GatherEmiss::processData( T& aData ) {
  assert( false );
}
template<>
void DegreeDaysFeedback::GatherEmiss::processData<Value>( Value& aData ) {
  mEmiss += aData;
}



template<typename T>
void DegreeDaysFeedback::processData( T& aData ) {
  assert( false );
}
template<>
void DegreeDaysFeedback::processData<Value>( Value& aData ) {
  // We are manipulating aData which is referenced back to the actual GCAM objects
  aData *= mCurrDDScaler;
}

template<class T>
void DegreeDaysFeedback::pushFilterStep( T* const& aContainer ) {
  // Do nothing
}

template<>
void DegreeDaysFeedback::pushFilterStep<INamed>( INamed* const& aContainer ) {
  std::cout << "Saw step " << aContainer->getName() << std::endl;
}

// Not working
//void DegreeDaysFeedback::pushFilterStep( IVisitable* const& aContainer ) {
//  std::cout << "Saw step " << aContainer->getName() << std::endl;
//}
//
//void DegreeDaysFeedback::pushFilterStep( IParsable* const& aContainer ) {
//  std::cout << "Saw step " << aContainer->getName() << std::endl;
//}
