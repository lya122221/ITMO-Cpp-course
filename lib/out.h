#include <iostream>

template <typename Stream>
struct OutPart {
  Stream& stream;
};

template <typename Stream>
OutPart<Stream> Out(Stream& stream) {
  return OutPart(stream);
}

template <typename Source, typename Stream>
class OutAdapter {
public:
  OutAdapter(Source source, Stream& stream) : source_(source), stream_(stream) {}

  Stream& operator()() {
    for (auto& item : source_) {
      stream_ << item;
    }
    return stream_;
  }
private:
  Source source_;
  Stream& stream_;
};

template <typename Source, typename Stream>
void operator|(Source source, OutPart<Stream> out_part) {
  OutAdapter out(source, out_part.stream);
  out();
}