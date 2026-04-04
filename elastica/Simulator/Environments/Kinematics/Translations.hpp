//==============================================================================
/*!
//  \file
//  \brief Translation Kinematics
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <tuple>
#include <type_traits>
#include <utility>

#include "Simulator/Environments/Kinematics/Types.hpp"
#include "Simulator/Frames.hpp"
//
#include "Time/SimulationTime.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/MakeNamedFunctor.hpp"
#include "Utilities/NamedType.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace kinematics {

    namespace detail {

      struct TranslationSignatures {
        /// [translation_signature]
        template <typename F>
        using signature = std::is_same<decltype(std::declval<F const&>()(
                                           std::declval<::elastica::real_t>())),
                                       elastica::real_t>;
        /// [translation_signature]
      };

      template <typename F>
      struct ImposedExpr;

      template <typename T, typename U>
      void swap(ImposedExpr<T>& first, ImposedExpr<U>& second);

      template <typename F>
      struct ImposedExpr {
        explicit ImposedExpr(F f) noexcept : f_(std::move(f)){};
        ImposedExpr(ImposedExpr const&) = default;
        ImposedExpr(ImposedExpr&&) noexcept = default;
        //        ImposedExpr& operator=(ImposedExpr const&) = default;
        //        ImposedExpr& operator=(ImposedExpr&&) noexcept = default;

        ImposedExpr& operator=(ImposedExpr other) {
          // using std::swap;
          ::elastica::kinematics::detail::swap(*this, other);
          return *this;
        }

        //        ImposedExpr& operator=(ImposedExpr const& other) {
        //          if (this != &other)
        //            this->f_ = other.f_;
        //          return *this;
        //        }
        //        ImposedExpr& operator=(ImposedExpr && other) noexcept {
        //          if (this != &other)
        //            this->f_ = std::move(other.f_);
        //          return *this;
        //        }

        template <typename G>
        ImposedExpr(ImposedExpr<G> const& other) : f_(other.f_){}
        template <typename G>
        ImposedExpr(ImposedExpr<G>&& other) noexcept
            : f_(std::move(other.f_)){}
        template <typename G>
        ImposedExpr& operator=(ImposedExpr<G> other) {
          // using std::swap;
          swap(*this, other);
          return *this;
        }

        ~ImposedExpr() = default;

        // C++14 doesnt have nothrow swappable, so hold off for now
        template <typename T, typename U>
        friend void swap(ImposedExpr<T>& first,
                         ImposedExpr<U>& second) /* noexcept */;
        explicit operator F() const { return f_; }
        auto operator()(Time time_v) const /*noexcept*/ {
          // invoke
          return f_(static_cast<real_t>(time_v));
        };
        F f_;
      };

      template <typename T, typename U>
      void swap(ImposedExpr<T>& first, ImposedExpr<U>& second) /*
      noexcept */
      {
        using std::swap;
        swap(first.f_, second.f_);
      }

      template <::elastica::EulerianFrame::DirectionType Direction_, typename F>
      struct TranslationExpr : public ImposedExpr<F> {
        using Parent = ImposedExpr<F>;
        using Parent::Parent;
        using Parent::operator=;
        static constexpr auto Direction = Direction_;
      };

      template <::elastica::EulerianFrame::DirectionType Direction_, typename F,
                typename G>
      struct TranslationKinematics {
        using Position = TranslationExpr<Direction_, F>;
        using Velocity = TranslationExpr<Direction_, G>;
        static constexpr auto Direction = Direction_;

        TranslationKinematics(Position p, Velocity v) noexcept
            : p_(std::move(p)), v_(std::move(v)){};
        TranslationKinematics(F pf, G vf) noexcept
            : TranslationKinematics(Position(std::move(pf)),
                                    Velocity(std::move(vf))){};
        TranslationKinematics(TranslationKinematics const&) = default;
        TranslationKinematics(TranslationKinematics&&) noexcept = default;

        TranslationKinematics& operator=(TranslationKinematics const& other) = default;
        TranslationKinematics& operator=(
            TranslationKinematics&& other) noexcept = default;


        ~TranslationKinematics() = default;

        template <typename OF, typename OG>
        TranslationKinematics(
            TranslationKinematics<Direction_, OF, OG> const& other)
            : TranslationKinematics(other.p_, other.v_){}
        template <typename OF, typename OG>
        TranslationKinematics(TranslationKinematics<Direction_, OF, OG>&& other)
            : TranslationKinematics(std::move(other.p_), std::move(other.v_)){}

        // C++14 doesnt have nothrow swappable, so hold off for now
        // friend void swap(
        //     TranslationKinematics<Direction_, F, G>& first,
        //     TranslationKinematics<Direction_, F, G>& second) /* noexcept */;

        auto operator()(Time time_v) const /*noexcept*/ { return p_(time_v); };
        auto derivative(Time time_v) const /*noexcept*/ { return v_(time_v); };
        Position p_;
        Velocity v_;
      };

      template <::elastica::EulerianFrame::DirectionType Direction_, typename P,
                typename V>
      /*constexpr*/ decltype(auto) translate_along(P position_func,
                                                   V velocity_func) noexcept {
        using RT = detail::TranslationKinematics<Direction_, P, V>;
        return RT(std::move(position_func), std::move(velocity_func));
      }

      template <::elastica::EulerianFrame::DirectionType Direction_,
                typename... Kinematics>
      constexpr decltype(auto) get_kinematics(
          GroupedTranslationKinematics<Kinematics...>&
              grouped_kinematics) noexcept;
      template <::elastica::EulerianFrame::DirectionType Direction_,
                typename... Kinematics>
      constexpr decltype(auto) get_kinematics(
          GroupedTranslationKinematics<Kinematics...> const&
              grouped_kinematics) noexcept;
      template <::elastica::EulerianFrame::DirectionType Direction_,
                typename... Kinematics>
      constexpr decltype(auto) get_kinematics(
          GroupedTranslationKinematics<Kinematics...>&&
              grouped_kinematics) noexcept;

      // do it linear for now, later on refactor based on Mechanica.
      template <typename... Kinematics_>
      class GroupedTranslationKinematics {
       private:
        using KinematicsList = tmpl::list<Kinematics_...>;
        using Kinematics = tmpl::as_tuple<KinematicsList>;
        using DirectionType = ::elastica::EulerianFrame::DirectionType;
        using Directions =
            tmpl::integral_list<DirectionType, Kinematics_::Direction...>;
        template <typename... OtherKinematics>
        friend class GroupedTranslationKinematics;

       public:
        explicit GroupedTranslationKinematics(Kinematics_... kins)
            : kinematics_(std::move(kins)...) {}
        ~GroupedTranslationKinematics() = default;

        GroupedTranslationKinematics(GroupedTranslationKinematics const&) =
            default;
        GroupedTranslationKinematics(GroupedTranslationKinematics&&) noexcept =
            default;

        GroupedTranslationKinematics& operator=(
            GroupedTranslationKinematics const& other) = default;
        GroupedTranslationKinematics& operator=(
            GroupedTranslationKinematics&& other) noexcept = default;

        //          GroupedTranslationKinematics& operator=(
        //            GroupedTranslationKinematics const& other) noexcept {
        //          this->kinematics_ = other.kinematics_;
        //          return *this;
        //        }
        //
        //        GroupedTranslationKinematics& operator=(
        //            GroupedTranslationKinematics&& other) noexcept {
        //          this->kinematics_ = std::move(other.kinematics_);
        //          return *this;
        //        }

        // coercion constructor for interop with std::function and friends

        template <typename... OtherKinematics>
        GroupedTranslationKinematics(
            GroupedTranslationKinematics<OtherKinematics...> const& other)
            : GroupedTranslationKinematics(Kinematics_(
                  get_kinematics<Kinematics_::Direction>(other))...) {}
        template <typename... OtherKinematics>
        GroupedTranslationKinematics(
            GroupedTranslationKinematics<OtherKinematics...>&& other)
            : GroupedTranslationKinematics(Kinematics_(
                  get_kinematics<Kinematics_::Direction>(other))...) {}

       protected:
        template <DirectionType Direction_>
        static constexpr inline auto access_impl() noexcept -> tmpl::index_of<
            Directions, tmpl::integral_constant<DirectionType, Direction_>>;

        template <DirectionType Direction_>
        static constexpr auto access() noexcept -> std::size_t {
          return decltype(access_impl<Direction_>())::value;
        }

        /*! \cond ELASTICA_INTERNAL */
        template <DirectionType Direction_, typename... Kinematics>
        friend constexpr decltype(auto) get_kinematics(
            GroupedTranslationKinematics<Kinematics...>&
                grouped_kinematics) noexcept;
        template <DirectionType Direction_, typename... Kinematics>
        friend constexpr decltype(auto) get_kinematics(
            GroupedTranslationKinematics<Kinematics...> const&
                grouped_kinematics) noexcept;
        /*! \endcond */

       public:
        //**Data access*********************************************************
        /*!\name Data access
         * \brief Access to the underlying data
         */
        //@{
        inline constexpr Kinematics& data() & noexcept { return kinematics_; }
        inline constexpr Kinematics const& data() const& noexcept {
          return kinematics_;
        }
        inline constexpr Kinematics&& data() && noexcept {
          return static_cast<Kinematics&&>(kinematics_);
        }
        //@}
        //**********************************************************************

        //**Kinematics**********************************************************
        /*!\name Kinematics
         * \brief Kinematics functions
         */
        //@{
        template <DirectionType Direction_>
        auto compute_position_along(Time time_v) const {
          return get_kinematics<Direction_>(*this)(time_v);
        }

        template <DirectionType Direction_>
        auto compute_velocity_along(Time time_v) const {
          return get_kinematics<Direction_>(*this).derivative(time_v);
        }
        //@}
        //**********************************************************************

        //**********************************************************************
        /*!\brief Get the number of kinematics
         */
        static inline constexpr auto size() noexcept -> std::size_t {
          return tmpl::size<KinematicsList>::value;
        }
        //**********************************************************************

       private:
        Kinematics kinematics_;
      };
      //************************************************************************

      //**Get functions*********************************************************
      // get_kinematics overloads (omitted from Doxygen: constexpr/decltype(auto)
      // order does not match Doxygen's member matcher for out-of-line style).
      /// \cond DOXYGEN_SHOULD_SKIP_THIS
      template <::elastica::EulerianFrame::DirectionType Direction_,
                typename... Kinematics>
      inline constexpr decltype(auto) get_kinematics(
          GroupedTranslationKinematics<Kinematics...>&
              grouped_kinematics) noexcept {
        using std::get;
        return get<GroupedTranslationKinematics<Kinematics...>::template access<
            Direction_>()>(grouped_kinematics.data());
      }
      template <::elastica::EulerianFrame::DirectionType Direction_,
                typename... Kinematics>
      inline constexpr decltype(auto) get_kinematics(
          GroupedTranslationKinematics<Kinematics...> const&
              grouped_kinematics) noexcept {
        using std::get;
        return get<GroupedTranslationKinematics<Kinematics...>::template access<
            Direction_>()>(grouped_kinematics.data());
      }
      template <::elastica::EulerianFrame::DirectionType Direction_,
                typename... Kinematics>
      inline constexpr decltype(auto) get_kinematics(
          GroupedTranslationKinematics<Kinematics...>&&
              grouped_kinematics) noexcept {
        using std::get;
        return get<GroupedTranslationKinematics<Kinematics...>::template access<
            Direction_>()>(
            static_cast<GroupedTranslationKinematics<Kinematics...>>(
                grouped_kinematics)
                .data());
      }
      /// \endcond
      //**************************************************************************

    }  // namespace detail

    //**Type definitions********************************************************
    /*! \cond ELASTICA_INTERNAL */
    //! Strongly type position kinematic
    template <typename Func>
    using MarkPosition =
        named_type::NamedType<Func, struct MarkPositionParameter,
                              named_type::Gettable>;

    //! Strongly typed velocity kinematic
    template <typename Func>
    using MarkVelocity =
        named_type::NamedType<Func, struct MarkVelocityParameter,
                              named_type::Gettable>;
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Marks a position kinematic from a set of functors
     * \ingroup kinematics
     *
     * \details
     * Marks the input functor f as a position kinematic within the context of
     * an \elastica kinematics
     *
     * \example
     * The following shows a typical use of mark_position with a lambda.
     * \snippet Test_Translations.cpp mark_position_example
     *
     * \note
     * `f` needs to conform to the expected signature of a position kinematic
     * \snippet this translation_signature
     *
     * \param f Input function that acts as a value constraint
     *
     * \see translate_along()
     */
    template <typename F>
    constexpr inline decltype(auto) mark_position(F f) noexcept(
        noexcept(named_type::make_named_functor<MarkPosition>(std::move(f)))) {
      return named_type::make_named_functor<MarkPosition>(std::move(f));
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Marks a velocity kinematic from a set of functors
     * \ingroup kinematics
     *
     * \details
     * Marks the input functor f as a velocity kinematic within the context of
     * an \elastica kinematics
     *
     * \example
     * The following shows a typical use of mark_velocity.
     * \snippet Test_Translations.cpp mark_velocity_example
     *
     * \note
     * `f` needs to conform to the expected signature of a velocity kinematic
     * \snippet this translation_signature
     *
     * \param f Input function that acts as a value constraint
     *
     * \see translate_along()
     */
    template <typename F>
    constexpr inline decltype(auto) mark_velocity(F f) noexcept(
        noexcept(named_type::make_named_functor<MarkVelocity>(std::move(f)))) {
      return named_type::make_named_functor<MarkVelocity>(std::move(f));
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a translation kinematic in an idiomatic way
     * \ingroup kinematics
     *
     * \details
     * For prototyping a translational kinematic function along a particular
     * direction within \elastica, we use translate_along() to reduce boiler-
     * plate. The translate_along() function takes two callables
     * (differentiated appropriately into a position and velocity function, see
     * below) marked as a function for position and function for velocity
     * respectively, see mark_position(), mark_velocity() Usually, but not
     * necessarily, these callables are C++ lambda expressions. The following
     * example is an idiomatic way of creating a TranslationKinematic
     *
     * \example
     * \snippet Test_Translations.cpp translate_along_eg
     *
     * We draw attention to the semantics and readability provided by using the
     * concomitant functions mark_position(), mark_velocity().
     * Note that the order of mark_position() and mark_velocity()
     * do not matter.
     *
     * \tparam Direction_ Direction to apply position and velocity
     * \param p Position functor wrapped with mark_position()
     * \param v Velocity functor wrapped with mark_velocity()
     * \return Translation
     *
     * \see mark_velocity(), mark_position()
     */
    template <::elastica::EulerianFrame::DirectionType Direction_, typename P,
              typename V>
    /*constexpr*/ decltype(auto) translate_along(MarkPosition<P> p,
                                                 MarkVelocity<V> v) noexcept {
      return detail::translate_along<Direction_>(std::move(p.get()),
                                                 std::move(v.get()));
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of translate_along for switched arguments
     * \ingroup kinematics
     */
    template <::elastica::EulerianFrame::DirectionType Direction_, typename P,
              typename V>
    /*constexpr*/ decltype(auto) translate_along(MarkVelocity<V> v,
                                                 MarkPosition<P> p) noexcept {
      return detail::translate_along<Direction_>(std::move(p.get()),
                                                 std::move(v.get()));
    }
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename... Kinematics>
    using Translations = detail::GroupedTranslationKinematics<Kinematics...>;
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes translations buffer in an idiomatic way
     * \ingroup kinematics
     *
     * \details
     * For prototyping a collection of translational kinematic functions along
     * different directions \elastica, we use make_translations() to reduce
     * boiler- plate. The make_translations() function takes the results of
     * translate_along() in different directions (::elastica::EulerianFrame::X,
     * ::elastica::EulerianFrame::Y, ::elastica::EulerianFrame::Z) and puts them
     * together for easy access. The following example is an idiomatic way of
     * creating a TranslationKinematic
     *
     * \example
     * \snippet Test_Translations.cpp make_translations_eg
     *
     * \param kins Kinematic translations using translate_along()
     * \return Translation buffer with all translations
     *
     * \see translate_along()
     */
    template <typename... Kinematics>
    /*constexpr*/ decltype(auto) make_translations(
        Kinematics... kins) noexcept {
      return Translations<Kinematics...>(std::move(kins)...);
    }
    //**************************************************************************

  }  // namespace kinematics

}  // namespace elastica