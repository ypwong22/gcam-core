#ifndef _DEGREE_DAYS_FEEDBACK_H_
#define _DEGREE_DAYS_FEEDBACK_H_
#if defined(_MSC_VER)
#pragma once
#endif


#include "containers/include/imodel_feedback_calc.h"

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

 protected:
  //! The name of this feedback
  std::string mName;

  //! A HDD feedback coefficient of sorts
  double mHDDCoef;

  //! A CDD feedback coefficient of sorts
  double mCDDCoef;

  //! The base year emissions value to calculate feedback from
  double mBaseYearValue;
};
