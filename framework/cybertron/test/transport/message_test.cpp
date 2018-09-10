#include "gtest/gtest.h"

#include "cybertron/common/global_data.h"
#include "cybertron/message/raw_message.h"
#include "cybertron/transport/message/history.h"
#include "cybertron/transport/message/history_attributes.h"
#include "cybertron/transport/message/listener_handler.h"
#include "cybertron/transport/message/message_info.h"

namespace apollo {
namespace cybertron {
namespace transport {

using apollo::cybertron::message::RawMessage;

TEST(MessageInfoTest, message_info_test) {
  Identity sender_id;
  sender_id.set_data("sender");
  Identity spare_id;
  spare_id.set_data("spare");
  Identity sender_id2;
  sender_id2.set_data("sender2");
  Identity spare_id2;
  spare_id2.set_data("spare2");

  MessageInfo info1(sender_id, 0, spare_id);
  EXPECT_EQ(std::string("sender"), std::string(info1.sender_id().data()));
  EXPECT_EQ(std::string("spare"), std::string(info1.spare_id().data()));
  EXPECT_EQ(0, info1.seq_num());
  MessageInfo info2(info1);
  MessageInfo info3(sender_id, 1, spare_id);
  MessageInfo info4(sender_id, 1);
  info4.set_seq_num(2);
  EXPECT_EQ(2, info4.seq_num());

  EXPECT_EQ(info1, info2);
  EXPECT_FALSE(info2 == info3);
  info3 = info3;
  info3 = info2;
  EXPECT_EQ(info1, info3);

  info1.set_sender_id(sender_id);
  info1.set_spare_id(spare_id);

  info1.set_sender_id(sender_id2);
  EXPECT_FALSE(info1 == info2);
  info1.set_sender_id(sender_id);
  info1.set_spare_id(spare_id2);
  EXPECT_FALSE(info1 == info2);

  std::string str;
  EXPECT_TRUE(info1.SerializeTo(&str));
  EXPECT_NE(std::string(""), str);
  EXPECT_TRUE(info2.DeserializeFrom(str));
  EXPECT_EQ(info1, info2);
  EXPECT_FALSE(info2.DeserializeFrom("error"));
}

TEST(HistoryTest, history_test) {
  Identity sender_id;
  sender_id.set_data("sender");
  Identity spare_id;
  spare_id.set_data("spare");
  MessageInfo message_info(sender_id, 0, spare_id);

  HistoryAttributes attr;
  History<RawMessage> history(attr);
  MessageInfo info;
  auto message = std::shared_ptr<RawMessage>(new RawMessage);

  history.Disable();
  history.Add(message, message_info);
  EXPECT_EQ(0, history.GetSize());
  history.Enable();
  history.Add(message, message_info);
  EXPECT_EQ(1, history.GetSize());
  message_info.set_seq_num(1);
  EXPECT_FALSE(history.is_full());

  int depth = 10;
  HistoryAttributes attr2(proto::QosHistoryPolicy::HISTORY_KEEP_LAST, depth);
  History<RawMessage> history2(attr2);
  EXPECT_EQ(depth, history2.depth());
  EXPECT_EQ(1000, history2.max_depth());
  history2.Enable();
  for (int i = 0; i < depth + 1; i++) {
    message_info.set_seq_num(i);
    history2.Add(message, message_info);
  }
  EXPECT_TRUE(history2.is_full());
  std::vector<History<RawMessage>::CachedMessage> messages;
  history2.GetCachedMessage(nullptr);
  history2.GetCachedMessage(&messages);
  EXPECT_EQ(depth, messages.size());

  HistoryAttributes attr3(proto::QosHistoryPolicy::HISTORY_KEEP_ALL, depth);
  History<RawMessage> history3(attr3);
  EXPECT_EQ(1000, history3.depth());
  HistoryAttributes attr4(proto::QosHistoryPolicy::HISTORY_KEEP_LAST, 1024);
  History<RawMessage> history4(attr4);
  EXPECT_EQ(1000, history4.depth());
}

TEST(ListenerHandlerTest, listener_handler_test) {
  Identity sender_id;
  sender_id.set_data("sender");
  Identity spare_id;
  spare_id.set_data("spare");
  MessageInfo message_info(sender_id, 0, spare_id);
  auto message = std::shared_ptr<RawMessage>(new RawMessage);
  int call_count = 0;
  ListenerHandler<RawMessage>::Listener listener =
      [&call_count](const std::shared_ptr<RawMessage>& message,
                    const MessageInfo& message_info) {
        ++call_count;
        AINFO << "Got Message from " << message_info.sender_id().data()
              << ", sequence_num: " << message_info.seq_num()
              << ", to spare: " << message_info.spare_id().data();
      };

  uint64_t self_id = 123;
  uint64_t opposite_id = 456;
  ListenerHandler<RawMessage> listener_handler;
  EXPECT_TRUE(listener_handler.IsRawMessage());
  listener_handler.Run(message, message_info);
  EXPECT_EQ(0, call_count);
  listener_handler.Connect(self_id, listener);
  listener_handler.Connect(self_id, opposite_id, listener);
  listener_handler.Connect(self_id, opposite_id, listener);
  listener_handler.Connect(self_id, message_info.spare_id().HashValue(),
                           listener);
  listener_handler.Run(message, message_info);
  EXPECT_EQ(1, call_count);
  Identity sender_id2;
  sender_id2.set_data("YOU");
  message_info.set_sender_id(sender_id2);
  listener_handler.Run(message, message_info);
  EXPECT_EQ(2, call_count);

  listener_handler.Disconnect(789);
  listener_handler.Disconnect(self_id);
  listener_handler.Disconnect(self_id, 789);
  listener_handler.Disconnect(self_id, opposite_id);
  listener_handler.Disconnect(self_id, opposite_id);
  listener_handler.Run(message, message_info);
}

}  // namespace transport
}  // namespace cybertron
}  // namespace apollo

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
