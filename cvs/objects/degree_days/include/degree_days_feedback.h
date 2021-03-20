#ifndef _DEGREE_DAYS_FEEDBACK_H_
#define _DEGREE_DAYS_FEEDBACK_H_
#if defined(_MSC_VER)
#pragma once
#endif


#include "containers/include/imodel_feedback_calc.h"
#include "util/base/include/xml_helper.h"
#include "util/base/include/value.h"
#include "util/base/include/gcam_fusion.hpp"


/*!
 * \ingroup Objects
 * \brief Calc feed back to heating and cooling degree days.
 * \details Test implementation.
 *
 * \author Pralit Patel
 */
class DegreeDaysFeedback : public IModelFeedbackCalc
{
 public:
  DegreeDaysFeedback();
  virtual ~DegreeDaysFeedback();

  static const std::string& getXMLNameStatic();

  // INamed methods
  virtual const std::string& getName() const;

  // IParsable methods
  virtual bool XMLParse( const xercesc::DOMNode* aNode );

  // IRoundTrippable methods
  virtual void toInputXML( std::ostream& aOut, Tabs* aTabs ) const;

  // IModelFeedbackCalc methods
  virtual void calcFeedbacksBeforePeriod( Scenario* aScenario, const
					  IClimateModel* aClimateModel, const int aPeriod );

  virtual void calcFeedbacksAfterPeriod( Scenario* aScenario, const
					 IClimateModel* aClimateModel, const int aPeriod );

  //
  struct GatherEmiss {
    // a variable to keep the sum
    double mEmiss = 0;

    // call back methods for GCAMFusion
    // called if the fourth template argument to GCAMFusion is true
    template<typename T>
    void processData( T& aData );

    // call back methods for GCAMFusion
    // called if the second templated argument to GCAMFusion is true
    // we won't be using it in this example.
    //template<typename T>
    //void pushFilterStep( const DataType& aData );

    // call back methods for GCAMFusion
    // called if the third templated argument to GCAMFusion is true
    // we won't be using it in this example.
    //template<typename T>
    //void popFilterStep( const DataType& aData );
  };

  //
  template<typename T>
  void processData( T& aData );

  template<class T>
  void pushFilterStep( T* const& aContainer );

  //Not working
  //void pushFilterStep( IVisitable* const& aContainer );
  //void pushFilterStep( IParsable* const& aContainer );
  
 protected:
  //! The name of this feedback
  std::string mName;

  //! A HDD feedback coefficient of sorts
  double mHDDCoef;

  //! A CDD feedback coefficient of sorts
  double mCDDCoef;

  //! The base year emissions value to calculate feedback from
  double mBaseYearValue;

  //! The scalar to modify coef values
  double mCurrDDScaler;
};

//
template<>
void DegreeDaysFeedback::GatherEmiss::processData<Value>( Value& aData );


//
template<>
void DegreeDaysFeedback::processData<Value>( Value& aData );

//
template<>
void DegreeDaysFeedback::pushFilterStep<INamed>( INamed* const& aContainer );

#endif
