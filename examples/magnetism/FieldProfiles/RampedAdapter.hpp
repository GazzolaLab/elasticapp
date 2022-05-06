//
// Created by Tejaswin Parthasarathy on 5/4/22.
//

#pragma once

#include <elastica/Utilities/DefineTypes.h>

#include "Rampup.hpp"

namespace elastica {

  template <typename Field_, typename Rampup_>
  class RampedFieldAdapter : public Field_ {
   protected:
    //**Type definitions********************************************************
    //! Field type
    using Field = Field_;
    //! Rampup type
    using Rampup = Rampup_;
    //**************************************************************************

   public:
    //**Type definitions********************************************************
    //! Real type
    using Real = typename Rampup::Real;
    //**************************************************************************

    //**Constructor***********************************************************
    /*!\name Constructor */
    //@{
    RampedFieldAdapter() : Field(), rampup_() {}
    RampedFieldAdapter(Field field, Rampup rampup)
        : Field(std::move(field)), rampup_(std::move(rampup)) {}
    RampedFieldAdapter(RampedFieldAdapter const& other)
        : Field_(other), rampup_(other.rampup_) {}
    RampedFieldAdapter(RampedFieldAdapter&& other) noexcept
        : Field_(std::move(other)), rampup_(std::move(other.rampup_)){};
    //@}
    //************************************************************************

    //**Assignment Operators**************************************************
    /*!\name Assignment Operators */
    //@{
    RampedFieldAdapter& operator=(RampedFieldAdapter other) noexcept {
      Field::operator=(std::move(other));
      swap(*this, other);
      return *this;
    }
    //@}
    //************************************************************************

    friend void swap(RampedFieldAdapter& first,
                     RampedFieldAdapter& second) noexcept {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two objects,
      // the two objects are effectively swapped
      swap(first.rampup_, second.rampup_);
    }

    //**Destructor************************************************************
    /*!\name Destructor */
    //@{
    ~RampedFieldAdapter() = default;
    //@}
    //************************************************************************

    //**Access operators******************************************************
    /*!\name Access operators */
    //@{
    inline auto rampup_profile() & noexcept -> Rampup& { return rampup_; }
    inline auto rampup_profile() const& noexcept -> Rampup const& {
      return rampup_;
    }
    //@}
    //************************************************************************

   private:
    using Field::operator();

   public:
    //**Function methods******************************************************
    /*!\name Function methods */
    //@{
    inline auto operator()(::elastica::Time time_v) const noexcept {
      // apply a multiplicative ramped field
      return rampup_(static_cast<Real>(time_v)) * Field::operator()(time_v);
    }
    //@}
    //************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    Rampup rampup_;
    //@}
    //**************************************************************************
  };

  //****************************************************************************
  /*!\brief Idiomatically makes a ramped field
   * \ingroup extension
   */
  template <typename Field_, typename Real_, typename F_>
  auto make_ramped_field(Field_ field, Rampup<Real_, F_> rampup_profile) {
    return RampedFieldAdapter<Field_, Rampup<Real_, F_>>(
        std::move(field), std::move(rampup_profile));
  }
  //****************************************************************************

}  // namespace elastica
