#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ),
    error_( false ),
    buffer_(),
    closed_( false ),
    bytes_pushed_( 0 ),
    bytes_popped_( 0 )
{}

void Writer::push( string data )
{
  (void)data; // Your code here.
  size_t can_push = min(available_capacity(), data.size());
  buffer_.append(data.substr(0, can_push));
  bytes_pushed_ += can_push;
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

bool Writer::is_closed() const
{
  return closed_; // Your code here.
}

uint64_t Writer::available_capacity() const
{
  return (capacity_ - buffer_.size()); // Your code here.
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_; // Your code here.
}

string_view Reader::peek() const
{
  return std::string_view(buffer_);
}

void Reader::pop( uint64_t len )
{
  (void)len; // Your code here.
  size_t can_pop = min(len, buffer_.size());
  buffer_.erase(0, can_pop);
  bytes_popped_ += can_pop;
}

bool Reader::is_finished() const
{
  return closed_ && buffer_.empty(); // Your code here.
}

uint64_t Reader::bytes_buffered() const
{
  return buffer_.size(); // Your code here.
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_; // Your code here.
}

