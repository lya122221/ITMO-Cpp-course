#include <iostream>

template <typename Stream, typename Separator>
struct WritePart {
  Stream& stream;
  Separator sep;
};

template <typename Stream, typename Separator>
WritePart<Stream, Separator> Write(Stream& stream, Separator sep) {
  return WritePart(stream, sep);
}

template <typename Source, typename Stream, typename Separator>
class WriteAdapter {
public:
  WriteAdapter(Source source, Stream& stream, Separator sep) : source_(source), stream_(stream), sep_(sep) {}

  Stream& operator()() {
    auto it = source_.begin();
    auto end = source_.end();

    if (it == end) {
      return stream_;
    }

    stream_ << *it;
    it++;

    for (; it != end; it++) {
      stream_ << sep_ << *it;
    }

    return stream_;
  }
private:
  Source source_;
  Stream& stream_;
  Separator sep_;
};

template <typename Source, typename Stream, typename Separator>
void operator|(Source source, WritePart<Stream, Separator> write_part) {
  WriteAdapter write(source, write_part.stream, write_part.sep);
  write();
}