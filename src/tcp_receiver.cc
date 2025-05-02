#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive(TCPSenderMessage message) {
  // ----------- 如果沒收到 SYN，不能處理任何資料 -----------
  if (message.RST) {
    writer().set_error(); 
    return;                
  }
  if (message.SYN && !syn_received_) {
      syn_received_ = true;
      isn_ = message.seqno;
  }

  if (!syn_received_) {
      return;  // 沒收到 SYN，直接退場
  }

  // ----------- unwrap TCP seqno 成絕對序號 -----------
  uint64_t abs_seqno = message.seqno.unwrap(isn_, checkpoint_);

  // ----------- 把 TCP 絕對序號轉成 stream index -----------
  uint64_t stream_index = abs_seqno - (message.SYN ? 0 : 1);  // SYN 佔一格但不算進資料流

  // ----------- 插入資料到 reassembler -----------
  reassembler_.insert(stream_index, message.payload, message.FIN);

  // ----------- 收到 FIN 就記錄下來 -----------
  if (message.FIN) {
      fin_received_ = true;
  }

  // ----------- 更新 checkpoint（給下一個 unwrap 用）-----------
  checkpoint_ = reassembler_.writer().bytes_pushed() + 1;
}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage msg;
  msg.window_size = static_cast<uint16_t>(min(reassembler_.writer().available_capacity(), static_cast<uint64_t>(UINT16_MAX)));
  if (syn_received_) {
      uint64_t ack_abs = reassembler_.writer().bytes_pushed() + 1;  // +1 因為 SYN 佔一格

      if (fin_received_ && reassembler_.writer().is_closed()) {
          ack_abs += 1;  // +1 ack 掉 FIN 這一格
      }

      msg.ackno = Wrap32::wrap(ack_abs, isn_);
  }

  
  msg.RST = reassembler_.writer().has_error(); 
  return msg;
}
