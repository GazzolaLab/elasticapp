#pragma once

#include <new>
#include <sstream>

#include "Utilities/Logging/LoggingLevel.hpp"

namespace elastica {
  namespace logging {

    /*
     * LoggingSections are responsible for taking the
     * stream of contents input to the logger and passing
     * it (after proper checking and reformatting) to the
     * Logger for writing it onto a file.
     */
    class LoggingSection {
     public:
      // Allows implicit conversion of level to LoggingSection by design
      // Used in Singleton Macros
      LoggingSection(LoggingLevel level);  // NOLINT

      inline LoggingSection(const LoggingSection& ls);
      inline LoggingSection(LoggingSection&& ls) noexcept;

      // Prevent access to the user explicitly calling a LoggingSection
      LoggingSection& operator=(const LoggingSection& ls) = delete;
      LoggingSection& operator=(LoggingSection&& ls) = delete;

      ~LoggingSection();

      // Allows implicit conversion of LoggingSection to bool by design,
      // Used in Singleton Macros
      inline operator bool() const;  // NOLINT

      template <typename Type>
      inline void log(const Type& message);
      void commit();

     private:
      // Disable dynamic memory allocation for LoggingSection objects using the
      // Concrete Data Type idiom :
      // https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Concrete_Data_Type
      void* operator new(std::size_t count);
      void* operator new[](std::size_t count);
      void* operator new(std::size_t count, const std::nothrow_t& tag) noexcept;
      void* operator new[](std::size_t count,
                           const std::nothrow_t& tag) noexcept;
      void* operator new(std::size_t count, void* ptr);
      void* operator new[](std::size_t count, void* ptr);

      void operator delete(void* ptr) noexcept;
      void operator delete[](void* ptr) noexcept;
      void operator delete(void* ptr, std::size_t size) noexcept;
      void operator delete[](void* ptr, std::size_t size) noexcept;
      void operator delete(void* ptr, const std::nothrow_t& tag) noexcept;
      void operator delete[](void* ptr, const std::nothrow_t& tag) noexcept;
      void operator delete(void* ptr, void* place) noexcept;
      void operator delete[](void* ptr, void* place) noexcept;

      LoggingLevel _logging_level;
      // Intermediate buffer for log messages.
      std::stringstream _message;
    };

    inline LoggingSection::LoggingSection(const LoggingSection& ls)
        : _logging_level(ls._logging_level) {}

    inline LoggingSection::LoggingSection(LoggingSection&& ls) noexcept
        : _logging_level(ls._logging_level), _message(std::move(ls._message)) {}

    /*!\brief Conversion operator to \a bool.
     *
     * The conversion operator returns \a true to indicate that the logging
     * section is active.
     *
     * Finds use in macros where the LoggingSection is created in the predicate
     * of an if condition, like so if (LoggingSection log = debug){...
     *
     * We need the if to always evaluate to true
     */
    inline LoggingSection::operator bool() const { return true; }

    template <typename Type>  // Type of the log message
    inline void LoggingSection::log(const Type& message) {
      _message << message;
    }

    //=================================================================================================
    //
    //  FREE FUNCTIONS
    //
    //=================================================================================================
    template <typename Type>  // Type of the log message
    inline LoggingSection& operator<<(LoggingSection& logsection,
                                      const Type& message) {
      logsection.log(message);
      return logsection;
    }

  }  // namespace logging
}  // namespace elastica
