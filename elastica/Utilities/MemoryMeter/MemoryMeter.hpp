//=================================================================================================
/*!
 *  \file pe/util/MemoryMeter.h
 *  \brief Memory meter for measuring memory consumption
 *
 *  Copyright (C) 2012 Tobias Preclik
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//=================================================================================================

#pragma once

//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <cstdint>
#include <limits>

// https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive
#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>
#include <mach/task.h>
#include <malloc/malloc.h>
#elif defined(__linux__)
#include <malloc.h>
#endif

namespace elastica {

  //=================================================================================================
  //
  //  CLASS DEFINITION
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Memory meter for measuring memory consumption.
   * \ingroup util
   */
  class MemoryMeter {
   public:
    //**Constructor*********************************************************************************
    /*!\name Constructor */
    //@{
    explicit inline MemoryMeter();
    //@}
    //**********************************************************************************************

    //**Destructor**********************************************************************************
    // No explicitly declared destructor.
    //**********************************************************************************************

    //**Timing
    // functions****************************************************************************
    /*!\name Timing functions */
    //@{
    inline void start();
    inline void stop();
    inline void reset();
    //@}
    //**********************************************************************************************

    //**Get
    // functions*******************************************************************************
    /*!\name Get functions */
    //@{
    inline size_t getCounter() const;
    //@}
    //**********************************************************************************************

    //**Memory evaluation
    // functions*******************************************************************
    /*!\name Memory evaluation functions */
    //@{
    inline int64_t totalAllocation() const;
    inline int64_t minAllocation() const;
    inline int64_t maxAllocation() const;
    inline int64_t lastAllocation() const;
    inline int64_t totalInUse() const;
    inline int64_t minInUse() const;
    inline int64_t maxInUse() const;
    inline int64_t lastInUse() const;
    //@}
    //**********************************************************************************************

   private:
#if defined(__APPLE__) && defined(__MACH__)
    static constexpr bool free_memory_before_every_measurement = false;
    struct malloc_info_internal {
      std::size_t bytesInUse, sizeAllocated;
    };
    // To mimic linux interface
    static malloc_info_internal mallinfo();
    static void free_as_much_memory_as_possible();
#endif

   private:
    size_t counter_;
    size_t memAllocatedOnStart_;
    int64_t minAllocatedDelta_;
    int64_t maxAllocatedDelta_;
    int64_t totalAllocatedDelta_;
    int64_t lastAllocatedDelta_;
    size_t memInUseOnStart_;
    int64_t minInUseDelta_;
    int64_t maxInUseDelta_;
    int64_t totalInUseDelta_;
    int64_t lastInUseDelta_;
  };
  //*************************************************************************************************

  //=================================================================================================
  //
  //  CONSTRUCTOR
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Constructor of the MemoryMeter class.
   */
  inline MemoryMeter::MemoryMeter()
      : counter_(0),
        memAllocatedOnStart_(0),
        minAllocatedDelta_(std::numeric_limits<int64_t>::max()),
        maxAllocatedDelta_(std::numeric_limits<int64_t>::min()),
        totalAllocatedDelta_(0),
        lastAllocatedDelta_(0),
        memInUseOnStart_(0),
        minInUseDelta_(std::numeric_limits<int64_t>::max()),
        maxInUseDelta_(std::numeric_limits<int64_t>::min()),
        totalInUseDelta_(0),
        lastInUseDelta_(0) {}
  //*************************************************************************************************

  //=================================================================================================
  //
  //  MEASUREMENT FUNCTIONS
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Starting a measurement.
   *
   * \return void
   *
   * This function starts the memory measurement.
   */
  inline void MemoryMeter::start() {
    // DONE : Works only when using mstats
    // https://stackoverflow.com/questions/25609945/why-does-mstats-and-malloc-zone-statistics-not-show-recovered-memory-after-free
#if defined(__APPLE__) && defined(__MACH__)
    // WARNING: Here we need to call f to get accurate statistics, but it might
    // hurt performance by doing it everytime, so use only when you need
    if (free_memory_before_every_measurement)
      free_as_much_memory_as_possible();
    auto x = mallinfo();
    memAllocatedOnStart_ = x.sizeAllocated;
    memInUseOnStart_ = x.bytesInUse;
#elif defined(__linux__)
    struct mallinfo x = mallinfo();
    memAllocatedOnStart_ =
        static_cast<size_t>(x.arena) + static_cast<size_t>(x.hblkhd);
    memInUseOnStart_ =
        static_cast<size_t>(x.uordblks) + static_cast<size_t>(x.hblkhd);
#else
    // Windows
    memAllocatedOnStart_ = 0;
    memInUseOnStart_ = 0;
#endif
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Ending a measurement.
   *
   * \return void
   *
   * This function ends the currently running measurement and performs the
   * necessary statistical calculations.
   */
  inline void MemoryMeter::stop() {
    ++counter_;

#if defined(__APPLE__) && defined(__MACH__)
    // WARNING: Here we need to call f to get accurate statistics, but it might
    // hurt performance by doing it everytime, so use only when you need
    if (free_memory_before_every_measurement)
      free_as_much_memory_as_possible();

    auto x = mallinfo();
    std::size_t memAllocatedOnStop = x.sizeAllocated;
    std::size_t memInUseOnStop = x.bytesInUse;
#elif defined(__linux__)
    struct mallinfo x = mallinfo();
    size_t memAllocatedOnStop =
        static_cast<size_t>(x.arena) + static_cast<size_t>(x.hblkhd);
    size_t memInUseOnStop =
        static_cast<size_t>(x.uordblks) + static_cast<size_t>(x.hblkhd);
#else
    size_t memAllocatedOnStop = 0;
    size_t memInUseOnStop = 0;
#endif

    if (memAllocatedOnStop > memAllocatedOnStart_)
      lastAllocatedDelta_ =
          static_cast<int64_t>(memAllocatedOnStop - memAllocatedOnStart_);
    else
      lastAllocatedDelta_ =
          -static_cast<int64_t>(memAllocatedOnStart_ - memAllocatedOnStop);

    minAllocatedDelta_ = std::min(lastAllocatedDelta_, minAllocatedDelta_);
    maxAllocatedDelta_ = std::max(lastAllocatedDelta_, maxAllocatedDelta_);
    totalAllocatedDelta_ += lastAllocatedDelta_;

    if (memInUseOnStop > memInUseOnStart_)
      lastInUseDelta_ = static_cast<int64_t>(memInUseOnStop - memInUseOnStart_);
    else
      lastInUseDelta_ =
          -static_cast<int64_t>(memInUseOnStart_ - memInUseOnStop);

    minInUseDelta_ = std::min(lastInUseDelta_, minInUseDelta_);
    maxInUseDelta_ = std::max(lastInUseDelta_, maxInUseDelta_);
    totalInUseDelta_ += lastInUseDelta_;
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Resetting the timer.
   *
   * \return void
   *
   * This function completely resets the timer and all information on the
   * performed time measurements. In order to start a new time measurement, the
   * start() function has to be used.
   */
  inline void MemoryMeter::reset() {
    counter_ = 0;
    memAllocatedOnStart_ = 0;
    minAllocatedDelta_ = std::numeric_limits<int64_t>::max();
    maxAllocatedDelta_ = std::numeric_limits<int64_t>::min();
    totalAllocatedDelta_ = 0;
    lastAllocatedDelta_ = 0;
    memInUseOnStart_ = 0;
    minInUseDelta_ = std::numeric_limits<int64_t>::max();
    maxInUseDelta_ = std::numeric_limits<int64_t>::min();
    totalInUseDelta_ = 0;
    lastInUseDelta_ = 0;
  }
  //*************************************************************************************************

  //=================================================================================================
  //
  //  GET FUNCTIONS
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Returns the total number of measurements performed.
   *
   * \return The number of performed measurements.
   */
  inline size_t MemoryMeter::getCounter() const { return counter_; }
  //*************************************************************************************************

  //=================================================================================================
  //
  //  MEMORY EVALUATION FUNCTIONS
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Returns the total amount of memory allocated from the system.
   *
   * \return The total amount of memory allocated from the system.
   *
   * The value can be negative if memory was released.
   */
  inline int64_t MemoryMeter::totalAllocation() const {
    return totalAllocatedDelta_;
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Returns the minimum amount of memory allocated from the system in
   * one measurement.
   *
   * \return The minimum amount of memory allocated from the system in one
   * measurement.
   *
   * The value can be negative if memory was released.
   */
  inline int64_t MemoryMeter::minAllocation() const {
    return minAllocatedDelta_;
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Returns the maximum amount of memory allocated from the system in
   * one measurement.
   *
   * \return The maximum amount of memory allocated from the system in one
   * measurement.
   *
   * The value can be negative if memory was released.
   */
  inline int64_t MemoryMeter::maxAllocation() const {
    return maxAllocatedDelta_;
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Returns the amount of memory allocated from the system in the last
   * measurement.
   *
   * \return The amount of memory allocated from the system in the last
   * measurement.
   *
   * The value can be negative if memory was released.
   */
  inline int64_t MemoryMeter::lastAllocation() const {
    return lastAllocatedDelta_;
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Returns the total amount of memory allocated by the application.
   *
   * \return The total amount of memory allocated by the application.
   *
   * The value can be negative if memory was released.
   */
  inline int64_t MemoryMeter::totalInUse() const { return totalInUseDelta_; }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Returns the minimum amount of memory allocated by the application in
   * one measurement.
   *
   * \return The minimum amount of memory allocated by the application in one
   * measurement.
   *
   * The value can be negative if memory was released.
   */
  inline int64_t MemoryMeter::minInUse() const { return minInUseDelta_; }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Returns the maximum amount of memory allocated by the application in
   * one measurement.
   *
   * \return The maximum amount of memory allocated by the application in one
   * measurement.
   *
   * The value can be negative if memory was released.
   */
  inline int64_t MemoryMeter::maxInUse() const { return maxInUseDelta_; }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Returns the amount of memory allocated by the application in the
   * last measurement.
   *
   * \return The amount of memory allocated by the application in the last
   * measurement.
   *
   * The value can be negative if memory was released.
   */
  inline int64_t MemoryMeter::lastInUse() const { return lastInUseDelta_; }

#if defined(__APPLE__) && defined(__MACH__)  // MacOs
  MemoryMeter::malloc_info_internal MemoryMeter::mallinfo() {
#if 0
    // This seems to be unusable as even printfs change the value!
    size_t bytesInUse(0UL), blocksInUse(0UL), sizeAllocated(0UL);
    // vm_address_t is unsigned long
    vm_address_t* zones;
    unsigned int count;
    kern_return_t rc =
        malloc_get_all_zones(mach_task_self(), 0, &zones, &count);
    if (rc != 0) {
      fprintf(stderr, "rc was %d\n", rc);
    }
    for (unsigned int i = 0; i < count; ++i) {
      auto zone = reinterpret_cast<malloc_zone_t*>(zones[i]);
      char const* name = malloc_get_zone_name(zone);
      if (nullptr == name) {
        continue;
      }
      /* As defined in malloc/malloc.h
      typedef struct malloc_statistics_t {
        unsigned blocks_in_use;
        size_t size_in_use;
        size_t max_size_in_use;  // high water mark of touched memory
        size_t size_allocated;   // reserved in memory
      } malloc_statistics_t;
      */
      malloc_statistics_t stats;
      stats.blocks_in_use = 0;
      stats.size_in_use = 0;
      stats.max_size_in_use = 0;
      stats.size_allocated = 0;
      malloc_zone_statistics(zone, &stats);
      bytesInUse += stats.size_in_use;
      blocksInUse += stats.blocks_in_use;
      sizeAllocated += stats.size_allocated;
    }
    return MemoryMeter::malloc_info_internal{bytesInUse, blocksInUse,
                                             sizeAllocated};
#else   // not 0
    // This is much more stable
    struct mstats ms = mstats();
    return MemoryMeter::malloc_info_internal{ms.bytes_used, ms.bytes_total};
#endif  // 1
  }     // End function

  //****************************************************************************

  //****************************************************************************
  void MemoryMeter::free_as_much_memory_as_possible() {
    vm_address_t* zones;
    unsigned int count;
    unsigned int i;

    kern_return_t rc =
        malloc_get_all_zones(mach_task_self(), nullptr, &zones, &count);
    if (0 != rc) {
      fprintf(stderr, "rc was %d\n", rc);
    }
    for (i = 0; i < count; ++i) {
      auto zone = reinterpret_cast<malloc_zone_t*>(zones[i]);
      char const* name = malloc_get_zone_name(zone);
      if (nullptr == name) {
        continue;
      }
      malloc_zone_pressure_relief(zone, 0);
    }
  }

#endif  // not MacOS

}  // namespace elastica
