//
// Created by Tejaswin Parthasarathy on 5/13/21.
//

#pragma once

#include <iomanip>
#include <iostream>
#include <streambuf>

namespace elastica {

  // https://codereview.stackexchange.com/a/107514
  class widthbuf : public std::streambuf {
   public:
    widthbuf(std::size_t w, std::streambuf* s);
    ~widthbuf() override;
    void set_indent(int w);

   private:
    typedef std::basic_string<char_type> string;
    // This is basically a line-buffering stream buffer.
    // The algorithm is:
    // - Explicit end of line ("\r" or "\n"): we flush our buffer
    //   to the underlying stream's buffer, and set our record of
    //   the line length to 0.
    // - An "alert" character: sent to the underlying stream
    //   without recording its length, since it doesn't normally
    //   affect the a appearance of the output.
    // - tab: treated as occupying `tab_width` characters, but is
    //   passed through undisturbed (but if we wanted to expand it
    //   to `tab_width` spaces, that would be pretty easy to do so
    //   you could adjust the tab-width if you wanted.
    // - Everything else: really basic buffering with word wrapping.
    //   We try to add the character to the buffer, and if it exceeds
    //   our line width, we search for the last space/tab in the
    //   buffer and break the line there. If there is no space/tab,
    //   we break the line at the limit.
    int_type overflow(int_type c) override;

   private:
    std::size_t indent_width;
    std::size_t def_width;
    std::size_t width;
    std::size_t count;
    static const int tab_width = 8;
    string prefix;

    char_type space = static_cast<char_type>(' ');

    std::streambuf* sbuf;

    string buffer;
  };

  class widthstream : public std::ostream {
   public:
    inline widthstream(size_t width, std::ostream& os)
        : std::ostream(&buf), buf(width, os.rdbuf()) {}

    inline widthstream& indent(int w) {
      buf.set_indent(w);
      return *this;
    }

   private:
    widthbuf buf;
  };

}  // namespace elastica